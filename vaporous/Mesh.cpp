#include "Mesh.h"
#include <glad/glad.h>

Mesh::Mesh() :
	_isBound(false)
{}

Mesh::~Mesh() {
	if (_isBound) unbind();
}

void Mesh::bind() {
	if (vertices.size() == 0) return;
	if (_isBound) unbind();

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	glBindVertexArray(_vao);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() *
		sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));

	// texture coord attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, uv));

	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() *
		sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	_isBound = true;
}

void Mesh::unbind() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);

	_isBound = false;
}

bool Mesh::isBound() {
	return _isBound;
}