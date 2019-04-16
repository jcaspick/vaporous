#pragma once
#include "Utilities.h"

class CameraMovement {
	friend class Demo;

public:
	CameraMovement() { _elapsed = 0.0f; };
	virtual ~CameraMovement() {};

	bool update(float dt) {
		_elapsed += dt;
		return _elapsed < _duration;
	}
	virtual float camDistance() { return _camDistance; }
	virtual float targetDistance() { return _targetDistance; }
	virtual vec3 camOffset() { 
		if (_useRotation)
			return vec3(cos(_elapsed * _rotationSpeed) * _rotationRadius,
				_camHeight, sin(_elapsed * _rotationSpeed) * _rotationRadius);
		else
			return vec3(0.0f, _camHeight, 0.0f);
	}

protected:
	float _elapsed;
	float _duration;
	float _camDistance;
	float _targetDistance;
	float _camHeight;
	float _rotationRadius;
	float _rotationSpeed;
	bool _useRotation;
};

class Cam_Chase : public CameraMovement {
public:
	Cam_Chase() {
		_duration = Util::randomRange(6.0f, 12.0f);
		_useRotation = false;
		_camDistance = Util::randomRange(1.5f, 2.6f);
		_camHeight = Util::randomRange(0.2f, 1.0f);
		_targetDistance = 4.0f;
	}
};

class Cam_ReverseChase : public CameraMovement {
public:
	Cam_ReverseChase() {
		_duration = Util::randomRange(6.0f, 12.0f);
		_useRotation = false;
		_camDistance = Util::randomRange(-3.2f, -2.0f);
		_camHeight = Util::randomRange(0.3f, 0.7f);
		_targetDistance = -3.0f;
	}
};

class Cam_Overhead : public CameraMovement {
public:
	Cam_Overhead() {
		_duration = Util::randomRange(10.0f, 25.0f);
		_useRotation = true;
		_camDistance = 0.0f;
		_targetDistance = 4.0f;
		_camHeight = Util::randomRange(10.0f, 16.0f);
		_rotationRadius = Util::randomRange(8.0f, 16.0f);
		_rotationSpeed = 0.0f;
	}
};

class Cam_Rotating : public CameraMovement {
public:
	Cam_Rotating() {
		_duration = Util::randomRange(10.0f, 25.0f);
		_useRotation = true;
		_camDistance = 0.0f;
		_targetDistance = Util::randomRange(0.0f, 1.5f);
		_camHeight = Util::randomRange(1.4f, 2.5f);
		_rotationRadius = Util::randomRange(3.0f, 9.0f);
		_rotationSpeed = Util::randomBool() ? 0.3f : -0.3f;
	}
};

class Cam_Whoosh : public CameraMovement {
public:
	Cam_Whoosh() {
		_duration = 4.2f;
		_useRotation = false;
		_targetDistance = 0.0f;
		_camHeight = Util::randomRange(0.8f, 2.5f);
	}

	virtual float camDistance() override {
		return Util::lerp(-30.0f, 45.0f, _elapsed / _duration);
	}
};