#ifndef BABYLON_CULLING_BOUNDING_INFO_H
#define BABYLON_CULLING_BOUNDING_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/culling/icullable.h>

namespace BABYLON {

class Collider;

class BABYLON_SHARED_EXPORT BoundingInfo : public ICullable {

public:
  BoundingInfo(const Vector3& minimum, const Vector3& maximum);
  BoundingInfo(const BoundingInfo& boundingInfo);
  BoundingInfo(BoundingInfo&& other);
  BoundingInfo& operator=(const BoundingInfo& other);
  BoundingInfo& operator=(BoundingInfo&& other);
  virtual ~BoundingInfo();

  /** Methods **/
  void update(const Matrix& world);

  /**
   * @brief Recreate the bounding info to be centered around a specific point
   * given a specific extend.
   * @param center New center of the bounding info
   * @param extend New extend of the bounding info
   */
  BoundingInfo& centerOn(const Vector3& center, const Vector3& extend);

  /**
   * @brief Scale the current bounding info by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding info
   */
  BoundingInfo& scale(float factor);

  /**
   * @brief Returns `true` if the bounding info is within the frustum defined by
   * the passed array of planes.
   * @param frustumPlanes defines the frustum to test
   * @param strategy defines the strategy to use for the culling (default is
   * BABYLON.Scene.CULLINGSTRATEGY_STANDARD)
   * @returns true if the bounding info is in the frustum planes
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                   unsigned int strategy
                   = 0 /*AbstractMesh::CULLINGSTRATEGY_STANDARD*/) override;

  /**
   * @brief Gets the world distance between the min and max points of the
   * bounding box.
   */
  float diagonalLength() const;

  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;

  /**
   * @brief Hidden
   */
  bool _checkCollision(const Collider& collider) const;

  bool intersectsPoint(const Vector3& point);
  bool intersects(const BoundingInfo& boundingInfo, bool precise);

protected:
  bool get_isLocked() const;
  void set_isLocked(bool value);

private:
  Extents computeBoxExtents(const Vector3& axis, const BoundingBox& box) const;
  bool extentsOverlap(float min0, float max0, float min1, float max1) const;
  bool axisOverlap(const Vector3& axis, const BoundingBox& box0,
                   const BoundingBox& box1) const;

public:
  Vector3 minimum;
  Vector3 maximum;
  BoundingBox boundingBox;
  BoundingSphere boundingSphere;
  Property<BoundingInfo, bool> isLocked;

private:
  bool _isLocked;

}; // end of class BoundingInfo

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_INFO_H
