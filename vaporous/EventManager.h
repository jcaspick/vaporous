#pragma once
#include "EventData.h"
#include "Observer.h"
#include <unordered_map>

class EventManager {
public:
	EventManager();
	void subscribe(EventType type, Observer* observer);
	void unsubscribe(EventType type, Observer* observer);
	void invoke(EventType type, EventData data);

private:
	std::unordered_map<EventType, std::vector<Observer*>> _events;
};