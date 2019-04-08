#pragma once
#include "RoadSegment.h"
#include "Context.h"
#include "HeightMap.h"
#include <vector>

class Road {
	friend class RoadGenerator;

public:
	Road(Context* context);
	void debugDraw(float distanceBetweenPoints);
	void draw();
	void buildMeshes();

	void addSegment(float angle, float radius, Orientation orientation);
	void closeLoop();
	void removeLastSegment();
	void clear();

	void initializeHeightmap(float distance, float interval, 
		float range);
	void clearHeightmap();

	float length();
	vec3 pointAtDistance(float distance);

private:
	Context* _context;

	std::vector<RoadSegment> _road;
	float _length;
	float _width = 1.0f;

	HeightMap _heightMap;

	std::vector<GLuint> _vao;
	std::vector<GLuint> _vbo;
	std::vector<GLuint> _ebo;
	std::vector<GLuint> _numIndices;
};