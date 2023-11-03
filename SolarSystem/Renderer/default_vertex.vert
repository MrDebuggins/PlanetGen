#version 400

layout(location = 0) in vec3 vPos;

uniform mat4 modelViewProjectionMatrix;

out vec3 pos;

void main() 
{
	vec3 npos = vPos;
	//npos.z = perlin(npos.x, npos.y);
	gl_Position = modelViewProjectionMatrix * vec4(npos, 1.0);

	pos = npos;
}