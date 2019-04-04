#pragma once
#include "EventManager.h"
#include <array>
#include <glm\glm.hpp>

using vec2 = glm::vec2;

class Window;
class InputManager {
public:
	InputManager(Window* window);
	void handleKeyboard(int key, int action);
	void handleMouseButton(int button, int action);
	void handleMouse(double xpos, double ypos);
	void handleScroll(double xoffset, double yoffset);
	bool getKey(int key);
	bool getMouseButton(int button);
	vec2 getMousePos();
	vec2 getJoystick(int joystick);
	bool getTrigger(int trigger);

private:
	Window* _window;
	EventManager* _eventMgr;

	std::array<bool, 1024> _keys;
	std::array<bool, 8> _mouseButtons;
	vec2 _mousePos;
};