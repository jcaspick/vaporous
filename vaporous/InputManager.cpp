#include "InputManager.h"
#include "Window.h"
#include <GLFW\glfw3.h>
#include <iostream>

InputManager::InputManager(Window* window) :
	_window(window),
	_eventMgr(window->getEventMgr()),
	_keys()
{
	std::fill(_keys.begin(), _keys.end(), false);
	std::fill(_mouseButtons.begin(), _mouseButtons.end(), false);
}

void InputManager::handleKeyboard(int key, int action) {
	if (key < 0 || key >= 1024) return;

	if (action == GLFW_PRESS) {
		if (!_keys[key]) _eventMgr->invoke(EventType::KeyDown, key);
		_keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		if (_keys[key]) _eventMgr->invoke(EventType::KeyUp, key);
		_keys[key] = false;
	}
}

void InputManager::handleMouseButton(int button, int action) {
	if (action == GLFW_PRESS) {
		_eventMgr->invoke(EventType::MouseDown, button);
		_mouseButtons[button] = true;
	}
	else if (action == GLFW_RELEASE) {
		_eventMgr->invoke(EventType::MouseUp, button);
		_mouseButtons[button] = false;
	}
}

void InputManager::handleMouse(double xpos, double ypos) {
	_mousePos.x = static_cast<float>(xpos);
	_mousePos.y = static_cast<float>(ypos);
}

void InputManager::handleScroll(double xoffset, double yoffset) {
	if (xoffset != 0) {
		_eventMgr->invoke(EventType::ScrollX, EventData(
			static_cast<float>(xoffset)));
	}
	if (yoffset != 0) {
		_eventMgr->invoke(EventType::ScrollY, EventData(
			static_cast<float>(yoffset)));
	}
}

bool InputManager::getKey(int key) {
	if (key < 0 || key >= 1024) return false;
	return _keys[key];
}

bool InputManager::getMouseButton(int button) {
	if (button < 0 || button >= 8) return false;
	return _mouseButtons[button];
}

vec2 InputManager::getMousePos() {
	return _mousePos;
}

vec2 InputManager::getJoystick(int joystick) {
	vec2 result(0);
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		if (count < 4) return result;

		if (joystick == 0) return vec2(axes[0], axes[1]);
		else if (joystick == 1) return vec2(axes[2], axes[3]);
	}
	return result;
}

bool InputManager::getTrigger(int trigger) {
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		if (count < 6) return false;

		if (trigger == 0) return axes[4] > 0;
		else if (trigger == 1) return axes[5] > 0;
	}
	return false;
}