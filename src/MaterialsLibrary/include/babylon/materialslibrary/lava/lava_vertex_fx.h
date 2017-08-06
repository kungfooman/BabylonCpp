﻿#ifndef BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_VERTEX_FX_H
#define BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_VERTEX_FX_H

namespace BABYLON {

extern const char* lavaVertexShader;

const char* lavaVertexShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "// Inputs\n"
    "uniform float time;\n"
    "uniform float lowFrequencySpeed;\n"
    "// Varying\n"
    "varying float noise;\n"
    "\n"
    "// Attributes\n"
    "attribute vec3 position;\n"
    "#ifdef NORMAL\n"
    "attribute vec3 normal;\n"
    "#endif\n"
    "#ifdef UV1\n"
    "attribute vec2 uv;\n"
    "#endif\n"
    "#ifdef UV2\n"
    "attribute vec2 uv2;\n"
    "#endif\n"
    "#ifdef VERTEXCOLOR\n"
    "attribute vec4 color;\n"
    "#endif\n"
    "\n"
    "#include<bonesDeclaration>\n"
    "\n"
    "// Uniforms\n"
    "#include<instancesDeclaration>\n"
    "\n"
    "uniform mat4 view;\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "#ifdef DIFFUSE\n"
    "varying vec2 vDiffuseUV;\n"
    "uniform mat4 diffuseMatrix;\n"
    "uniform vec2 vDiffuseInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef POINTSIZE\n"
    "uniform float pointSize;\n"
    "#endif\n"
    "\n"
    "// Output\n"
    "varying vec3 vPositionW;\n"
    "#ifdef NORMAL\n"
    "varying vec3 vNormalW;\n"
    "#endif\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "varying vec4 vColor;\n"
    "#endif\n"
    "\n"
    "\n"
    "#include<clipPlaneVertexDeclaration>\n"
    "\n"
    "#include<fogVertexDeclaration>\n"
    "#include<__decl__lightFragment>[0..maxSimultaneousLights]\n"
    "\n"
    "/* NOISE FUNCTIONS */\n"
    "////// ASHIMA webgl noise\n"
    "////// https://github.com/ashima/webgl-noise/blob/master/src/classicnoise3D.glsl\n"
    "vec3 mod289(vec3 x)\n"
    "{\n"
    "  return x - floor(x * (1.0 / 289.0)) * 289.0;\n"
    "}\n"
    "\n"
    "vec4 mod289(vec4 x)\n"
    "{\n"
    "  return x - floor(x * (1.0 / 289.0)) * 289.0;\n"
    "}\n"
    "\n"
    "vec4 permute(vec4 x)\n"
    "{\n"
    "  return mod289(((x*34.0)+1.0)*x);\n"
    "}\n"
    "\n"
    "vec4 taylorInvSqrt(vec4 r)\n"
    "{\n"
    "  return 1.79284291400159 - 0.85373472095314 * r;\n"
    "}\n"
    "\n"
    "vec3 fade(vec3 t) {\n"
    "  return t*t*t*(t*(t*6.0-15.0)+10.0);\n"
    "}\n"
    "\n"
    "// Classic Perlin noise, periodic variant\n"
    "float pnoise(vec3 P, vec3 rep)\n"
    "{\n"
    "  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period\n"
    "  vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period\n"
    "  Pi0 = mod289(Pi0);\n"
    "  Pi1 = mod289(Pi1);\n"
    "  vec3 Pf0 = fract(P); // Fractional part for interpolation\n"
    "  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0\n"
    "  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);\n"
    "  vec4 iy = vec4(Pi0.yy, Pi1.yy);\n"
    "  vec4 iz0 = Pi0.zzzz;\n"
    "  vec4 iz1 = Pi1.zzzz;\n"
    "\n"
    "  vec4 ixy = permute(permute(ix) + iy);\n"
    "  vec4 ixy0 = permute(ixy + iz0);\n"
    "  vec4 ixy1 = permute(ixy + iz1);\n"
    "\n"
    "  vec4 gx0 = ixy0 * (1.0 / 7.0);\n"
    "  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;\n"
    "  gx0 = fract(gx0);\n"
    "  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);\n"
    "  vec4 sz0 = step(gz0, vec4(0.0));\n"
    "  gx0 -= sz0 * (step(0.0, gx0) - 0.5);\n"
    "  gy0 -= sz0 * (step(0.0, gy0) - 0.5);\n"
    "\n"
    "  vec4 gx1 = ixy1 * (1.0 / 7.0);\n"
    "  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;\n"
    "  gx1 = fract(gx1);\n"
    "  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);\n"
    "  vec4 sz1 = step(gz1, vec4(0.0));\n"
    "  gx1 -= sz1 * (step(0.0, gx1) - 0.5);\n"
    "  gy1 -= sz1 * (step(0.0, gy1) - 0.5);\n"
    "\n"
    "  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);\n"
    "  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);\n"
    "  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);\n"
    "  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);\n"
    "  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);\n"
    "  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);\n"
    "  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);\n"
    "  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);\n"
    "\n"
    "  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));\n"
    "  g000 *= norm0.x;\n"
    "  g010 *= norm0.y;\n"
    "  g100 *= norm0.z;\n"
    "  g110 *= norm0.w;\n"
    "  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));\n"
    "  g001 *= norm1.x;\n"
    "  g011 *= norm1.y;\n"
    "  g101 *= norm1.z;\n"
    "  g111 *= norm1.w;\n"
    "\n"
    "  float n000 = dot(g000, Pf0);\n"
    "  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));\n"
    "  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));\n"
    "  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));\n"
    "  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));\n"
    "  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));\n"
    "  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));\n"
    "  float n111 = dot(g111, Pf1);\n"
    "\n"
    "  vec3 fade_xyz = fade(Pf0);\n"
    "  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);\n"
    "  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);\n"
    "  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);\n"
    "  return 2.2 * n_xyz;\n"
    "}\n"
    "/* END FUNCTION */\n"
    "\n"
    "float turbulence( vec3 p ) {\n"
    "  float w = 100.0;\n"
    "  float t = -.5;\n"
    "  for (float f = 1.0 ; f <= 10.0 ; f++ ){\n"
    "  float power = pow( 2.0, f );\n"
    "  t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );\n"
    "  }\n"
    "  return t;\n"
    "}\n"
    "\n"
    "void main(void) {\n"
    "\n"
    "#include<instancesVertex>\n"
    "#include<bonesVertex>\n"
    "\n"
    "#ifdef NORMAL\n"
    "  // get a turbulent 3d noise using the normal, normal to high freq\n"
    "  noise = 10.0 *  -.10 * turbulence( .5 * normal + time*1.15 );\n"
    "  // get a 3d noise using the position, low frequency\n"
    "  float b = lowFrequencySpeed * 5.0 * pnoise( 0.05 * position +vec3(time*1.025), vec3( 100.0 ) );\n"
    "  // compose both noises\n"
    "  float displacement = - 1.5 * noise + b;\n"
    "\n"
    "  // move the position along the normal and transform it\n"
    "  vec3 newPosition = position + normal * displacement;\n"
    "  gl_Position = viewProjection * finalWorld * vec4( newPosition, 1.0 );\n"
    "\n"
    "\n"
    "  vec4 worldPos = finalWorld * vec4(newPosition, 1.0);\n"
    "  vPositionW = vec3(worldPos);\n"
    "\n"
    "  vNormalW = normalize(vec3(finalWorld * vec4(normal, 0.0)));\n"
    "#endif\n"
    "\n"
    "  // Texture coordinates\n"
    "#ifndef UV1\n"
    "  vec2 uv = vec2(0., 0.);\n"
    "#endif\n"
    "#ifndef UV2\n"
    "  vec2 uv2 = vec2(0., 0.);\n"
    "#endif\n"
    "\n"
    "#ifdef DIFFUSE\n"
    "  if (vDiffuseInfos.x == 0.)\n"
    "  {\n"
    "  vDiffuseUV = vec2(diffuseMatrix * vec4(uv, 1.0, 0.0));\n"
    "  }\n"
    "  else\n"
    "  {\n"
    "  vDiffuseUV = vec2(diffuseMatrix * vec4(uv2, 1.0, 0.0));\n"
    "  }\n"
    "#endif\n"
    "\n"
    "  // Clip plane\n"
    "#include<clipPlaneVertex>\n"
    "\n"
    "  // Fog\n"
    "#include<fogVertex>\n"
    "#include<shadowsVertex>[0..maxSimultaneousLights]\n"
    "\n"
    "  // Vertex color\n"
    "#ifdef VERTEXCOLOR\n"
    "  vColor = color;\n"
    "#endif\n"
    "\n"
    "  // Point size\n"
    "#ifdef POINTSIZE\n"
    "  gl_PointSize = pointSize;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_VERTEX_FX_H
