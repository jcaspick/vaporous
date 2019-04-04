#pragma once
#ifndef CAMERA
#define CAMERA

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using mat4 = glm::mat4;

class Camera {
public:
	virtual mat4 getViewMatrix() = 0;
	virtual mat4 getProjectionMatrix() = 0;
};

#endif // ! CAMERA