#include <babylon/layer/highlight_layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/glow_blur_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/states/_stencil_state.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Color4 HighlightLayer::NeutralColor             = Color4(0.f, 0.f, 0.f, 0.f);
int HighlightLayer::GlowingMeshStencilReference = 0x02;
int HighlightLayer::NormalMeshStencilReference  = 0x01;

HighlightLayer::HighlightLayer(const string_t& name, Scene* scene)
    : HighlightLayer(name, scene,
                     IHighlightLayerOptions{
                       0.5f,                           // mainTextureRatio
                       nullopt_t,                      // mainTextureFixedSize
                       0.5f,                           // blurTextureSizeRatio
                       1.f,                            // blurVerticalSize
                       1.f,                            // blurHorizontalSize
                       EngineConstants::ALPHA_COMBINE, // alphaBlendingMode
                       nullptr,                        // camera
                       nullopt_t,                      // isStroke
                       -1                              // renderingGroupId
                     })
{
}

HighlightLayer::HighlightLayer(const string_t& iName, Scene* scene,
                               const IHighlightLayerOptions& options)
    : EffectLayer{iName, scene}
    , innerGlow{true}
    , outerGlow{true}
    , blurHorizontalSize{this, &HighlightLayer::get_blurHorizontalSize,
                         &HighlightLayer::set_blurHorizontalSize}
    , blurVerticalSize{this, &HighlightLayer::get_blurVerticalSize,
                       &HighlightLayer::set_blurVerticalSize}
    , _downSamplePostprocess{nullptr}
    , _horizontalBlurPostprocess{nullptr}
    , _verticalBlurPostprocess{nullptr}
    , _blurTexture{nullptr}
{
  neutralColor = HighlightLayer::NeutralColor;

  // Warn on stencil.
  if (!_engine->isStencilEnable()) {
    BABYLON_LOG_WARN(
      "HighlightLayer",
      "Rendering the Highlight Layer requires the stencil to be active on the "
      "canvas.");
  }

  // Adapt options
  _options.mainTextureRatio     = options.mainTextureRatio;
  _options.blurTextureSizeRatio = options.blurTextureSizeRatio;
  _options.blurHorizontalSize   = options.blurHorizontalSize;
  _options.blurVerticalSize     = options.blurVerticalSize;
  _options.alphaBlendingMode    = options.alphaBlendingMode;
  _options.camera               = options.camera;
  _options.renderingGroupId     = options.renderingGroupId;

  // Initialize the layer
  IEffectLayerOptions effectLayerOptions;
  effectLayerOptions.alphaBlendingMode    = EngineConstants::ALPHA_ADD;
  effectLayerOptions.camera               = _options.camera;
  effectLayerOptions.mainTextureFixedSize = _options.mainTextureFixedSize;
  effectLayerOptions.mainTextureRatio     = _options.mainTextureRatio;
  effectLayerOptions.renderingGroupId     = _options.renderingGroupId;
  _init(effectLayerOptions);

  // Do not render as long as no meshes have been added
  _shouldRender = false;
}

HighlightLayer::~HighlightLayer()
{
}

void HighlightLayer::set_blurHorizontalSize(float value)
{
  _horizontalBlurPostprocess->kernel = value;
}

void HighlightLayer::set_blurVerticalSize(float value)
{
  _verticalBlurPostprocess->kernel = value;
}

float HighlightLayer::get_blurHorizontalSize() const
{
  return _horizontalBlurPostprocess->kernel;
}

float HighlightLayer::get_blurVerticalSize() const
{
  return _verticalBlurPostprocess->kernel;
}

string_t HighlightLayer::getEffectName() const
{
  return HighlightLayer::EffectName;
}

