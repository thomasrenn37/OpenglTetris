#pragma once

#define GLEW_STATIC // Need to define to be able to statically link.
#include <glew.h>
#include <glfw3.h>

class Texture
{
public:
	static GLuint generate2DTexture(const char* fileName);
};