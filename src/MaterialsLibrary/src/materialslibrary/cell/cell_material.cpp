#include <babylon/materialslibrary/cell/cell_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/cell/cell_fragment_fx.h>
#include <babylon/materialslibrary/cell/cell_vertex_fx.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

CellMaterial::CellMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseTexture{this, &CellMaterial::get_diffuseTexture,
                     &CellMaterial::set_diffuseTexture}
    , diffuseColor{Color3(1, 1, 1)}
    , computeHighLevel{this, &CellMaterial::get_computeHighLevel,
                       &CellMaterial::set_computeHighLevel}
    , disableLighting{this, &CellMaterial::get_disableLighting,
                      &CellMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &CellMaterial::get_maxSimultaneousLights,
                            &CellMaterial::set_maxSimultaneousLights}
    , _diffuseTexture{nullptr}
    , _computeHighLevel{false}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["cellVertexShader"] = cellVertexShader;

  // Fragment shader
  Effect::ShadersStore()["cellPixelShader"] = cellPixelShader;
}

CellMaterial::~CellMaterial()
{
}

TexturePtr& CellMaterial::get_diffuseTexture()
{
  return _diffuseTexture;
}

void CellMaterial::set_diffuseTexture(const TexturePtr& value)
{
  if (_diffuseTexture != value) {
    _diffuseTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool CellMaterial::get_computeHighLevel() const
{
  return _computeHighLevel;
}

void CellMaterial::set_computeHighLevel(bool value)
{
  if (_computeHighLevel != value) {
    _computeHighLevel = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool CellMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void CellMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int CellMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void CellMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool CellMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool CellMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr CellMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool CellMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                     bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<CellMaterialDefines>();
  }

  auto defines
    = *(static_cast<CellMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.boolDef["DIFFUSE"] = true;
        }
      }
    }
  }

  // High level
  defines.boolDef["CELLBASIC"] = !_computeHighLevel;

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, true, true);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    const std::string shaderName{"cell"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",        "view",          "viewProjection", "vEyePosition",
      "vLightsType",  "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",    "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix"};
    const std::vector<std::string> samplers{"diffuseSampler"};
    const std::vector<std::string> uniformBuffers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = _maxSimultaneousLights;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters
      = {{"maxSimultaneousLights", _maxSimultaneousLights}};

    MaterialHelper::PrepareUniformsAndSamplersList(options);
    subMesh->setEffect(
      scene->getEngine()->createEffect(shaderName, options, engine), defines);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void CellMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<CellMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2(
        "vDiffuseInfos", static_cast<float>(_diffuseTexture->coordinatesIndex),
        _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", static_cast<float>(pointSize));
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  // Lights
  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines,
                               _maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE()) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> CellMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> CellMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  if (_diffuseTexture) {
    activeTextures.emplace_back(_diffuseTexture);
  }

  return activeTextures;
}

bool CellMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  return (_diffuseTexture == texture);
}

void CellMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

const string_t CellMaterial::getClassName() const
{
  return "CellMaterial";
}

MaterialPtr CellMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object CellMaterial::serialize() const
{
  return Json::object();
}

CellMaterial* CellMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
