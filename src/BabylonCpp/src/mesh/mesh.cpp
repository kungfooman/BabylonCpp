#include <babylon/mesh/sub_mesh.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/math/matrix.h>
#include <babylon/math/tmp.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/_visible_instances.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/mesh_lod_level.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/morph/morph_target.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Mesh::Mesh(const string_t& iName, Scene* scene, Node* iParent, Mesh* source,
           bool doNotCloneChildren, bool clonePhysicsImpostor)
    : AbstractMesh{iName, scene}
    , onBeforeRenderObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onAfterRenderObservable{this, &Mesh::get_onAfterRenderObservable}
    , onBeforeDrawObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onBeforeDraw{this, &Mesh::set_onBeforeDraw}
    , delayLoadState{EngineConstants::DELAYLOADSTATE_NONE}
    , onLODLevelSelection{nullptr}
    , morphTargetManager{this, &Mesh::get_morphTargetManager,
                         &Mesh::set_morphTargetManager}
    , _geometry{nullptr}
    , _visibleInstances{nullptr}
    , _shouldGenerateFlatShading{false}
    , _originalBuilderSideOrientation{Mesh::DEFAULTSIDE()}
    , overrideMaterialSideOrientation{nullopt_t}
    , source{this, &Mesh::get_source}
    , isUnIndexed{this, &Mesh::get_isUnIndexed, &Mesh::set_isUnIndexed}
    , hasLODLevels{this, &Mesh::get_hasLODLevels}
    , geometry{this, &Mesh::get_geometry}
    , areNormalsFrozen{this, &Mesh::get_areNormalsFrozen}
    , _positions{this, &Mesh::get__positions}
    , _onBeforeDrawObserver{nullptr}
    , _morphTargetManager{nullptr}
    , _batchCache{::std::make_unique<_InstancesBatch>()}
    , _instancesBufferSize{32 * 16 * 4} // maximum of 32 instances
    , _overridenInstanceCount{0}
    , _effectiveMaterial{nullptr}
    , _preActivateId{-1}
    , _areNormalsFrozen{false}
    , _source{nullptr}
    , _tessellation{0}
    , _cap{Mesh::NO_CAP()}
    , _arc{1.f}
    , _closePath{false}
    , _closeArray{false}
{
  _boundingInfo = nullptr;

  scene = getScene();

  if (source) {
    // Geometry
    if (source->_geometry) {
      source->_geometry->applyToMesh(this);
    }

    // Deep copy
#if 0
    Tools::DeepCopy(source, this,
                    [
                      "name", "material", "skeleton", "instances", "parent",
                      "uniqueId", "source", "metadata", "hasLODLevels",
                      "geometry", "isBlocked", "areNormalsFrozen",
                      "onBeforeDrawObservable", "onBeforeRenderObservable",
                      "onAfterRenderObservable", "onBeforeDraw"
                    ],
                    ["_poseMatrix"]);
#endif

    // Source mesh
    _source = source;

    // Parent
    Node::set_parent(source->parent());

    // Pivot
    setPivotMatrix(source->getPivotMatrix());

    id = name + "." + source->id;

    // Material
    material = source->getMaterial();
    if (!doNotCloneChildren) {
      // Children
      for (auto& mesh : scene->meshes) {
        if (mesh->parent() == source) {
          // doNotCloneChildren is always going to be False
          mesh->clone(name + "." + mesh->name, this, doNotCloneChildren);
        }
      }
    }

    // Physics clone
    auto physicsEngine = getScene()->getPhysicsEngine();
    if (clonePhysicsImpostor && physicsEngine) {
      // auto impostor = physicsEngine->getImpostorForPhysicsObject(source);
      // if (impostor) {
      //  physicsImpostor = impostor->clone(this);
      //}
    }

    // Particles
    for (auto& system : scene->particleSystems) {
      auto& emitter = system->emitter;
      if (emitter.is<AbstractMeshPtr>()
          && (emitter.get<AbstractMeshPtr>().get() == source)) {
        system->clone(system->name, this);
      }
    }
    refreshBoundingInfo();
    computeWorldMatrix(true);
  }

  // Parent
  if (iParent != nullptr) {
    Node::set_parent(iParent);
  }
}

Mesh::~Mesh()
{
}

MorphTargetManagerPtr& Mesh::get_morphTargetManager()
{
  return _morphTargetManager;
}

void Mesh::set_morphTargetManager(const MorphTargetManagerPtr& value)
{
  if (_morphTargetManager == value) {
    return;
  }
  _morphTargetManager = value;
  _syncGeometryWithMorphTargetManager();
}

Mesh*& Mesh::get_source()
{
  return _source;
}

bool Mesh::get_isUnIndexed() const
{
  return _unIndexed;
}

void Mesh::set_isUnIndexed(bool value)
{
  if (_unIndexed != value) {
    _unIndexed = value;
    _markSubMeshesAsAttributesDirty();
  }
}

const string_t Mesh::getClassName() const
{
  return "Mesh";
}

IReflect::Type Mesh::type() const
{
  return IReflect::Type::MESH;
}

Observable<Mesh>& Mesh::get_onBeforeRenderObservable()
{
  return _onBeforeRenderObservable;
}

Observable<Mesh>& Mesh::get_onAfterRenderObservable()
{
  return _onAfterRenderObservable;
}

Observable<Mesh>& Mesh::get_onBeforeDrawObservable()
{
  return _onBeforeDrawObservable;
}

void Mesh::set_onBeforeDraw(
  const ::std::function<void(Mesh*, EventState&)>& callback)
{
  if (_onBeforeDrawObserver) {
    onBeforeDrawObservable().remove(_onBeforeDrawObserver);
  }
  _onBeforeDrawObserver = onBeforeDrawObservable().add(callback);
}

string_t Mesh::toString(bool fullDetails)
{
  std::ostringstream oss;
  oss << AbstractMesh::toString(fullDetails);
  oss << ", n vertices: " << getTotalVertices();
  oss << ", parent: "
      << ((!_waitingParentId.empty()) ? _waitingParentId :
                                        (parent() ? parent()->name : "NONE"));

  if (!animations.empty()) {
    for (auto& animation : animations) {
      oss << ", animation[0]: " << animation->toString(fullDetails);
    }
  }

  if (fullDetails) {
    if (_geometry) {
      auto ib = getIndices();
      auto vb = getVerticesData(VertexBuffer::PositionKind);

      if (!vb.empty() && !ib.empty()) {
        oss << ", flat shading: "
            << (vb.size() / 3 == ib.size() ? "YES" : "NO");
      }
    }
    else {
      oss << ", flat shading: UNKNOWN";
    }
  }
  return oss.str();
}

void Mesh::addSubMesh(SubMesh* subMesh)
{
  subMeshes.emplace_back(subMesh);
}

void Mesh::_unBindEffect()
{
  AbstractMesh::_unBindEffect();

  for (auto& instance : instances) {
    instance->_unBindEffect();
  }
}

bool Mesh::get_hasLODLevels() const
{
  return _LODLevels.size() > 0;
}

vector_t<MeshLODLevel*> Mesh::getLODLevels()
{
  return stl_util::to_raw_ptr_vector(_LODLevels);
}

void Mesh::_sortLODLevels()
{
  ::std::sort(_LODLevels.begin(), _LODLevels.end(),
              [](const unique_ptr_t<MeshLODLevel>& a,
                 const unique_ptr_t<MeshLODLevel>& b) {
                if (a->distance < b->distance) {
                  return 1;
                }
                if (a->distance > b->distance) {
                  return -1;
                }

                return 0;
              });
}

Mesh& Mesh::addLODLevel(float distance, Mesh* mesh)
{
  if (mesh && mesh->_masterMesh) {
    BABYLON_LOG_WARN("Mesh", "You cannot use a mesh as LOD level twice");
    return *this;
  }

  auto level = ::std::make_unique<MeshLODLevel>(distance, mesh);
  _LODLevels.emplace_back(::std::move(level));

  if (mesh) {
    mesh->_masterMesh = this;
  }

  _sortLODLevels();

  return *this;
}

Mesh* Mesh::getLODLevelAtDistance(float distance)
{
  for (const auto& level : _LODLevels) {
    if (stl_util::almost_equal(level->distance, distance)) {
      return level->mesh;
    }
  }
  return nullptr;
}

Mesh& Mesh::removeLODLevel(Mesh* mesh)
{
  for (auto& level : _LODLevels) {
    if (level->mesh == mesh) {
      stl_util::erase(_LODLevels, level);
      if (mesh) {
        mesh->_masterMesh = nullptr;
      }
    }
  }

  _sortLODLevels();
  return *this;
}

AbstractMesh* Mesh::getLOD(const CameraPtr& camera,
                           BoundingSphere* boundingSphere)
{
  if (_LODLevels.empty()) {
    return this;
  }

  BoundingSphere* bSphere = nullptr;

  if (boundingSphere) {
    bSphere = boundingSphere;
  }
  else {
    auto boundingInfo = getBoundingInfo();

    bSphere = &boundingInfo.boundingSphere;
  }

  auto distanceToCamera
    = bSphere->centerWorld.subtract(camera->globalPosition()).length();

  if (_LODLevels.back()->distance > distanceToCamera) {
    if (onLODLevelSelection) {
      onLODLevelSelection(distanceToCamera, this, _LODLevels.back()->mesh);
    }
    return this;
  }

  for (auto& level : _LODLevels) {
    if (level->distance < distanceToCamera) {
      if (level->mesh) {
        level->mesh->_preActivate();
        level->mesh->_updateSubMeshesBoundingInfo(worldMatrixFromCache());
      }

      if (onLODLevelSelection) {
        onLODLevelSelection(distanceToCamera, this, level->mesh);
      }
      return level->mesh;
    }
  }

  if (onLODLevelSelection) {
    onLODLevelSelection(distanceToCamera, this, this);
  }
  return this;
}

Geometry*& Mesh::get_geometry()
{
  return _geometry;
}

void Mesh::setGeometry(Geometry* geometry)
{
  _geometry = geometry;
}

size_t Mesh::getTotalVertices() const
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalVertices();
}

Float32Array Mesh::getVerticesData(unsigned int kind, bool copyWhenShared,
                                   bool forceCopy)
{
  if (!_geometry) {
    return Float32Array();
  }
  return _geometry->getVerticesData(kind, copyWhenShared, forceCopy);
}

VertexBuffer* Mesh::getVertexBuffer(unsigned int kind) const
{
  if (!_geometry) {
    return nullptr;
  }
  return _geometry->getVertexBuffer(kind);
}

