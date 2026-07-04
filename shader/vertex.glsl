#version 330 core

// Standard attributes from your mesh
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords; 
layout (location = 2) in vec3 aNormal;

// The batched instanced matrix (takes up 4 slots)
layout (location = 3) in mat4 aInstanceMatrix; 
layout (location = 7) in vec3 aInstanceColor;

out vec3 uColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // aInstanceMatrix is the model matrix.
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
    uColor = aInstanceColor;
}