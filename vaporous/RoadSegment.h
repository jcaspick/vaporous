#pragma once
#include "Transformable.h"
#include "HeightMap.h"

enum class Orientation {
	Right,
	Left
};

class RoadSegment : public Transformable {
public:
	RoadSegment(float angle, float radius, Orientation orientation,
		float distanceOffset, HeightMap* heightMap);
	float arcLength();
	vec3 endPoint();
	quat endRot();
	vec3 pointAtDistance(float distance);

	float angle;
	float radius;
	Orientation orientation;

private:
	HeightMap* _heightMap;
	float _distanceOffset;
};