#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
	width(0),
	height(0),
	internalFormat(GL_RGB),
	imageFormat(GL_RGB),
	wrapS(GL_REPEAT),
	wrapT(GL_REPEAT),
	filterMin(GL_NEAREST),
	filterMag(GL_NEAREST) 
{
	glGenTextures(1, &id);
}

void Texture::bind() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::loadFromFile(const std::string& path, bool alpha) {
	int sourceWidth, sourceHeight, numChannels;
	int mode = alpha ? STBI_rgb_alpha : STBI_rgb;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path.c_str(), &sourceWidth, 
		&sourceHeight, &numChannels, mode);

	if (data) {
		width = sourceWidth;
		height = sourceHeight;
		imageFormat = alpha ? GL_RGBA : GL_RGB;
		internalFormat = alpha ? GL_RGBA : GL_RGB;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
			imageFormat, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
		return true;
	}
	else {
		std::cout << "Error: failed to load texture " << path.c_str() << std::endl;
		return false;
	}
}