#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>

#include <babylon/core/json.h>

namespace BABYLON {

PBRMetallicRoughnessMaterial::PBRMetallicRoughnessMaterial(
  const string_t& iName, Scene* scene)
    : PBRBaseSimpleMaterial{iName, scene}
    , baseTexture{nullptr}
    , metallic{0.f}
    , roughness{0.f}
    , metallicRoughnessTexture{nullptr}
{
  _useRoughnessFromMetallicTextureAlpha = false;
  _useRoughnessFromMetallicTextureGreen = true;
  _useMetallnessFromMetallicTextureBlue = true;

  metallic  = 1.f;
  roughness = 1.f;
}

PBRMetallicRoughnessMaterial::~PBRMetallicRoughnessMaterial()
{
}

const string_t PBRMetallicRoughnessMaterial::getClassName() const
{
  return "PBRMetallicRoughnessMaterial";
}

vector_t<BaseTexturePtr> PBRMetallicRoughnessMaterial::getActiveTextures() const
{
  auto activeTextures = PBRBaseSimpleMaterial::getActiveTextures();

  if (baseTexture) {
    activeTextures.emplace_back(baseTexture);
  }

  if (metallicRoughnessTexture) {
    activeTextures.emplace_back(metallicRoughnessTexture);
  }

  return activeTextures;
}

bool PBRMetallicRoughnessMaterial::hasTexture(
  const BaseTexturePtr& texture) const
{
  if (PBRBaseSimpleMaterial::hasTexture(texture)) {
    return true;
  }

  if (baseTexture == texture) {
    return true;
  }

  if (metallicRoughnessTexture == texture) {
    return true;
  }

  return false;
}

MaterialPtr PBRMetallicRoughnessMaterial::clone(const string_t& /*name*/,
                                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object PBRMetallicRoughnessMaterial::serialize() const
{
  return Json::object();
}

PBRMetallicRoughnessMaterial* PBRMetallicRoughnessMaterial::Parse(
  const Json::value& /*source*/, Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
