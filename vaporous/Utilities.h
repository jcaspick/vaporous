#pragma once

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