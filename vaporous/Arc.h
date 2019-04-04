#pragma once
#include "MeshUtilities.h"
#include "Transformable.h"
#include "Context.h"
#include "OrbitCamera.h"

class aArc : public Transformable {
public:
	aArc(Context* context, float radius, float angle, 
		float width, float divisions);
	void init();
	void draw(OrbitCamera* cam);

	vec3 getEndPoint();
	quat getEndRot();

private:
	Context* _context;
	Mesh mesh;

	float _angle, _radius;

	unsigned int vao, vbo, ebo;
	Shader* shader;
};