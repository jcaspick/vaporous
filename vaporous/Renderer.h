#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

struct Context;
class Renderer {
public:
	Renderer(Context* context);
	void init();
	void setCamera(Camera* camera);
	void drawPoint(vec3 point, vec4 color, float size = 0.1f);

private:
	Context* _context;

	Camera* _activeCamera;
	Shader* _debugShader;

	GLuint _pointVbo, _pointVao;
};