bool Mesh::isVerticesDataPresent(unsigned int kind) const
{
  if (!_geometry) {
    if (!_delayInfo.empty()) {
      return ::std::find_if(
               _delayInfo.begin(), _delayInfo.end(),
               [&](VertexBuffer* item) { return item->getKind() == kind; })
             != _delayInfo.end();
    }
    return false;
  }

  return _geometry->isVerticesDataPresent(kind);
}

bool Mesh::isVertexBufferUpdatable(unsigned int kind) const
{
  if (!_geometry) {
    if (!_delayInfo.empty()) {
      return ::std::find_if(
               _delayInfo.begin(), _delayInfo.end(),
               [&](VertexBuffer* item) { return item->getKind() == kind; })
             != _delayInfo.end();
    }
    return false;
  }

  return _geometry->isVertexBufferUpdatable(kind);
}

Uint32Array Mesh::getVerticesDataKinds() const
{
  if (!_geometry) {
    Uint32Array result;
    if (!_delayInfo.empty()) {
      for (auto& vertexBuffer : _delayInfo) {
        result.emplace_back(vertexBuffer->getKind());
      }
    }
    return result;
  }
  return _geometry->getVerticesDataKinds();
}

size_t Mesh::getTotalIndices() const
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalIndices();
}

IndicesArray Mesh::getIndices(bool copyWhenShared, bool forceCopy)
{
  if (!_geometry) {
    return Uint32Array();
  }
  return _geometry->getIndices(copyWhenShared, forceCopy);
}

bool Mesh::get_isBlocked() const
{
  return _masterMesh != nullptr;
}

bool Mesh::isReady(bool completeCheck, bool forceInstanceSupport)
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
    return false;
  }

  if (!AbstractMesh::isReady(completeCheck)) {
    return false;
  }

  if (subMeshes.empty()) {
    return true;
  }

  if (!completeCheck) {
    return true;
  }

  auto engine = getEngine();
  auto scene  = getScene();
  auto hardwareInstancedRendering
    = forceInstanceSupport
      || (engine->getCaps().instancedArrays && instances.size() > 0);

  computeWorldMatrix();

  auto mat = material() ? material() : scene->defaultMaterial();
  if (mat) {
    if (mat->storeEffectOnSubMeshes) {
      for (auto& subMesh : subMeshes) {
        auto effectiveMaterial = subMesh->getMaterial();
        if (effectiveMaterial) {
          if (effectiveMaterial->storeEffectOnSubMeshes) {
            if (!effectiveMaterial->isReadyForSubMesh(
                  this, subMesh.get(), hardwareInstancedRendering)) {
              return false;
            }
          }
          else {
            if (!effectiveMaterial->isReady(this, hardwareInstancedRendering)) {
              return false;
            }
          }
        }
      }
    }
    else {
      if (!mat->isReady(this, hardwareInstancedRendering)) {
        return false;
      }
    }
  }

  // Shadows
  for (auto& light : _lightSources) {
    auto generator = light->getShadowGenerator();

    if (generator) {
      for (auto& subMesh : subMeshes) {
        if (!generator->isReady(subMesh.get(), hardwareInstancedRendering)) {
          return false;
        }
      }
    }
  }

  // LOD
  for (auto& lod : _LODLevels) {
    if (lod->mesh && !lod->mesh->isReady(hardwareInstancedRendering)) {
      return false;
    }
  }

  return true;
}

bool Mesh::get_areNormalsFrozen() const
{
  return _areNormalsFrozen;
}

Mesh& Mesh::freezeNormals()
{
  _areNormalsFrozen = true;
  return *this;
}

Mesh& Mesh::unfreezeNormals()
{
  _areNormalsFrozen = false;
  return *this;
}

void Mesh::setOverridenInstanceCount(size_t count)
{
  _overridenInstanceCount = count;
}

void Mesh::_preActivate()
{
  int sceneRenderId = getScene()->getRenderId();
  if (_preActivateId == sceneRenderId) {
    return;
  }

  _preActivateId    = sceneRenderId;
  _visibleInstances = nullptr;
}

void Mesh::_preActivateForIntermediateRendering(int renderId)
{
  if (_visibleInstances) {
    _visibleInstances->intermediateDefaultRenderId = renderId;
  }
}

Mesh& Mesh::_registerInstanceForRenderId(InstancedMesh* instance, int renderId)
{
  if (!_visibleInstances) {
    _visibleInstances = ::std::make_unique<_VisibleInstances>();
    _visibleInstances->defaultRenderId     = renderId;
    _visibleInstances->selfDefaultRenderId = _renderId;
  }

  if (_visibleInstances->meshes.find(renderId)
      == _visibleInstances->meshes.end()) {
    _visibleInstances->meshes[renderId] = vector_t<InstancedMesh*>();
  }

  _visibleInstances->meshes[renderId].emplace_back(instance);

  return *this;
}

Mesh& Mesh::refreshBoundingInfo()
{
  return _refreshBoundingInfo(false);
}

Mesh& Mesh::_refreshBoundingInfo(bool applySkeleton)
{
  if (_boundingInfo && _boundingInfo->isLocked()) {
    return *this;
  }

  auto data = _getPositionData(applySkeleton);
  if (!data.empty()) {
    auto extend   = Tools::ExtractMinAndMax(data, 0, getTotalVertices());
    _boundingInfo = ::std::make_unique<BoundingInfo>(extend.min, extend.max);
  }

  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->refreshBoundingInfo();
    }
  }

  _updateBoundingInfo();

  return *this;
}

Float32Array Mesh::_getPositionData(bool applySkeleton)
{
  auto data = getVerticesData(VertexBuffer::PositionKind);

  if (!data.empty() && applySkeleton && skeleton()) {
    auto matricesIndicesData
      = getVerticesData(VertexBuffer::MatricesIndicesKind);
    auto matricesWeightsData
      = getVerticesData(VertexBuffer::MatricesWeightsKind);
    if (!matricesWeightsData.empty() && !matricesIndicesData.empty()) {
      auto needExtras = numBoneInfluencers() > 4;
      auto matricesIndicesExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) :
                       Float32Array();
      auto matricesWeightsExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) :
                       Float32Array();

      auto skeletonMatrices = skeleton()->getTransformMatrices(this);

      auto& tempVector  = Tmp::Vector3Array[0];
      auto& finalMatrix = Tmp::MatrixArray[0];
      auto& tempMatrix  = Tmp::MatrixArray[1];

      unsigned int matWeightIdx = 0;
      for (unsigned int index = 0; index < data.size();
           index += 3, matWeightIdx += 4) {
        finalMatrix.reset();

        unsigned int inf = 0;
        float weight     = 0.f;
        for (inf = 0; inf < 4; inf++) {
          weight = matricesWeightsData[matWeightIdx + inf];
          if (weight > 0) {
            Matrix::FromFloat32ArrayToRefScaled(
              skeletonMatrices,
              static_cast<unsigned int>(
                ::std::floor(matricesIndicesData[matWeightIdx + inf] * 16)),
              weight, tempMatrix);
            finalMatrix.addToSelf(tempMatrix);
          }
        }
        if (needExtras) {
          for (inf = 0; inf < 4; inf++) {
            weight = matricesWeightsExtraData[matWeightIdx + inf];
            if (weight > 0) {
              Matrix::FromFloat32ArrayToRefScaled(
                skeletonMatrices,
                static_cast<unsigned int>(::std::floor(
                  matricesIndicesExtraData[matWeightIdx + inf] * 16)),
                weight, tempMatrix);
              finalMatrix.addToSelf(tempMatrix);
            }
          }
        }

        Vector3::TransformCoordinatesFromFloatsToRef(
          data[index], data[index + 1], data[index + 2], finalMatrix,
          tempVector);
        tempVector.toArray(data, index);
      }
    }
  }

  return data;
}

shared_ptr_t<SubMesh> Mesh::_createGlobalSubMesh(bool force)
{
  auto totalVertices = getTotalVertices();
  if (!totalVertices || (!_geometry->isReady() && getIndices().empty())) {
    return nullptr;
  }

  // Check if we need to recreate the submeshes
  if (!subMeshes.empty()) {
    auto ib = getIndices();

    if (ib.empty()) {
      return nullptr;
    }

    const auto totalIndices = ib.size();
    bool needToRecreate     = false;

    if (force) {
      needToRecreate = true;
    }
    else {
      for (auto& submesh : subMeshes) {
        if (submesh->indexStart + submesh->indexCount >= totalIndices) {
          needToRecreate = true;
          break;
        }

        if (submesh->verticesStart + submesh->verticesCount >= totalVertices) {
          needToRecreate = true;
          break;
        }
      }
    }

    if (!needToRecreate) {
      return subMeshes[0];
    }
  }

  releaseSubMeshes();
  return SubMesh::New(0, 0, totalVertices, 0, getTotalIndices(),
                      shared_from_base<Mesh>());
}

void Mesh::subdivide(size_t count)
{
  if (count < 1) {
    return;
  }

  size_t totalIndices    = getTotalIndices();
  size_t subdivisionSize = (totalIndices / count);
  size_t offset          = 0;

  // Ensure that subdivisionSize is a multiple of 3
  while (subdivisionSize % 3 != 0) {
    ++subdivisionSize;
  }

  releaseSubMeshes();
  for (unsigned int index = 0; index < count; ++index) {
    if (offset >= totalIndices) {
      break;
    }

    SubMesh::CreateFromIndices(
      0, static_cast<unsigned>(offset),
      ::std::min(subdivisionSize, totalIndices - offset),
      shared_from_base<Mesh>());

    offset += subdivisionSize;
  }

  synchronizeInstances();
}

Mesh* Mesh::setVerticesData(unsigned int kind, const Float32Array& data,
                            bool updatable, const nullable_t<size_t>& stride)
{
  if (!_geometry) {
    auto vertexData = ::std::make_unique<VertexData>();
    vertexData->set(data, kind);

    auto scene = getScene();

    Geometry::New(Geometry::RandomId(), scene, vertexData.get(), updatable,
                  this);
  }
  else {
    _geometry->setVerticesData(kind, data, updatable, stride);
  }

  return this;
}

void Mesh::markVerticesDataAsUpdatable(unsigned int kind, bool updatable)
{
  auto vb = getVertexBuffer(kind);

  if (!vb || vb->isUpdatable() == updatable) {
    return;
  }

  setVerticesData(kind, getVerticesData(kind), updatable);
}

Mesh& Mesh::setVerticesBuffer(unique_ptr_t<VertexBuffer>&& buffer)
{
  if (!_geometry) {
    _geometry = Geometry::CreateGeometryForMesh(this).get();
  }

  _geometry->setVerticesBuffer(::std::move(buffer));

  return *this;
}

