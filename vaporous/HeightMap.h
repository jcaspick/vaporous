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
	void clear();

private:
	std::vector<HeightPoint> _heightMap;
	float _length;
};