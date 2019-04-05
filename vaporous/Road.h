#pragma once
#include "RoadSegment.h"
#include <vector>

class Road {
public:
	Road();
	void debugDraw(float distanceBetweenPoints);
	void draw();
	void buildMeshes();

	void addSegment(float angle, float radius, Orientation orientation);
	void removeLastSegment();
	void clear();

	float length();
	vec3 pointAtDistance(float distance);

private:
	std::vector<RoadSegment> _road;
	float _length;
};