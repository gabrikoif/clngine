#version 330 core

uniform vec3 uColor;
out vec4 result;

void main()
{
  result = vec4(uColor, 1.0); 
}