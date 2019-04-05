#include "RoadSegment.h"

RoadSegment::RoadSegment(float angle, float radius, Orientation orientation) :
	angle(angle),
	radius(radius),
	orientation(orientation)
{}

float RoadSegment::arcLength() {
	return glm::two_pi<float>() * radius * (angle / 360.0f);
}

vec3 RoadSegment::pointAtDistance(float distance) {
	float normalizedDistance = glm::clamp(distance / arcLength(), 0.0f, 1.0f);
	float rad = glm::radians(angle * normalizedDistance);
	vec3 localPoint = vec3(cos(rad), 0, sin(rad)) * radius
		- vec3(radius, 0, 0);
	return (_rotation * localPoint) + _position;
}

vec3 RoadSegment::endPoint() {
	float rad = glm::radians(angle);
	vec3 localPoint = vec3(cos(rad), 0, sin(rad)) * radius
		- vec3(radius, 0, 0);
	return (_rotation * localPoint) + _position;
}

quat RoadSegment::endRot() {
	return _rotation * glm::angleAxis(glm::radians(-angle), vec3(0, 1, 0));
}