Mesh* Mesh::updateVerticesData(unsigned int kind, const Float32Array& data,
                               bool updateExtends, bool makeItUnique)
{
  if (!_geometry) {
    return this;
  }
  if (!makeItUnique) {
    _geometry->updateVerticesData(kind, data, updateExtends);
  }
  else {
    makeGeometryUnique();
    updateVerticesData(kind, data, updateExtends, false);
  }

  return this;
}

Mesh& Mesh::updateMeshPositions(
  ::std::function<void(Float32Array& positions)> positionFunction,
  bool computeNormals)
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  if (positions.empty()) {
    return *this;
  }

  positionFunction(positions);
  updateVerticesData(VertexBuffer::PositionKind, positions, false, false);

  if (computeNormals) {
    auto indices = getIndices();
    auto normals = getVerticesData(VertexBuffer::NormalKind);

    if (normals.empty()) {
      return *this;
    }

    VertexData::ComputeNormals(positions, indices, normals);
    updateVerticesData(VertexBuffer::NormalKind, normals, false, false);
  }
  return *this;
}

Mesh& Mesh::makeGeometryUnique()
{
  if (!_geometry) {
    return *this;
  }
  auto oldGeometry = _geometry;
  auto geometry_   = _geometry->copy(Geometry::RandomId());
  oldGeometry->releaseForMesh(this, true);
  geometry_->applyToMesh(this);

  return *this;
}

Mesh* Mesh::setIndices(const IndicesArray& indices, size_t totalVertices,
                       bool updatable)
{
  if (!_geometry) {
    auto vertexData     = ::std::make_unique<VertexData>();
    vertexData->indices = indices;

    auto scene = getScene();

    Geometry(Geometry::RandomId(), scene, vertexData.get(), updatable, this);
  }
  else {
    _geometry->setIndices(indices, totalVertices, updatable);
  }

  return this;
}

Mesh& Mesh::updateIndices(const IndicesArray& indices, int offset)
{
  if (!_geometry) {
    return *this;
  }

  _geometry->updateIndices(indices, offset);
  return *this;
}

Mesh& Mesh::toLeftHanded()
{
  if (!_geometry) {
    return *this;
  }

  _geometry->toLeftHanded();

  return *this;
}

void Mesh::_bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode)
{
  if (!_geometry) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Wireframe
  GL::IGLBuffer* indexToBind;

  if (_unIndexed) {
    indexToBind = nullptr;
  }
  else {
    switch (fillMode) {
      case Material::PointFillMode():
        indexToBind = nullptr;
        break;
      case Material::WireFrameFillMode():
        indexToBind = subMesh->getLinesIndexBuffer(getIndices(), engine);
        break;
      default:
      case Material::TriangleFillMode():
        indexToBind = _unIndexed ? nullptr : _geometry->getIndexBuffer();
        break;
    }
  }

  // VBOs
  _geometry->_bind(effect, indexToBind);
}

void Mesh::_draw(SubMesh* subMesh, int fillMode, size_t instancesCount,
                 bool alternate)
{
  if (!_geometry || !_geometry->getVertexBuffers().size()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return;
  }

  onBeforeDrawObservable().notifyObservers(this);

  auto scene  = getScene();
  auto engine = scene->getEngine();

  auto _fillMode = static_cast<unsigned>(fillMode);
  if (_unIndexed || fillMode == Material::PointFillMode()) {
    // or triangles as points
    engine->drawArraysType(_fillMode, static_cast<int>(subMesh->verticesStart),
                           static_cast<int>(subMesh->verticesCount),
                           static_cast<int>(instancesCount));
  }
  else if (fillMode == Material::WireFrameFillMode()) {
    // Triangles as wireframe
    engine->drawElementsType(_fillMode, 0,
                             static_cast<int>(subMesh->linesIndexCount),
                             static_cast<int>(instancesCount));
  }
  else {
    engine->drawElementsType(_fillMode, static_cast<int>(subMesh->indexStart),
                             static_cast<int>(subMesh->indexCount),
                             static_cast<int>(instancesCount));
  }

  if (scene->_isAlternateRenderingEnabled() && !alternate) {
    auto effect
      = subMesh->effect() ? subMesh->effect() : _effectiveMaterial->getEffect();
    if (!effect || !scene->activeCamera) {
      return;
    }
    scene->_switchToAlternateCameraConfiguration(true);
    _effectiveMaterial->bindView(effect);
    _effectiveMaterial->bindViewProjection(effect);

    engine->setViewport(scene->activeCamera->_alternateCamera->viewport);
    _draw(subMesh, fillMode, instancesCount, true);
    engine->setViewport(scene->activeCamera->viewport);

    scene->_switchToAlternateCameraConfiguration(false);
    _effectiveMaterial->bindView(effect);
    _effectiveMaterial->bindViewProjection(effect);
  }
}

Mesh& Mesh::registerBeforeRender(
  const ::std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterBeforeRender(
  const ::std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().removeCallback(func);
  return *this;
}

Mesh& Mesh::registerAfterRender(
  const ::std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterAfterRender(
  const ::std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().removeCallback(func);
  return *this;
}

_InstancesBatch* Mesh::_getInstancesRenderList(size_t subMeshId)
{
  auto scene                               = getScene();
  _batchCache->mustReturn                  = false;
  _batchCache->renderSelf[subMeshId]       = isEnabled() && isVisible;
  _batchCache->visibleInstances[subMeshId] = vector_t<InstancedMesh*>();

  if (_visibleInstances) {
    auto currentRenderId_ = scene->getRenderId();
    auto defaultRenderId  = scene->_isInIntermediateRendering() ?
                             _visibleInstances->intermediateDefaultRenderId :
                             _visibleInstances->defaultRenderId;
    _batchCache->visibleInstances[subMeshId]
      = _visibleInstances->meshes[currentRenderId_];
    int selfRenderId = _renderId;

    if ((_batchCache->visibleInstances.find(subMeshId)
         == _batchCache->visibleInstances.end())
        && defaultRenderId) {
      _batchCache->visibleInstances[subMeshId]
        = _visibleInstances->meshes[defaultRenderId];
      currentRenderId_ = ::std::max(defaultRenderId, currentRenderId_);
      selfRenderId
        = ::std::max(_visibleInstances->selfDefaultRenderId, currentRenderId_);
    }

    if ((_batchCache->visibleInstances.find(subMeshId)
         != _batchCache->visibleInstances.end())
        && _batchCache->visibleInstances[subMeshId].size() > 0) {
      if (subMeshId < _renderIdForInstances.size()
          && _renderIdForInstances[subMeshId] == currentRenderId_) {
        _batchCache->mustReturn = true;
        return _batchCache.get();
      }

      if (currentRenderId_ != selfRenderId) {
        _batchCache->renderSelf[subMeshId] = false;
      }
    }
    if (subMeshId >= _renderIdForInstances.size()) {
      _renderIdForInstances.resize(subMeshId + 1);
    }
    _renderIdForInstances[subMeshId] = currentRenderId_;
  }

  return _batchCache.get();
}

Mesh& Mesh::_renderWithInstances(SubMesh* subMesh, unsigned int fillMode,
                                 _InstancesBatch* batch, Effect* effect,
                                 Engine* engine)
{
  if (batch->visibleInstances.find(subMesh->_id)
      == batch->visibleInstances.end()) {
    return *this;
  }

  auto& visibleInstances = batch->visibleInstances[subMesh->_id];
  if (visibleInstances.empty()) {
    return *this;
  }

  size_t matricesCount = visibleInstances.size() + 1;
  size_t bufferSize    = matricesCount * 16 * 4;

  auto currentInstancesBufferSize = _instancesBufferSize;

  while (_instancesBufferSize < bufferSize) {
    _instancesBufferSize *= 2;
  }

  if (_instancesData.empty()
      || currentInstancesBufferSize != _instancesBufferSize) {
    _instancesData = Float32Array(_instancesBufferSize / 4);
  }

  unsigned int offset         = 0;
  unsigned int instancesCount = 0;

  auto world = getWorldMatrix();
  if (batch->renderSelf[subMesh->_id]) {
    world->copyToArray(_instancesData, offset);
    offset += 16;
    instancesCount++;
  }

  if (!visibleInstances.empty()) {
    for (size_t instanceIndex = 0; instanceIndex < visibleInstances.size();
         ++instanceIndex) {
      InstancedMesh* instance = visibleInstances[instanceIndex];
      instance->getWorldMatrix()->copyToArray(_instancesData, offset);
      offset += 16;
      ++instancesCount;
    }
  }

  if (!_instancesBuffer || currentInstancesBufferSize != _instancesBufferSize) {
    if (_instancesBuffer) {
      _instancesBuffer->dispose();
    }

    _instancesBuffer = ::std::make_unique<Buffer>(engine, _instancesData, true,
                                                  16, false, true);

    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World0Kind, 0, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World1Kind, 4, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World2Kind, 8, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World3Kind, 12, 4));
  }
  else {
    _instancesBuffer->updateDirectly(_instancesData, 0, instancesCount);
  }

  _bind(subMesh, effect, fillMode);

  _draw(subMesh, static_cast<int>(fillMode), instancesCount);

  engine->unbindInstanceAttributes();

  return *this;
}

Mesh& Mesh::_processRendering(
  SubMesh* subMesh, Effect* effect, int fillMode, _InstancesBatch* batch,
  bool hardwareInstancedRendering,
  ::std::function<void(bool isInstance, const Matrix& world,
                       Material* effectiveMaterial)>
    onBeforeDraw,
  Material* effectiveMaterial)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (hardwareInstancedRendering) {
    _renderWithInstances(subMesh, static_cast<unsigned>(fillMode), batch,
                         effect, engine);
  }
  else {
    if (batch->renderSelf[subMesh->_id]) {
      // Draw
      if (onBeforeDraw) {
        onBeforeDraw(false, *getWorldMatrix(), effectiveMaterial);
      }

      _draw(subMesh, fillMode, _overridenInstanceCount);
    }

    auto& visibleInstancesForSubMesh = batch->visibleInstances[subMesh->_id];
    if (!visibleInstancesForSubMesh.empty()) {
      for (auto& instance : visibleInstancesForSubMesh) {
        // World
        auto world = instance->getWorldMatrix();
        if (onBeforeDraw) {
          onBeforeDraw(true, *world, effectiveMaterial);
        }

        // Draw
        _draw(subMesh, fillMode);
      }
    }
  }

  return *this;
}

