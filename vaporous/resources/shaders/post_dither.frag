#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D mainTex;
uniform sampler2D blurTex;
uniform sampler2D watermark;
uniform vec2 window;
uniform float fade;

void main()
{
	float dither[36] = float[36](
		0.0f, 32.0f, 22.0f, 7.0f, 24.0f, 12.0f,
		14.0f, 2.0f, 34.0f, 19.0f, 9.0f, 26.0f,
		28.0f, 16.0f, 4.0f, 31.0f, 21.0f, 11.0f,
		6.0f, 25.0f, 13.0f, 1.0f, 33.0f, 23.0f,
		18.0f, 8.0f, 27.0f, 15.0f, 3.0f, 35.0f,
		30.0f, 20.0f, 10.0f, 29.0f, 17.0f, 5.0f);

	ivec2 index = ivec2(
		int(uv.x * window.x) % 6,
		int(uv.y * window.y) % 6);
	float alpha = 1.0f - ceil(fade - 
		dither[index.x + index.y * 6] / 36);
	vec3 color = texture(mainTex, uv).rgb;
	vec3 blur = texture(blurTex, uv).rgb;

	vec2 wmUv = vec2((uv.x * window.x - (window.x - 128))
		/ 128, uv.y * window.y / 16);
	vec3 wm = texture(watermark, wmUv).rgb;

	fragColor = vec4(color + blur + wm * 0.2f, alpha);
}