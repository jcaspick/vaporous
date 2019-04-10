#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Transformable.h"

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
	void drawPoint(vec3 point, vec4 color, float size = 1.0f);
	void drawLine(vec3 a, vec3 b, vec4 color);
	void drawCircle(vec3 center, float radius, vec4 color);
	void drawMesh(Mesh& mesh, mat4 tform, Shader* shader) const;
	void drawAxis(Transformable* tform, float size = 1.0f);

private:
	void createPointBuffer();
	void createLineBuffer();
	void createCircleBuffer();

	Context* _context;

	Camera* _activeCamera;
	Shader* _debugShader;

	GLuint _pointVbo, _pointVao;
	GLuint _lineVbo, _lineVao;
	GLuint _circleVbo, _circleVao;

	int _circleResolution = 64;
};