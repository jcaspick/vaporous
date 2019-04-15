#include "Road.h"
#include "Utilities.h"
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
	if (_mesh.vertices.size() == 0) return;

	_context->renderer->drawMesh(_mesh, mat4(1),
		&_context->resourceMgr->getShader(Shaders::BasicTextured), 0.75f);
}

void Road::buildMesh() {
	if (_road.size() == 0) return;

	// build mesh from first segment
	_mesh = MeshUtil::segmentMesh(_road.at(0), _width, 
		1.0f, &_heightMap);

	// concatenate segments into a single mesh
	for (int i = 1; i < _road.size(); ++i) {
		Mesh segment = MeshUtil::segmentMesh(_road.at(i), 
			_width, 1.0f, &_heightMap);
		
		// remove redundant vertices where the meshes meet
		_mesh.vertices.pop_back();
		_mesh.vertices.pop_back();

		// get world space vertices
		for (auto& vertex : segment.vertices) {
			vertex.position = Util::transformVec3(vertex.position,
				_road.at(i).getModelMatrix());
		}

		// concatenate vertices
		_mesh.vertices.insert(_mesh.vertices.end(), segment.vertices.begin(),
			segment.vertices.end());

		// offset indices
		int iOffset = _mesh.indices.back() - 1;
		for (auto& ind : segment.indices) {
			ind += iOffset;
		}

		// concatenate indices
		_mesh.indices.insert(_mesh.indices.end(), segment.indices.begin(),
			segment.indices.end());
	}

	// scale UVs
	float textureScale = 12.0f;
	textureScale += fmod(_length, textureScale) / floor(_length / textureScale);
	for (auto& vertex : _mesh.vertices) {
		vertex.uv.y /= textureScale;
	}

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
	_mesh.unbind();
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
	distance = Util::wrapRange(distance, 0, _length);
	float sum = 0.0f;

	for (auto segment : _road) {
		if (sum + segment.arcLength() >= distance) {
			return segment.pointAtDistance(distance - sum);
		}
		sum += segment.arcLength();
	}

	return _road.back().pointAtDistance(_road.back().arcLength());
}

quat Road::rotationAtDistance(float distance) {
	vec3 point = pointAtDistance(distance);
	vec3 forward = pointAtDistance(distance - 1.0f);

	return glm::inverse(glm::lookAt(point, forward, vec3(0, 1, 0)));
}

float Road::sharpnessAtDistance(float distance) {
	distance = Util::wrapRange(distance, 0, _length);
	float sum = 0.0f;

	for (auto segment : _road) {
		if (sum + segment.arcLength() >= distance) {
			float sharpness = Util::easeInCubic(1.0f
				- std::min(segment.radius / 100.0f, 1.0f));
			if (segment.orientation == Orientation::Left)
				return sharpness;
			else
				return -sharpness;
		}
		sum += segment.arcLength();
	}

	float sharpness = 1.0f - (_road.back().radius / 100.0f);
	if (_road.back().orientation == Orientation::Left)
		return sharpness;
	else
		return -sharpness;
}