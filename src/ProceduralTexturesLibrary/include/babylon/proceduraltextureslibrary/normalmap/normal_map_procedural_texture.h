#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT NormalMapProceduralTexture
    : public ProceduralTexture {

public:
  template <typename... Ts>
  static NormalMapProceduralTexture* New(Ts&&... args)
  {
    auto texture = new NormalMapProceduralTexture(::std::forward<Ts>(args)...);
    texture->addToScene(
      static_cast<unique_ptr_t<NormalMapProceduralTexture>>(texture));

    return texture;
  }
  ~NormalMapProceduralTexture();

  void updateShaderUniforms();
  void render(bool useCameraPostProcess = false);
  void resize(const Size& size, bool generateMipMaps = false);

  /**
   * @brief Serializes this normal map procedural texture.
   * @returns a serialized normal map procedural texture object
   */
  Json::object serialize() const;

  /**
   * @brief Creates a Normal Map Procedural Texture from parsed normal map
   * procedural texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing normal map procedural
   * texture information
   * @returns a parsed Normal Map Procedural Texture
   */
  static unique_ptr_t<NormalMapProceduralTexture>
  Parse(const Json::value& parsedTexture, Scene* scene,
        const string_t& rootUrl);

protected:
  NormalMapProceduralTexture(const std::string& name, const Size& size,
                             Scene* scene, Texture* fallbackTexture = nullptr,
                             bool generateMipMaps = false);

  TexturePtr& get_baseTexture();
  void set_baseTexture(const TexturePtr& texture);

public:
  Property<NormalMapProceduralTexture, TexturePtr> baseTexture;

private:
  TexturePtr _baseTexture;

}; // end of class NormalMapProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
