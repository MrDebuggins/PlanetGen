#version 400

layout(location = 0) in vec3 vPos;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform vec2 noiseMat[36];

out vec3 pos;

float interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

vec2 randGrad(int x, int y)
{
	uint w = 8 * 4;
	uint s = w / 2;

	x = x * 3284157443; y = y ^ x << s | x >> w - s;
	y = y * 1911520717; x = x ^ y << s | y >> w - s;
	x = x * 2048419325;

	float rand = x * (3.14159265 / 2147483648);

	return vec2(cos(x), sin(x));
}

float dotGridGradient(int ix, int iy, float x, float y)
{
	vec2 g = randGrad(ix, iy);

	float dx = x - ix;
	float dy = y - iy;

	return (dx*g.x + dy*g.y);
}

float perlin(float x, float y)
{
	int x0 = int(floor(x));
    int x1 = x0 + 1;
    int y0 = int(floor(y));
    int y1 = y0 + 1;

	float sx = x - float(x0);
    float sy = y - float(y0);

	float n0, n1, ix0, ix1, value;

	n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

	return interpolate (ix0, ix1, sy);
}

void main() 
{
	vec3 npos = vPos;
	npos.z = perlin(npos.x, npos.y);
	gl_Position = modelViewProjectionMatrix * vec4(npos, 1.0);

	pos = npos;
}