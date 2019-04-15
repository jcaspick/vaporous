#pragma once
#include "Road.h"
#include "Context.h"

enum class SegmentType {
	Straight,
	ShallowCurve,
	ModerateCurve,
	SharpCurve
};

class RoadGenerator {
public:
	RoadGenerator(Context* context);
	void generate();
	void reset();
	bool hasRoad();
	Road& getRoad();
	std::vector<vec3> getSamples();

	float pStraight = 0.1875f;
	float pShallow = 0.1875f;
	float pModerate = 0.5f;
	float pSharp = 0.125f;
	float _worldRadius = 150.0f;

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
	void updateSamples();
	bool evaluateLoop();
	SegmentType chooseSegmentType();
	void getSegmentProperties(SegmentType type, 
		float& angle, float& radius);

	Context* _context;
	Road _road;
	std::vector<Iteration> _stack;

	bool _hasRoad = false;

	float _goalLength = 400.0f;
	float _deadZoneRadius = 50.0f;
	int _maxAttemptsPerIter = 3;

	std::vector<Sample> _samples;
	float _sampleInterval = 4.0f;
	float _furthestSample = 0;
	float _minClearance = 3.0f;
};