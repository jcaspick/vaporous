#pragma once
#include "Context.h"
#include "RoadSegment.h"
#include "HeightMap.h"
#include "Mesh.h"

class Road {
	friend class RoadGenerator;
	friend class LiveRoadGenerator;

public:
	Road(Context* context);
	void debugDraw(float distanceBetweenPoints);
	void draw();
	void buildMesh();

	void addSegment(float angle, float radius, Orientation orientation);
	void closeLoop();
	void removeLastSegment();
	void clear();

	void initializeHeightmap(float distance, float interval, 
		float range);
	void clearHeightmap();

	float length();
	vec3 pointAtDistance(float distance);
	quat rotationAtDistance(float distance);
	float sharpnessAtDistance(float distance);

private:
	Context* _context;

	std::vector<RoadSegment> _road;
	float _length;
	float _width = 4.0f;

	HeightMap _heightMap;
	Mesh _mesh;
};