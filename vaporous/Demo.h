#pragma once
#include "Context.h"
#include "GL.h"
#include "OrbitCamera.h"
#include "TargetCamera.h"
#include "Observer.h"
#include "RoadGenerator.h"
#include "Car.h"
#include "LoopingSpline.h"

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
	void drawMotionPath();
	void drawUI();
	virtual void handleEvent(EventType type, EventData data) override;

private:
	void toggleRunning();
	void buildMotionPath();
	float offsetAtDistance(float d);
	vec3 sampleMotionPath(float d);

	Context _context;
	GL _glContext;
	p_Window _window;
	ResourceManager _resourceMgr;
	Renderer _renderer;
	p_Camera _cam;
	TargetCamera _followCam;

	RoadGenerator _roadGenerator;
	Road& _road;
	Car _car;

	float _lastFrame;

	bool isRunning = false;
	float carDistance = 0.0f;
	float camFollowDistance = 2.0f;
	float camHeight = 0.5f;

	float carSpeed = 20.0f;
	float carRotationOffset = 4.0f;

	LoopingSpline _xOffsets;
	float _motionPathInterval = 10.0f;
	float _sampleRange = 50.0f;
	float _rangeOffset = -15.0f;
	int _numSamples = 50;
	float _offsetStrength = 2.0f;
};