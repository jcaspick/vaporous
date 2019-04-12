#version 330 core
out vec4 fragColor;

in vec2 uv;
in float xFacing;

uniform float width;
uniform float height;
uniform float lngth;

void main()
{
	float uvScale = mix(width, lngth, xFacing);
	int column = int(uv.x * (uvScale * 2 + 1));
	int row = int(uv.y * (height + 1));
	float light = float(column % 2) * float(row % 2);

	fragColor = vec4(light, light, xFacing, 1.0f);
}