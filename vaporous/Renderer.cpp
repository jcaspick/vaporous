#include "Renderer.h"
#include "Context.h"

#include <glm/gtx/norm.hpp>

Renderer::Renderer(Context* context) :
	_context(context),
	_activeCamera(nullptr),
	_debugShader(nullptr)
{}

void Renderer::init() {
	// load shader
	_debugShader = _context->resourceMgr->loadShader(Shaders::SingleColor,
		"shaders/basic.vert", "shaders/basic_singleColor.frag");

	// create buffers for debug drawing
	createPointBuffer();
	createLineBuffer();
	createCircleBuffer();
}

void Renderer::setCamera(Camera* camera) {
	_activeCamera = camera;
}

void Renderer::drawPoint(vec3 point, vec4 color, float size) {
	if (!_activeCamera) {
		std::cout << "can't render, no camera set" << std::endl;
		return;
	}

	_context->gl->useShader(_debugShader->id);
	_context->gl->bindVAO(_pointVao);
	_context->gl->setLineMode(true);

	mat4 model = glm::scale(glm::translate(mat4(1), point), vec3(size));
	_debugShader->setMat4("model", model);
	_debugShader->setMat4("view", _activeCamera->getViewMatrix());
	_debugShader->setMat4("projection", _activeCamera->getProjectionMatrix());
	_debugShader->setVec4("color", color);

	glDrawArrays(GL_LINES, 0, 6);
}

void Renderer::drawLine(vec3 a, vec3 b, vec4 color) {
	if (!_activeCamera) {
		std::cout << "can't render, no camera set" << std::endl;
		return;
	}

	_context->gl->useShader(_debugShader->id);
	_context->gl->bindVAO(_lineVao);
	_context->gl->setLineMode(true);

	mat4 model = mat4(1);
	vec3 up = vec3(0, 1, 0);
	if (a.x == b.x && a.z == b.z) up = vec3(1, 0, 0);
	model = model * glm::inverse(glm::lookAt(a, b, up));
	model = glm::scale(model, vec3(glm::length(a - b)));

	_debugShader->setMat4("model", model);
	_debugShader->setMat4("view", _activeCamera->getViewMatrix());
	_debugShader->setMat4("projection", _activeCamera->getProjectionMatrix());
	_debugShader->setVec4("color", color);

	glDrawArrays(GL_LINES, 0, 6);
}

void Renderer::drawCircle(vec3 center, float radius, vec4 color) {
	if (!_activeCamera) {
		std::cout << "can't render, no camera set" << std::endl;
		return;
	}

	_context->gl->useShader(_debugShader->id);
	_context->gl->bindVAO(_circleVao);
	_context->gl->setLineMode(true);

	mat4 model = glm::scale(glm::translate(mat4(1), center), vec3(radius));
	_debugShader->setMat4("model", model);
	_debugShader->setMat4("view", _activeCamera->getViewMatrix());
	_debugShader->setMat4("projection", _activeCamera->getProjectionMatrix());
	_debugShader->setVec4("color", color);

	glDrawArrays(GL_LINE_LOOP, 0, _circleResolution);
}

void Renderer::drawMesh(Mesh& mesh, mat4 tform, Shader* shader) const {
	if (!_activeCamera) {
		std::cout << "can't render, no camera set" << std::endl;
		return;
	}

	if (!mesh.isBound()) return;

	_context->gl->useShader(shader->id);
	_context->gl->bindVAO(mesh._vao);
	_context->gl->setLineMode(false);

	shader->setMat4("model", tform);
	shader->setMat4("view", _activeCamera->getViewMatrix());
	shader->setMat4("projection", _activeCamera->getProjectionMatrix());
	shader->setInt("mainTex", 0);

	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void Renderer::createPointBuffer() {
	float vertices[] = {
		-1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f,  1.0f
	};

	glGenVertexArrays(1, &_pointVao);
	glGenBuffers(1, &_pointVbo);

	glBindVertexArray(_pointVao);
	glBindBuffer(GL_ARRAY_BUFFER, _pointVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), (void*)0);
}

void Renderer::createLineBuffer() {
	float vertices[] = {
		0.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  -1.0f,
	};

	glGenVertexArrays(1, &_lineVao);
	glGenBuffers(1, &_lineVbo);

	glBindVertexArray(_lineVao);
	glBindBuffer(GL_ARRAY_BUFFER, _lineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), (void*)0);
}

void Renderer::createCircleBuffer() {
	std::vector<float> vertices;
	float angleStep = 360.0f / static_cast<float>(_circleResolution);
	for (float a = 0.0f; a < 360.0f ; a += angleStep) {
		vertices.emplace_back(cos(glm::radians(a))); // x
		vertices.emplace_back(0);					 // y
		vertices.emplace_back(sin(glm::radians(a))); // z
	}

	glGenVertexArrays(1, &_circleVao);
	glGenBuffers(1, &_circleVbo);

	glBindVertexArray(_circleVao);
	glBindBuffer(GL_ARRAY_BUFFER, _circleVbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		&vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), (void*)0);
}