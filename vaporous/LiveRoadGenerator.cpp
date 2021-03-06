#include "LiveRoadGenerator.h"
#include "Utilities.h"

LiveRoadGenerator::LiveRoadGenerator(Context* context) :
	_context(context),
	_road(_context)
{}

void LiveRoadGenerator::update(float dt) {
	if (_generating) {
		_elapsed += dt;
		if (_elapsed >= _stepInterval) {
			step();
			_elapsed = 0.0f;
		}
	}
}

void LiveRoadGenerator::draw() {
	_road.debugDraw(2.0f);
	_context->renderer->drawCircle(vec3(0), _worldRadius,
		vec4(0, 1, 0, 1));
	_context->renderer->drawCircle(vec3(0), _deadZoneRadius,
		vec4(0, 1, 1, 1));
}

void LiveRoadGenerator::start() {
	if (_generating) return;
	reset();

	// initialize heightmap
	_road.initializeHeightmap(_goalLength * 2, 45.0f, 10.0f);

	// add straight section as starting piece
	_road.addSegment(2, 1000, Orientation::Left);
	for (float d = 0.0f; d < _road.length(); d += _sampleInterval) {
		_samples.emplace_back(Sample(d, _road.pointAtDistance(d)));
		_furthestSample = d;
	}

	// push the first task to the stack
	_stack.emplace_back(Iteration());

	// activate generator
	_generating = true;
	_elapsed = 0.0f;
}

void LiveRoadGenerator::reset() {
	_stack.clear();
	_road.clear();
	_road.clearHeightmap();
	_samples.clear();
	_furthestSample = 0.0f;
	_generating = false;
}

Road& LiveRoadGenerator::getRoad() {
	return _road;
}

void LiveRoadGenerator::step() {
	if (_stack.empty()) {
		std::cout << "road generation failed" << std::endl;
		reset();
		return;
	}

	Iteration &iteration = _stack.back();

	// if the current road segment is invalid, remove it
	if (!iteration.isValid) {
		_road.removeLastSegment();
		updateSamples();
		iteration.isValid = true;
		return;
	}
	// if the maximum number of attempts has been reached
	// return to the previous iteration
	else if (iteration.attempts >= _maxAttemptsPerIter) {
		_road.removeLastSegment();
		updateSamples();
		_stack.pop_back();
		return;
	}

	// create a new road segment
	float radius, angle;
	SegmentType type = chooseSegmentType();
	getSegmentProperties(type, angle, radius);
	Orientation orientation = Util::randomBool() ?
		Orientation::Right : Orientation::Left;
	_road.addSegment(angle, radius, orientation);
	RoadSegment segment = _road._road.back();

	// if the road goes outside the world boundary the segment is invalid
	if (glm::length(segment.endPoint()) > _worldRadius) {
		iteration.isValid = false;
	}
	// if the segment intersects a previous segment it is invalid
	for (float d = _furthestSample + _sampleInterval; d < _road.length();
		d += _sampleInterval)
	{
		for (auto itr = _samples.rbegin(); itr != _samples.rend(); ++itr) {
			vec3 a = _road.pointAtDistance(d);
			vec3 b = itr->point;

			if (glm::length2(Util::flatten(a) - Util::flatten(b)) <
				_sampleInterval * _sampleInterval - 0.1f)
			{
				if (abs(a.y - b.y) < _minClearance) {
					iteration.isValid = false;
					d += _road.length(); // breaks outer for loop
					break;
				}
			}
		}
	}

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
			_road._heightMap.setLoopDistance(_road.length());
			// the biarc solver somtimes comes up with ugly solutions
			// when closing the loop so we check to see if the solution
			// is acceptable before declaring the road complete
			if (evaluateLoop()) {
				_generating = false;
			}
			else {
				_road.removeLastSegment();
				_road.removeLastSegment();
				_road._heightMap.setLoopDistance(FLT_MAX);
				iteration.isValid = false;
			}
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
		updateSamples();
	}
}

void LiveRoadGenerator::updateSamples() {
	if (_furthestSample > _road.length()) {
		while (_furthestSample > _road.length()) {
			_samples.pop_back();
			_furthestSample = _samples.back().distance;
		}
	}
	else {
		for (float d = _furthestSample; d < _road.length(); d += _sampleInterval) {
			_samples.emplace_back(d, _road.pointAtDistance(d));
			_furthestSample = d;
		}
	}
}

bool LiveRoadGenerator::evaluateLoop() {
	RoadSegment a = _road._road.at(_road._road.size() - 2);
	RoadSegment b = _road._road.at(_road._road.size() - 1);

	// check for intersections
	for (float d = _furthestSample + _sampleInterval; d < _road.length();
		d += _sampleInterval)
	{
		for (auto itr = _samples.rbegin(); itr != _samples.rend() - 1; ++itr) {
			vec3 a = _road.pointAtDistance(d);
			vec3 b = itr->point;

			if (glm::length2(Util::flatten(a) - Util::flatten(b)) <
				_sampleInterval * _sampleInterval - 0.1f)
			{
				if (abs(a.y - b.y) < _minClearance) {
					return false;
				}
			}
		}
	}

	// curves shouldn't be sharper than the sharp road segments
	// or wider than the shallow road segments
	if (a.radius > 10.0f && a.radius < 90.0f &&
		b.radius > 10.0f && b.radius < 90.0f) return true;

	return false;
}

SegmentType LiveRoadGenerator::chooseSegmentType() {
	float r = Util::randomRange(0.0f, 1.0f);
	if (r < pStraight) return SegmentType::Straight;
	else r -= pStraight;
	if (r < pShallow) return SegmentType::ShallowCurve;
	else r -= pShallow;
	if (r < pModerate) return SegmentType::ModerateCurve;
	else return SegmentType::SharpCurve;
}

void LiveRoadGenerator::getSegmentProperties(SegmentType type,
	float& angle, float& radius)
{
	switch (type) {
	case SegmentType::Straight:
		angle = Util::randomRange(4.0f, 12.0f);
		radius = Util::randomRange(120.0f, 200.0f);
		break;
	case SegmentType::ShallowCurve:
		angle = Util::randomRange(30.0f, 90.0f);
		radius = Util::randomRange(40.0f, 90.0f);
		break;
	case SegmentType::ModerateCurve:
		angle = Util::randomRange(30.0f, 100.0f);
		radius = Util::randomRange(20.0f, 40.0f);
		break;
	case SegmentType::SharpCurve:
		angle = Util::randomRange(90.0f, 120.0f);
		radius = Util::randomRange(10.0f, 20.0f);
		break;
	}
}