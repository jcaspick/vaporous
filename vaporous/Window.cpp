#include "Window.h"
#include <iostream>

Window::Window(int width, int height, const char* title) :
	_width(width),
	_height(height),
	_inputMgr(this)
{
	// create window and OpenGL context
	_window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (_window == NULL) {
		throw std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(_window);

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, glfwKeyCallback);
	glfwSetMouseButtonCallback(_window, glfwMouseButtonCallback);
	glfwSetScrollCallback(_window, glfwScrollCallback);
	glfwSetCursorPosCallback(_window, glfwCursorPosCallback);
	glfwSetFramebufferSizeCallback(_window, glfwResizeCallback);
}

Window::~Window() {
	glfwDestroyWindow(_window);
}

void Window::init() {
	// create viewport
	glViewport(0, 0, _width, _height);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::close() {
	glfwSetWindowShouldClose(_window, GL_TRUE);
}

bool Window::isOpen() {
	return !glfwWindowShouldClose(_window);
}

void Window::beginDraw() {
	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::endDraw() {
	glfwSwapBuffers(_window);
}

void Window::glfwKeyCallback(GLFWwindow* window, int key, 
	int scancode, int action, int mods) 
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->
		_inputMgr.handleKeyboard(key, action);
}

void Window::glfwMouseButtonCallback(GLFWwindow* window, int button,
	int action, int mods)
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->
		_inputMgr.handleMouseButton(button, action);
}

void Window::glfwScrollCallback(GLFWwindow* window, double xoffset,
	double yoffset)
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->
		_inputMgr.handleScroll(xoffset, yoffset);
}

void Window::glfwCursorPosCallback(GLFWwindow* window,
	double xpos, double ypos) 
{
	Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	thisWindow->_inputMgr.handleMouse(xpos, thisWindow->_height - ypos);
}

void Window::glfwResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	thisWindow->_width = width;
	thisWindow->_height = height;
	thisWindow->getEventMgr()->invoke(EventType::WindowResize, EventData());
}

GLFWwindow* Window::getGlfwWindow() {
	return _window;
}

EventManager* Window::getEventMgr() {
	return &_eventMgr;
}

InputManager* Window::getInputMgr() { 
	return &_inputMgr; 
}

vec2 Window::getSize() {
	return vec2(_width, _height);
}

void Window::setClearColor(vec4 color) {
	_clearColor = color;
}