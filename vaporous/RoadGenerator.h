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
		Iteration() : attempts(0) {}
		int attempts;
	};

private:
	void step();

	Context* _context;

	Road _road;
	std::vector<Iteration> _stack;

	bool _generating = false;
	float _elapsed = 0.0f;
	float _stepInterval = 0.25f;

	float _goalLength = 500.0f;
};