#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using quat = glm::quat;

struct Context;
class Renderer {
public:
	Renderer(Context* context);
	void init();
	void setCamera(Camera* camera);
	void drawPoint(vec3 point, vec4 color, float size = 0.1f);
	void drawLine(vec3 a, vec3 b, vec4 color);

private:
	void createPointBuffer();
	void createLineBuffer();

	Context* _context;

	Camera* _activeCamera;
	Shader* _debugShader;

	GLuint _pointVbo, _pointVao;
	GLuint _lineVbo, _lineVao;
};