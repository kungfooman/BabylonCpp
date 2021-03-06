﻿#ifndef BABYLON_SHADERS_COLOR_VERTEX_FX_H
#define BABYLON_SHADERS_COLOR_VERTEX_FX_H

namespace BABYLON {

extern const char* colorVertexShader;

const char* colorVertexShader
  = "// Attributes\n"
    "attribute vec3 position;\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "attribute vec4 color;\n"
    "#endif\n"
    "\n"
    "#include<bonesDeclaration>\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 viewProjection;\n"
    "uniform mat4 world;\n"
    "\n"
    "// Output\n"
    "#ifdef VERTEXCOLOR\n"
    "varying vec4 vColor;\n"
    "#endif\n"
    "\n"
    "void main(void) {\n"
    "  mat4 finalWorld = world;\n"
    "#include<bonesVertex>\n"
    "  gl_Position = viewProjection * finalWorld * vec4(position, 1.0);\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "  // Vertex color\n"
    "  vColor = color;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_VERTEX_FX_H
