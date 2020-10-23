#version 460 core

in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

uniform Material material;

void main()
{
    gl_FragColor = texture(material.diffuse, TexCoords);
}