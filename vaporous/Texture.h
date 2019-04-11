#pragma once

#include <glad\glad.h>
#include <string>

class Texture {
public:
	Texture();
	bool loadFromFile(const std::string& path, bool alpha);
	bool loadAsCubemap(const std::string& path, GLuint face);
	void bind() const;

	GLuint id;
	GLuint width, height;
	GLuint wrapS, wrapT;
	GLuint filterMin, filterMag;
	GLuint internalFormat;
	GLuint imageFormat;
};