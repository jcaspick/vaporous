#include "Road.h"
#include "BiarcUtility.h"

Road::Road(Context* context) :
	_context(context),
	_length(0)
{}

void Road::debugDraw(float distanceBetweenPoints) {
	if (_road.size() == 0) {
		return;
	}

	_context->renderer->drawPoint(pointAtDistance(0), 
		vec4(0, 1, 0, 1), 0.3f);
	for (float d = distanceBetweenPoints; d < _length; 
		d += distanceBetweenPoints) 
	{
		_context->renderer->drawPoint(pointAtDistance(d), 
			vec4(1, 1, 1, 1));
	}
	_context->renderer->drawPoint(pointAtDistance(_length), 
		vec4(1, 0, 0, 1), 0.3f);
}

void Road::draw() {

}

void Road::buildMeshes() {

}

void Road::addSegment(float angle, float radius, Orientation orientation) {
	RoadSegment segment(angle, radius, orientation);

	if (_road.size() > 0) {
		segment.setPosition(_road.back().endPoint());
		segment.setRotation(_road.back().endRot());
	}

	_road.push_back(segment);
	_length += segment.arcLength();
}

void Road::closeLoop() {
	if (_road.size() == 0) return;

	Biarc biarc = BiarcUtil::biarc(
		_road.back().endPoint(),
		_road.back().endRot() * vec3(0, 0, 1),
		vec3(0),
		vec3(0, 0, 1)
	);

	addSegment(biarc.arc1_angle, biarc.arc1_radius, 
		biarc.arc1_flipped ? Orientation::Left : Orientation::Right);
	addSegment(biarc.arc2_angle, biarc.arc2_radius,
		biarc.arc2_flipped ? Orientation::Left : Orientation::Right);
}

void Road::removeLastSegment() {
	_length -= _road.back().arcLength();
	_road.pop_back();
}

void Road::clear() {
	_road.clear();
	_length = 0.0f;
}

float Road::length() {
	return _length;
}

vec3 Road::pointAtDistance(float distance) {
	distance = glm::clamp(distance, 0.0f, _length);
	float sum = 0.0f;

	for (auto segment : _road) {
		if (sum + segment.arcLength() >= distance) {
			return segment.pointAtDistance(distance - sum);
		}
		sum += segment.arcLength();
	}
}