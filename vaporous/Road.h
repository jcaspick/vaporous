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
	void removeLastSegment();
	void clear();

	float length();
	vec3 pointAtDistance(float distance);

private:
	Context* _context;

	std::vector<RoadSegment> _road;
	float _length;
};