#include "Demo.h"
#include "Utilities.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Demo::Demo() :
	_lastFrame(0),
	_resourceMgr(&_context),
	_renderer(&_context),
	_roadGenerator(&_context),
	_road(_roadGenerator.getRoad()),
	_car(&_context),
	_city(&_context)
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

	// initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(_window->getGlfwWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	// initialize window (create viewport)
	_window->init();
	_window->setClearColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// connect systems
	_context.gl = &_glContext;
	_context.window = _window.get();
	_context.inputMgr = _window->getInputMgr();
	_context.eventMgr = _window->getEventMgr();
	_context.resourceMgr = &_resourceMgr;
	_context.renderer = &_renderer;

	// initialize entities
	_mainCam.setScreenSize(_window->getSize());
	_mainCam.setFOV(75.0f);
	getNextCameraMovement();
	getNextRoadSettings();
	_car.init();

	// subscribe to events
	_context.eventMgr->subscribe(EventType::KeyDown, this);
	_context.eventMgr->subscribe(EventType::WindowResize, this);
}

Demo::~Demo() {
	_context.eventMgr->unsubscribe(EventType::KeyDown, this);
	_context.eventMgr->unsubscribe(EventType::WindowResize, this);
	glfwTerminate();
}

void Demo::init() {
	_resourceMgr.loadTexture(Textures::Rainbow,
		"resources/gradients.png", false);
	_resourceMgr.loadTexture(Textures::CarDiffuse,
		"resources/car_diffuse.png", false);
	_resourceMgr.loadShader(Shaders::BasicTextured,
		"shaders/basic.vert", "shaders/basic_textured.frag");

	// create camera
	_debugCam = p_Camera(new OrbitCamera(&_context));

	// initialize renderer
	_renderer.init();
	_renderer.setCamera(&_mainCam);

	// draw blank frame
	_renderer.endDraw(_fade);

	_city.init();
}

void Demo::run() {
	while (_window->isOpen()) {
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - _lastFrame;
		_lastFrame = currentFrame;
		update(std::min(_maxStep, deltaTime));

		draw();
		glfwPollEvents();
	}
}

void Demo::update(float dt) {
	_elapsed += dt;

	if (_hasWorld) {
		quat drift = glm::angleAxis(glm::radians(
			Util::easeInCubic(_xOffsets.sample(_carDistance))
			* _driftAngle), vec3(0, 1, 0));

		_car.setPosition(pointOnMotionPath(_carDistance));
		_car.setRotation(rotOnMotionPath(_carDistance
			+ _carRotationOffset) * drift);

		if (!_camMove->update(dt)) getNextCameraMovement();
		_mainCam.setPosition(pointOnMotionPath(_carDistance
			- _camMove->camDistance()) + _camMove->camOffset());
		_mainCam.setTarget(pointOnMotionPath(_carDistance
			+ _camMove->targetDistance()));

		_carDistance += dt * _carSpeed;
	}

	if (_debugMode) {
		_debugCam->handleInput();
	}

	switch (_state) {
	case State::Begin:
		generateWorld();
		setState(State::FadeIn);
		break;
	case State::FadeIn:
		_fade = Util::lerp(1.0f, 0.0f, _elapsed / 1.0f);
		if (_elapsed > 1.0f) {
			_fade = 0.0f;
			setState(State::Running);
		}
		break;
	case State::FadeOut:
		_fade = Util::lerp(0.0f, 1.0f, _elapsed / 0.5f);
		if (_elapsed > 0.5f) {
			_fade = 1.0f;
			_carDistance = 0.0f;
			setState(State::Begin);
		}
		break;
	case State::Running:
		break;
	}
}

void Demo::setState(State state) {
	_state = state;
	_elapsed = 0.0f;
}

void Demo::draw() {
	_window->beginDraw();
	_renderer.beginDraw();

	_city.draw();
	_resourceMgr.bindTexture(Textures::CarDiffuse);
	_car.draw();
	_resourceMgr.bindTexture(Textures::Rainbow);
	_road.draw();
	if (_drawMotionPath) drawMotionPath();

	_renderer.endDraw(_fade);

	if (_debugMode) {
		drawUI();
	}

	_window->endDraw();
}

void Demo::generateWorld() {
	_hasWorld = false;
	_roadGenerator.setRoadSettings(_generatorSettings);
	do _roadGenerator.generate();
	while (!_roadGenerator.hasRoad());
	_city.setCenter(_roadGenerator.getRoadCenter());
	_city.generate(_generatorSettings.worldRadius, _roadGenerator.getSamples());
	buildMotionPath();
	_road.buildMesh();
	_debugCam->setPosition(_roadGenerator.getRoadCenter());
	getNextCameraMovement();
	_hasWorld = true;
}

