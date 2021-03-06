#ifndef BABYLON_SAMPLES_TEXTURES_TEXTURES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_TEXTURES_TEXTURES_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _TexturesSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Textures";
  }

public:
  _TexturesSamplesIndex();
  virtual ~_TexturesSamplesIndex();

}; // end of class _TexturesSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_TEXTURES_SAMPLES_INDEX_H
