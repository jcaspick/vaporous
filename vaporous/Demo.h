#pragma once
#include "Context.h"
#include "GL.h"
#include "OrbitCamera.h"
#include "Observer.h"
#include "MeshUtilities.h"
#include "Arc.h"

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
	void initDebugGeo();
	void drawPoint(vec3 point, vec4 color, float size = 1);

	Context _context;
	GL _glContext;
	p_Window _window;
	ResourceManager _resourceMgr;
	p_Camera _cam;

	float _lastFrame;

	GLuint vbo, ebo, vao;
	Shader* shader;

	GLuint pointVbo, pointVao;
	Shader* _debugShader;

	Mesh arc;
	aArc arc2;
};