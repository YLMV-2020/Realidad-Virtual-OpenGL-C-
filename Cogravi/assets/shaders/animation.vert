#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBone;    
layout (location = 4) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

void main()
{
	mat4 boneTransform = bones[aBone[0]] * aWeights[0];
		boneTransform += bones[aBone[1]] * aWeights[1];
		boneTransform += bones[aBone[2]] * aWeights[2];
		boneTransform += bones[aBone[3]] * aWeights[3];		
    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;

	gl_Position = projection * view * model * boneTransform * vec4(aPos, 1.0);
}