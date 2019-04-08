#pragma once
#include "Road.h"
#include "Context.h"

class RoadGenerator {
public:
	RoadGenerator(Context* context);
	
	void update(float dt);
	void draw();
	void start();
	void reset();

private:
	struct Iteration {
		Iteration() : attempts(0), isValid(true) {}
		int attempts;
		bool isValid;
	};

	struct Sample {
		Sample(float distance, vec3 point) :
			distance(distance), point(point) {}

		float distance;
		vec3 point;
	};

private:
	void step();
	void updateSamples();

	Context* _context;

	Road _road;
	std::vector<Iteration> _stack;

	bool _generating = false;
	float _elapsed = 0.0f;
	float _stepInterval = 0.00f;

	float _goalLength = 1000.0f;
	float _worldRadius = 150.0f;
	float _deadZoneRadius = 30.0f;
	int _maxAttemptsPerIter = 3;

	std::vector<Sample> _samples;
	float _sampleInterval = 4.0f;
	float _furthestSample = 0;
	float _minClearance = 3.0f;
};