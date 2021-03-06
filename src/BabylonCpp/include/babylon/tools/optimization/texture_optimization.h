#ifndef BABYLON_TOOLS_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
#define BABYLON_TOOLS_OPTIMIZATION_TEXTURE_OPTIMIZATION_H

#include <babylon/babylon_global.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT TextureOptimization : public SceneOptimization {

public:
  TextureOptimization(int priority = 0, int maximumSize = 1024);
  ~TextureOptimization();

  bool apply(Scene* scene) override;

public:
  int maximumSize;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
