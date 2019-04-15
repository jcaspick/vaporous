#pragma once
#include "Context.h"

class City {
public:
	City(Context* context);
	void draw();
	void init();
	void setCenter(vec3 center);
	void generate(float worldSize, std::vector<vec3> samples);

	float testWidth = 14.0f;
	float testHeight = 24.0f;
	float testLength = 9.0f;
	float hueShift = 0.0f;
	float noiseOffsetX = 0.0f;
	float noiseOffsetY = 0.0f;
	float rotation = 0.0f;

private:
	void createBgBuffer();
	void createCityBuffer();
	void generateBuildings(float worldRadius, std::vector<vec3> samples);
	void generateBg();

	Context* _context;
	Shader* _bgShader;
	Shader* _cityShader;

	vec3 _center;
	std::vector<vec4> _bgTransforms;
	std::vector<vec2> _bgSizes;
	std::vector<vec4> _buildingTransforms;
	std::vector<vec4> _buildingSizes;
	float _buildingScale = 0.57f;

	GLuint _bgVbo, _bgVao, _bgData1, _bgData2;
	GLuint _cityVbo, _cityVao, _cityData1, _cityData2;
};