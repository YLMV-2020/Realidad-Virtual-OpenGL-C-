#version 460 core

in vec2 text_coords;
in vec3 frag_pos;
in vec3 normal;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
	float transparency;
};

uniform Material material;

void main()
{
	gl_FragColor =  texture(material.diffuse, text_coords);
}