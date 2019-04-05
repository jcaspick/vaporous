#pragma once
#include "RoadSegment.h"
#include "Context.h"
#include <vector>

class Road {
public:
	Road(Context* context);
	void debugDraw(float distanceBetweenPoints);
	void draw();
	void buildMeshes();

	void addSegment(float angle, float radius, Orientation orientation);
	void closeLoop();
	void removeLastSegment();
	void clear();

	float length();
	vec3 pointAtDistance(float distance);

private:
	Context* _context;

	std::vector<RoadSegment> _road;
	float _length;
	float _width = 1.0f;

	std::vector<GLuint> _vao;
	std::vector<GLuint> _vbo;
	std::vector<GLuint> _ebo;
	std::vector<GLuint> _numIndices;
};