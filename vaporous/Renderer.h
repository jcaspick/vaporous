#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Transformable.h"
#include "Observer.h"

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using quat = glm::quat;

struct Context;
class Renderer : public Observer {
public:
	Renderer(Context* context);
	~Renderer();

	void init();
	void setCamera(Camera* camera);
	Camera* getCamera();
	virtual void handleEvent(EventType type, EventData data) override;

	void beginDraw();
	void endDraw(float fade);
	void drawPoint(vec3 point, vec4 color, float size = 1.0f);
	void drawLine(vec3 a, vec3 b, vec4 color);
	void drawCircle(vec3 center, float radius, vec4 color);
	void drawMesh(Mesh& mesh, mat4 tform, Shader* shader, 
		float alpha = 1.0f) const;
	void drawAxis(Transformable* tform, float size = 1.0f);
	void drawSky();
	void beginCubemapDraw(GLuint faceIndex);

private:
	void createPointBuffer();
	void createLineBuffer();
	void createCircleBuffer();
	void createScreenBuffer();
	void buildFramebuffers();

	void initReflections();

	Context* _context;
	Camera* _activeCamera;
	Shader* _debugShader;
	Shader* _screenShader;
	Shader* _skyShader;
	Shader* _skyboxShader;

	GLuint _fbo, _colorBuffer, _depthBuffer;
	GLuint _cube, _cubeFbo, _cubeDepthBuffer;
	GLsizei _cubeMapSize = 256;

	GLuint _pointVbo, _pointVao;
	GLuint _lineVbo, _lineVao;
	GLuint _circleVbo, _circleVao;
	GLuint _screenVbo, _screenVao;
	GLuint _skyboxVbo, _skyboxVao;

	int _circleResolution = 64;
};