#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 3) in vec4 _offset;
layout (location = 4) in vec4 _size;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 pos = vec3(cos(-_offset.w) * _pos.x * _size.x * 0.5f, 
		_pos.y * _size.y, sin(-_offset.w) * _pos.x * _size.x * 0.5f);
	gl_Position = projection * view * model * vec4(pos + _offset.xyz, 1.0f);
}