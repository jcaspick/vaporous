#include "Demo.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

Demo::Demo() :
	_lastFrame(0),
	_resourceMgr(&_context),
	_renderer(&_context),
	_roadGenerator(&_context),
	_road(_roadGenerator.getRoad())
{
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window & openGL context
	_window = p_Window(new Window(1280, 720, "hello"));

	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::exception("Failed to initialize GLAD");
	}

	// initialize window (create viewport)
	_window->init();
	_window->setClearColor(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	// connect systems
	_context.gl = &_glContext;
	_context.window = _window.get();
	_context.inputMgr = _window->getInputMgr();
	_context.eventMgr = _window->getEventMgr();
	_context.resourceMgr = &_resourceMgr;
	_context.renderer = &_renderer;

	_roadCam.setScreenSize(_window->getSize());
	_roadCam.setFOV(75.0f);

	// subscribe to events
	_context.eventMgr->subscribe(EventType::KeyDown, this);
}

Demo::~Demo() {
	_context.eventMgr->unsubscribe(EventType::KeyDown, this);
	glfwTerminate();
}

void Demo::init() {
	_resourceMgr.loadTexture(Textures::Rainbow,
		"resources/gradients.png", false);
	_resourceMgr.bindTexture(Textures::Rainbow);

	_resourceMgr.loadShader(Shaders::BasicTextured,
		"shaders/basic.vert", "shaders/basic_textured.frag");

	// create camera
	_cam = p_Camera(new OrbitCamera(&_context));

	// initialize renderer
	_renderer.init();
	_renderer.setCamera(_cam.get());
}

void Demo::run() {
	while (_window->isOpen()) {
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - _lastFrame;
		_lastFrame = currentFrame;

		update(deltaTime);
		draw();
		glfwPollEvents();
	}
}

void Demo::update(float dt) {
	_cam->handleInput();
	_roadGenerator.update(dt);

	if (camAttached) {
		_roadCam.setPosition(_road.pointAtDistance(
			camDistance) + vec3(0, 1, 0));
		_roadCam.setRotation(_road.rotationAtDistance(
			camDistance));
		camDistance += dt * 25.0f;
	}
}

void Demo::draw() {
	_window->beginDraw();

	_renderer.drawPoint(vec3(0), vec4(1, 1, 1, 0.2f), 10.0f);
	_renderer.drawPoint(vec3(10, 0, 0), vec4(1, 0, 0, 1));
	_renderer.drawPoint(vec3(0, 10, 0), vec4(0, 1, 0, 1));
	_renderer.drawPoint(vec3(0, 0, 10), vec4(0, 0, 1, 1));

	if (!camAttached) _roadGenerator.draw();
	_road.draw();

	_window->endDraw();
}

void Demo::toggleRoadCam() {
	camAttached = !camAttached;
	camDistance = 0.0f;
	if (camAttached) _renderer.setCamera(&_roadCam);
	else _renderer.setCamera(_cam.get());
}

void Demo::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::KeyDown:
		if (data.intData == GLFW_KEY_ESCAPE)
			_window->close();
		if (data.intData == GLFW_KEY_S) {
			_roadGenerator.start();
		}
		if (data.intData == GLFW_KEY_D) {
			_roadGenerator.reset();
		}
		if (data.intData == GLFW_KEY_B) {
			_road.buildMesh();
		}
		if (data.intData == GLFW_KEY_C) {
			toggleRoadCam();
		}
		break;
	}
}