#pragma once
#include "Context.h"
#include "GL.h"
#include "OrbitCamera.h"
#include "TargetCamera.h"
#include "ReflectionCamera.h"
#include "Observer.h"
#include "RoadGenerator.h"
#include "Car.h"
#include "LoopingSpline.h"
#include "City.h"
#include "CameraMovements.h"

#include <memory>

using p_Window = std::unique_ptr<Window>;
using p_Camera = std::unique_ptr<OrbitCamera>;
using p_CamMovement = std::unique_ptr<CameraMovement>;

enum class State {
	Begin,
	FadeIn,
	Running,
	FadeOut,
};

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
	void generateWorld();
	void buildMotionPath();
	void getNextCameraMovement();
	void getNextRoadSettings();
	void normalizeRoadSettings();

	// engine
	Context _context;
	GL _glContext;
	p_Window _window;
	ResourceManager _resourceMgr;
	Renderer _renderer;
	float _lastFrame;

	// scene objects
	p_Camera _debugCam;
	TargetCamera _mainCam;
	RoadGenerator _roadGenerator;
	Road& _road;
	Car _car;
	City _city;

	// logic
	void setState(State state);
	float _elapsed = 0.0f;
	float _maxStep = 1.0f / 60.0f;
	bool _hasWorld = false;
	State _state = State::Begin;

	// entities
	float _carDistance = 0.0f;
	float _carSpeed = 20.0f;
	float _carRotationOffset = 4.0f;
	float _driftAngle = 120.0f;
	p_CamMovement _camMove;
	RoadSettings _generatorSettings;

	// motion path
	float offsetAtDistance(float d);
	vec3 pointOnMotionPath(float d);
	quat rotOnMotionPath(float d);
	LoopingSpline _xOffsets;
	float _motionPathInterval = 10.0f;
	float _sampleRange = 50.0f;
	float _rangeOffset = -15.0f;
	int _numSamples = 50;
	float _offsetStrength = 2.5f;

	// rendering
	float _fade = 1.0f;
	ReflectionCamera _reflectionCam;

	// debug
	void toggleDebugMode();
	bool _debugMode = false;
	bool _useDebugCam = false;
	bool _useCustomRoadSettings = false;
	bool _useCustomCamera = false;
	bool _drawMotionPath = false;
};