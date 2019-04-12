#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec3 _uv;
//layout (location = 3) in vec4 _data1;
//layout (location = 4) in vec4 _data2;

out vec2 uv;
out float xFacing;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float width;
uniform float height;
uniform float lngth;

void main()
{
	float globalScale = 0.57f; // from ruedehue

	uv = vec2(_uv.x, _uv.y);
	xFacing = _uv.z;
	vec3 scaled = vec3(_pos.x * width * globalScale, 
		_pos.y * height * globalScale, _pos.z * lngth * globalScale);
	gl_Position = projection * view * model * vec4(scaled, 1.0);
}