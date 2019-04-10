#include "LoopingSpline.h"
#include "Utilities.h"

LoopingSpline::LoopingSpline() :
	_interval(10.0f)
{}

float LoopingSpline::sample(float distance) {
	if (empty()) return 0.0f;

	distance = Util::wrapRange(distance, 0.0f, length());
	int i = static_cast<int>(floor(distance / _interval));

	float p0 = _values[Util::wrapRangeInt(i - 1, 0, _values.size() - 1)];
	float p1 = _values[i];
	float p2 = _values[Util::wrapRangeInt(i + 1, 0, _values.size() - 1)];
	float p3 = _values[Util::wrapRangeInt(i + 2, 0, _values.size() - 1)];

	float t = fmod(distance, _interval) / _interval;
	return CRSpline::interpolate(t, p0, p1, p2, p3);
}

void LoopingSpline::addValue(float value) {
	_values.push_back(value);
}

void LoopingSpline::clear() {
	_values.clear();
}

bool LoopingSpline::empty() {
	return _values.empty();
}

float LoopingSpline::length() {
	return _values.size() * _interval;
}

void LoopingSpline::setInterval(float interval) {
	_interval = interval;
}