#include <babylon/lights/directional_light.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

DirectionalLight::DirectionalLight(const string_t& iName,
                                   const Vector3& direction, Scene* scene)
    : ShadowLight{iName, scene}
    , autoUpdateExtends{true}
    , _shadowFrustumSize{0.f}
    , _shadowOrthoScale{0.5f}
    , _orthoLeft{numeric_limits_t<float>::max()}
    , _orthoRight{numeric_limits_t<float>::min()}
    , _orthoTop{numeric_limits_t<float>::min()}
    , _orthoBottom{numeric_limits_t<float>::max()}
{
  setPosition(direction.scale(-1.f));
  setDirection(direction);
}

DirectionalLight::~DirectionalLight()
{
}

IReflect::Type DirectionalLight::type() const
{
  return IReflect::Type::DIRECTIONALLIGHT;
}

const char* DirectionalLight::getClassName() const
{
  return "DirectionalLight";
}

unsigned int DirectionalLight::getTypeID() const
{
  return Light::LIGHTTYPEID_DIRECTIONALLIGHT();
}

float DirectionalLight::shadowFrustumSize() const
{
  return _shadowFrustumSize;
}

void DirectionalLight::setShadowFrustumSize(float value)
{
  _shadowFrustumSize = value;
  forceProjectionMatrixCompute();
}

float DirectionalLight::shadowOrthoScale() const
{
  return _shadowOrthoScale;
}

void DirectionalLight::setShadowOrthoScale(float value)
{
  _shadowOrthoScale = value;
  forceProjectionMatrixCompute();
}

void DirectionalLight::_setDefaultShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const vector_t<AbstractMesh*>& renderList)
{
  if (shadowFrustumSize() > 0.f) {
    _setDefaultFixedFrustumShadowProjectionMatrix(matrix, viewMatrix);
  }
  else {
    _setDefaultAutoExtendShadowProjectionMatrix(matrix, viewMatrix, renderList);
  }
}

void DirectionalLight::_setDefaultFixedFrustumShadowProjectionMatrix(
  Matrix& matrix, const Matrix& /*viewMatrix*/)
{
  auto& activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  Matrix::PerspectiveFovLHToRef(
    shadowFrustumSize(), shadowFrustumSize(),
    shadowMinZ() ? *shadowMinZ() : activeCamera->minZ,
    shadowMaxZ() ? *shadowMaxZ() : activeCamera->maxZ, matrix);
}

void DirectionalLight::_setDefaultAutoExtendShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const vector_t<AbstractMesh*>& renderList)
{
  auto& activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  // Check extends
  if (autoUpdateExtends
      || stl_util::almost_equal(_orthoLeft, numeric_limits_t<float>::max())) {
    auto tempVector3 = Vector3::Zero();

    _orthoLeft   = numeric_limits_t<float>::max();
    _orthoRight  = numeric_limits_t<float>::min();
    _orthoTop    = numeric_limits_t<float>::min();
    _orthoBottom = numeric_limits_t<float>::max();

    // Check extends
    for (const auto& mesh : renderList) {
      if (!mesh) {
        continue;
      }

      const auto& boundingInfo = mesh->getBoundingInfo();
      const auto& boundingBox  = boundingInfo.boundingBox;

      for (const auto& vector : boundingBox.vectorsWorld) {
        Vector3::TransformCoordinatesToRef(vector, viewMatrix, tempVector3);

        if (tempVector3.x < _orthoLeft) {
          _orthoLeft = tempVector3.x;
        }
        if (tempVector3.y < _orthoBottom) {
          _orthoBottom = tempVector3.y;
        }

        if (tempVector3.x > _orthoRight) {
          _orthoRight = tempVector3.x;
        }
        if (tempVector3.y > _orthoTop) {
          _orthoTop = tempVector3.y;
        }
      }
    }

    const float xOffset = _orthoRight - _orthoLeft;
    const float yOffset = _orthoTop - _orthoBottom;

    Matrix::OrthoOffCenterLHToRef(
      _orthoLeft - xOffset * shadowOrthoScale(),
      _orthoRight + xOffset * shadowOrthoScale(),
      _orthoBottom - yOffset * shadowOrthoScale(),
      _orthoTop + yOffset * shadowOrthoScale(),
      shadowMinZ() ? *shadowMinZ() : activeCamera->minZ,
      shadowMaxZ() ? *shadowMaxZ() : activeCamera->maxZ, matrix);
  }
}

void DirectionalLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void DirectionalLight::transferToEffect(Effect* /*effect*/,
                                        const string_t& lightIndex)
{
  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData", transformedDirection().x,
                                 transformedDirection().y,
                                 transformedDirection().z, 1, lightIndex);
    return;
  }

  const auto& _direction = direction();
  _uniformBuffer->updateFloat4("vLightData", _direction.x, _direction.y,
                               _direction.z, 1, lightIndex);
}

float DirectionalLight::getDepthMinZ(Camera* /*activeCamera*/) const
{
  return 1.f;
}

float DirectionalLight::getDepthMaxZ(Camera* /*activeCamera*/) const
{
  return 1.f;
}

} // end of namespace BABYLON
