#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 bone_ids;     
layout (location = 4) in vec4 weights;
layout (location = 5) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 frag_pos;

uniform mat4 projection;
uniform mat4 view;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

void main()
{
    mat4 bone_transform = bones[bone_ids[0]] * weights[0];
		bone_transform += bones[bone_ids[1]] * weights[1];
		bone_transform += bones[bone_ids[2]] * weights[2];
		bone_transform += bones[bone_ids[3]] * weights[3];
  
    vec4 boned_position = bone_transform * vec4(aPos,1.0); // transformed by bones

    
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * boned_position;
}