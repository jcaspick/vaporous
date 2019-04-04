#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "EventManager.h"
#include "InputManager.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

class Window {
public:
	Window(int width, int height, const char* title);
	~Window();

	void init();
	void close();
	bool isOpen();
	void beginDraw();
	void endDraw();

	GLFWwindow* getGlfwWindow();
	EventManager* getEventMgr();
	InputManager* getInputMgr();
	vec2 getSize();

	void setClearColor(vec4 color);

	static void glfwKeyCallback(GLFWwindow* window, int key, 
		int scancode, int action, int mods);
	static void glfwMouseButtonCallback(GLFWwindow* window, int button,
		int action, int mods);
	static void glfwScrollCallback(GLFWwindow* window, double xoffset,
		double yoffset);
	static void glfwCursorPosCallback(GLFWwindow* window,
		double xpos, double ypos);
	static void glfwResizeCallback(GLFWwindow* window, int width,
		int height);

private:
	GLFWwindow* _window;
	EventManager _eventMgr;
	InputManager _inputMgr;
	int _width;
	int _height;
	vec4 _clearColor;
};