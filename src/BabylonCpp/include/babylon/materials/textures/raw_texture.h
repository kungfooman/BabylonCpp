#ifndef BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RawTexture : public Texture {

public:
  RawTexture(const ArrayBufferView& data, int width, int height,
             unsigned int format, Scene* scene, bool generateMipMaps = true,
             bool invertY = false,
             unsigned int samplingMode
             = TextureConstants::TRILINEAR_SAMPLINGMODE,
             unsigned int type = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~RawTexture();

  void update(const ArrayBufferView& data);

  // Statics
  static unique_ptr_t<RawTexture> CreateLuminanceTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture> CreateLuminanceAlphaTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture> CreateAlphaTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture> CreateRGBTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  static unique_ptr_t<RawTexture> CreateRGBATexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  static unique_ptr_t<RawTexture> CreateRTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    unsigned int type         = EngineConstants::TEXTURETYPE_FLOAT);

  unsigned int format;
  unsigned int type;

private:
  Engine* _engine;

}; // struct of class RawTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
