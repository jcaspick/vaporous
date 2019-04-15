#version 330 core
out vec4 fragColor;

in vec2 uv;
in float xFacing;

uniform sampler2D windowColor;
uniform sampler2D noise;
uniform float width;
uniform float height;
uniform float lngth;
uniform float hueShift;
uniform float noiseOffsetX;
uniform float noiseOffsetY;

void main()
{
	// width/height of the noise texture
	float noiseRes = 256.0f;

	// modulo over the area of the wall to determine if it is a window
	float uvScale = mix(width, lngth, xFacing);
	int column = int(uv.x * (uvScale * 2) + 0.5f);
	int row = int(uv.y * (height * 2) + 0.5f);
	float isWindow = float(column % 2) * float(row % 2);

	// a bit of math on the UV coordinates to sample a noise texture such that
	// 1 pixel lines up with one window. Determines if the window is lit
	vec2 noiseIndex = vec2(
		0.125f * uv.x * (1.0f / noiseRes) * (uvScale * 2.0f)
			+ ((row + noiseOffsetX) * 0.25f / noiseRes),
		uv.y * (1.0f / noiseRes) * (height * 2.0f)
			+ ((0.5f + noiseOffsetY) / noiseRes)
	);
	float isLit = max(floor(texture(noise, noiseIndex).r + 0.5f), 0.1f);

	// create window color gradient based on hue shift value
	float c = 6.283f * hueShift;
	vec3 topColor = vec3(
		((sin(c) + 1.0f) * 0.5f) * 0.2f + 0.8f,
		((sin(c + 1.0f) + 1.0f) * 0.5f) * 0.2f + 0.8f,
		((sin(c + 2.0f) + 1.0f) * 0.5f) * 0.2f + 0.8f
	);
	vec3 bottomColor = vec3(
		((sin(c + 1.0f) + 1.0f) * 0.5f) * 0.8f + 0.2f,
		((sin(c + 2.0f) + 1.0f) * 0.5f) * 0.8f + 0.2f,
		((sin(c + 3.0f) + 1.0f) * 0.5f) * 0.8f + 0.2f
	);

	fragColor = vec4(mix(bottomColor, topColor, uv.y) * isWindow * isLit, 1.0f);
}