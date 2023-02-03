#include <iostream>

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint Texture::generate2DTexture(const char* fileName)
{
	///	TEXTURES 
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);


	// load and generate the texture
	int x, y, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(fileName, &x, &y, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}