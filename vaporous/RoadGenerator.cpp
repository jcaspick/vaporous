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
	_context->renderer->drawCircle(vec3(0), _worldRadius, 
		vec4(0, 1, 0, 1));
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
	Iteration &iteration = _stack.back();

	// if the current road segment is invalid, remove it
	if (!iteration.isValid) {
		_road.removeLastSegment();
		iteration.isValid = true;
		return;
	}
	// if the maximum number of attempts has been reached
	// return to the previous iteration
	else if (iteration.attempts >= _maxAttemptsPerIter) {
		_road.removeLastSegment();
		_stack.pop_back();
		return;
	}

	// create a new road segment
	float radius = Util::randomRange(20.0f, 40.0f);
	float angle = Util::randomRange(30.0f, 100.0f);
	Orientation orientation = Util::randomBool() ? 
		Orientation::Right : Orientation::Left;
	_road.addSegment(angle, radius, orientation);
	RoadSegment segment = _road._road.back();

	// if the road goes outside the world boundary the segment is invalid
	if (glm::length(segment.endPoint()) > _worldRadius) {
		iteration.isValid = false;
	}
	// if the segment intersects a previous segment it is invalid
	// TODO

	// if the goal length has been reached and the new segment does not
	// lead back towards the origin it is invalid
	float distanceFromOrigin = glm::length(segment.endPoint());
	if (_road.length() > _goalLength) {
		RoadSegment previous = _road._road.at(_road._road.size() - 2);
		if (distanceFromOrigin >= glm::length(previous.endPoint())) {
			iteration.isValid = false;
		}
	}

	// if the new segment passes close to the origin of the road...
	if (distanceFromOrigin <= _deadZoneRadius) {
		// if the goal length has been reached, complete the track
		if (_road.length() > _goalLength && iteration.isValid) {
			_road.closeLoop();
			_generating = false;
			return;
		}
		// if not the segment is invalid
		else {
			iteration.isValid = false;
		}
	}

	iteration.attempts++;

	// if current iteration is a valid move, begin the next
	if (iteration.isValid) {
		_stack.emplace_back(Iteration());
	}
}