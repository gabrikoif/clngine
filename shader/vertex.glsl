#version 330 core

// Standard attributes from your mesh
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords; 
layout (location = 2) in vec3 aNormal;

// The batched instanced matrix (takes up 4 slots)
layout (location = 3) in mat4 aInstanceMatrix; 
layout (location = 7) in vec3 aInstanceColor;

out vec3 objectColor;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // aInstanceMatrix is the model matrix.
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    objectColor = aInstanceColor;
    Normal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}