Mesh& Mesh::render(SubMesh* subMesh, bool enableAlphaMode)
{
  _checkOcclusionQuery();
  if (_isOccluded) {
    return *this;
  }

  auto scene = getScene();

  // Managing instances
  auto batch = _getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return *this;
  }

  // Checking geometry state
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return *this;
  }

  onBeforeRenderObservable().notifyObservers(this);

  auto engine = scene->getEngine();
  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  // Material
  auto material = subMesh->getMaterial();

  if (!material) {
    return *this;
  }

  _effectiveMaterial = material;

  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    if (!_effectiveMaterial->isReadyForSubMesh(this, subMesh,
                                               hardwareInstancedRendering)) {
      return *this;
    }
  }
  else if (!_effectiveMaterial->isReady(this, hardwareInstancedRendering)) {
    return *this;
  }

  // Alpha mode
  if (enableAlphaMode) {
    engine->setAlphaMode(
      static_cast<unsigned>(_effectiveMaterial->alphaMode()));
  }

  // Outline - step 1
  auto savedDepthWrite = engine->getDepthWrite();
  if (renderOutline) {
    engine->setDepthWrite(false);
    scene->getOutlineRenderer()->render(subMesh, batch);
    engine->setDepthWrite(savedDepthWrite);
  }

  Effect* effect = nullptr;
  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    effect = subMesh->effect();
  }
  else {
    effect = _effectiveMaterial->getEffect();
  }

  if (!effect) {
    return *this;
  }

  auto sideOrientation = overrideMaterialSideOrientation;
  if (!sideOrientation.has_value()) {
    sideOrientation
      = static_cast<unsigned>(_effectiveMaterial->sideOrientation);
    if (_getWorldMatrixDeterminant() < 0) {
      sideOrientation
        = (sideOrientation == Material::ClockWiseSideOrientation() ?
             Material::CounterClockWiseSideOrientation() :
             Material::ClockWiseSideOrientation());
    }
  }

  auto reverse = _effectiveMaterial->_preBind(effect, *sideOrientation);

  if (_effectiveMaterial->forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  // Bind
  auto fillMode = scene->forcePointsCloud() ?
                    Material::PointFillMode() :
                    (scene->forceWireframe() ? Material::WireFrameFillMode() :
                                               _effectiveMaterial->fillMode());

  // Binding will be done later because we need to add more info to the VB
  if (!hardwareInstancedRendering) {
    _bind(subMesh, effect, fillMode);
  }

  auto _world = getWorldMatrix();

  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    _effectiveMaterial->bindForSubMesh(_world, this, subMesh);
  }
  else {
    _effectiveMaterial->bind(_world, this);
  }

  if (!_effectiveMaterial->backFaceCulling()
      && _effectiveMaterial->separateCullingPass) {
    engine->setState(true, _effectiveMaterial->zOffset, false, !reverse);
    _processRendering(
      subMesh, effect, static_cast<int>(fillMode), batch,
      hardwareInstancedRendering,
      [&](bool isInstance, Matrix world, Material* _effectiveMaterial) {
        _onBeforeDraw(isInstance, world, _effectiveMaterial);
      },
      _effectiveMaterial.get());
    engine->setState(true, _effectiveMaterial->zOffset, false, reverse);
  }

  // Draw
  _processRendering(
    subMesh, effect, static_cast<int>(fillMode), batch,
    hardwareInstancedRendering,
    [&](bool isInstance, Matrix world, Material* _effectiveMaterial) {
      _onBeforeDraw(isInstance, world, _effectiveMaterial);
    });

  // Unbind
  _effectiveMaterial->unbind();

  // Outline - step 2
  if (renderOutline && savedDepthWrite) {
    engine->setDepthWrite(true);
    engine->setColorWrite(false);
    scene->getOutlineRenderer()->render(subMesh, batch);
    engine->setColorWrite(true);
  }

  // Overlay
  if (renderOverlay) {
    auto currentMode = engine->getAlphaMode();
    engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
    scene->getOutlineRenderer()->render(subMesh, batch, true);
    engine->setAlphaMode(currentMode);
  }

  onAfterRenderObservable().notifyObservers(this);

  return *this;
}

Mesh& Mesh::_onBeforeDraw(bool isInstance, Matrix& world,
                          Material* effectiveMaterial)
{
  if (isInstance && effectiveMaterial) {
    effectiveMaterial->bindOnlyWorldMatrix(world);
  }
  return *this;
}

vector_t<IParticleSystemPtr> Mesh::getEmittedParticleSystems()
{
  vector_t<IParticleSystemPtr> results;
  for (auto& particleSystem : getScene()->particleSystems) {
    auto& emitter = particleSystem->emitter;
    if (emitter.is<AbstractMeshPtr>()
        && (emitter.get<AbstractMeshPtr>().get() == this)) {
      results.emplace_back(particleSystem);
    }
  }

  return results;
}

vector_t<IParticleSystemPtr> Mesh::getHierarchyEmittedParticleSystems()
{
  vector_t<IParticleSystemPtr> results;
  auto descendants = getDescendants();
  descendants.emplace_back(this);

  for (auto& particleSystem : getScene()->particleSystems) {
    if (particleSystem->emitter.is<AbstractMeshPtr>()) {
      if (::std::find(descendants.begin(), descendants.end(),
                      particleSystem->emitter.get<AbstractMeshPtr>())
          != descendants.end()) {
        results.emplace_back(particleSystem.get());
      }
    }
  }

  return results;
}

void Mesh::cleanMatrixWeights()
{
  const float epsilon = 1e-3f;

  size_t noInfluenceBoneIndex = 0;
  if (skeleton()) {
    noInfluenceBoneIndex = skeleton()->bones.size();
  }
  else {
    return;
  }

  auto matricesIndices = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesIndicesExtra
    = getVerticesData(VertexBuffer::MatricesIndicesExtraKind);
  auto matricesWeights = getVerticesData(VertexBuffer::MatricesWeightsKind);
  auto matricesWeightsExtra
    = getVerticesData(VertexBuffer::MatricesWeightsExtraKind);
  auto influencers = static_cast<int>(numBoneInfluencers());
  auto size        = matricesWeights.size();

  for (size_t i = 0; i < size; i += 4) {
    float weight        = 0.f;
    int firstZeroWeight = -1;
    for (size_t j = 0; j < 4; j++) {
      auto w = matricesWeights[i + j];
      weight += w;
      if (w < epsilon && firstZeroWeight < 0) {
        firstZeroWeight = static_cast<int>(j);
      }
    }
    if (!matricesWeightsExtra.empty()) {
      for (size_t j = 0; j < 4; j++) {
        auto w = matricesWeightsExtra[i + j];
        weight += w;
        if (w < epsilon && firstZeroWeight < 0) {
          firstZeroWeight = static_cast<int>(j) + 4;
        }
      }
    }
    if (firstZeroWeight < 0 || firstZeroWeight > (influencers - 1)) {
      firstZeroWeight = influencers - 1;
    }
    if (weight > epsilon) {
      auto mweight = 1.f / weight;
      for (size_t j = 0; j < 4; j++) {
        matricesWeights[i + j] *= mweight;
      }
      if (!matricesWeightsExtra.empty()) {
        for (size_t j = 0; j < 4; j++) {
          matricesWeightsExtra[i + j] *= mweight;
        }
      }
    }
    else {
      auto _firstZeroWeight = static_cast<size_t>(firstZeroWeight);
      if (firstZeroWeight >= 4) {
        matricesWeightsExtra[i + _firstZeroWeight - 4] = 1.f - weight;
        matricesIndicesExtra[i + _firstZeroWeight - 4] = noInfluenceBoneIndex;
      }
      else {
        matricesWeights[i + _firstZeroWeight] = 1.f - weight;
        matricesIndices[i + _firstZeroWeight] = noInfluenceBoneIndex;
      }
    }
  }

  setVerticesData(VertexBuffer::MatricesIndicesKind, matricesIndices);
  if (!matricesIndicesExtra.empty()) {
    setVerticesData(VertexBuffer::MatricesIndicesExtraKind,
                    matricesIndicesExtra);
  }

  setVerticesData(VertexBuffer::MatricesWeightsKind, matricesWeights);
  if (!matricesWeightsExtra.empty()) {
    setVerticesData(VertexBuffer::MatricesWeightsExtraKind,
                    matricesWeightsExtra);
  }
}

vector_t<NodePtr> Mesh::getChildren()
{
  vector_t<NodePtr> results;
  for (auto& mesh : getScene()->meshes) {
    if (mesh->parent() == this) {
      results.emplace_back(mesh);
    }
  }

  return results;
}

Mesh& Mesh::_checkDelayState()
{
  auto scene = getScene();

  if (_geometry) {
    _geometry->load(scene);
  }
  else if (delayLoadState == EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    delayLoadState = EngineConstants::DELAYLOADSTATE_LOADING;

    _queueLoad(scene);
  }

  return *this;
}

Mesh& Mesh::_queueLoad(Scene* scene)
{
  scene->_addPendingData(this);
  return *this;
}

bool Mesh::isInFrustum(const array_t<Plane, 6>& frustumPlanes,
                       unsigned int /*strategy*/)
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
    return false;
  }

  if (!AbstractMesh::isInFrustum(frustumPlanes)) {
    return false;
  }

  _checkDelayState();

  return true;
}

Mesh& Mesh::setMaterialByID(const string_t& iId)
{
  const auto& materials = getScene()->materials;
  for (std::size_t index = materials.size(); index-- > 0;) {
    if (materials[index]->id == iId) {
      material = materials[index];
      return *this;
    }
  }

  // Multi
  const auto& multiMaterials = getScene()->multiMaterials;
  for (std::size_t index = multiMaterials.size(); index-- > 0;) {
    if (multiMaterials[index]->id == iId) {
      material = multiMaterials[index];
      return *this;
    }
  }

  return *this;
}

vector_t<IAnimatablePtr> Mesh::getAnimatables()
{
  vector_t<IAnimatablePtr> results;

  if (material()) {
    results.emplace_back(material());
  }

  if (skeleton()) {
    results.emplace_back(skeleton());
  }

  return results;
}

