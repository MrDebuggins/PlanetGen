#version 430

layout(location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

void main() 
{
	vec3 npos = vPos - cameraPos;
	gl_Position = projection * view * model * vec4(npos, 1.0);
}