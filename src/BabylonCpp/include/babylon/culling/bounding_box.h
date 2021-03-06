#ifndef BABYLON_CULLING_BOUNDING_BOX_H
#define BABYLON_CULLING_BOUNDING_BOX_H

#include <babylon/babylon_api.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BoundingSphere;

class BABYLON_SHARED_EXPORT BoundingBox : public ICullable {

public:
  /**
   * @brief Creates a new bounding box.
   * @param min defines the minimum vector (in local space)
   * @param max defines the maximum vector (in local space)
   */
  BoundingBox(const Vector3& min, const Vector3& max);
  BoundingBox(const BoundingBox& other);
  BoundingBox(BoundingBox&& other);
  BoundingBox& operator=(const BoundingBox& other);
  BoundingBox& operator=(BoundingBox&& other);
  virtual ~BoundingBox();

  /** Methods **/

  /**
   * @brief Recreates the entire bounding box from scratch.
   * @param min defines the new minimum vector (in local space)
   * @param max defines the new maximum vector (in local space)
   */
  void reConstruct(const Vector3& min, const Vector3& max);

  /**
   * @brief Scale the current bounding box by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding box
   */
  BoundingBox& scale(float factor);

  Matrix& getWorldMatrix();
  BoundingBox& setWorldMatrix(const Matrix& matrix);

  /**
   * @brief Hidden
   */
  void _update(const Matrix& world);

  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                   unsigned int strategy = 0) override;
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;
  bool intersectsPoint(const Vector3& point);
  bool intersectsSphere(const BoundingSphere& sphere);
  bool intersectsMinMax(const Vector3& min, const Vector3& max) const;

  /** Statics **/
  static bool Intersects(const BoundingBox& box0, const BoundingBox& box1);
  static bool IntersectsSphere(const Vector3& minPoint, const Vector3& maxPoint,
                               const Vector3& sphereCenter, float sphereRadius);
  static bool IsCompletelyInFrustum(const std::vector<Vector3>& boundingVectors,
                                    const std::array<Plane, 6>& frustumPlanes);
  static bool IsInFrustum(const std::vector<Vector3>& boundingVectors,
                          const std::array<Plane, 6>& frustumPlanes);

public:
  std::vector<Vector3> vectors;
  Vector3 center;
  Vector3 centerWorld;
  Vector3 extendSize;
  Vector3 extendSizeWorld;
  std::vector<Vector3> directions;
  std::vector<Vector3> vectorsWorld;
  Vector3 minimumWorld;
  Vector3 maximumWorld;

  Vector3 minimum;
  Vector3 maximum;

private:
  Matrix _worldMatrix;

}; // end of class BoundingBox

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_BOX_H
