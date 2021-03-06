#ifndef BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/fur/fur_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT FurMaterial : public PushMaterial {

public:
  FurMaterial(const std::string& name, Scene* scene);
  ~FurMaterial() override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  void updateFur();
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables();
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  const string_t getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static FurMaterial* Parse(const Json::value& source, Scene* scene,
                            const std::string& rootUrl);
  static DynamicTexture* GenerateTexture(const std::string& name, Scene* scene);

  /**
   * Creates and returns an array of meshes used as shells for the Fur Material
   * that can be disposed later in your code
   * The quality is in interval [0, 100]
   */
  static std::vector<Mesh*> FurifyMesh(Mesh* sourceMesh, unsigned int quality);

protected:
  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_heightTexture();
  void set_heightTexture(const BaseTexturePtr& value);
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);
  float get_furTime() const;
  void set_furTime(float newFurTime);

public:
  Property<FurMaterial, BaseTexturePtr> diffuseTexture;
  Property<FurMaterial, BaseTexturePtr> heightTexture;
  Color3 diffuseColor;
  float furLength;
  float furAngle;
  Color3 furColor;
  float furOffset;
  float furSpacing;
  Vector3 furGravity;
  float furSpeed;
  float furDensity;
  float furOcclusion;
  DynamicTexturePtr furTexture;
  Property<FurMaterial, bool> disableLighting;
  Property<FurMaterial, unsigned int> maxSimultaneousLights;
  bool highLevelFur;
  std::vector<AbstractMeshPtr> _meshes;
  Property<FurMaterial, float> furTime;

private:
  BaseTexturePtr _diffuseTexture;
  BaseTexturePtr _heightTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  int _renderId;
  float _furTime;
  FurMaterialDefines _defines;

}; // end of class FurMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H
