#pragma once
#include <glm/glm.hpp>
#include <vector>

using vec3 = glm::vec3;
using vec2 = glm::vec2;

struct Vertex {
	Vertex() : position(vec3(0)), uv(vec2(0)) {};
	Vertex(vec3 position, vec2 uv) :
		position(position), uv(uv) {};

	vec3 position;
	vec2 uv;
};

using Vertices = std::vector<Vertex>;
using Indices = std::vector<unsigned int>;

struct Mesh {
	Vertices vertices;
	Indices indices;
};