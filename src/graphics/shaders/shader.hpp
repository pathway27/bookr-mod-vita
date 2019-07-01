/*
 *  From learnopengl.com
 */

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>

namespace bookr {

class Shader
{
public:
  GLuint Program;
  // Constructor generates the shader on the fly
  Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

  // Uses the current shader
  void Use();
};

extern std::map<std::string, Shader> shaders;

}

#endif