#include "Renderer.h"
#include "Context.h"

#include <glm/gtx/norm.hpp>

Renderer::Renderer(Context* context) :
	_context(context),
	_activeCamera(nullptr),
	_debugShader(nullptr)
{}

void Renderer::init() {
	// load shaders
	_debugShader = _context->resourceMgr->loadShader(Shaders::SingleColor,
		"shaders/basic.vert", "shaders/basic_singleColor.frag");
	_screenShader = _context->resourceMgr->loadShader(Shaders::Screen,
		"shaders/screen.vert", "shaders/post_dither.frag");
	_skyShader = _context->resourceMgr->loadShader(Shaders::Sky,
		"shaders/screen.vert", "shaders/gradient_sky.frag");

	// load textures
	_context->resourceMgr->loadTexture(Textures::Sky,
		"resources/sky.png", false, false, true);

	// generate framebuffers
	glGenFramebuffers(1, &_fbo);
	glGenTextures(1, &_colorBuffer);
	glGenRenderbuffers(1, &_depthBuffer);
	buildFramebuffers();

	// create buffers for debug drawing
	createPointBuffer();
	createLineBuffer();
	createCircleBuffer();
	createScreenBuffer();
}

void Renderer::setCamera(Camera* camera) {
	_activeCamera = camera;
}

void Renderer::beginDraw() {
	// switch to framebuffer
	_context->gl->bindFBO(_fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw sky
	_context->gl->setLineMode(false);
	_context->gl->useShader(_skyShader->id);
	_context->gl->bindVAO(_screenVao);

	_context->resourceMgr->bindTexture(Textures::Sky, 1);
	_skyShader->setInt("mainTex", 1);
	_skyShader->setFloat("skyColor", fmod(glfwGetTime() * 0.05f, 1.0f));
	_skyShader->setMat4("iview", glm::inverse(
		_activeCamera->getViewMatrix()));
	_skyShader->setMat4("iprojection", glm::inverse(
		_activeCamera->getProjectionMatrix()));

	glDepthMask(GL_FALSE);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDepthMask(GL_TRUE);
}

void Renderer::endDraw(float fade) {
	// switch to main render target
	_context->gl->bindFBO(0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw framebuffer to screen
	_context->gl->setLineMode(false);
	_context->gl->useShader(_screenShader->id);
	_context->gl->bindVAO(_screenVao);
	_context->gl->bindTexture0(_colorBuffer);

	_screenShader->setInt("mainTex", 0);
	_screenShader->setVec2("window", _context->window->getSize());
	_screenShader->setFloat("fade", fade);

	glDrawArrays(GL_TRIANGLES, 0, 6);
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

void Renderer::drawAxis(Transformable* tform, float size) {
	drawLine(tform->getPosition(), tform->getPosition()
		+ tform->right() * size, vec4(1, 0, 0, 1));
	drawLine(tform->getPosition(), tform->getPosition()
		+ tform->up() * size, vec4(0, 1, 0, 1));
	drawLine(tform->getPosition(), tform->getPosition()
		+ tform->forward() * size, vec4(0, 0, 1, 1));
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

void Renderer::buildFramebuffers() {
	vec2 window = _context->window->getSize();

	// build color buffer
	glBindTexture(GL_TEXTURE_2D, _colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.x, window.y,
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// build depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 
		window.x, window.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// build framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, _colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, _depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer is incomplete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Renderer::createScreenBuffer() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &_screenVao);
	glGenBuffers(1, &_screenVbo);

	glBindBuffer(GL_ARRAY_BUFFER, _screenVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 
		vertices, GL_STATIC_DRAW);

	glBindVertexArray(_screenVao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
		4 * sizeof(GLfloat), (GLvoid*)0);
}