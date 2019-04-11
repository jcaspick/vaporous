#include "ResourceManager.h"
#include "Context.h"

#include <assert.h>
#include <iostream>

ResourceManager::ResourceManager(Context* context) :
	_context(context)
{}

Texture* ResourceManager::loadTexture(Textures id, const std::string& path, bool alpha) {
	std::unique_ptr<Texture> texture(new Texture());
	if (!texture->loadFromFile(path, alpha))
		throw std::runtime_error("ResourceManager failed to load texture " + path);
	
	_textures.insert(std::make_pair(id, std::move(texture)));
	return _textures[id].get();
}

Texture* ResourceManager::loadCubemap(Textures id, const std::string& folder) {
	std::unique_ptr<Texture> texture(new Texture());
	if (!texture->loadAsCubemap(folder + "/positiveX.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X))
		throw std::runtime_error("ResourceManager failed to load cubemap");
	if (!texture->loadAsCubemap(folder + "/negativeX.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X))
		throw std::runtime_error("ResourceManager failed to load cubemap");
	if (!texture->loadAsCubemap(folder + "/positiveY.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y))
		throw std::runtime_error("ResourceManager failed to load cubemap");
	if (!texture->loadAsCubemap(folder + "/negativeY.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y))
		throw std::runtime_error("ResourceManager failed to load cubemap");
	if (!texture->loadAsCubemap(folder + "/positiveZ.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z))
		throw std::runtime_error("ResourceManager failed to load cubemap");
	if (!texture->loadAsCubemap(folder + "/negativeZ.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
		throw std::runtime_error("ResourceManager failed to load cubemap");

	_textures.insert(std::make_pair(id, std::move(texture)));
	return _textures[id].get();
}

Shader* ResourceManager::loadShader(Shaders id, const std::string& vert,
	const std::string& frag, const std::string& geo) 
{
	std::unique_ptr<Shader> shader(new Shader());
	if (!shader->loadFromFile(vert, frag, geo))
		throw std::runtime_error("ResourceManager failed to load shader");

	_shaders.insert(std::make_pair(id, std::move(shader)));
	return _shaders[id].get();
}

Texture& ResourceManager::getTexture(Textures id) {
	auto found = _textures.find(id);
	assert(found != _textures.end());
	return *found->second;
}

Shader& ResourceManager::getShader(Shaders id) {
	auto found = _shaders.find(id);
	assert(found != _shaders.end());
	return *found->second;
}

void ResourceManager::bindTexture(Textures id, GLuint slot) {
	switch (slot) {
	case 0:
		_context->gl->bindTexture0(getTexture(id).id);
		break;
	case 1:
		_context->gl->bindTexture1(getTexture(id).id);
		break;
	case 2:
		_context->gl->bindTexture2(getTexture(id).id);
		break;
	case 3:
		_context->gl->bindTexture3(getTexture(id).id);
		break;
	}
}

void ResourceManager::bindCubemap(Textures id) {
	_context->gl->bindCubemap0(getTexture(id).id);
}

void ResourceManager::bindShader(Shaders id) {
	_context->gl->useShader(getShader(id).id);
}