#include "Demo.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

Demo::Demo() :
	_lastFrame(0),
	_resourceMgr(&_context)
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

	// subscribe to events
	_context.eventMgr->subscribe(EventType::KeyDown, this);

	// camera
	_cam = p_Camera(new OrbitCamera(&_context));
}

Demo::~Demo() {
	_context.eventMgr->unsubscribe(EventType::KeyDown, this);
	glfwTerminate();
}

void Demo::init() {
	arc = MeshUtil::arcMesh(5, 45, 2, 8);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, arc.vertices.size() * sizeof(Vertex), 
		&arc.vertices[0], GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	
	// texture coord attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, uv));

	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, arc.indices.size() * sizeof(GLuint),
		&arc.indices[0], GL_STATIC_DRAW);

	

	_resourceMgr.loadTexture(Textures::YungFrink, 
		"resources/yungFrink.png", false);
	_resourceMgr.loadTexture(Textures::Rainbow,
		"resources/gradients.png", false);
	_resourceMgr.bindTexture(Textures::YungFrink);

	shader = _resourceMgr.loadShader(Shaders::BasicTextured, 
		"shaders/basic.vert", "shaders/basic_textured.frag");
	_resourceMgr.bindShader(Shaders::BasicTextured);
}

void Demo::run() {
	while (_window->isOpen()) {
		float currentFrame = glfwGetTime();
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

	_glContext.bindVAO(vao);
	shader->setMat4("model", mat4(1));
	shader->setMat4("view", _cam->getViewMatrix());
	shader->setMat4("projection", _cam->getProjectionMatrix());
	glDrawElements(GL_TRIANGLES, arc.indices.size(), GL_UNSIGNED_INT, 0);

	_window->endDraw();
}

void Demo::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::KeyDown:
		if (data.intData == GLFW_KEY_ESCAPE)
			_window->close();
		break;
	}
}