// Geometry
Mesh& Mesh::bakeTransformIntoVertices(const Matrix& transform)
{
  // Position
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)) {
    return *this;
  }

  auto _submeshes = ::std::move(subMeshes);

  _resetPointsArrayCache();

  auto data = getVerticesData(VertexBuffer::PositionKind);
  Float32Array temp;
  unsigned int index;
  for (index = 0; index < data.size(); index += 3) {
    Vector3::TransformCoordinates(Vector3::FromArray(data, index), transform)
      .toArray(temp, index);
  }

  setVerticesData(VertexBuffer::PositionKind, temp,
                  getVertexBuffer(VertexBuffer::PositionKind)->isUpdatable());

  // Normals
  if (isVerticesDataPresent(VertexBuffer::NormalKind)) {
    data = getVerticesData(VertexBuffer::NormalKind);
    temp.clear();
    for (index = 0; index < data.size(); index += 3) {
      Vector3::TransformNormal(Vector3::FromArray(data, index), transform)
        .normalize()
        .toArray(temp, index);
    }
    setVerticesData(VertexBuffer::NormalKind, temp,
                    getVertexBuffer(VertexBuffer::NormalKind)->isUpdatable());
  }

  // flip faces?
  if (transform.m[0] * transform.m[5] * transform.m[10] < 0) {
    flipFaces();
  }

  // Restore submeshes
  releaseSubMeshes();
  subMeshes = ::std::move(_submeshes);

  return *this;
}

// Will apply current transform to mesh and reset world matrix
Mesh& Mesh::bakeCurrentTransformIntoVertices()
{
  bakeTransformIntoVertices(computeWorldMatrix(true));
  scaling().copyFromFloats(1.f, 1.f, 1.f);
  position().copyFromFloats(0.f, 0.f, 0.f);
  rotation().copyFromFloats(0.f, 0.f, 0.f);
  // only if quaternion is already set
  if (rotationQuaternion()) {
    rotationQuaternion = Quaternion::Identity();
  }
  _worldMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
  return *this;
}

// Cache
vector_t<Vector3>& Mesh::get__positions()
{
  if (_geometry) {
    return _geometry->_positions;
  }
  return _emptyPositions;
}

Mesh& Mesh::_resetPointsArrayCache()
{
  if (_geometry) {
    _geometry->_resetPointsArrayCache();
  }
  return *this;
}

bool Mesh::_generatePointsArray()
{
  if (_geometry) {
    return _geometry->_generatePointsArray();
  }

  return false;
}

MeshPtr Mesh::clone(const string_t& iName, Node* newParent,
                    bool doNotCloneChildren, bool clonePhysicsImpostor)
{
  return Mesh::New(iName, getScene(), newParent, this, doNotCloneChildren,
                   clonePhysicsImpostor);
}

void Mesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  morphTargetManager = nullptr;

  if (_geometry) {
    _geometry->releaseForMesh(this, true);
  }

  onBeforeDrawObservable().clear();
  onBeforeRenderObservable().clear();
  onAfterRenderObservable().clear();

  // Sources
  for (auto& mesh : getScene()->meshes) {
    if (mesh->type() == IReflect::Type::MESH) {
      auto _mesh = static_cast<Mesh*>(mesh.get());
      if (_mesh->_source && _mesh->_source == this) {
        _mesh->_source = nullptr;
      }
    }
  }
  _source = nullptr;

  // Instances
  if (_instancesBuffer) {
    _instancesBuffer->dispose();
    _instancesBuffer.reset(nullptr);
  }

  for (auto& instance : instances) {
    instance->dispose();
  }

  AbstractMesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

void Mesh::applyDisplacementMap(
  const string_t& url, int minHeight, int maxHeight,
  const ::std::function<void(Mesh* mesh)> onSuccess,
  const nullable_t<Vector2>& /*uvOffset*/,
  const nullable_t<Vector2>& /*uvScale*/, bool /*boolforceUpdate*/)
{
  ::std::cout << url << minHeight << maxHeight;
  onSuccess(nullptr);
}

void Mesh::applyDisplacementMapFromBuffer(const Uint8Array& buffer,
                                          unsigned int heightMapWidth,
                                          unsigned int heightMapHeight,
                                          int minHeight, int maxHeight,
                                          const nullable_t<Vector2>& iUvOffset,
                                          const nullable_t<Vector2>& iUvScale,
                                          bool forceUpdate)
{
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)
      || !isVerticesDataPresent(VertexBuffer::NormalKind)
      || !isVerticesDataPresent(VertexBuffer::UVKind)) {
    BABYLON_LOG_WARN("Mesh",
                     "Cannot call applyDisplacementMap: Given mesh is not "
                     "complete. Position, Normal or UV are missing.");
    return;
  }

  auto positions = getVerticesData(VertexBuffer::PositionKind, true, true);
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto uvs       = getVerticesData(VertexBuffer::UVKind);
  auto position  = Vector3::Zero();
  auto normal    = Vector3::Zero();
  auto uv        = Vector2::Zero();

  Vector2 uvOffset = iUvOffset.has_value() ? *iUvOffset : Vector2::Zero();
  Vector2 uvScale  = iUvScale.has_value() ? *iUvScale : Vector2(1.f, 1.f);

  for (unsigned int index = 0; index < positions.size(); index += 3) {
    Vector3::FromArrayToRef(positions, index, position);
    Vector3::FromArrayToRef(normals, index, normal);
    Vector2::FromArrayToRef(uvs, (index / 3) * 2, uv);

    // Compute height
    auto u
      = (static_cast<unsigned int>(::std::abs(uv.x * uvScale.x + uvOffset.x))
         * heightMapWidth)
        % heightMapWidth;
    auto v
      = (static_cast<unsigned int>(::std::abs(uv.y * uvScale.y + uvOffset.y))
         * heightMapHeight)
        % heightMapHeight;

    auto pos = (u + v * heightMapWidth) * 4;
    auto r   = buffer[pos] / 255.f;
    auto g   = buffer[pos + 1] / 255.f;
    auto b   = buffer[pos + 2] / 255.f;

    auto gradient = r * 0.3f + g * 0.59f + b * 0.11f;

    normal.normalize();
    auto scale = static_cast<float>(minHeight)
                 + static_cast<float>(maxHeight - minHeight) * gradient;
    normal.scaleInPlace(scale);
    position = position.add(normal);

    position.toArray(positions, index);
  }

  VertexData::ComputeNormals(positions, getIndices(), normals);

  if (forceUpdate) {
    setVerticesData(VertexBuffer::PositionKind, positions);
    setVerticesData(VertexBuffer::NormalKind, normals);
  }
  else {
    updateVerticesData(VertexBuffer::PositionKind, positions);
    updateVerticesData(VertexBuffer::NormalKind, normals);
  }
}

Mesh& Mesh::convertToFlatShadedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::map<unsigned int, VertexBuffer*> vbs;
  std::map<unsigned int, Float32Array> data;
  std::map<unsigned int, Float32Array> newdata;
  bool updatableNormals = false;
  unsigned int kindIndex;
  unsigned int kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind              = kinds[kindIndex];
    auto vertexBuffer = getVertexBuffer(kind);

    if (kind == VertexBuffer::NormalKind) {
      updatableNormals = vertexBuffer->isUpdatable();
      kinds.erase(kinds.begin() + kindIndex);
      --kindIndex;
      continue;
    }

    vbs[kind]     = vertexBuffer;
    data[kind]    = vbs[kind]->getData();
    newdata[kind] = Float32Array();
  }

  // Save previous submeshes
  auto previousSubmeshes = stl_util::to_raw_ptr_vector(subMeshes);

  auto indices      = getIndices();
  auto totalIndices = getTotalIndices();

  // Generating unique vertices per face
  unsigned int index;
  for (index = 0; index < totalIndices; ++index) {
    auto vertexIndex = static_cast<unsigned int>(indices[index]);

    for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
      kind          = kinds[kindIndex];
      size_t stride = static_cast<size_t>(vbs[kind]->getStrideSize());

      for (unsigned int offset = 0; offset < stride; ++offset) {
        newdata[kind].emplace_back(data[kind][vertexIndex * stride + offset]);
      }
    }
  }

  // Updating faces & normal
  Float32Array normals;
  auto& positions = newdata[VertexBuffer::PositionKind];
  for (index = 0; index < totalIndices; index += 3) {
    indices[index]     = static_cast<uint16_t>(index);
    indices[index + 1] = static_cast<uint16_t>(index + 1);
    indices[index + 2] = static_cast<uint16_t>(index + 2);

    auto p1 = Vector3::FromArray(positions, index * 3);
    auto p2 = Vector3::FromArray(positions, (index + 1) * 3);
    auto p3 = Vector3::FromArray(positions, (index + 2) * 3);

    auto p1p2 = p1.subtract(p2);
    auto p3p2 = p3.subtract(p2);

    auto normal = Vector3::Normalize(Vector3::Cross(p1p2, p3p2));

    // Store same normals for every vertex
    for (unsigned int localIndex = 0; localIndex < 3; ++localIndex) {
      normals.emplace_back(normal.x);
      normals.emplace_back(normal.y);
      normals.emplace_back(normal.z);
    }
  }

  setIndices(indices);
  setVerticesData(VertexBuffer::NormalKind, normals, updatableNormals);

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating submeshes
  releaseSubMeshes();
  for (const auto& previousOne : previousSubmeshes) {
    SubMesh::AddToMesh(previousOne->materialIndex,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, shared_from_base<Mesh>());
  }

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::convertToUnIndexedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::map<unsigned int, VertexBuffer*> vbs;
  std::map<unsigned int, Float32Array> data;
  std::map<unsigned int, Float32Array> newdata;
  unsigned int kindIndex;
  unsigned int kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind          = kinds[kindIndex];
    vbs[kind]     = getVertexBuffer(kind);
    data[kind]    = vbs[kind]->getData();
    newdata[kind] = Float32Array();
  }

  // Save previous submeshes
  auto previousSubmeshes = stl_util::to_raw_ptr_vector(subMeshes);

  auto indices      = getIndices();
  auto totalIndices = getTotalIndices();

  // Generating unique vertices per face
  unsigned int index;
  for (index = 0; index < totalIndices; ++index) {
    auto vertexIndex = static_cast<unsigned int>(indices[index]);

    for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
      kind          = kinds[kindIndex];
      size_t stride = static_cast<size_t>(vbs[kind]->getStrideSize());

      for (unsigned int offset = 0; offset < stride; ++offset) {
        newdata[kind].emplace_back(data[kind][vertexIndex * stride + offset]);
      }
    }
  }

  // Updating indices
  for (index = 0; index < totalIndices; index += 3) {
    indices[index + 0] = static_cast<uint16_t>(index + 0);
    indices[index + 1] = static_cast<uint16_t>(index + 1);
    indices[index + 2] = static_cast<uint16_t>(index + 2);
  }

  setIndices(indices);

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating submeshes
  releaseSubMeshes();
  for (const auto& previousOne : previousSubmeshes) {
    SubMesh::AddToMesh(previousOne->materialIndex,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, shared_from_base<Mesh>());
  }

  _unIndexed = true;

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::flipFaces(bool flipNormals)
{
  auto vertex_data = VertexData::ExtractFromMesh(this);
  unsigned int i;
  if (flipNormals && isVerticesDataPresent(VertexBuffer::NormalKind)
      && !vertex_data->normals.empty()) {
    for (i = 0; i < vertex_data->normals.size(); ++i) {
      vertex_data->normals[i] *= -1;
    }
  }

  if (!vertex_data->indices.empty()) {
    for (i = 0; i < vertex_data->indices.size(); i += 3) {
      // reassign indices
      ::std::swap(vertex_data->indices[i + 1], vertex_data->indices[i + 2]);
    }
  }

  vertex_data->applyToMesh(*this);
  return *this;
}

