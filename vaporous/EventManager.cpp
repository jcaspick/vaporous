#include "EventManager.h"
#include <iostream>
#include <algorithm>

EventManager::EventManager() {

}

void EventManager::subscribe(EventType type, Observer* observer) {
	auto& foundEvent = _events.find(type);
	if (foundEvent == _events.end()) {
		std::vector<Observer*> newEvent;
		newEvent.push_back(observer);
		_events.emplace(type, newEvent);
	}
	else {
		foundEvent->second.push_back(observer);
	}
}

void EventManager::unsubscribe(EventType type, Observer* observer) {
	auto& foundEvent = _events.find(type);
	if (foundEvent == _events.end()) {
		std::cout << "tried to unsubscribe from non-existant event" << std::endl;
	}
	else {
		auto foundObserver = std::find_if(foundEvent->second.begin(),
			foundEvent->second.end(), [&observer](Observer* other) { 
			return observer == other; 
		});
		if (foundObserver == foundEvent->second.end()) {
			std::cout << "tried to remove non-existant observer" << std::endl;
		}
		else {
			foundEvent->second.erase(foundObserver);
		}
	}
}

void EventManager::invoke(EventType type, EventData data) {
	auto& foundEvent = _events.find(type);
	if (foundEvent != _events.end()) {
		for (auto observer : foundEvent->second) {
			observer->handleEvent(type, data);
		}
	}
}