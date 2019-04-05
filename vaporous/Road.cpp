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
		_context->renderer->drawPoint(pointAtDistance(d), 
			vec4(1, 1, 1, 1));
	}
	_context->renderer->drawPoint(pointAtDistance(_length), 
		vec4(1, 0, 0, 1), 0.3f);
}

void Road::draw() {
	if (_vao.size() == 0) return;

	for (int i = 0; i < _road.size(); i++) {
		_context->renderer->drawMesh(_vao[i], _numIndices[i],
			_road.at(i).getModelMatrix(), 
			&_context->resourceMgr->getShader(Shaders::BasicTextured));
	}
}

void Road::buildMeshes() {
	if (_road.size() == 0) return;

	for (auto segment : _road) {
		Mesh mesh;
		if (segment.orientation == Orientation::Left)
			mesh = MeshUtil::arcCCW(segment.radius, segment.angle, _width, 16.0f);
		else
			mesh = MeshUtil::arcCW(segment.radius, segment.angle, _width, 16.0f);

		GLuint vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
			&mesh.vertices[0], GL_STATIC_DRAW);

		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(void*)offsetof(Vertex, position));
	
		// texture coord attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(void*)offsetof(Vertex, uv));

		// indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint),
			&mesh.indices[0], GL_STATIC_DRAW);

		_vao.push_back(vao);
		_vbo.push_back(vbo);
		_ebo.push_back(ebo);
		_numIndices.push_back(mesh.indices.size());
	}
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

	for (auto vao : _vao) {
		glDeleteVertexArrays(1, &vao);
	}
	for (auto vbo : _vbo) {
		glDeleteBuffers(1, &vbo);
	}
	for (auto ebo : _ebo) {
		glDeleteBuffers(1, &ebo);
	}

	_vao.clear();
	_vbo.clear();
	_ebo.clear();
	_numIndices.clear();
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