Effect* HighlightLayer::_createMergeEffect()
{
  // Effect
  EffectCreationOptions effectCreationOptions;
  effectCreationOptions.attributes          = {VertexBuffer::PositionKindChars};
  effectCreationOptions.uniformBuffersNames = {"offset"};
  effectCreationOptions.samplers            = {"textureSampler"};
  effectCreationOptions.defines = _options.isStroke ? "#define STROKE \n" : "";

  return _engine->createEffect("glowMapMerge", effectCreationOptions,
                               _scene->getEngine());
}

void HighlightLayer::_createTextureAndPostProcesses()
{
  int blurTextureWidth
    = static_cast<int>(static_cast<float>(_mainTextureDesiredSize.width)
                       * _options.blurTextureSizeRatio);
  int blurTextureHeight
    = static_cast<int>(static_cast<float>(_mainTextureDesiredSize.height)
                       * _options.blurTextureSizeRatio);
  blurTextureWidth = _engine->needPOTTextures() ?
                       Tools::GetExponentOfTwo(blurTextureWidth, _maxSize) :
                       blurTextureWidth;
  blurTextureHeight = _engine->needPOTTextures() ?
                        Tools::GetExponentOfTwo(blurTextureHeight, _maxSize) :
                        blurTextureHeight;

  unsigned int textureType = 0;
  if (_engine->getCaps().textureHalfFloatRender) {
    textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
  }
  else {
    textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  }

  _blurTexture = RenderTargetTexture::New(
    "HighlightLayerBlurRTT", ISize{blurTextureWidth, blurTextureHeight}, _scene,
    false, true, textureType);
  _blurTexture->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture->anisotropicFilteringLevel = 16;
  _blurTexture->updateSamplingMode(TextureConstants::TRILINEAR_SAMPLINGMODE);
  _blurTexture->renderParticles      = false;
  _blurTexture->ignoreCameraViewport = true;

  _textures = {_blurTexture};

  if (_options.alphaBlendingMode == EngineConstants::ALPHA_COMBINE) {
    _downSamplePostprocess = ::std::make_unique<PassPostProcess>(
      "HighlightLayerPPP", _options.blurTextureSizeRatio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _downSamplePostprocess->onApplyObservable.add(
      [&](Effect* effect, EventState&) {
        effect->setTexture("textureSampler", _mainTexture);
      });

    _horizontalBlurPostprocess = ::std::make_unique<GlowBlurPostProcess>(
      "HighlightLayerHBP", Vector2(1.f, 0.f), _options.blurHorizontalSize,
      ToVariant<float, PostProcessOptions>(1.f), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _horizontalBlurPostprocess->onApplyObservable.add(
      [&](Effect* effect, EventState&) {
        effect->setFloat2("screenSize", static_cast<float>(blurTextureWidth),
                          static_cast<float>(blurTextureHeight));
      });

    _verticalBlurPostprocess = ::std::make_unique<GlowBlurPostProcess>(
      "HighlightLayerVBP", Vector2(0.f, 1.f), _options.blurVerticalSize,
      ToVariant<float, PostProcessOptions>(1.f), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _verticalBlurPostprocess->onApplyObservable.add(
      [&](Effect* effect, EventState&) {
        effect->setFloat2("screenSize", static_cast<float>(blurTextureWidth),
                          static_cast<float>(blurTextureHeight));
      });

    _postProcesses
      = {_downSamplePostprocess.get(), _horizontalBlurPostprocess.get(),
         _verticalBlurPostprocess.get()};
  }
  else {
    _horizontalBlurPostprocess = ::std::make_unique<GlowBlurPostProcess>(
      "HighlightLayerHBP", Vector2(1.f, 0.f), _options.blurHorizontalSize,
      ToVariant<float, PostProcessOptions>(1.f), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine()/*, false,
      textureType*/);
    _horizontalBlurPostprocess->width  = blurTextureWidth;
    _horizontalBlurPostprocess->height = blurTextureHeight;
    _horizontalBlurPostprocess->onApplyObservable.add(
      [&](Effect* effect, EventState&) {
        effect->setTexture("textureSampler", _mainTexture);
      });

    _verticalBlurPostprocess = ::std::make_unique<GlowBlurPostProcess>(
      "HighlightLayerVBP", Vector2(0.f, 1.f), _options.blurVerticalSize,
      ToVariant<float, PostProcessOptions>(1.f), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine()/*, false,
      textureType*/);
    _verticalBlurPostprocess->onApplyObservable.add(
      [&](Effect* effect, EventState&) {
        effect->setFloat2("screenSize", static_cast<float>(blurTextureWidth),
                          static_cast<float>(blurTextureHeight));
      });

    _postProcesses
      = {_horizontalBlurPostprocess.get(), _verticalBlurPostprocess.get()};
  }

  _mainTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture* /*renderTargetTexture*/, EventState& /*es*/) {
      onBeforeBlurObservable.notifyObservers(this);

      auto internalTexture = _blurTexture->getInternalTexture();
      if (internalTexture) {
        _scene->postProcessManager->directRender(_postProcesses,
                                                 internalTexture, true);
      }

      onAfterBlurObservable.notifyObservers(this);
    });

  // Prevent autoClear.
  for (auto& pp : _postProcesses) {
    pp->autoClear = false;
  }
}

bool HighlightLayer::needStencil() const
{
  return true;
}

bool HighlightLayer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();
  auto mesh     = subMesh->getRenderingMesh();

  if (!material || !mesh || _meshes.empty()) {
    return false;
  }

  BaseTexturePtr emissiveTexture = nullptr;

  if ((_meshes.find(mesh->uniqueId) != _meshes.end())
      && _meshes[mesh->uniqueId].glowEmissiveOnly && material
      && (material->type() == IReflect::Type::STANDARDMATERIAL)) {
    emissiveTexture = ::std::static_pointer_cast<StandardMaterial>(material)
                        ->emissiveTexture();
  }
  return EffectLayer::_isReady(subMesh, useInstances, emissiveTexture);
}

