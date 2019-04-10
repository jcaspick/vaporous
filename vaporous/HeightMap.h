#pragma once
#include <vector>

struct HeightPoint {
	HeightPoint(float distance, float height) :
		distance(distance), height(height) {}

	float distance;
	float height;
};

class HeightMap {
public:
	HeightMap();
	float sample(float distance);
	float getLength();
	void addPoint(float height, float interval);
	void setLoopDistance(float loopDistance);
	bool empty();
	void clear();

private:
	std::vector<HeightPoint> _heightMap;
	float _length;
	float _loopDistance = FLT_MAX;
	float _loopSmoothing = 50.0f;
};