#ifndef BABYLON_MESH_MESH_SIMPLIFICATION_SCENE_COMPONENT_H
#define BABYLON_MESH_MESH_SIMPLIFICATION_SCENE_COMPONENT_H

#include <babylon/babylon_global.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/scene_component_constants.h>

namespace BABYLON {

/**
 * @brief Defines the simplification queue scene component responsible to help
 * scheduling the various simplification task created in a scene.
 */
class BABYLON_SHARED_EXPORT SimplicationQueueSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_SIMPLIFICATIONQUEUE;

  template <typename... Ts>
  static SimplicationQueueSceneComponentPtr New(Ts&&... args)
  {
    return shared_ptr_t<SimplicationQueueSceneComponent>(
      new SimplicationQueueSceneComponent(::std::forward<Ts>(args)...));
  }
  virtual ~SimplicationQueueSceneComponent();

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene
   * @param scene Defines the scene to register the component in
   */
  SimplicationQueueSceneComponent(Scene* scene);

private:
  void _beforeCameraUpdate();

}; // end of class MeshLODLevel

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_MESH_SIMPLIFICATION_SCENE_COMPONENT_H
