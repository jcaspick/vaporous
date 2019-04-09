#pragma once
#include <glm/glm.hpp>
#include <vector>

using vec3 = glm::vec3;
using vec2 = glm::vec2;

struct Vertex {
	Vertex() : position(vec3(0)), uv(vec2(0)), normal(vec3(0)) {};
	Vertex(vec3 position, vec2 uv, vec3 normal = vec3(0)) :
		position(position), uv(uv), normal(normal) {};

	vec3 position;
	vec2 uv;
	vec3 normal;
};

using Vertices = std::vector<Vertex>;
using Indices = std::vector<unsigned int>;
using GLuint = unsigned int;

class Mesh {
	friend class Renderer;

public:
	Mesh();
	~Mesh();
	void bind();
	void unbind();
	bool isBound();

	Vertices vertices;
	Indices indices;

private:
	bool _isBound;
	GLuint _vao, _vbo, _ebo;
};