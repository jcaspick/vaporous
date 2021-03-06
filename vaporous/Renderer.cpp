#include "Renderer.h"
#include "Context.h"

#include <glm/gtx/norm.hpp>

Renderer::Renderer(Context* context) :
	_context(context),
	_activeCamera(nullptr),
	_debugShader(nullptr)
{}

Renderer::~Renderer() {
	_context->eventMgr->unsubscribe(EventType::WindowResize, this);
}

void Renderer::init() {
	// load resources
	_debugShader = _context->resourceMgr->loadShader(Shaders::SingleColor,
		"resources/shaders/basic.vert", "resources/shaders/basic_singleColor.frag");
	_screenShader = _context->resourceMgr->loadShader(Shaders::Screen,
		"resources/shaders/screen.vert", "resources/shaders/post_dither.frag");
	_blurShader = _context->resourceMgr->loadShader(Shaders::GaussianBlur,
		"resources/shaders/screen.vert", "resources/shaders/gaussian_blur.frag");
	_skyShader = _context->resourceMgr->loadShader(Shaders::Sky,
		"resources/shaders/screen.vert", "resources/shaders/gradient_sky.frag");
	_skyboxShader = _context->resourceMgr->loadShader(Shaders::Skybox,
		"resources/shaders/skybox.vert", "resources/shaders/skybox.frag");
	_context->resourceMgr->loadTexture(Textures::Watermark,
		"resources/images/watermark.png", true, false);

	// generate framebuffers
	glGenFramebuffers(1, &_fbo);
	glGenTextures(1, &_colorBuffer);
	glGenTextures(1, &_glowBuffer);
	glGenRenderbuffers(1, &_depthBuffer);
	glGenFramebuffers(2, _blurFbo);
	glGenTextures(2, _blurBuffer);
	buildFramebuffers();
	buildCubemapFramebuffer();
	buildBlurFramebuffer();

	// create vaos for different types of drawing
	createPointBuffer();
	createLineBuffer();
	createCircleBuffer();
	createScreenBuffer();
	createSkyboxBuffer();

	// subscribe to window resize events
	_context->eventMgr->subscribe(EventType::WindowResize, this);
}

void Renderer::setCamera(Camera* camera) {
	_activeCamera = camera;
}

Camera* Renderer::getCamera() {
	return _activeCamera;
}

GLsizei Renderer::getCubemapSize() {
	return _cubeMapSize;
}

void Renderer::beginDraw() {
	// switch to framebuffer
	_context->gl->bindFBO(_fbo);
	_context->gl->setNumBuffers(2);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_context->gl->setNumBuffers(1);

	// set viewport size to window size
	_context->gl->setViewportSize(
		static_cast<GLsizei>(_context->window->getSize().x),
		static_cast<GLsizei>(_context->window->getSize().y));
}

void Renderer::applyBlur() {
	// perform blur at half resolution to get a more dramatic effect
	// in fewer iterations
	vec2 window = _context->window->getSize();
	_context->gl->setViewportSize(window.x / 2, window.y / 2);

	// ping-pong between two buffers, increasing blur amount
	int buffer = 0;
	for (int i = 0; i < 6; i++) {
		_context->gl->bindFBO(_blurFbo[buffer]);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		_context->gl->setLineMode(false);
		_context->gl->useShader(_blurShader->id);
		_context->gl->bindVAO(_screenVao);
		if (i == 0) _context->gl->bindTexture0(_glowBuffer);
		else _context->gl->bindTexture0(_blurBuffer[!buffer]);

		_blurShader->setInt("horizontal", buffer);
		_blurShader->setInt("image", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		buffer = !buffer;
	}

	// restore original resolution
	_context->gl->setViewportSize(window.x, window.y);
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
	_context->gl->bindTexture1(_blurBuffer[1]);
	_context->resourceMgr->bindTexture(Textures::Watermark, 3);

	_screenShader->setInt("mainTex", 0);
	_screenShader->setInt("blurTex", 1);
	_screenShader->setInt("watermark", 3);
	_screenShader->setVec2("window", _context->window->getSize());
	_screenShader->setFloat("fade", fade);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::bindCubemapFace(GLuint faceIndex) {
	// bind cubemap framebuffer and use specified face as color attachment
	_context->gl->bindFBO(_cubeFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, _cube, 0);

	// set viewport to cubemap size
	_context->gl->setViewportSize(_cubeMapSize, _cubeMapSize);

	// clear buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawSky() {
	// draw sky
	_context->gl->setLineMode(false);
	_context->gl->useShader(_skyShader->id);
	_context->gl->bindVAO(_screenVao);

	_skyShader->setFloat("hueShift", glfwGetTime() * 0.01f);
	_skyShader->setMat4("iview", glm::inverse(
		_activeCamera->getViewMatrix()));
	_skyShader->setMat4("iprojection", glm::inverse(
		_activeCamera->getProjectionMatrix()));

	glDepthMask(GL_FALSE);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDepthMask(GL_TRUE);
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

void Renderer::drawMesh(Mesh& mesh, mat4 tform, Shader* shader, 
	float alpha) const 
{
	if (!_activeCamera) {
		std::cout << "can't render, no camera set" << std::endl;
		return;
	}

	if (!mesh.isBound()) return;

	_context->gl->useShader(shader->id);
	_context->gl->bindVAO(mesh._vao);
	_context->gl->setLineMode(false);
	_context->gl->bindCubemap4(_cube);

	shader->setMat4("model", tform);
	shader->setMat4("view", _activeCamera->getViewMatrix());
	shader->setMat4("projection", _activeCamera->getProjectionMatrix());
	shader->setVec3("cameraPos", _activeCamera->getPosition());
	shader->setFloat("alpha", alpha);
	shader->setInt("mainTex", 0);
	shader->setInt("reflectiveMap", 1);
	shader->setInt("cubeMap", 4);

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

	// build glow buffer
	glBindTexture(GL_TEXTURE_2D, _glowBuffer);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, _glowBuffer, 0);
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

void Renderer::createSkyboxBuffer() {
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &_skyboxVao);
	glGenBuffers(1, &_skyboxVbo);

	glBindBuffer(GL_ARRAY_BUFFER, _skyboxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),
		skyboxVertices, GL_STATIC_DRAW);

	glBindVertexArray(_skyboxVao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), (GLvoid*)0);
}

void Renderer::buildCubemapFramebuffer() {
	// create empty cubemap
	glGenTextures(1, &_cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cube);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			_cubeMapSize, _cubeMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// create framebuffer for rendering to the cubemap
	glGenFramebuffers(1, &_cubeFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _cubeFbo);

	// create depth buffer
	glGenRenderbuffers(1, &_cubeDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _cubeDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _cubeMapSize, _cubeMapSize);

	// complete framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, _cubeDepthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, _cube, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error: cubemap framebuffer is incomplete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::buildBlurFramebuffer() {
	vec2 window = _context->window->getSize();

	for (int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, _blurFbo[i]);
		glBindTexture(GL_TEXTURE_2D, _blurBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.x / 2, window.y / 2, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			_blurBuffer[i], 0);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error: blur framebuffer is incomplete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::WindowResize:
		buildFramebuffers();
		buildBlurFramebuffer();
		break;
	}
}