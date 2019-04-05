#include "Demo.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

Demo::Demo() :
	_lastFrame(0),
	_resourceMgr(&_context),
	_renderer(&_context),
	_road(&_context)
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
	//_window->setClearColor(vec4(0.8f, 0.6f, 0.8f, 1.0f));
	_window->setClearColor(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	// connect systems
	_context.gl = &_glContext;
	_context.window = _window.get();
	_context.inputMgr = _window->getInputMgr();
	_context.eventMgr = _window->getEventMgr();
	_context.resourceMgr = &_resourceMgr;
	_context.renderer = &_renderer;

	// subscribe to events
	_context.eventMgr->subscribe(EventType::KeyDown, this);
}

Demo::~Demo() {
	_context.eventMgr->unsubscribe(EventType::KeyDown, this);
	glfwTerminate();
}

void Demo::init() {
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glGenBuffers(1, &ebo);

	//glBindVertexArray(vao);

	//// vertices
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, arc.vertices.size() * sizeof(Vertex), 
	//	&arc.vertices[0], GL_STATIC_DRAW);

	//// position attribute
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	//	(void*)offsetof(Vertex, position));
	//
	//// texture coord attribute
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	//	(void*)offsetof(Vertex, uv));

	//// indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, arc.indices.size() * sizeof(GLuint),
	//	&arc.indices[0], GL_STATIC_DRAW);

	_resourceMgr.loadTexture(Textures::YungFrink, 
		"resources/yungFrink.png", false);
	_resourceMgr.loadTexture(Textures::Rainbow,
		"resources/gradients.png", false);
	_resourceMgr.bindTexture(Textures::YungFrink);

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
}

void Demo::draw() {
	_window->beginDraw();

	_renderer.drawPoint(vec3(0), vec4(1, 1, 1, 0.2f), 1);
	_renderer.drawPoint(vec3(1, 0, 0), vec4(1, 0, 0, 1), 0.1f);
	_renderer.drawPoint(vec3(0, 1, 0), vec4(0, 1, 0, 1), 0.1f);
	_renderer.drawPoint(vec3(0, 0, 1), vec4(0, 0, 1, 1), 0.1f);

	_road.debugDraw(0.5f);

	_window->endDraw();
}

void Demo::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::KeyDown:
		if (data.intData == GLFW_KEY_ESCAPE)
			_window->close();
		if (data.intData == GLFW_KEY_A) {
			_road.addSegment(30, 5, Orientation::Left);
		}
		if (data.intData == GLFW_KEY_S) {
			_road.addSegment(30, 5, Orientation::Right);
		}
		if (data.intData == GLFW_KEY_D) {
			_road.clear();
		}
		break;
	}
}