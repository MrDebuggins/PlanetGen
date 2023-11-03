#include <stdio.h>
#include <vector>
#include <GL/glew.h>


std::vector<glm::fvec2> getNoiseMat(int w, int h)
{
	std::vector<glm::fvec2> mat(w * h);

	for (int i = 0; i < w * h; i++)
	{
		mat[i].x = rand() / RAND_MAX;
		mat[i].y = rand() / RAND_MAX;
	}
	
	return mat;
}