void Demo::buildMotionPath() {
	_xOffsets.clear();

	// snap the interval to a value that divides evenly into the road length
	float scaledInterval = _motionPathInterval + fmod(_road.length(), 
		_motionPathInterval) / floor(_road.length() / _motionPathInterval);
	_xOffsets.setInterval(scaledInterval);

	for (float d = 0.0f; d < _road.length() - 0.5f; d += scaledInterval) {
		_xOffsets.addValue(offsetAtDistance(d));
	}
	_xOffsets.addValue(offsetAtDistance(0.0f));
}

void Demo::getNextCameraMovement() {
	CameraStyle previousStyle = _camMove ? _camMove->style : CameraStyle::None;
	_camMove.reset();

	std::vector<CameraStyle> styles = {
		CameraStyle::Chase,
		CameraStyle::Overhead,
		CameraStyle::ReverseChase,
		CameraStyle::Rotating,
		CameraStyle::Whoosh
	};
	styles.erase(std::remove_if(styles.begin(), styles.end(), 
		[previousStyle](CameraStyle style) { return style == previousStyle;
	}), styles.end());
	CameraStyle nextStyle = styles.at(Util::randomInt(0, styles.size() - 1));

	if (nextStyle == CameraStyle::Chase) 
		_camMove = p_CamMovement(new Cam_Chase());
	else if (nextStyle == CameraStyle::Overhead) 
		_camMove = p_CamMovement(new Cam_Overhead());
	else if (nextStyle == CameraStyle::ReverseChase) 
		_camMove = p_CamMovement(new Cam_ReverseChase());
	else if (nextStyle == CameraStyle::Rotating) 
		_camMove = p_CamMovement(new Cam_Rotating());
	else if (nextStyle == CameraStyle::Whoosh) 
		_camMove = p_CamMovement(new Cam_Whoosh());
}

void Demo::getNextRoadSettings() {
	float r = Util::randomRange(0.0f, 1.0f);
	// simple circuit
	if (r < 0.33f) {
		_generatorSettings.goalLength = 400.0f;
		_generatorSettings.worldRadius = 150.0f;
		_generatorSettings.pStraight = 0.1875f;
		_generatorSettings.pShallow = 0.1875f;
		_generatorSettings.pModerate = 0.5f;
		_generatorSettings.pSharp = 0.125f;
	}
	// extra loopy
	else if (r < 0.66f) {
		_generatorSettings.goalLength = 750.0f;
		_generatorSettings.worldRadius = 200.0f;
		_generatorSettings.pStraight = 0.0;
		_generatorSettings.pShallow = 0.22f;
		_generatorSettings.pModerate = 0.78f;
		_generatorSettings.pSharp = 0.0f;
	}
	// long and leisurely
	else {
		_generatorSettings.goalLength = 850.0f;
		_generatorSettings.worldRadius = 300.0f;
		_generatorSettings.pStraight = 0.430f;
		_generatorSettings.pShallow = 0.5f;
		_generatorSettings.pModerate = 0.07f;
		_generatorSettings.pSharp = 0.0f;
	}
}

float Demo::offsetAtDistance(float d) {
	float total = 0;
	for (int i = 0; i < _numSamples; i++) {
		float sampleDistance = d + (i * (_sampleRange / _numSamples)
			- (_sampleRange / 2)) + _rangeOffset;
		total += _road.sharpnessAtDistance(sampleDistance);
	}
	return total / _numSamples;
}

vec3 Demo::pointOnMotionPath(float d) {
	vec3 center = _road.pointAtDistance(d);
	vec3 right = Util::rotateVec3(vec3(-1, 0, 0),
		glm::toMat4(_road.rotationAtDistance(d)));
	return center + _xOffsets.sample(d) * _offsetStrength * right;
}

quat Demo::rotOnMotionPath(float d) {
	vec3 point = pointOnMotionPath(d);
	vec3 forward = pointOnMotionPath(d - 1.0f);

	return glm::inverse(glm::lookAt(point, forward, vec3(0, 1, 0)));
}

void Demo::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::KeyDown:
		if (data.intData == GLFW_KEY_ESCAPE)
			_window->close();
		if (data.intData == GLFW_KEY_R) {
			if (!_useCustomRoadSettings) getNextRoadSettings();
			setState(State::FadeOut);
		}
		if (data.intData == GLFW_KEY_C) {
			getNextCameraMovement();
		}
		if (data.intData == GLFW_KEY_GRAVE_ACCENT) {
			toggleDebugMode();
		}
		break;
	case EventType::WindowResize:
		_mainCam.setScreenSize(_window->getSize());
		break;
	}
}

