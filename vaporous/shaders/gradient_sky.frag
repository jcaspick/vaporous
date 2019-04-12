#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D mainTex;
uniform mat4 iview;
uniform mat4 iprojection;
uniform float skyColor;

void main()
{
	vec2 ndc = uv * 2 - vec2(1, 1);
	vec4 clipSpace = vec4(ndc, -1.0f, 1.0f);
	vec4 eyeSpace = iprojection * clipSpace;
	eyeSpace = vec4(eyeSpace.xy, -1.0f, 0.0f);
	vec3 worldRay = normalize((iview * eyeSpace).xyz);

	fragColor = texture(mainTex, vec2(skyColor, worldRay.y * 3.0f))
		* floor(worldRay.y + 1.0f);

//	float c = uv.x * 6.283f;
//	float minc = 0.0f;
//	float maxc = 0.4f;
//	fragColor = vec4(
//		(sin(c) + 0.5f) * 0.5f * (maxc - minc) + minc,
//		(sin(c + 1.5f) + 1.0f) * 0.5f * (maxc - minc) + minc,
//		(sin(c + 2.5f) + 1.0f) * 0.5f * (maxc - minc) + minc,
//		1.0f
//	);
}