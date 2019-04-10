#pragma once
#include <vector>

class LoopingSpline {
public:
	LoopingSpline();
	float sample(float distance);
	void addValue(float value);
	void clear();
	bool empty();
	float length();
	void setInterval(float interval);

private:
	std::vector<float> _values;
	float _interval;
};