#pragma once
#include "Context.h"

class City {
public:
	City(Context* context);
	void draw();
	void init();
	void generate();

	float testWidth = 14.0f;
	float testHeight = 24.0f;
	float testLength = 9.0f;

private:
	void createBgBuffer();
	void createCityBuffer();

	Context* _context;
	Shader* _bgShader;
	Shader* _cityShader;

	vec3 _center;
	std::vector<vec4> _bgTransforms;
	std::vector<vec2> _bgSizes;

	GLuint _bgVbo, _bgVao, _bgData1, _bgData2;
	GLuint _cityVbo, _cityVao, _cityData1, _cityData2;
};