void HighlightLayer::_internalRender(Effect* effect)
{
  // Texture
  effect->setTexture("textureSampler", _blurTexture);

  // Cache
  auto engine                       = _engine;
  auto previousStencilBuffer        = engine->getStencilBuffer();
  auto previousStencilFunction      = engine->getStencilFunction();
  auto previousStencilMask          = engine->getStencilMask();
  auto previousStencilOperationPass = engine->getStencilOperationPass();
  auto previousStencilOperationFail = engine->getStencilOperationFail();
  auto previousStencilOperationDepthFail
    = engine->getStencilOperationDepthFail();
  auto previousStencilReference = engine->getStencilFunctionReference();

  // Stencil operations
  engine->setStencilOperationPass(EngineConstants::REPLACE);
  engine->setStencilOperationFail(EngineConstants::KEEP);
  engine->setStencilOperationDepthFail(EngineConstants::KEEP);

  // Draw order
  engine->setStencilMask(0x00);
  engine->setStencilBuffer(true);
  engine->setStencilFunctionReference(
    static_cast<int>(_instanceGlowingMeshStencilReference));

  // 2 passes inner outer
  if (outerGlow) {
    effect->setFloat("offset", 0);
    engine->setStencilFunction(EngineConstants::NOTEQUAL);
    engine->drawElementsType(Material::TriangleFillMode(), 0, 6);
  }
  if (innerGlow) {
    effect->setFloat("offset", 1);
    engine->setStencilFunction(EngineConstants::EQUAL);
    engine->drawElementsType(Material::TriangleFillMode(), 0, 6);
  }

  // Restore Cache
  engine->setStencilFunction(previousStencilFunction);
  engine->setStencilMask(previousStencilMask);
  engine->setStencilBuffer(previousStencilBuffer);
  engine->setStencilOperationPass(previousStencilOperationPass);
  engine->setStencilOperationFail(previousStencilOperationFail);
  engine->setStencilOperationDepthFail(previousStencilOperationDepthFail);
  engine->setStencilFunctionReference(previousStencilReference);
}

