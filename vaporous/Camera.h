#pragma once
#ifndef CAMERA
#define CAMERA

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using mat4 = glm::mat4;

class Camera {
public:
	virtual mat4 getViewMatrix() { return mat4(1); };
	virtual mat4 getProjectionMatrix() { return mat4(1); };
};

#endif // ! CAMERA