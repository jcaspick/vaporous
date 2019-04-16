#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform mat4 iview;
uniform mat4 iprojection;
uniform float hueShift;

void main()
{
	vec2 ndc = uv * 2 - vec2(1, 1);
	vec4 clipSpace = vec4(ndc, -1.0f, 1.0f);
	vec4 eyeSpace = iprojection * clipSpace;
	eyeSpace = vec4(eyeSpace.xy, -1.0f, 0.0f);
	vec3 worldRay = normalize((iview * eyeSpace).xyz);

	float c = 6.283 * hueShift;
	vec3 bright = vec3(
		((sin(c) + 1.0f) * 0.5f) * 0.8f + 0.2f,
		((sin(c + 1.0f) + 1.0f) * 0.5f) * 0.8f + 0.2f,
		((sin(c + 2.0f) + 1.0f) * 0.5f) * 0.8f + 0.2f
	);
	vec3 dark = vec3(
		((sin(c + 0.5f) + 1.0f) * 0.5f) * 0.4f,
		((sin(c + 1.5f) + 1.0f) * 0.5f) * 0.4f,
		((sin(c + 2.5f) + 1.0f) * 0.5f) * 0.4f
	);

	float gradientIndex = min(worldRay.y / 0.2f, 1.0f);
	float isAboveHorizon = floor(worldRay.y + 1.0f);
	float horizonLight = 1.0f - min(worldRay.y / 0.05f, 1.0f);
	float shadow = 1.0f - 0.5f * worldRay.y;

	fragColor = vec4(mix(bright, dark, gradientIndex), 1.0f)
		* isAboveHorizon * (1 + 0.25f * horizonLight) * shadow;
}