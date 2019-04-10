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
	_window->setClearColor(vec4(0.1f, 0.1f, 0.1f, 1.0f));

	// connect systems
	_context.gl = &_glContext;
	_context.window = _window.get();
	_context.inputMgr = _window->getInputMgr();
	_context.eventMgr = _window->getEventMgr();
	_context.resourceMgr = &_resourceMgr;
	_context.renderer = &_renderer;

	_roadCam.setScreenSize(_window->getSize());
	_roadCam.setFOV(75.0f);
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
	_cam = p_Camera(new OrbitCamera(&_context));

	// initialize renderer
	_renderer.init();
	_renderer.setCamera(_cam.get());
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
	_cam->handleInput();
	_roadGenerator.update(dt);

	if (isRunning) {
		_roadCam.setPosition(_road.pointAtDistance(
			carDistance - camFollowDistance) + vec3(0, camHeight, 0) + 
			_car.right() * xOffsets.sample(carDistance - camFollowDistance));
		_roadCam.setRotation(_road.rotationAtDistance(
			carDistance - camFollowDistance));

		_car.setPosition(_road.pointAtDistance(carDistance)
			+ _car.right() * xOffsets.sample(carDistance));
		_car.setRotation(_road.rotationAtDistance(
			carDistance + carRotationOffset));

		carDistance += dt * carSpeed;
	}
}

void Demo::draw() {
	_window->beginDraw();

	_renderer.drawPoint(vec3(0), vec4(1, 1, 1, 0.2f), 10.0f);
	_renderer.drawPoint(vec3(10, 0, 0), vec4(1, 0, 0, 1));
	_renderer.drawPoint(vec3(0, 10, 0), vec4(0, 1, 0, 1));
	_renderer.drawPoint(vec3(0, 0, 10), vec4(0, 0, 1, 1));

	if (!xOffsets.empty()) {
		vec3 last = _road.pointAtDistance(0.0f) + Util::rotateVec3(vec3(1, 0, 0),
			glm::toMat4(_road.rotationAtDistance(0.0f))) * xOffsets.sample(0.0f);
		for (float d = 0.0f; d < _road.length(); d += 0.9f) {
			vec3 point = _road.pointAtDistance(d) + Util::rotateVec3(vec3(-1, 0, 0),
				glm::toMat4(_road.rotationAtDistance(d))) * xOffsets.sample(d);
			_renderer.drawLine(last, point, vec4(1, 1, 1, 1));
			last = point;
		}
	}

	_resourceMgr.bindTexture(Textures::Rainbow);
	_road.draw();

	_resourceMgr.bindTexture(Textures::CarDiffuse);
	_car.draw();

	drawUI();

	_window->endDraw();
}

void Demo::toggleRunning() {
	isRunning = !isRunning;
	carDistance = 0.0f;
	if (isRunning) _renderer.setCamera(&_roadCam);
	else _renderer.setCamera(_cam.get());
}

void Demo::buildMotionPath() {
	xOffsets.clear();

	for (float d = 0.0f; d < _road.length(); d += 10.0f) {
		xOffsets.addPoint(getOffsetAtDistance(d), 10.0f);
	}

	xOffsets.setLoopDistance(_road.length());
}

float Demo::getOffsetAtDistance(float d) {
	float total = 0;

	for (int i = 0; i < numSamples; i++) {
		float sampleDistance = d + (i * (sampleRange / numSamples)
			- (sampleRange / 2)) + rangeOffset;
		total += _road.sharpnessAtDistance(sampleDistance);
	}

	return driftStrength * (total / numSamples);
}

void Demo::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::KeyDown:
		if (data.intData == GLFW_KEY_ESCAPE)
			_window->close();
		if (data.intData == GLFW_KEY_S) {
			_roadGenerator.generate();
		}
		if (data.intData == GLFW_KEY_D) {
			_roadGenerator.reset();
		}
		if (data.intData == GLFW_KEY_B) {
			_road.buildMesh();
		}
		if (data.intData == GLFW_KEY_C) {
			toggleRunning();
		}
		if (data.intData == GLFW_KEY_M) {
			buildMotionPath();
		}
		break;
	}
}

void Demo::drawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::DragFloat("carSpeed", &carSpeed);
	ImGui::DragFloat("driftStrength", &driftStrength);
	ImGui::DragFloat("carRotationOffset", &carRotationOffset);

	ImGui::Separator();

	ImGui::DragFloat("camFollowDistance", &camFollowDistance, 0.1f);
	ImGui::DragFloat("camHeight", &camHeight, 0.1f);

	ImGui::Separator();

	ImGui::DragFloat("sampleRange", &sampleRange);
	ImGui::DragFloat("rangeOffset", &rangeOffset);
	ImGui::DragInt("numSamples", &numSamples);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}