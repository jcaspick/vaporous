#version 330 core
layout (location = 0) in vec4 vertex; // xy position, zw tex coords

out vec2 uv;

void main()
{
	gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f);
	uv = vertex.zw;
}