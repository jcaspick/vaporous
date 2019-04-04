#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using vec3 = glm::vec3;
using quat = glm::quat;
using mat4 = glm::mat4;

class Transformable {
public:
	Transformable();
	vec3 getPosition();
	quat getRotation();
	void setPosition(vec3 pos);
	void setRotation(quat rot);
	void move(vec3 offset);
	mat4 getModelMatrix();

protected:
	vec3 _position;
	quat _rotation;
	vec3 _scale;
};