#include "RoadGenerator.h"
#include "Utilities.h"

RoadGenerator::RoadGenerator(Context* context) :
	_context(context),
	_road(_context)
{}

void RoadGenerator::update(float dt) {
	if (_generating) {
		_elapsed += dt;
		if (_elapsed >= _stepInterval) {
			step();
			_elapsed = 0.0f;
		}
	}
}

void RoadGenerator::draw() {
	_road.debugDraw(2.0f);
}

void RoadGenerator::start() {
	if (_generating) {
		return;
	}

	reset();

	// initialize heightmap
	_road.initializeHeightmap(_goalLength * 2, 45.0f, 10.0f);

	// add straight section as starting piece
	_road.addSegment(2, 1000, Orientation::Left);

	// push the first task to the stack
	_stack.emplace_back(Iteration());

	// activate generator
	_generating = true;
	_elapsed = 0.0f;
}

void RoadGenerator::reset() {
	_road.clear();
	_road.clearHeightmap();
	_generating = false;
}

void RoadGenerator::step() {
	if (_road.length() > _goalLength) {
		_generating = false;
		return;
	}

	float radius = Util::randomRange(20.0f, 40.0f);
	float angle = Util::randomRange(30.0f, 100.0f);
	Orientation orientation = Util::randomBool() ? 
		Orientation::Right : Orientation::Left;
	_road.addSegment(angle, radius, orientation);
}