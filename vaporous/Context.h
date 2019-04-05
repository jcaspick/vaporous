#pragma once
#include "GL.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Renderer.h"

struct Context {
	Context() :
		gl(nullptr),
		window(nullptr),
		inputMgr(nullptr),
		eventMgr(nullptr),
		resourceMgr(nullptr),
		renderer(nullptr)
	{}

	GL* gl;
	Window* window;
	InputManager* inputMgr;
	EventManager* eventMgr;
	ResourceManager* resourceMgr;
	Renderer* renderer;
};