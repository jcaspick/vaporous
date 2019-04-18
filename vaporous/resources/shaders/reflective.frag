#version 330 core
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 glowColor;

in vec2 uv;
in vec3 normal;
in vec3 fragPos;

uniform vec3 cameraPos;
uniform sampler2D mainTex;
uniform sampler2D reflectiveMap;
uniform samplerCube cubeMap;
uniform float alpha;

void main()
{
	vec3 i = normalize(fragPos - cameraPos);
	vec3 r = reflect(i, normalize(normal));

	vec3 color = vec3(texture(mainTex, uv));
	vec4 reflective = texture(reflectiveMap, uv);
	vec3 reflections = vec3(texture(cubeMap, r));

	fragColor = vec4(mix(vec3(0), reflections, reflective.r) + color, 1.0f);
	glowColor = vec4(color, 1.0f);
}