bool HighlightLayer::shouldRender() const
{
  if (EffectLayer::shouldRender()) {
    return !_meshes.empty() ? true : false;
  }

  return false;
}

bool HighlightLayer::_shouldRenderMesh(const MeshPtr& mesh) const
{
  // Excluded Mesh
  if (!_excludedMeshes.empty()
      && (_excludedMeshes.find(mesh->uniqueId) != _excludedMeshes.end())) {
    return false;
  };

  if (!EffectLayer::hasMesh(mesh.get())) {
    return false;
  }

  return true;
}

void HighlightLayer::_setEmissiveTextureAndColor(const MeshPtr& mesh,
                                                 SubMesh* /*subMesh*/,
                                                 const MaterialPtr& material)
{
  auto hasHighlightLayerMesh = _meshes.find(mesh->uniqueId) != _meshes.end();
  if (hasHighlightLayerMesh) {
    const auto& highlightLayerMesh = _meshes[mesh->uniqueId];
    _emissiveTextureAndColor.color.set(highlightLayerMesh.color.r, //
                                       highlightLayerMesh.color.g, //
                                       highlightLayerMesh.color.b, //
                                       1.f);
  }
  else {
    _emissiveTextureAndColor.color.set(neutralColor.r, //
                                       neutralColor.g, //
                                       neutralColor.b, //
                                       neutralColor.a);
  }

  if (hasHighlightLayerMesh && _meshes[mesh->uniqueId].glowEmissiveOnly
      && material && (material->type() == IReflect::Type::STANDARDMATERIAL)) {
    _emissiveTextureAndColor.texture
      = ::std::static_pointer_cast<StandardMaterial>(material)
          ->emissiveTexture();
    _emissiveTextureAndColor.color.set(1.0, //
                                       1.0, //
                                       1.0, //
                                       1.0);
  }
  else {
    _emissiveTextureAndColor.texture = nullptr;
  }
}

void HighlightLayer::addExcludedMesh(Mesh* mesh)
{
  if (_excludedMeshes.empty()) {
    return;
  }

  if (!stl_util::contains(_excludedMeshes, mesh->uniqueId)) {
    IHighlightLayerExcludedMesh meshExcluded;
    meshExcluded.mesh = mesh;
    meshExcluded.beforeRender
      = mesh->onBeforeRenderObservable().add([](Mesh* mesh, EventState&) {
          mesh->getEngine()->setStencilBuffer(false);
        });
    meshExcluded.afterRender
      = mesh->onAfterRenderObservable().add([](Mesh* mesh, EventState&) {
          mesh->getEngine()->setStencilBuffer(true);
        });
    _excludedMeshes[mesh->uniqueId] = meshExcluded;
  }
}

void HighlightLayer::removeExcludedMesh(Mesh* mesh)
{
  if (_excludedMeshes.empty()) {
    return;
  }

  if (stl_util::contains(_excludedMeshes, mesh->uniqueId)) {
    auto& meshExcluded = _excludedMeshes[mesh->uniqueId];
    if (meshExcluded.beforeRender) {
      mesh->onBeforeRenderObservable().remove(meshExcluded.beforeRender);
    }

    if (meshExcluded.afterRender) {
      mesh->onAfterRenderObservable().remove(meshExcluded.afterRender);
    }

    _excludedMeshes.erase(mesh->uniqueId);
  }
}

bool HighlightLayer::hasMesh(AbstractMesh* mesh) const
{
  if (_meshes.empty()) {
    return false;
  }

  if (!EffectLayer::hasMesh(mesh)) {
    return false;
  }

  return _meshes.find(mesh->uniqueId) != _meshes.end();
}

