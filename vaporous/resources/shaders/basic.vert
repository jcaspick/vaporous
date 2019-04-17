#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec2 _uv;
layout (location = 2) in vec3 _normal;

out vec2 uv;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	uv = vec2(_uv.x, _uv.y);
	normal = mat3(transpose(inverse(model))) * _normal;
	fragPos = vec3(model * vec4(_pos, 1.0f));
	gl_Position = projection * view * model * vec4(_pos, 1.0);
}