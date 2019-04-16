#pragma once
#include <glm/glm.hpp>

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

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

	inline float randomInt(float min, float max) {
		return min + (rand() % static_cast<int>(max - min + 1));
	}

	inline bool randomBool() {
		return rand() % 2;
	}

	inline vec3 flatten(vec3 v) {
		return vec3(v.x, 0, v.z);
	}

	inline float lerp(float a, float b, float t)
	{
		return (a * (1.0 - t)) + (b * t);
	}

	inline float easeInCubic(float t) {
		return t * t * t;
	}

	inline float easeOutCubic(float t) {
		return (--t) * t * t + 1;
	}

	inline float easeInOutCubic(float t) {
		return t < 0.5f ? 4 * t * t * t : (t - 1) * 
			(2 * t - 2) * (2 * t - 2) + 1;
	}

	inline vec3 transformVec3(vec3 v, mat4 m) {
		vec4 v2(v, 1);
		v2 = m * v2;
		return { v2.x, v2.y, v2.z };
	}

	inline vec3 rotateVec3(vec3 v, mat4 m) {
		vec4 v2(v, 0);
		v2 = m * v2;
		return { v2.x, v2.y, v2.z };
	}

	inline float wrapRange(float a, float min, float max) {
		if (a < min) {
			a += (max - min);
		}
		else if (a >= max) {
			a = fmod(a, max);
		}
		return a;
	}

	inline int wrapRangeInt(int a, int min, int max) {
		if (a < min) {
			return a + (max - min);
		}
		else if (a >= max) {
			return a % max;
		}
		return a;
	}
}