#pragma once
#include <glm/glm.hpp>

using vec3 = glm::vec3;

namespace CRSpline {
	inline float interpolate(float t, float p0, 
		float p1, float p2, float p3) 
	{
		float a = 0.5f * (2.0f * p1);
		float b = 0.5f * (p2 - p0);
		float c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
		float d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);
		return a + (b * t) + (c * t * t) + (d * t * t * t);
	}
}

namespace Util {
	inline float randomRange(float min, float max) {
		return min + static_cast<float>(rand()) /
			static_cast<float>(RAND_MAX / (max - min));
	}

	inline bool randomBool() {
		return rand() % 2;
	}

	inline vec3 flatten(vec3 v) {
		return vec3(v.x, 0, v.z);
	}
}