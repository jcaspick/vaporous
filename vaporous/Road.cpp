#include "Road.h"
#include "BiarcUtility.h"
#include "MeshUtilities.h"

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
		vec3 p = pointAtDistance(d);
		vec4 lineColor = p.y >= 0 ? vec4(0.75f, 1.0f, 0.75f, 1.0f) :
			vec4(1.0f, 0.75f, 0.75f, 1.0f);
		_context->renderer->drawPoint(p, vec4(1, 1, 1, 1));
		_context->renderer->drawLine(vec3(p.x, 0, p.z), p, lineColor);
	}
	_context->renderer->drawPoint(pointAtDistance(_length), 
		vec4(1, 0, 0, 1), 0.3f);
}

void Road::draw() {
	//if (_vao.size() == 0) return;

	//for (int i = 0; i < _road.size(); i++) {
	//	_context->renderer->drawMesh(_vao[i], _numIndices[i],
	//		_road.at(i).getModelMatrix(), 
	//		&_context->resourceMgr->getShader(Shaders::BasicTextured));
	//}

	if (_mesh.vertices.size() == 0) return;

	_context->renderer->drawMesh(_mesh, mat4(1),
		&_context->resourceMgr->getShader(Shaders::BasicTextured));
}

void Road::buildMesh() {
	if (_road.size() == 0) return;

	_mesh = MeshUtil::segmentMesh(_road.at(0), _width, 1.0f);
	//for (int i = 1; i < _road.size(); ++i) {
	//	Mesh segment = MeshUtil::segmentMesh(_road.at(i), _width, 1.0f);
	//	_mesh = MeshUtil::concat(_mesh, segment);
	//}
	_mesh.bind();
}

void Road::addSegment(float angle, float radius, Orientation orientation) {
	RoadSegment segment(angle, radius, orientation, _length, &_heightMap);

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

void Road::initializeHeightmap(float distance, float interval,
	float range)
{
	while (_heightMap.getLength() < distance) {
		float height = (-range * 0.5f) + static_cast<float>(rand()) /
			static_cast<float>(RAND_MAX / range);
		_heightMap.addPoint(height, interval);
	}
}

void Road::clearHeightmap() {
	_heightMap.clear();
}

float Road::length() {
	if (_road.empty()) return 0.0f;
	else return _length;
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

	return _road.back().pointAtDistance(_road.back().arcLength());
}