InstancedMeshPtr Mesh::createInstance(const string_t& iName)
{
  return InstancedMesh::New(iName, this);
}

Mesh& Mesh::synchronizeInstances()
{
  for (auto& instance : instances) {
    instance->_syncSubMeshes();
  }
  return *this;
}

void Mesh::optimizeIndices(
  const ::std::function<void(Mesh* mesh)>& successCallback)
{
  successCallback(nullptr);
}

void Mesh::_syncGeometryWithMorphTargetManager()
{
  auto _geometry = geometry();
  if (!_geometry) {
    return;
  }

  _markSubMeshesAsAttributesDirty();

  auto morphTargetManager = _morphTargetManager;
  if (morphTargetManager && morphTargetManager->vertexCount()) {
    if (morphTargetManager->vertexCount() != getTotalVertices()) {
      BABYLON_LOG_ERROR("Mesh",
                        "Mesh is incompatible with morph targets. Targets and "
                        "mesh must all have the same vertices count.");
      morphTargetManager = nullptr;
      return;
    }

    for (unsigned int index = 0; index < morphTargetManager->numInfluencers();
         ++index) {
      auto morphTarget = morphTargetManager->getActiveTarget(index);

      const auto positions = morphTarget->getPositions();
      if (positions.empty()) {
        BABYLON_LOG_ERROR("Mesh",
                          "Invalid morph target. Target must have positions.");
        return;
      }

      _geometry->setVerticesData(VertexBuffer::PositionKind + index, positions,
                                 false, 3);

      const auto normals = morphTarget->getNormals();
      if (!normals.empty()) {
        _geometry->setVerticesData(VertexBuffer::NormalKind + index, normals,
                                   false, 3);
      }

      const auto tangents = morphTarget->getTangents();
      if (!tangents.empty()) {
        _geometry->setVerticesData(VertexBuffer::TangentKind + index, tangents,
                                   false, 3);
      }
    }
  }
  else {
    unsigned int index = 0;

    // Positions
    while (
      _geometry->isVerticesDataPresent(VertexBuffer::PositionKind + index)) {
      _geometry->removeVerticesData(VertexBuffer::PositionKind + index);

      if (_geometry->isVerticesDataPresent(VertexBuffer::NormalKind + index)) {
        _geometry->removeVerticesData(VertexBuffer::NormalKind + index);
      }
      if (_geometry->isVerticesDataPresent(VertexBuffer::TangentKind + index)) {
        _geometry->removeVerticesData(VertexBuffer::TangentKind + index);
      }
      ++index;
    }
  }
}

MeshPtr Mesh::Parse(const Json::value& parsedMesh, Scene* scene,
                    const string_t& rootUrl)
{
  MeshPtr mesh = nullptr;
  if (Json::GetString(parsedMesh, "type") == "GroundMesh") {
    mesh = GroundMesh::Parse(parsedMesh, scene);
  }
  else {
    mesh = Mesh::New(Json::GetString(parsedMesh, "name"), scene);
  }
  mesh->id = Json::GetString(parsedMesh, "id");

  // Tags.AddTagsTo(mesh, parsedMesh.tags);

  if (parsedMesh.contains("position")) {
    mesh->position
      = Vector3::FromArray(Json::ToArray<float>(parsedMesh, "position"));
  }

  if (parsedMesh.contains("metadata")) {
    // mesh.metadata = parsedMesh.metadata;
  }

  if (parsedMesh.contains("rotationQuaternion")) {
    mesh->rotationQuaternion = Quaternion::FromArray(
      Json::ToArray<float>(parsedMesh, "rotationQuaternion"));
  }
  else if (parsedMesh.contains("rotation")) {
    mesh->rotation
      = Vector3::FromArray(Json::ToArray<float>(parsedMesh, "rotation"));
  }

  if (parsedMesh.contains("scaling")) {
    mesh->position
      = Vector3::FromArray(Json::ToArray<float>(parsedMesh, "scaling"));
  }

  if (parsedMesh.contains("localMatrix")) {
    auto tmpMatrix
      = Matrix::FromArray(Json::ToArray<float>(parsedMesh, "localMatrix"));
    mesh->setPreTransformMatrix(tmpMatrix);
  }
  else if (parsedMesh.contains("pivotMatrix")) {
    auto tmpMatrix
      = Matrix::FromArray(Json::ToArray<float>(parsedMesh, "pivotMatrix"));
    mesh->setPivotMatrix(tmpMatrix);
  }

  mesh->setEnabled(Json::GetBool(parsedMesh, "isEnabled", true));
  mesh->isVisible        = Json::GetBool(parsedMesh, "isVisible", true);
  mesh->infiniteDistance = Json::GetBool(parsedMesh, "infiniteDistance");

  mesh->showSubMeshesBoundingBox
    = Json::GetBool(parsedMesh, "showSubMeshesBoundingBox");

  if (parsedMesh.contains("applyFog")) {
    mesh->applyFog = Json::GetBool(parsedMesh, "applyFog", true);
  }

  if (parsedMesh.contains("isPickable")) {
    mesh->isPickable = Json::GetBool(parsedMesh, "isPickable", true);
  }

  if (parsedMesh.contains("alphaIndex")) {
    mesh->alphaIndex
      = Json::GetNumber(parsedMesh, "alphaIndex", numeric_limits_t<int>::max());
  }

  mesh->receiveShadows = Json::GetBool(parsedMesh, "receiveShadows", false);
  mesh->billboardMode  = Json::GetNumber(parsedMesh, "billboardMode",
                                        AbstractMesh::BILLBOARDMODE_NONE);

  if (parsedMesh.contains("visibility")) {
    mesh->visibility = Json::GetNumber(parsedMesh, "visibility", 1.f);
  }

  mesh->checkCollisions = Json::GetBool(parsedMesh, "checkCollisions");

  if (parsedMesh.contains("isBlocker")) {
    mesh->isBlocker = Json::GetBool(parsedMesh, "isBlocker");
  }

  mesh->_shouldGenerateFlatShading
    = Json::GetBool(parsedMesh, "useFlatShading");

  // freezeWorldMatrix
  if (parsedMesh.contains("freezeWorldMatrix")) {
    mesh->_waitingFreezeWorldMatrix
      = Json::GetBool(parsedMesh, "freezeWorldMatrix");
  }

  // Parent
  if (parsedMesh.contains("parentId")) {
    mesh->_waitingParentId = Json::GetString(parsedMesh, "parentId");
  }

  // Actions
  if (parsedMesh.contains("actions")) {
    mesh->_waitingActions = Json::GetArray(parsedMesh, "actions");
  }

  // Overlay
  if (parsedMesh.contains("overlayAlpha")) {
    mesh->overlayAlpha = Json::GetNumber(parsedMesh, "overlayAlpha", 0.5f);
  }

  if (parsedMesh.contains("overlayColor")) {
    mesh->overlayColor
      = Color3::FromArray(Json::ToArray<float>(parsedMesh, "overlayColor"));
  }

  if (parsedMesh.contains("renderOverlay")) {
    mesh->renderOverlay = Json::GetBool(parsedMesh, "renderOverlay");
  }

  // Geometry
  mesh->isUnIndexed    = Json::GetBool(parsedMesh, "isUnIndexed", false);
  mesh->hasVertexAlpha = Json::GetBool(parsedMesh, "hasVertexAlpha", false);

  if (parsedMesh.contains("delayLoadingFile")) {
    mesh->delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    mesh->delayLoadingFile
      = rootUrl + Json::GetString(parsedMesh, "delayLoadingFile");
    mesh->_boundingInfo = ::std::make_unique<BoundingInfo>(
      Vector3::FromArray(
        Json::ToArray<float>(parsedMesh, "boundingBoxMinimum")),
      Vector3::FromArray(
        Json::ToArray<float>(parsedMesh, "boundingBoxMaximum")));

    if (parsedMesh.contains("_binaryInfo")) {
      mesh->_binaryInfo = Json::GetString(parsedMesh, "_binaryInfo");
    }

    mesh->_delayInfoKinds.clear();
    if (parsedMesh.contains("hasUVs")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UVKind);
    }

    if (parsedMesh.contains("hasUVs2")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV2Kind);
    }

    if (parsedMesh.contains("hasUVs3")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV3Kind);
    }

    if (parsedMesh.contains("hasUVs4")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV4Kind);
    }

    if (parsedMesh.contains("hasUVs5")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV5Kind);
    }

    if (parsedMesh.contains("hasUVs6")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV6Kind);
    }

    if (parsedMesh.contains("hasColors")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::ColorKind);
    }

    if (parsedMesh.contains("hasMatricesIndices")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesIndicesKind);
    }

    if (parsedMesh.contains("hasMatricesWeights")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesWeightsKind);
    }

    mesh->_delayLoadingFunction = Geometry::_ImportGeometry;

    if (SceneLoader::ForceFullSceneLoadingForIncremental()) {
      mesh->_checkDelayState();
    }
  }
  else {
    Geometry::_ImportGeometry(parsedMesh, mesh);
  }

  // Material
  if (parsedMesh.contains("materialId")) {
    mesh->setMaterialByID(Json::GetString(parsedMesh, "materialId"));
  }
  else {
    mesh->material = nullptr;
  }

  // Morph targets
  const int morphTargetManagerId
    = Json::GetNumber(parsedMesh, "morphTargetManagerId", -1);
  if (morphTargetManagerId > -1) {
    mesh->morphTargetManager = scene->getMorphTargetManagerById(
      static_cast<unsigned>(morphTargetManagerId));
  }

  // Skeleton
  if (parsedMesh.contains("skeletonId")) {
    string_t parsedSkeletonId = Json::GetString(parsedMesh, "skeletonId");
    if (!parsedSkeletonId.empty()) {
      mesh->skeleton = scene->getLastSkeletonByID(parsedSkeletonId);
      if (parsedMesh.contains("numBoneInfluencers")) {
        mesh->numBoneInfluencers
          = Json::GetNumber(parsedMesh, "numBoneInfluencers", 0u);
      }
    }
  }

  // Animations
  if (parsedMesh.contains("animations")) {
    for (auto& parsedAnimation : Json::GetArray(parsedMesh, "animations")) {
      mesh->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(*mesh, parsedMesh, scene);
  }

  if (parsedMesh.contains("autoAnimate")) {
    scene->beginAnimation(mesh,
                          Json::GetNumber(parsedMesh, "autoAnimateFrom", 0),
                          Json::GetNumber(parsedMesh, "autoAnimateTo", 0),
                          Json::GetBool(parsedMesh, "autoAnimateLoop", false),
                          Json::GetNumber(parsedMesh, "autoAnimateSpeed", 1.f));
  }

  // Layer Mask
  if (parsedMesh.contains("layerMask")) {
    auto layerMask = Json::GetString(parsedMesh, "layerMask");
    if (!layerMask.empty()) {
      mesh->layerMask = static_cast<unsigned>(::std::stoi(layerMask));
    }
  }
  else {
    mesh->layerMask = static_cast<unsigned>(
      Json::GetNumber(parsedMesh, "layerMask", 0x0FFFFFFF));
  }

  // Instances
  if (parsedMesh.contains("instances")) {
    for (auto& parsedInstance : Json::GetArray(parsedMesh, "instances")) {
      auto instance
        = mesh->createInstance(Json::GetString(parsedInstance, "name"));

      if (parsedInstance.contains("id")) {
        instance->id = Json::GetString(parsedInstance, "id");
      }

      // Tags.AddTagsTo(instance, parsedInstance.tags);

      mesh->position
        = Vector3::FromArray(Json::ToArray<float>(parsedInstance, "position"));

      if (parsedInstance.contains("parentId")) {
        instance->_waitingParentId
          = Json::GetString(parsedInstance, "parentId");
      }

      if (parsedInstance.contains("rotationQuaternion")) {
        instance->rotationQuaternion = Quaternion::FromArray(
          Json::ToArray<float>(parsedInstance, "rotationQuaternion"));
      }
      else if (parsedInstance.contains("rotation")) {
        instance->rotation = Vector3::FromArray(
          Json::ToArray<float>(parsedInstance, "rotation"));
      }

      if (parsedMesh.contains("scaling")) {
        instance->position
          = Vector3::FromArray(Json::ToArray<float>(parsedInstance, "scaling"));
      }

      instance->checkCollisions
        = Json::GetBool(parsedInstance, "checkCollisions");

      if (parsedMesh.contains("animations")) {
        for (auto& parsedAnimation : Json::GetArray(parsedMesh, "animations")) {
          instance->animations.emplace_back(Animation::Parse(parsedAnimation));
        }
        Node::ParseAnimationRanges(*instance, parsedMesh, scene);

        if (parsedMesh.contains("autoAnimate")
            && Json::GetBool(parsedMesh, "autoAnimate")) {
          scene->beginAnimation(
            instance, Json::GetNumber<int>(parsedMesh, "autoAnimateFrom", 0),
            Json::GetNumber<int>(parsedMesh, "autoAnimateTo", 0),
            Json::GetBool(parsedMesh, "autoAnimateLoop"),
            Json::GetNumber<float>(parsedMesh, "autoAnimateSpeed", 1.0));
        }
      }
    }
  }

  return mesh;
}

