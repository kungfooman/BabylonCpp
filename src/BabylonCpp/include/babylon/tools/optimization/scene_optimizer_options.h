#ifndef BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
#define BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SceneOptimizerOptions {

public:
  SceneOptimizerOptions(float targetFrameRate = 60, int trackerDuration = 2000);
  SceneOptimizerOptions(const SceneOptimizerOptions& other);
  SceneOptimizerOptions(SceneOptimizerOptions&& other);
  SceneOptimizerOptions& operator=(const SceneOptimizerOptions& other);
  SceneOptimizerOptions& operator=(SceneOptimizerOptions&& other);
  ~SceneOptimizerOptions();

  static SceneOptimizerOptions LowDegradationAllowed(float targetFrameRate
                                                     = 60);

  static SceneOptimizerOptions ModerateDegradationAllowed(float targetFrameRate
                                                          = 60);

  static SceneOptimizerOptions HighDegradationAllowed(float targetFrameRate
                                                      = 60);

public:
  vector_t<SceneOptimization> optimizations;
  float targetFrameRate;
  int trackerDuration;

}; // end of class SceneOptimizerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
