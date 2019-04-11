#version 330 core
out vec4 fragColor;

in vec3 uvw;
uniform samplerCube cubemap;

void main()
{    
    fragColor = texture(cubemap, uvw);
}