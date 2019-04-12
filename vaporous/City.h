#pragma once
#include "Context.h"

class City {
public:
	City(Context* context);
	void draw();
	void init();
	void generate();

private:
	void createBgBuffer();
	void createCityBuffer();

	Context* _context;
	Shader* _bgShader;

	vec3 _center;
	std::vector<vec4> _bgTransforms;
	std::vector<vec2> _bgSizes;

	GLuint _bgVbo, _bgVao, _bgData1, _bgData2;
};