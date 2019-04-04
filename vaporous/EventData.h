#pragma once

class Entity;

enum class EventType {
	WindowResize,
	KeyDown,
	KeyUp,
	MouseDown,
	MouseUp,
	ScrollX,
	ScrollY
};

struct EventData {
	EventData() {}
	EventData(float floatData) :
		floatData(floatData) {}
	EventData(int intData) :
		intData(intData) {}
	EventData(bool boolData) :
		boolData(boolData) {}

	float floatData;
	int intData;
	bool boolData;
};