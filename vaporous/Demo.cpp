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
	_car(&_context)
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
	_car.init();

	// subscribe to events
	_context.eventMgr->subscribe(EventType::KeyDown, this);
}

Demo::~Demo() {
	_context.eventMgr->unsubscribe(EventType::KeyDown, this);
	glfwTerminate();
}

void Demo::init() {
	_resourceMgr.loadTexture(Textures::Rainbow,
		"resources/gradients.png", false);
	_resourceMgr.loadTexture(Textures::CarDiffuse,
		"resources/car_diffuse.png", false);
	_resourceMgr.bindTexture(Textures::Rainbow);

	_resourceMgr.loadShader(Shaders::BasicTextured,
		"shaders/basic.vert", "shaders/basic_textured.frag");

	// create camera
	_debugCam = p_Camera(new OrbitCamera(&_context));

	// initialize renderer
	_renderer.init();
	_renderer.setCamera(&_mainCam);

	// draw blank frame
	_renderer.endDraw(_fade);
}

void Demo::run() {
	while (_window->isOpen()) {
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - _lastFrame;
		_lastFrame = currentFrame;

		update(deltaTime);
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

		_mainCam.setPosition(pointOnMotionPath(_carDistance
			- _camFollowDistance) + vec3(0, _camHeight, 0));
		_mainCam.setTarget(_car.getPosition() + _car.forward() * 2.0f);

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

	_resourceMgr.bindTexture(Textures::Rainbow);
	_road.draw();
	_resourceMgr.bindTexture(Textures::CarDiffuse);
	_car.draw();

	_renderer.endDraw(_fade);

	if (_debugMode) {
		if (_drawMotionPath) drawMotionPath();
		drawUI();
	}

	_window->endDraw();
}

void Demo::generateWorld() {
	_hasWorld = false;
	do _roadGenerator.generate();
	while (!_roadGenerator.hasRoad());
	buildMotionPath();
	_road.buildMesh();
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
			setState(State::FadeOut);
		}
		if (data.intData == GLFW_KEY_0) {
			toggleDebugMode();
		}
		break;
	}
}

void Demo::toggleDebugMode() {
	_debugMode = !_debugMode;
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
			_renderer.drawPoint(_road.pointAtDistance(d), vec4(1, 0, 1, 1), 3.0f);
		}
	}
}

void Demo::drawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::DragFloat("screenFade", &_fade, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat("carSpeed", &_carSpeed);
	ImGui::DragFloat("carRotationOffset", &_carRotationOffset);

	ImGui::Separator();

	ImGui::DragFloat("camFollowDistance", &_camFollowDistance, 0.1f);
	ImGui::DragFloat("camHeight", &_camHeight, 0.1f);

	ImGui::Separator();

	ImGui::DragFloat("motionPathInterval", &_motionPathInterval);
	ImGui::DragFloat("sampleRange", &_sampleRange);
	ImGui::DragFloat("rangeOffset", &_rangeOffset);
	ImGui::DragInt("numSamples", &_numSamples);
	ImGui::DragFloat("offsetStrength", &_offsetStrength, 0.1f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}