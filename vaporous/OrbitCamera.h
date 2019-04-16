#pragma once
#include "Camera.h"
#include "Context.h"
#include "Observer.h"

class OrbitCamera : public Camera, public Observer {
public:
	OrbitCamera(Context* context);
	~OrbitCamera();

	virtual mat4 getViewMatrix() override;
	virtual mat4 getProjectionMatrix() override;
	void handleInput();
	virtual void handleEvent(EventType type, EventData data) override;
	void setPosition(vec3 position);

private:
	Context* _context;
	void setAzimuth(float az);
	void setAltitude(float at);

	vec3 _position = vec3(0);
	float _azimuth = 45;
	float _altitude = 35.264f;
	float _zoom = 40;

	vec2 _mouseAnchor;
	vec2 _lastOrientation;
	float _lastZoom;
	bool _isRotating;
	bool _isZooming;
};