#include "HeightMap.h"
#include "Utilities.h"
#include <glm/glm.hpp>

using glm::clamp;

HeightMap::HeightMap() :
	_length(0)
{
	_heightMap.emplace_back(0.0f, 0.0f);
}

float HeightMap::sample(float distance) {
	if (_heightMap.size() <= 1) {
		return 0.0f;
	}
	if (distance <= 0) {
		return 0.0f;
	}
	else if (distance >= _length) {
		return _heightMap.back().height;
	}

	for (int i = 0; i < _heightMap.size() - 1; i++) {
		if (_heightMap[i + 1].distance >= distance) {
			int maxIdx = static_cast<int>(_heightMap.size() - 1);
			float p0 = _heightMap[clamp(i - 1, 0, maxIdx)].height;
			float p1 = _heightMap[i].height;
			float p2 = _heightMap[i + 1].height;
			float p3 = _heightMap[clamp(i + 2, 0, maxIdx)].height;

			float t = (distance - _heightMap[i].distance) /
				(_heightMap[i + 1].distance - _heightMap[i].distance);
			return CRSpline::interpolate(t, p0, p1, p2, p3);
		}
	}
}

float HeightMap::getLength() {
	return _length;
}

void HeightMap::addPoint(float height, float interval) {
	_length += interval;
	_heightMap.emplace_back(_length, height);
}

void HeightMap::clear() {
	_heightMap.clear();
	_heightMap.emplace_back(0.0f, 0.0f);
	_length = 0;
}