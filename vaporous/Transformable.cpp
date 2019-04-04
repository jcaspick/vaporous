#include "Transformable.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Transformable::Transformable() :
	_position(vec3(0)),
	_rotation(),
	_scale(1, 1, 1)
{}

vec3 Transformable::getPosition() {
	return _position;
}

quat Transformable::getRotation() {
	return _rotation;
}

void Transformable::setPosition(vec3 pos) {
	_position = pos;
}

void Transformable::setRotation(quat rot) {
	_rotation = rot;
}

void Transformable::move(vec3 offset) {
	_position += offset;
}

mat4 Transformable::getModelMatrix() {
	mat4 model;
	model = glm::translate(model, _position);
	model = model * glm::toMat4(_rotation);
	model = glm::scale(model, _scale);
	return model;
}