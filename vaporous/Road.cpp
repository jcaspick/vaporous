#include "Road.h"

Road::Road() :
	_length(0)
{}

void Road::debugDraw(float distanceBetweenPoints) {

}

void Road::draw() {

}

void Road::buildMeshes() {

}

void Road::addSegment(float angle, float radius, Orientation orientation) {
	RoadSegment segment(angle, radius, orientation);
	_road.push_back(segment);
	_length += segment.arcLength();
}

void Road::removeLastSegment() {
	_length -= _road.back().arcLength();
	_road.pop_back();
}

void Road::clear() {
	_road.clear();
}

float Road::length() {
	return _length;
}

vec3 Road::pointAtDistance(float distance) {
	distance = glm::clamp(distance, 0.0f, _length);
	float sum = 0.0f;

	for (auto segment : _road) {
		if (sum + segment.arcLength() > distance) {
			return segment.pointAtDistance(distance - sum);
		}
		sum += segment.arcLength();
	}
}