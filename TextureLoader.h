#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <string>
#include <GL/glew.h>
#include <GL/glfw.h>

GLuint loadTexture(const std::string& name, int mode=GL_CLAMP_TO_EDGE);

#endif
