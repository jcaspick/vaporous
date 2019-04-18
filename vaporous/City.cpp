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

	_cityShader->setMat4("model", mat4(1));
	_cityShader->setMat4("view", cam->getViewMatrix());
	_cityShader->setMat4("projection", cam->getProjectionMatrix());
	_cityShader->setInt("noise", 3);
	_cityShader->setFloat("globalScale", _buildingScale);
	_cityShader->setFloat("cityBottom", _cityBottom);
	_cityShader->setFloat("time", static_cast<float>(glfwGetTime()));
	_cityShader->setVec3("camPos", cam->getPosition());
	_cityShader->setFloat("fogDist", 75.0f);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 30, _buildingTransforms.size());

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
		"resources/shaders/city_bg.vert", "resources/shaders/basic_singleColor.frag");
	_cityShader = _context->resourceMgr->loadShader(Shaders::City,
		"resources/shaders/city_building.vert", "resources/shaders/city_building.frag");
	_context->resourceMgr->loadTexture(Textures::Noise,
		"resources/images/uniformNoise.png", false);

	glGenBuffers(1, &_bgData1);
	glGenBuffers(1, &_bgData2);
	glGenBuffers(1, &_cityData1);
	glGenBuffers(1, &_cityData2);
	glGenBuffers(1, &_cityData3);

	createBgBuffer();
	createCityBuffer();
}

void City::setCenter(vec3 center) {
	_center = center;
}

void City::generate(float worldRadius, std::vector<vec3> samples) {
	generateBg();
	generateBuildings(worldRadius, samples);
}

void City::generateBuildings(float worldRadius, std::vector<vec3> samples) {
	_buildingTransforms.clear();
	_buildingSizes.clear();
	_buildingAppearances.clear();

	int numBuildings = static_cast<int>(worldRadius) * 4;
	for (int i = 0; i < numBuildings; ++i) {
		// select random position and rotation for building
		float distance = Util::randomRange(0.0f, worldRadius * 1.5f);
		float angle = glm::radians(Util::randomRange(0.0f, 360.0f));
		vec3 position = vec3(sin(angle) * distance, -24.0f, 
			cos(angle) * distance) + _center;
		float rotation = glm::radians(static_cast<int>(
			Util::randomRange(0, 7)) * 22.5f);

		// select random size for building
		vec3 size = vec3(
			round(Util::randomRange(14, 24)), 
			round(Util::randomRange(30, 110)), 
			round(Util::randomRange(9, 16)));
		float safeRadius = glm::length(vec2(_buildingScale * size.x / 2,
			_buildingScale * size.y / 2));

		// select random values for hue shift and percent windows lit
		vec2 appearance = vec2(
			Util::randomRange(0.0f, 1.0f),
			Util::randomRange(0.25f, 0.6f));

		// check for intersections with road
		for (auto& sample : samples) {
			float distanceToRoad = glm::length(Util::flatten(sample)
				- Util::flatten(position));
			if (distanceToRoad < safeRadius) {
				// intersecting buildings aren't removed, but have their height
				// limited so they appear underneath the road
				size.y = round(Util::randomRange(14, 18));
				break;
			}
		}

		// check for intersections with other buildings
		bool intersection = false;
		for (auto& transform : _buildingTransforms) {
			float distanceToBuilding = glm::length(Util::flatten(position)
				- Util::flatten(vec3(transform)));
			if (distanceToBuilding < safeRadius) {
				intersection = true;
				break;
			}
			// buildings that are close together but not intersecting
			// have matching rotations to suggest that they are aligned
			// to city blocks
			else if (distanceToBuilding < 100.0f * _buildingScale) {
				rotation = Util::randomBool ?
					transform.w : transform.w + glm::radians(90.0f);
			}
		}
		if (intersection) continue;

		// construct the building
		float capHeight = round(Util::randomRange(4, 9));
		bool isTiered = size.y > 50.0f && Util::randomBool();
		float minTierHeight = 16.0f;
		float minTierWidth = std::max(8.0f, round(size.x / 2));
		float minTierLength = std::max(8.0f, round(size.z / 2));
		float height = 0.0f;
		float goalHeight = size.y - capHeight;
		float sectionProportion = isTiered ? 
			Util::randomRange(0.4f, 0.8f) : 1.0f;
		vec3 sectionSize = vec3(size.x, 0.0f, size.z);

		while (height < (goalHeight * _buildingScale)) {
			// decide height of new section
			sectionSize.y = round(sectionProportion * (goalHeight - height));
			if (sectionSize.y < minTierHeight && 
				Util::randomRange(0.0f, 1.0f) > 0.2f) 
			{
				sectionSize.y = ceil(goalHeight - height);
			}

			// decide width/length of new section
			// there is a random chance for each tier of a tiered
			// building to be scaled down in one dimension
			if (Util::randomBool() && height > 0.0f) {
				sectionSize.x = round(std::max(sectionSize.x * 0.5f, minTierWidth));
			}
			else if (height > 0.0f) {
				sectionSize.z = round(std::max(sectionSize.z * 0.5f, minTierLength));
			}
			float offsetX = (size.x - sectionSize.x) * 0.5f * _buildingScale;
			float offsetZ = (size.z - sectionSize.z) * 0.5f * _buildingScale;

			vec3 sectionPos = vec3(position.x + cos(rotation) * offsetX
				- sin(rotation) * offsetZ, height + _cityBottom, 
				position.z + cos(rotation) * offsetZ + sin(rotation) * offsetX);

			_buildingTransforms.push_back(vec4(sectionPos, rotation));
			_buildingSizes.push_back(vec4(sectionSize, goalHeight));
			_buildingAppearances.push_back(appearance);
			height += sectionSize.y * _buildingScale;

			// create windowless divider
			sectionPos.y = height + _cityBottom;
			sectionSize.y = capHeight;
			_buildingTransforms.push_back(vec4(sectionPos, rotation));
			_buildingSizes.push_back(vec4(sectionSize, 0.0f));
			_buildingAppearances.push_back(appearance);
			height += capHeight * _buildingScale;
		}
	}

	// created instanced arrays of data
	glBindVertexArray(_cityVao);

	// position
	glBindBuffer(GL_ARRAY_BUFFER, _cityData1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * _buildingTransforms.size(),
		&_buildingTransforms[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)0);
	glVertexAttribDivisor(3, 1);

	// size
	glBindBuffer(GL_ARRAY_BUFFER, _cityData2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * _buildingSizes.size(),
		&_buildingSizes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)0);
	glVertexAttribDivisor(4, 1);

	// appearance
	glBindBuffer(GL_ARRAY_BUFFER, _cityData3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * _buildingAppearances.size(),
		&_buildingAppearances[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

void City::generateBg() {
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
			Util::randomRange(24, 45)));

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