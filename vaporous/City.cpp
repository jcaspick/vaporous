#include "City.h"
#include "Utilities.h"

City::City(Context* context) : 
	_context(context) 
{}

void City::draw() {
	Camera* cam = _context->renderer->getCamera();
	_context->gl->setLineMode(false);

	// draw city
	_context->gl->useShader(_cityShader->id);
	_context->gl->bindVAO(_cityVao);
	_context->resourceMgr->bindTexture(Textures::Noise, 3);

	_cityShader->setMat4("model", glm::translate(mat4(1), _center));
	_cityShader->setMat4("view", cam->getViewMatrix());
	_cityShader->setMat4("projection", cam->getProjectionMatrix());
	_cityShader->setFloat("width", round(testWidth));
	_cityShader->setFloat("height", round(testHeight));
	_cityShader->setFloat("lngth", round(testLength));
	_cityShader->setFloat("hueShift", hueShift);
	_cityShader->setFloat("noiseOffsetX", noiseOffsetX);
	_cityShader->setFloat("noiseOffsetY", noiseOffsetY);
	_cityShader->setInt("noise", 3);

	glDrawArrays(GL_TRIANGLES, 0, 30);

	// draw background
	_context->gl->useShader(_bgShader->id);
	_context->gl->bindVAO(_bgVao);

	_bgShader->setMat4("model", glm::translate(mat4(1), _center));
	_bgShader->setMat4("view", cam->getViewMatrix());
	_bgShader->setMat4("projection", cam->getProjectionMatrix());
	_bgShader->setVec4("color", vec4(0, 0, 0, 1));

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _bgTransforms.size());
}

void City::init() {
	_bgShader = _context->resourceMgr->loadShader(Shaders::CityBg,
		"shaders/city_bg.vert", "shaders/basic_singleColor.frag");
	_cityShader = _context->resourceMgr->loadShader(Shaders::City,
		"shaders/city_building.vert", "shaders/city_building.frag");
	_context->resourceMgr->loadTexture(Textures::Noise,
		"resources/uniformNoise.png", false);

	glGenBuffers(1, &_bgData1);
	glGenBuffers(1, &_bgData2);
	glGenBuffers(1, &_cityData1);
	glGenBuffers(1, &_cityData2);

	createBgBuffer();
	createCityBuffer();
}

void City::generate() {
	_bgTransforms.clear();
	_bgSizes.clear();

	// place background buildings
	for (float a = 0.0f; a < 360.0f;) {
		float distanceFromCenter = Util::randomRange(600.0f, 800.0f);

		_bgTransforms.emplace_back(vec4(
			sin(glm::radians(a)) * distanceFromCenter,
			0.0f,
			cos(glm::radians(a)) * distanceFromCenter,
			glm::radians(a)));
		_bgSizes.emplace_back(vec2(
			Util::randomRange(16, 32),
			Util::randomRange(48, 90)));

		a += Util::randomRange(0.7f, 2.4f);
	}
	
	// create instanced arrays of data
	glBindVertexArray(_bgVao);

	// transform
	glBindBuffer(GL_ARRAY_BUFFER, _bgData1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * _bgTransforms.size(), 
		&_bgTransforms[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)0);
	glVertexAttribDivisor(3, 1);

	// size
	glBindBuffer(GL_ARRAY_BUFFER, _bgData2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * _bgSizes.size(),
		&_bgSizes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
}

void City::createBgBuffer() {
	float vertices[] = {
		// position          // uv
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,

		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &_bgVao);
	glGenBuffers(1, &_bgVbo);

	glBindVertexArray(_bgVao);
	glBindBuffer(GL_ARRAY_BUFFER, _bgVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
		(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
		(void*)(3* sizeof(float)));
}

void City::createCityBuffer() {
	float vertices[] = {
		// position           // uv        // x-facing flag
		 0.5f,  0.0f, -0.5f,  0.0f, 0.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,  1.0f, 0.0f,  0.0f,
		-0.5f,  1.0f, -0.5f,  1.0f, 1.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  0.0f, 0.0f,  0.0f,
		-0.5f,  1.0f, -0.5f,  1.0f, 1.0f,  0.0f,
		 0.5f,  1.0f, -0.5f,  0.0f, 1.0f,  0.0f,

		-0.5f,  0.0f, -0.5f,  1.0f, 0.0f,  1.0f,
		-0.5f,  0.0f,  0.5f,  2.0f, 0.0f,  1.0f,
		-0.5f,  1.0f,  0.5f,  2.0f, 1.0f,  1.0f,
		-0.5f,  0.0f, -0.5f,  1.0f, 0.0f,  1.0f,
		-0.5f,  1.0f,  0.5f,  2.0f, 1.0f,  1.0f,
		-0.5f,  1.0f, -0.5f,  1.0f, 1.0f,  1.0f,

		-0.5f,  0.0f,  0.5f,  2.0f, 0.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  3.0f, 0.0f,  0.0f,
		 0.5f,  1.0f,  0.5f,  3.0f, 1.0f,  0.0f,
		-0.5f,  0.0f,  0.5f,  2.0f, 0.0f,  0.0f,
		 0.5f,  1.0f,  0.5f,  3.0f, 1.0f,  0.0f,
		-0.5f,  1.0f,  0.5f,  2.0f, 1.0f,  0.0f,

		 0.5f,  0.0f,  0.5f,  3.0f, 0.0f,  1.0f,
		 0.5f,  0.0f, -0.5f,  4.0f, 0.0f,  1.0f,
		 0.5f,  1.0f, -0.5f,  4.0f, 1.0f,  1.0f,
		 0.5f,  0.0f,  0.5f,  3.0f, 0.0f,  1.0f,
		 0.5f,  1.0f, -0.5f,  4.0f, 1.0f,  1.0f,
		 0.5f,  1.0f,  0.5f,  3.0f, 1.0f,  1.0f,

		 0.5f,  1.0f, -0.5f,  0.0f, 0.0f,  0.0f,
		-0.5f,  1.0f, -0.5f,  0.0f, 0.0f,  0.0f,
		-0.5f,  1.0f,  0.5f,  0.0f, 0.0f,  0.0f,
		 0.5f,  1.0f, -0.5f,  0.0f, 0.0f,  0.0f,
		-0.5f,  1.0f,  0.5f,  0.0f, 0.0f,  0.0f,
		 0.5f,  1.0f,  0.5f,  0.0f, 0.0f,  0.0f,
	};

	glGenVertexArrays(1, &_cityVao);
	glGenBuffers(1, &_cityVbo);

	glBindVertexArray(_cityVao);
	glBindBuffer(GL_ARRAY_BUFFER, _cityVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
}