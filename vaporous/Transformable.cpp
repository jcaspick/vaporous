#include "Transformable.h"
#include "Utilities.h"
#include <glm/gtc/matrix_transform.hpp>

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

vec3 Transformable::forward() {
	return Util::rotateVec3(vec3(0, 0, 1), getModelMatrix());
}

vec3 Transformable::up() {
	return Util::rotateVec3(vec3(0, 1, 0), getModelMatrix());
}

vec3 Transformable::right() {
	return Util::rotateVec3(vec3(-1, 0, 0), getModelMatrix());
}