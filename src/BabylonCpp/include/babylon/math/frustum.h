#ifndef BABYLON_MATH_FRUSTUM_H
#define BABYLON_MATH_FRUSTUM_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Matrix;
class Plane;

/**
 * @brief Represents a Frustum.
 */
class BABYLON_SHARED_EXPORT Frustum {

public:
  /** Statics **/

  /**
   * @brief Returns a new array of 6 Frustum planes computed by the given
   * transformation matrix.
   */
  static std::array<Plane, 6> GetPlanes(const Matrix& transform);

  static void GetNearPlaneToRef(const Matrix& transform, Plane& frustumPlane);
  static void GetFarPlaneToRef(const Matrix& transform, Plane& frustumPlane);
  static void GetLeftPlaneToRef(const Matrix& transform, Plane& frustumPlane);
  static void GetRightPlaneToRef(const Matrix& transform, Plane& frustumPlane);
  static void GetTopPlaneToRef(const Matrix& transform, Plane& frustumPlane);
  static void GetBottomPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Sets the given array "frustumPlanes" with the 6 Frustum planes
   * computed by the given transformation matrix.
   */
  static void GetPlanesToRef(const Matrix& transform,
                             std::array<Plane, 6>& frustumPlanes);

}; // end of class Frustum

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_FRUSTUM_H
