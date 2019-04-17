#version 330 core
out vec4 fragColor;

in vec2 uv;
in vec3 normal;

uniform sampler2D mainTex;
uniform float alpha;

void main()
{
	fragColor = vec4(vec3(texture(mainTex, uv)), alpha);
}