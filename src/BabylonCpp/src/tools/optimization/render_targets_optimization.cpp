#include <babylon/tools/optimization/render_targets_optimization.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

RenderTargetsOptimization::RenderTargetsOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

RenderTargetsOptimization::~RenderTargetsOptimization()
{
}

bool RenderTargetsOptimization::apply(Scene* scene)
{
  scene->renderTargetsEnabled = false;
  return true;
}

} // end of namespace BABYLON