// Statics
MeshPtr Mesh::CreateRibbon(const string_t& name,
                           const vector_t<vector_t<Vector3>>& pathArray,
                           bool closeArray, bool closePath, int offset,
                           Scene* scene, bool updatable,
                           unsigned int sideOrientation,
                           const MeshPtr& instance)
{
  RibbonOptions options(pathArray, offset);
  options.closeArray      = closeArray;
  options.closePath       = closePath;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return MeshBuilder::CreateRibbon(name, options, scene);
}

MeshPtr Mesh::CreateDisc(const string_t& name, float radius,
                         unsigned int tessellation, Scene* scene,
                         bool updatable, unsigned int sideOrientation)
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateDisc(name, options, scene);
}

MeshPtr Mesh::CreateBox(const string_t& name, float size, Scene* scene,
                        bool updatable, unsigned int sideOrientation)
{
  BoxOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateBox(name, options, scene);
}

MeshPtr Mesh::CreateSphere(const string_t& name, unsigned int segments,
                           float diameter, Scene* scene, bool updatable,
                           unsigned int sideOrientation)
{
  SphereOptions options;
  options.segments        = segments;
  options.diameterX       = diameter;
  options.diameterY       = diameter;
  options.diameterZ       = diameter;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateSphere(name, options, scene);
}

// Cylinder and cone
MeshPtr Mesh::CreateCylinder(const string_t& name, float height,
                             float diameterTop, float diameterBottom,
                             unsigned int tessellation,
                             unsigned int subdivisions, Scene* scene,
                             bool updatable, unsigned int sideOrientation)
{
  CylinderOptions options;
  options.height          = height;
  options.diameterTop     = diameterTop;
  options.diameterBottom  = diameterBottom;
  options.tessellation    = tessellation;
  options.subdivisions    = subdivisions;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateCylinder(name, options, scene);
}

// Torus
MeshPtr Mesh::CreateTorus(const string_t& name, float diameter, float thickness,
                          unsigned int tessellation, Scene* scene,
                          bool updatable, unsigned int sideOrientation)
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateTorus(name, options, scene);
}

MeshPtr Mesh::CreateTorusKnot(const string_t& name, float radius, float tube,
                              unsigned int radialSegments,
                              unsigned int tubularSegments, float p, float q,
                              Scene* scene, bool updatable,
                              unsigned int sideOrientation)
{
  TorusKnotOptions options;
  options.radius          = radius;
  options.tube            = tube;
  options.radialSegments  = radialSegments;
  options.tubularSegments = tubularSegments;
  options.p               = p;
  options.q               = q;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateTorusKnot(name, options, scene);
}

LinesMeshPtr Mesh::CreateLines(const string_t& name,
                               const vector_t<Vector3>& points, Scene* scene,
                               bool updatable, const LinesMeshPtr& instance)
{
  LinesOptions options;
  options.points    = points;
  options.updatable = updatable;
  options.instance  = instance;

  return MeshBuilder::CreateLines(name, options, scene);
}

LinesMeshPtr Mesh::CreateDashedLines(const string_t& name,
                                     vector_t<Vector3>& points, float dashSize,
                                     float gapSize, unsigned int dashNb,
                                     Scene* scene, bool updatable,
                                     const LinesMeshPtr& instance)
{
  DashedLinesOptions options;
  options.points    = points;
  options.dashSize  = dashSize;
  options.gapSize   = gapSize;
  options.dashNb    = dashNb;
  options.updatable = updatable;
  options.instance  = instance;

  return MeshBuilder::CreateDashedLines(name, options, scene);
}

MeshPtr Mesh::CreatePolygon(const string_t& name,
                            const vector_t<Vector3>& shape, Scene* scene,
                            const vector_t<vector_t<Vector3>>& holes,
                            bool updatable, unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return MeshBuilder::CreatePolygon(name, options, scene);
}

MeshPtr Mesh::ExtrudePolygon(const string_t& name,
                             const vector_t<Vector3>& shape, float depth,
                             Scene* scene,
                             const vector_t<vector_t<Vector3>>& holes,
                             bool updatable, unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.depth           = depth;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return MeshBuilder::ExtrudePolygon(name, options, scene);
}

MeshPtr Mesh::ExtrudeShape(const string_t& name, const vector_t<Vector3>& shape,
                           const vector_t<Vector3>& path, float scale,
                           float rotation, unsigned int cap, Scene* scene,
                           bool updatable, unsigned int sideOrientation,
                           const MeshPtr& instance)
{
  ExtrudeShapeOptions options;
  options.shape           = shape;
  options.path            = path;
  options.scale           = scale;
  options.rotation        = rotation;
  options.cap             = cap;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;
  options.updatable       = updatable;

  return MeshBuilder::ExtrudeShape(name, options, scene);
}

MeshPtr Mesh::ExtrudeShapeCustom(
  const string_t& name, const vector_t<Vector3>& shape,
  const vector_t<Vector3>& path,
  const ::std::function<float(float i, float distance)>& scaleFunction,
  const ::std::function<float(float i, float distance)>& rotationFunction,
  bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene* scene,
  bool updatable, unsigned int sideOrientation, const MeshPtr& instance)
{
  ExtrudeShapeCustomOptions options;
  options.shape            = shape;
  options.path             = path;
  options.scaleFunction    = scaleFunction;
  options.rotationFunction = rotationFunction;
  options.ribbonCloseArray = ribbonCloseArray;
  options.ribbonClosePath  = ribbonClosePath;
  options.cap              = (cap == 0) ? 0 : Mesh::NO_CAP();
  options.sideOrientation  = sideOrientation;
  options.instance         = instance;
  options.updatable        = updatable;

  return MeshBuilder::ExtrudeShapeCustom(name, options, scene);
}

MeshPtr Mesh::CreateLathe(const string_t& name, const vector_t<Vector3>& shape,
                          float radius, unsigned int tessellation, Scene* scene,
                          bool updatable, unsigned int sideOrientation)
{
  LatheOptions options;
  options.shape           = shape;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateLathe(name, options, scene);
}

MeshPtr Mesh::CreatePlane(const string_t& name, float size, Scene* scene,
                          bool updatable, unsigned int sideOrientation)
{
  PlaneOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreatePlane(name, options, scene);
}

MeshPtr Mesh::CreateGround(const string_t& name, unsigned int width,
                           unsigned int height, unsigned int subdivisions,
                           Scene* scene, bool updatable)
{
  GroundOptions options(subdivisions);
  options.width     = width;
  options.height    = height;
  options.updatable = updatable;

  return MeshBuilder::CreateGround(name, options, scene);
}

MeshPtr Mesh::CreateTiledGround(const string_t& name, float xmin, float zmin,
                                float xmax, float zmax,
                                const ISize& subdivisions,
                                const ISize& precision, Scene* scene,
                                bool updatable)
{
  TiledGroundOptions options;
  options.xmin         = xmin;
  options.zmin         = zmin;
  options.xmax         = xmax;
  options.zmax         = zmax;
  options.subdivisions = subdivisions;
  options.precision    = precision;
  options.updatable    = updatable;

  return MeshBuilder::CreateTiledGround(name, options, scene);
}

