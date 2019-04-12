#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D mainTex;
uniform mat4 iview;
uniform mat4 iprojection;

void main()
{
	vec2 ndc = uv * 2 - vec2(1, 1);
	vec4 clipSpace = vec4(ndc, -1.0f, 1.0f);
	vec4 eyeSpace = iprojection * clipSpace;
	eyeSpace = vec4(eyeSpace.xy, -1.0f, 0.0f);
	vec3 worldRay = normalize((iview * eyeSpace).xyz);

	fragColor = texture(mainTex, vec2(0.5f, worldRay.y * 3.0f))
		* floor(worldRay.y + 1.0f);
}