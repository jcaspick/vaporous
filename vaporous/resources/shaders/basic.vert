#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec2 _uv;

out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	uv = vec2(_uv.x, _uv.y);
	gl_Position = projection * view * model * vec4(_pos, 1.0);
}