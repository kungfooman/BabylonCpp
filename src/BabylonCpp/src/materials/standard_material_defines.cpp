#include <babylon/materials/standard_material_defines.h>

namespace BABYLON {

StandardMaterialDefines::StandardMaterialDefines() : MaterialDefines{}
{
  _keys = {"MAINUV1",
           "MAINUV2",
           "DIFFUSE",
           "AMBIENT",
           "OPACITY",
           "OPACITYRGB",
           "REFLECTION",
           "EMISSIVE",
           "SPECULAR",
           "BUMP",
           "PARALLAX",
           "PARALLAXOCCLUSION",
           "SPECULAROVERALPHA",
           "CLIPPLANE",
           "ALPHATEST",
           "DEPTHPREPASS",
           "ALPHAFROMDIFFUSE",
           "POINTSIZE",
           "FOG",
           "SPECULARTERM",
           "DIFFUSEFRESNEL",
           "OPACITYFRESNEL",
           "REFLECTIONFRESNEL",
           "REFRACTIONFRESNEL",
           "EMISSIVEFRESNEL",
           "FRESNEL",
           "NORMAL",
           "UV1",
           "UV2",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "INSTANCES",
           "GLOSSINESS",
           "ROUGHNESS",
           "EMISSIVEASILLUMINATION",
           "LINKEMISSIVEWITHDIFFUSE",
           "REFLECTIONFRESNELFROMSPECULAR",
           "LIGHTMAP",
           "OBJECTSPACE_NORMALMAP",
           "USELIGHTMAPASSHADOWMAP",
           "REFLECTIONMAP_3D",
           "REFLECTIONMAP_SPHERICAL",
           "REFLECTIONMAP_PLANAR",
           "REFLECTIONMAP_CUBIC",
           "USE_LOCAL_REFLECTIONMAP_CUBIC",
           "REFLECTIONMAP_PROJECTION",
           "REFLECTIONMAP_SKYBOX",
           "REFLECTIONMAP_EXPLICIT",
           "REFLECTIONMAP_EQUIRECTANGULAR",
           "REFLECTIONMAP_EQUIRECTANGULAR_FIXED",
           "REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED",
           "INVERTCUBICMAP",
           "LOGARITHMICDEPTH",
           "REFRACTION",
           "REFRACTIONMAP_3D",
           "REFLECTIONOVERALPHA",
           "TWOSIDEDLIGHTING",
           "SHADOWFLOAT",
           "MORPHTARGETS",
           "MORPHTARGETS_NORMAL",
           "MORPHTARGETS_TANGENT",
           "NONUNIFORMSCALING",
           "PREMULTIPLYALPHA",
           "IMAGEPROCESSING",
           "VIGNETTE",
           "VIGNETTEBLENDMODEMULTIPLY",
           "VIGNETTEBLENDMODEOPAQUE",
           "TONEMAPPING",
           "CONTRAST",
           "COLORCURVES",
           "COLORGRADING",
           "COLORGRADING3D",
           "SAMPLER3DGREENDEPTH",
           "SAMPLER3DBGRMAP",
           "IMAGEPROCESSINGPOSTPROCESS",
           "IS_REFLECTION_LINEAR",
           "IS_REFRACTION_LINEAR",
           "EXPOSURE"};
  rebuild();

  DIFFUSEDIRECTUV       = 0;
  AMBIENTDIRECTUV       = 0;
  OPACITYDIRECTUV       = 0;
  EMISSIVEDIRECTUV      = 0;
  SPECULARDIRECTUV      = 0;
  BUMPDIRECTUV          = 0;
  NUM_BONE_INFLUENCERS  = 0;
  BonesPerMesh          = 0;
  LIGHTMAPDIRECTUV      = 0;
  NUM_MORPH_INFLUENCERS = 0;
}

StandardMaterialDefines::~StandardMaterialDefines()
{
}

void StandardMaterialDefines::setReflectionMode(unsigned int modeToEnable)
{
  static const array_t<unsigned int, 9> modes{{
    REFLECTIONMAP_CUBIC,
    REFLECTIONMAP_EXPLICIT,
    REFLECTIONMAP_PLANAR,
    REFLECTIONMAP_PROJECTION,
    REFLECTIONMAP_SKYBOX,
    REFLECTIONMAP_SPHERICAL,
    REFLECTIONMAP_EQUIRECTANGULAR,
    REFLECTIONMAP_EQUIRECTANGULAR_FIXED,
    REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED,
  }};

  for (auto& mode : modes) {
    defines[mode] = (mode == modeToEnable);
  }
}

} // end of namespace BABYLON
