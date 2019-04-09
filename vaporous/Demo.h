#pragma once
#include "Context.h"
#include "GL.h"
#include "OrbitCamera.h"
#include "FreeCamera.h"
#include "Observer.h"
#include "RoadGenerator.h"

#include <memory>

using p_Window = std::unique_ptr<Window>;
using p_Camera = std::unique_ptr<OrbitCamera>;

class Demo : public Observer {
public:
	Demo();
	~Demo();

	void init();
	void run();
	void update(float dt);
	void draw();
	virtual void handleEvent(EventType type, EventData data) override;

private:
	void toggleRoadCam();

	Context _context;
	GL _glContext;
	p_Window _window;
	ResourceManager _resourceMgr;
	Renderer _renderer;
	p_Camera _cam;
	FreeCamera _roadCam;

	RoadGenerator _roadGenerator;
	Road& _road;
	Mesh _car;

	float _lastFrame;

	bool camAttached = false;
	float camDistance = 0.0f;
};