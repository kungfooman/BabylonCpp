#include <babylon/gamepad/controllers/generic_controller.h>

#include <babylon/core/logging.h>
#include <babylon/gamepad/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {

GenericController::GenericController(
  const shared_ptr_t<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
{
  _defaultModel = nullptr;
}

GenericController::~GenericController()
{
}

void GenericController::initControllerMesh(
  Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, GenericController::MODEL_BASE_URL, GenericController::MODEL_FILENAME,
    scene,
    [this,
     &meshLoaded](const vector_t<AbstractMeshPtr>& newMeshes,
                  const vector_t<IParticleSystemPtr>& /*particleSystems*/,
                  const vector_t<SkeletonPtr>& /*skeletons*/,
                  const vector_t<AnimationGroupPtr>& /*animationGroups*/) {
      _defaultModel = newMeshes[1];
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel.get());
      }
    });
}

void GenericController::_handleButtonChange(unsigned int buttonIdx,
                                            const ExtendedGamepadButton& state,
                                            const GamepadButtonChanges& changes)
{
  BABYLON_LOGF_INFO("GenericController", "Button id: %d", buttonIdx);
  BABYLON_LOGF_INFO("GenericController", "State: %s", state.toString().c_str());
  BABYLON_LOGF_INFO("GenericController", "Changes: %s",
                    changes.toString().c_str());
}

} // end of namespace BABYLON
