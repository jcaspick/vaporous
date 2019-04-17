#version 330 core
out vec4 fragColor;

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

	vec3 diffuse = vec3(texture(mainTex, uv));
	vec4 reflective = texture(reflectiveMap, uv);
	vec3 reflections = vec3(texture(cubeMap, r));

	fragColor = vec4(mix(diffuse, reflections, reflective.r), 1.0f);
}