#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 ambient;

void main()
{    
    FragColor = texture(texture1, TexCoords) * vec4(ambient, 1.0f);
}