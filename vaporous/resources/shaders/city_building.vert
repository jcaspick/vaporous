#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec3 _uv;
layout (location = 3) in vec4 _data1;
layout (location = 4) in vec4 _data2;
layout (location = 5) in vec2 _data3;

out vec2 uv;
out float xFacing;
out vec3 size;
out vec3 worldPos;
out float totalHeight;
out float percentLit;
out float hueShift;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float globalScale;

void main()
{
	// parsed vertex attributes
	vec3 offset = _data1.xyz;
	float rotation = _data1.w;

	// out variables
	uv = vec2(_uv.x, _uv.y);
	xFacing = _uv.z;
	size = _data2.xyz;
	totalHeight = _data2.w;
	hueShift = _data3.x;
	percentLit = _data3.y;

	// transforming model space positions with size and rotation data
	vec3 scaled = vec3(_pos.x * size.x * globalScale, 
		_pos.y * size.y * globalScale, _pos.z * size.z * globalScale);
	vec3 rotated = vec3(cos(rotation) * scaled.x - sin(rotation) * scaled.z, 
		scaled.y, sin(rotation) * scaled.x + cos(rotation) * scaled.z);

	worldPos = rotated + offset;

	gl_Position = projection * view * model * vec4(rotated + offset, 1.0);
}