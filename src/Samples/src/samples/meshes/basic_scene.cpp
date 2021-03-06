#include <babylon/samples/meshes/basic_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

BasicScene::BasicScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

BasicScene::~BasicScene()
{
}

const char* BasicScene::getName()
{
  return "Basic Scene";
}

void BasicScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
  auto camera = FreeCamera::New("camera1", Vector3(0, 5, -10), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a built-in "sphere" shape; its constructor takes 5 params: name,
  // subdivs, size, scene
  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  // Move the sphere upward 1/2 of its height
  sphere->position().y = 1.f;

  // Create a built-in "ground" shape.
  // Params: name, width, depth, subdivs, scene
  Mesh::CreateGround("ground1", 6, 6, 2, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
