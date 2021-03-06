#include <babylon/particles/emittertypes/point_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/scalar.h>

namespace BABYLON {

PointParticleEmitter::PointParticleEmitter()
    : direction1{Vector3(0.f, 1.f, 0.f)}, direction2{Vector3(0.f, 1.f, 0.f)}
{
}

PointParticleEmitter::~PointParticleEmitter()
{
}

void PointParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                  Vector3& directionToUpdate,
                                                  Particle* /*particle*/)
{
  auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  auto randZ = Scalar::RandomRange(direction1.z, direction2.z);

  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                          directionToUpdate);
}

void PointParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                 Vector3& positionToUpdate,
                                                 Particle* /*particle*/)
{
  Vector3::TransformCoordinatesFromFloatsToRef(0.f, 0.f, 0.f, worldMatrix,
                                               positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> PointParticleEmitter::clone() const
{

  auto newOne = ::std::make_unique<PointParticleEmitter>();

  return newOne;
}

void PointParticleEmitter::applyToShader(Effect* effect)
{
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
}

const char* PointParticleEmitter::getEffectDefines() const
{
  return "PointParticleEmitter";
}

const char* PointParticleEmitter::getClassName() const
{
  return "PointParticleEmitter";
}

Json::object PointParticleEmitter::serialize() const
{
  return Json::object();
}

void PointParticleEmitter::parse(const Json::value& serializationObject)
{
  if (serializationObject.contains("direction1")) {
    Vector3::FromArrayToRef(
      Json::ToArray<float>(serializationObject, "direction1"), 0, direction1);
  }
  if (serializationObject.contains("direction2")) {
    Vector3::FromArrayToRef(
      Json::ToArray<float>(serializationObject, "direction2"), 0, direction2);
  }
}

} // end of namespace BABYLON
