#pragma once
#include "EventData.h"

class Observer {
public:
	virtual ~Observer() {}
	virtual void handleEvent(EventType type, EventData data) = 0;
};