#include "Renderer.h"
#include "Context.h"

Renderer::Renderer(Context* context) :
	_context(context),
	_activeCamera(nullptr),
	_debugShader(nullptr)
{}

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

void Renderer::init() {
	// load shader
	_debugShader = _context->resourceMgr->loadShader(Shaders::SingleColor,
		"shaders/basic.vert", "shaders/basic_singleColor.frag");

	// create vertex buffer for point drawing
	float pointVertices[] = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), 
		pointVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		3 * sizeof(float), (void*)0);
}