void Demo::toggleDebugMode() {
	_debugMode = !_debugMode;
	if (_debugMode && _useDebugCam) _renderer.setCamera(_debugCam.get());
	else if (!_debugMode) _renderer.setCamera(&_mainCam);
}

void Demo::drawMotionPath() {
	if (!_xOffsets.empty()) {
		float interval = _motionPathInterval + fmod(_road.length(), _motionPathInterval)
			/ floor(_road.length() / _motionPathInterval);

		// draw path
		vec3 last = pointOnMotionPath(0.0f);
		for (float d = 0.0f; d < _road.length(); d += interval / 4.0f) {
			vec3 point = pointOnMotionPath(d);
			_renderer.drawLine(last, point, vec4(1, 0.5f, 1, 1));
			last = point;
		}

		// draw control points
		for (float d = 0.0f; d < _road.length(); d += interval) {
			_renderer.drawPoint(_road.pointAtDistance(d), vec4(1, 0, 1, 1), 0.5f);
		}
	}
}

void Demo::drawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::DragFloat("Car speed", &_carSpeed, 0.1f);

	if (ImGui::CollapsingHeader("Camera")) {
		if (ImGui::Checkbox("Use world camera", &_useDebugCam)) {
			if (_useDebugCam) _renderer.setCamera(_debugCam.get());
			else _renderer.setCamera(&_mainCam);
		}
		if (ImGui::Checkbox("Use custom camera", &_useCustomCamera)) {
			if (_useCustomCamera) {
				_camMove.reset();
				_camMove = p_CamMovement(new Cam_Rotating());
				_camMove->_duration = FLT_MAX;
			}
			else {
				getNextCameraMovement();
			}
		}
		if (_useCustomCamera) {
			ImGui::DragFloat("Camera distance", &_camMove->_camDistance, 0.1f);
			ImGui::DragFloat("Target distance", &_camMove->_targetDistance, 0.1f);
			ImGui::DragFloat("Camera height", &_camMove->_camHeight, 0.1f);
			ImGui::Checkbox("Use rotation", &_camMove->_useRotation);
			ImGui::DragFloat("Rotation radius", &_camMove->_rotationRadius, 0.1f);
			ImGui::DragFloat("Rotation speed", &_camMove->_rotationSpeed, 0.02f);
		}
	}

	if (ImGui::CollapsingHeader("Road generator")) {
		ImGui::Checkbox("Use custom road settings", &_useCustomRoadSettings);
		if (_useCustomRoadSettings) {
			ImGui::DragFloat("Road length", &_generatorSettings.goalLength);
			ImGui::DragFloat("World radius", &_generatorSettings.worldRadius);

			ImGui::Text("Segment type ratios");
			if (ImGui::SliderFloat("Straight", &_generatorSettings.pStraight, 0.0f, 1.0f))
				normalizeRoadSettings();
			if (ImGui::SliderFloat("Shallow curve", &_generatorSettings.pShallow, 0.0f, 1.0f))
				normalizeRoadSettings();
			if (ImGui::SliderFloat("Moderate curve", &_generatorSettings.pModerate, 0.0f, 1.0f))
				normalizeRoadSettings();
			if (ImGui::SliderFloat("Sharp curve", &_generatorSettings.pSharp, 0.0f, 1.0f))
				normalizeRoadSettings();
		}

		if (ImGui::Button("Generate")) {
			if (!_useCustomRoadSettings) getNextRoadSettings();
			setState(State::FadeOut);
		}
	}

	if (ImGui::CollapsingHeader("Motion path")) {
		ImGui::Checkbox("Draw motion path", &_drawMotionPath);

		ImGui::Text("Motion path settings");
		ImGui::DragFloat("Interval", &_motionPathInterval, 0.5f, 1.0f);
		ImGui::DragFloat("Range", &_sampleRange, 0.5f, 0.0f);
		ImGui::DragFloat("Offset", &_rangeOffset);
		ImGui::DragInt("Samples", &_numSamples, 1.0f, 0.0f);
		ImGui::DragFloat("Curve strength", &_offsetStrength, 0.1f);

		if (ImGui::Button("Rebuild motion path")) buildMotionPath();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Demo::normalizeRoadSettings() {
	float total = _generatorSettings.pStraight + _generatorSettings.pShallow
		+ _generatorSettings.pModerate + _generatorSettings.pSharp;
	_generatorSettings.pStraight /= total;
	_generatorSettings.pShallow /= total;
	_generatorSettings.pModerate /= total;
	_generatorSettings.pSharp /= total;
}