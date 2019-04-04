#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(Context* context) :
	_context(context),
	_isRotating(false),
	_isZooming(false)
{
	_context->eventMgr->subscribe(EventType::MouseDown, this);
	_context->eventMgr->subscribe(EventType::MouseUp, this);
}

OrbitCamera::~OrbitCamera() {
	_context->eventMgr->unsubscribe(EventType::MouseDown, this);
	_context->eventMgr->unsubscribe(EventType::MouseUp, this);
}

mat4 OrbitCamera::getViewMatrix() {
	mat4 rotation = glm::rotate(mat4(1), glm::radians(_azimuth), vec3(0, 1, 0));
	rotation = glm::rotate(rotation, glm::radians(_altitude), vec3(1, 0, 0));
	vec3 eye = rotation * vec4(0, 0, -_zoom, 1);

	mat4 view = glm::lookAt(_position + eye, _position, vec3(0, 1, 0));
	return view;
}

mat4 OrbitCamera::getProjectionMatrix() {
	vec2 window = _context->window->getSize();
	mat4 projection = glm::perspective(45.0f, window.x / window.y,
		0.1f, 100.0f);
	return projection;
}

void OrbitCamera::handleInput() {
	if (_isRotating) {
		vec2 mousePos = _context->inputMgr->getMousePos();
		vec2 offset = static_cast<vec2>(mousePos - _mouseAnchor);

		setAzimuth(_lastOrientation.x - offset.x);
		setAltitude(_lastOrientation.y - offset.y);
	}
	else if (_isZooming) {
		vec2 mousePos = _context->inputMgr->getMousePos();
		vec2 offset = static_cast<vec2>(mousePos - _mouseAnchor);

		_zoom = std::fmax(1.0f, _lastZoom - offset.y * 0.05f);
	}
}

void OrbitCamera::handleEvent(EventType type, EventData data) {
	switch (type) {
	case EventType::MouseDown:
		if (data.intData == 1 && !_isZooming) {
			_mouseAnchor = _context->inputMgr->getMousePos();
			_lastOrientation = { _azimuth, _altitude };
			_isRotating = true;
		}
		else if (data.intData == 2 && !_isRotating) {
			_mouseAnchor = _context->inputMgr->getMousePos();
			_lastZoom = _zoom;
			_isZooming = true;
		}
		break;
	case EventType::MouseUp:
		_isRotating = false;
		_isZooming = false;
		break;
	}
}

void OrbitCamera::setAzimuth(float az) {
	while (az < 0.0f) az += 360.0f;
	while (az >= 360.0f) az -= 360.0f;
	_azimuth = az;
}

void OrbitCamera::setAltitude(float at) {
	_altitude = std::fmin(std::fmax(at, -89.9f), 89.9f);
}