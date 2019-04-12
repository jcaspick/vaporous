#pragma once
#include "ResourceIdentifiers.h"
#include "Texture.h"
#include "Shader.h"
#include <map>
#include <string>
#include <memory>

#include <iostream>

struct Context;
class ResourceManager {
public:
	ResourceManager(Context* context);
	Texture* loadTexture(Textures id, const std::string& path, bool alpha);
	Shader* loadShader(Shaders id, const std::string& vert, const std::string& frag,
		const std::string& geo = "");

	Texture& getTexture(Textures id);
	Shader& getShader(Shaders id);
	void bindTexture(Textures id, GLuint slot = 0);
	void bindShader(Shaders id);

private:
	Context* _context;
	std::map<Textures, std::unique_ptr<Texture>> _textures;
	std::map<Shaders, std::unique_ptr<Shader>> _shaders;
};