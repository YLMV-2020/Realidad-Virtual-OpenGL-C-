#version 460 core

in vec2 tex_cord;
in vec3 v_normal;
in vec3 v_pos;
in vec4 bw;
out vec4 color;

uniform sampler2D diff_texture;

void main()
{
	color =  texture(diff_texture, tex_cord);
}