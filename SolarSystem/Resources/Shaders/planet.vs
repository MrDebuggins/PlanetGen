#version 430

layout(location = 0) in vec3 vPos;

void main() 
{
	vec3 npos = vPos;

	gl_Position = vec4(npos, 1.0);
}