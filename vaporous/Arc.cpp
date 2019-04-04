#include "Arc.h"
#include <glad/glad.h>

aArc::aArc(Context* context, float radius, float angle, 
	float width, float divisions) :
	_context(context),
	_angle(angle),
	_radius(radius)
{
	mesh = MeshUtil::arcCW(radius, angle, width, divisions);
}

void aArc::init() {
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

	shader = &_context->resourceMgr->getShader(Shaders::BasicTextured);
}

void aArc::draw(OrbitCamera* cam) {
	_context->resourceMgr->bindShader(Shaders::BasicTextured);
	_context->gl->bindVAO(vao);
	_context->gl->setLineMode(false);
	shader->setMat4("model", getModelMatrix());
	shader->setMat4("view", cam->getViewMatrix());
	shader->setMat4("projection", cam->getProjectionMatrix());
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

vec3 aArc::getEndPoint() {
	float rad = glm::radians(_angle);
	vec3 localPoint = vec3(cos(rad), 0, sin(rad)) * _radius
		- vec3(_radius, 0, 0);
	return (_rotation * localPoint) + _position;
}

quat aArc::getEndRot() {
	return _rotation * glm::angleAxis(glm::radians(-_angle), vec3(0, 1, 0));
}