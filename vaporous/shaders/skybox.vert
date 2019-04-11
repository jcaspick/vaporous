#version 330 core
layout (location = 0) in vec3 _pos;

out vec3 uvw;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    uvw = _pos;
    gl_Position = projection * view * vec4(_pos, 1.0);
}  