GroundMeshPtr Mesh::CreateGroundFromHeightMap(
  const string_t& name, const string_t& url, unsigned int width,
  unsigned int height, unsigned int subdivisions, unsigned int minHeight,
  unsigned int maxHeight, Scene* scene, bool updatable,
  const ::std::function<void(GroundMesh* mesh)>& onReady)
{
  GroundFromHeightMapOptions options;
  options.width        = static_cast<float>(width);
  options.height       = static_cast<float>(height);
  options.subdivisions = subdivisions;
  options.minHeight    = static_cast<float>(minHeight);
  options.maxHeight    = static_cast<float>(maxHeight);
  options.updatable    = updatable;
  options.onReady      = onReady;

  return MeshBuilder::CreateGroundFromHeightMap(name, url, options, scene);
}

MeshPtr Mesh::CreateTube(
  const string_t& name, const vector_t<Vector3>& path, float radius,
  unsigned int tessellation,
  const ::std::function<float(unsigned int i, float distance)>& radiusFunction,
  unsigned int cap, Scene* scene, bool updatable, unsigned int sideOrientation,
  const MeshPtr& instance)
{
  TubeOptions options;
  options.path           = path;
  options.radius         = radius;
  options.tessellation   = tessellation;
  options.radiusFunction = radiusFunction;
  options.setArc(1.f);
  options.cap             = cap;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return MeshBuilder::CreateTube(name, options, scene);
}

MeshPtr Mesh::CreatePolyhedron(const string_t& name, PolyhedronOptions& options,
                               Scene* scene)
{
  return MeshBuilder::CreatePolyhedron(name, options, scene);
}

MeshPtr Mesh::CreateIcoSphere(const string_t& name, IcoSphereOptions& options,
                              Scene* scene)
{
  return MeshBuilder::CreateIcoSphere(name, options, scene);
}

MeshPtr Mesh::CreateDecal(const string_t& name, AbstractMesh* sourceMesh,
                          const Vector3& position, const Vector3& normal,
                          const Vector3& size, float angle)
{
  DecalOptions options;
  options.position = position;
  options.normal   = normal;
  options.size     = size;
  options.angle    = angle;

  return MeshBuilder::CreateDecal(name, sourceMesh, options);
}

Float32Array& Mesh::setPositionsForCPUSkinning()
{
  Float32Array source;
  if (_sourcePositions.empty()) {
    source = getVerticesData(VertexBuffer::PositionKind);
    if (source.empty()) {
      return _sourceNormals;
    }

    _sourcePositions = source;

    if (!isVertexBufferUpdatable(VertexBuffer::PositionKind)) {
      setVerticesData(VertexBuffer::PositionKind, source, true);
    }
  }
  return _sourcePositions;
}

Float32Array& Mesh::setNormalsForCPUSkinning()
{
  Float32Array source;
  if (_sourceNormals.empty()) {
    source = getVerticesData(VertexBuffer::NormalKind);
    if (source.empty()) {
      return _sourceNormals;
    }

    _sourceNormals = source;

    if (!isVertexBufferUpdatable(VertexBuffer::NormalKind)) {
      setVerticesData(VertexBuffer::NormalKind, source, true);
    }
  }
  return _sourceNormals;
}

Mesh* Mesh::applySkeleton(const SkeletonPtr& skeleton)
{
  if (!_geometry) {
    return this;
  }

  if (_geometry->_softwareSkinningRenderId == getScene()->getRenderId()) {
    return this;
  }

  _geometry->_softwareSkinningRenderId = getScene()->getRenderId();

  if (!isVerticesDataPresent(VertexBuffer::PositionKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesWeightsKind)) {
    return this;
  }

  if (_sourcePositions.empty()) {
    auto _submeshes = ::std::move(subMeshes);
    setPositionsForCPUSkinning();
    subMeshes = ::std::move(_submeshes);
  }

  if (_sourceNormals.empty()) {
    setNormalsForCPUSkinning();
  }

  // positionsData checks for not being Float32Array will only pass at most once
  auto positionsData = getVerticesData(VertexBuffer::PositionKind);

  if (positionsData.empty()) {
    return this;
  }

  // normalsData checks for not being Float32Array will only pass at most once
  auto normalsData = getVerticesData(VertexBuffer::NormalKind);

  if (normalsData.empty()) {
    return this;
  }

  auto matricesIndicesData = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesWeightsData = getVerticesData(VertexBuffer::MatricesWeightsKind);

  if (matricesWeightsData.empty() || matricesIndicesData.empty()) {
    return this;
  }

  bool needExtras = numBoneInfluencers() > 4;
  auto matricesIndicesExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) :
                   Float32Array();
  auto matricesWeightsExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) :
                   Float32Array();

  auto skeletonMatrices = skeleton->getTransformMatrices(this);

  auto tempVector3 = Vector3::Zero();
  Matrix finalMatrix;
  Matrix tempMatrix;

  unsigned int matWeightIdx = 0;
  unsigned int inf;
  float weight;
  for (unsigned int index = 0; index < positionsData.size();
       index += 3, matWeightIdx += 4) {
    for (inf = 0; inf < 4; ++inf) {
      weight = matricesWeightsData[matWeightIdx + inf];
      if (weight > 0.f) {
        Matrix::FromFloat32ArrayToRefScaled(
          skeletonMatrices,
          static_cast<unsigned int>(
            ::std::floor(matricesIndicesData[matWeightIdx + inf] * 16)),
          weight, tempMatrix);
        finalMatrix.addToSelf(tempMatrix);
      }
    }
    if (needExtras) {
      for (inf = 0; inf < 4; ++inf) {
        weight = matricesWeightsExtraData[matWeightIdx + inf];
        if (weight > 0.f) {
          Matrix::FromFloat32ArrayToRefScaled(
            skeletonMatrices,
            static_cast<unsigned int>(
              ::std::floor(matricesIndicesExtraData[matWeightIdx + inf] * 16)),
            weight, tempMatrix);
          finalMatrix.addToSelf(tempMatrix);
        }
      }
    }

    Vector3::TransformCoordinatesFromFloatsToRef(
      _sourcePositions[index], _sourcePositions[index + 1],
      _sourcePositions[index + 2], finalMatrix, tempVector3);
    tempVector3.toArray(positionsData, index);

    Vector3::TransformNormalFromFloatsToRef(
      _sourceNormals[index], _sourceNormals[index + 1],
      _sourceNormals[index + 2], finalMatrix, tempVector3);
    tempVector3.toArray(normalsData, index);

    finalMatrix.reset();
  }

  updateVerticesData(VertexBuffer::PositionKind, positionsData);
  updateVerticesData(VertexBuffer::NormalKind, normalsData);

  return this;
}

MinMax Mesh::GetMinMax(const vector_t<AbstractMeshPtr>& meshes)
{
  bool minVectorSet = false;
  Vector3 minVector;
  Vector3 maxVector;
  for (auto& mesh : meshes) {
    const BoundingBox& boundingBox = mesh->getBoundingInfo().boundingBox;
    if (!minVectorSet) {
      minVector    = boundingBox.minimumWorld;
      maxVector    = boundingBox.maximumWorld;
      minVectorSet = true;
    }
    else {
      minVector.minimizeInPlace(boundingBox.minimumWorld);
      maxVector.maximizeInPlace(boundingBox.maximumWorld);
    }
  }

  MinMax minMax;
  if (!minVectorSet) {
    minMax.min = minVector;
    minMax.max = maxVector;
  }
  else {
    minMax.min = Vector3::Zero();
    minMax.max = Vector3::Zero();
  }

  return minMax;
}

Vector3 Mesh::Center(const MinMaxDistance& MinMaxVectorAndDistance)
{
  return Vector3::Center(MinMaxVectorAndDistance.min,
                         MinMaxVectorAndDistance.max);
}

Vector3 Mesh::Center(const MinMax& minMaxVector)
{
  return Vector3::Center(minMaxVector.min, minMaxVector.max);
}

Vector3 Mesh::Center(const vector_t<AbstractMeshPtr>& meshes)
{
  MinMax minMaxVector = Mesh::GetMinMax(meshes);
  return Vector3::Center(minMaxVector.min, minMaxVector.max);
}

MeshPtr Mesh::MergeMeshes(const vector_t<MeshPtr>& meshes, bool disposeSource,
                          bool allow32BitsIndices, MeshPtr meshSubclass,
                          bool subdivideWithSubMeshes)
{
  unsigned int index = 0;
  if (!allow32BitsIndices) {
    size_t totalVertices = 0;

    // Counting vertices
    for (index = 0; index < meshes.size(); ++index) {
      if (meshes[index]) {
        totalVertices += meshes[index]->getTotalVertices();

        if (totalVertices > 65536) {
          BABYLON_LOG_WARN("Mesh",
                           "Cannot merge meshes because resulting mesh will "
                           "have more than 65536 vertices. Please use "
                           "allow32BitsIndices = true to use 32 bits indices.");
          return nullptr;
        }
      }
    }
  }

  // Merge
  unique_ptr_t<VertexData> vertexData      = nullptr;
  unique_ptr_t<VertexData> otherVertexData = nullptr;
  IndicesArray indiceArray;
  MeshPtr source = nullptr;
  for (auto& mesh : meshes) {
    if (mesh) {
      const auto& wm  = mesh->computeWorldMatrix(true);
      otherVertexData = VertexData::ExtractFromMesh(mesh.get(), true, true);

      otherVertexData->transform(wm);

      if (vertexData) {
        vertexData->merge(*otherVertexData.get(), allow32BitsIndices);
      }
      else {
        vertexData = ::std::move(otherVertexData);
        source     = meshes[index];
      }

      if (subdivideWithSubMeshes) {
        indiceArray.emplace_back(
          static_cast<uint32_t>(meshes[index]->getTotalIndices()));
      }
    }
  }

  if (!meshSubclass) {
    meshSubclass = Mesh::New(source->name + "_merged", source->getScene());
  }

  if ((!vertexData) || (!source))
    return meshSubclass;

  vertexData->applyToMesh(*meshSubclass);

  // Setting properties
  meshSubclass->material        = source->getMaterial();
  meshSubclass->checkCollisions = source->checkCollisions();

  // Cleaning
  if (disposeSource) {
    for (auto& mesh : meshes) {
      if (mesh) {
        mesh->dispose();
      }
    }
  }

  // Subdivide
  if (subdivideWithSubMeshes) {
    //-- removal of global submesh
    meshSubclass->releaseSubMeshes();
    index               = 0;
    unsigned int offset = 0;
    //-- apply subdivision according to index table
    while (index < indiceArray.size()) {
      SubMesh::CreateFromIndices(0, offset, indiceArray[index], meshSubclass);
      offset += indiceArray[index];
      ++index;
    }
  }

  return meshSubclass;
}

} // end of namespace BABYLON
