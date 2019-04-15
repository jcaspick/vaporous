#version 330 core
out vec4 fragColor;

in vec2 uv;
in float xFacing;
in vec3 size;
in vec3 worldPos;
in float totalHeight;

uniform sampler2D windowColor;
uniform sampler2D noise;

void main()
{
	// width/height of the noise texture
	float noiseRes = 256.0f;

	// common world space y-position of all buildings
	float cityBottom = -24.0f;

	// using the size values to pick values for the noise offset
	// and hue shift, creates random looking variety, and has the
	// advantage of ensuring multiple tiers of the same building
	// have the same color
	float noiseOffsetX = round(size.x);
	float noiseOffsetY = round(size.y);
	float hueShift = size.x * 0.01f + size.z * 0.07f;

	// modulo over the area of the wall to determine if it is a window
	float uvScale = mix(size.x, size.z, xFacing);
	int column = int(uv.x * (uvScale * 2) + 0.5f);
	int row = int(uv.y * (size.y * 2) + 0.5f);
	float isWindow = float(column % 2) * float(row % 2)
		* min(1.0f, totalHeight);

	// a bit of math on the UV coordinates to sample a noise texture such that
	// 1 pixel lines up with one window. Determines if the window is lit
	vec2 noiseIndex = vec2(
		0.125f * uv.x * (1.0f / noiseRes) * (uvScale * 2.0f)
			+ ((row + noiseOffsetX) * 0.25f / noiseRes),
		uv.y * (1.0f / noiseRes) * (size.y * 2.0f)
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

	// put everything together, plus a fade to black near the bottom
	float heightIndex = (worldPos.y - cityBottom) / (totalHeight - cityBottom);
	fragColor = vec4(mix(bottomColor, topColor, heightIndex) * isWindow
		* isLit * min((worldPos.y * 0.1f - cityBottom * 0.1f), 1.0f), 1.0f);
}