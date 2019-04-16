#pragma once
#include "Road.h"
#include "Context.h"

enum class SegmentType {
	Straight,
	ShallowCurve,
	ModerateCurve,
	SharpCurve
};

struct RoadSettings {
	RoadSettings() {};

	float goalLength;
	float worldRadius;
	float pStraight;
	float pShallow;
	float pModerate;
	float pSharp;
};

class RoadGenerator {
public:
	RoadGenerator(Context* context);
	void generate();
	void reset();
	bool hasRoad();
	Road& getRoad();
	std::vector<vec3> getSamples();
	vec3 getRoadCenter();
	void setRoadSettings(RoadSettings settings);

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
	void calculateExtents();
	bool evaluateLoop();
	SegmentType chooseSegmentType();
	void getSegmentProperties(SegmentType type, 
		float& angle, float& radius);

	Context* _context;
	Road _road;
	RoadSettings _settings;
	std::vector<Iteration> _stack;

	bool _hasRoad = false;

	float _deadZoneRadius = 50.0f;
	int _maxAttemptsPerIter = 3;

	std::vector<Sample> _samples;
	float _sampleInterval = 4.0f;
	float _furthestSample = 0;
	float _minClearance = 3.0f;

	float _minX, _maxX, _minZ, _maxZ;
};