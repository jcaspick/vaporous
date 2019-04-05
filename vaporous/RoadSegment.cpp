#include "RoadSegment.h"

RoadSegment::RoadSegment(float angle, float radius, 
	Orientation orientation, float distanceOffset, HeightMap* heightMap) :
	angle(angle),
	radius(radius),
	orientation(orientation),
	_distanceOffset(distanceOffset),
	_heightMap(heightMap)
{}

float RoadSegment::arcLength() {
	return glm::two_pi<float>() * radius * (angle / 360.0f);
}

vec3 RoadSegment::pointAtDistance(float distance) {
	float height = _heightMap->sample(distance + _distanceOffset);
	if (orientation == Orientation::Right) {
		float normalizedDistance = glm::clamp(distance / arcLength(), 0.0f, 1.0f);
		float rad = glm::radians(angle * normalizedDistance);
		vec3 localPoint = vec3(cos(rad), 0.0f, sin(rad)) * radius
			- vec3(radius, 0, 0);
		localPoint.y = height;
		return (_rotation * localPoint) + _position;
	}
	else {
		float normalizedDistance = glm::clamp(distance / arcLength(), 0.0f, 1.0f);
		float rad = glm::radians(-angle * normalizedDistance + 180);
		vec3 localPoint = vec3(cos(rad), 0.0f, sin(rad)) * radius
			+ vec3(radius, 0, 0);
		localPoint.y = height;
		return (_rotation * localPoint) + _position;
	}
}

vec3 RoadSegment::endPoint() {
	if (orientation == Orientation::Right) {
		float rad = glm::radians(angle);
		vec3 localPoint = vec3(cos(rad), 0.0f, sin(rad)) * radius
			- vec3(radius, 0, 0);
		return (_rotation * localPoint) + _position;
	}
	else {
		float rad = glm::radians(-angle + 180);
		vec3 localPoint = vec3(cos(rad), 0.0f, sin(rad)) * radius
			+ vec3(radius, 0, 0);
		return (_rotation * localPoint) + _position;
	}
}

quat RoadSegment::endRot() {
	if (orientation == Orientation::Right) {
		return _rotation * glm::angleAxis(glm::radians(-angle), vec3(0, 1, 0));
	}
	else {
		return _rotation * glm::angleAxis(glm::radians(angle), vec3(0, 1, 0));
	}
}