void HighlightLayer::addMesh(Mesh* mesh, const Color3& color,
                             bool glowEmissiveOnly)
{
  if (_meshes.empty()) {
    return;
  }

  if (stl_util::contains(_meshes, mesh->uniqueId)) {
    _meshes[mesh->uniqueId].color = color;
  }
  else {
    IHighlightLayerMesh newMesh;
    newMesh.mesh  = mesh;
    newMesh.color = color;
    // Lambda required for capture due to Observable this context
    newMesh.observerHighlight
      = mesh->onBeforeRenderObservable().add([&](Mesh* mesh, EventState&) {
          if (!_excludedMeshes.empty()
              && stl_util::contains(_excludedMeshes, mesh->uniqueId)) {
            _defaultStencilReference(mesh);
          }
          else {
            mesh->getScene()->getEngine()->setStencilFunctionReference(
              static_cast<int>(_instanceGlowingMeshStencilReference));
          }
        });
    newMesh.observerDefault = mesh->onAfterRenderObservable().add(
      [&](Mesh* mesh, EventState&) { _defaultStencilReference(mesh); });
    newMesh.glowEmissiveOnly = glowEmissiveOnly;
    _meshes[mesh->uniqueId]  = newMesh;

    mesh->onDisposeObservable.add(
      [this, mesh](Node* /*node*/, EventState& /*es*/) { _disposeMesh(mesh); });
  }

  _shouldRender = true;
}

void HighlightLayer::removeMesh(Mesh* mesh)
{
  if (_meshes.empty()) {
    return;
  }

  if (stl_util::contains(_meshes, mesh->uniqueId)) {
    auto& meshHighlight = _meshes[mesh->uniqueId];
    if (meshHighlight.observerHighlight) {
      mesh->onBeforeRenderObservable().remove(meshHighlight.observerHighlight);
    }

    if (meshHighlight.observerDefault) {
      mesh->onAfterRenderObservable().remove(meshHighlight.observerDefault);
    }

    _meshes.erase(mesh->uniqueId);
  }

  _shouldRender = false;
  for (auto& meshHighlightToCheck : _meshes) {
    if (_meshes.find(meshHighlightToCheck.first) != _meshes.end()) {
      _shouldRender = true;
      break;
    }
  }
}

void HighlightLayer::_defaultStencilReference(Mesh* mesh)
{
  mesh->getScene()->getEngine()->setStencilFunctionReference(
    HighlightLayer::NormalMeshStencilReference);
}

void HighlightLayer::_disposeMesh(Mesh* mesh)
{
  removeMesh(mesh);
  removeExcludedMesh(mesh);
}

void HighlightLayer::dispose()
{
  if (!_meshes.empty()) {
    // Clean mesh references
    for (auto item : _meshes) {
      auto& meshHighlight = item.second;
      if (meshHighlight.mesh) {
        if (meshHighlight.observerHighlight) {
          meshHighlight.mesh->onBeforeRenderObservable().remove(
            meshHighlight.observerHighlight);
        }
        if (meshHighlight.observerDefault) {
          meshHighlight.mesh->onAfterRenderObservable().remove(
            meshHighlight.observerDefault);
        }
      }
    }
    _meshes.clear();
  }

  if (_excludedMeshes.empty()) {
    for (auto item : _excludedMeshes) {
      auto& meshHighlight = item.second;
      if (meshHighlight.beforeRender) {
        meshHighlight.mesh->onBeforeRenderObservable().remove(
          meshHighlight.beforeRender);
      }
      if (meshHighlight.afterRender) {
        meshHighlight.mesh->onAfterRenderObservable().remove(
          meshHighlight.afterRender);
      }
    }
    _excludedMeshes.clear();
  }

  EffectLayer::dispose();
}

string_t HighlightLayer::getClassName() const
{
  return "HighlightLayer";
}

Json::object HighlightLayer::serialize() const
{
  return Json::object();
}

HighlightLayer*
HighlightLayer::Parse(const Json::value& /*parsedHightlightLayer*/,
                      Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
