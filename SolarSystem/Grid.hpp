#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Grid
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> primitives;
};

Grid createBasicGrid(int w, int h)
{
	float tmp = w > h ? w : h;
	float wstep = 0.1f;
	float hstep = 0.1f;

	Grid grid
	{
		std::vector<glm::vec3>(w * h),
		std::vector<unsigned int>(3 * (w - 1) * (h - 1) * 2)
	};

	for (int j = 0; j < h; j++)
	{
		int tmp = w * j;
		int hoff = (w - 1) * 6 * j;
		for (int i = 0; i < w; i++)
		{
			grid.vertices[i + tmp].x = (float)(i * wstep);
			grid.vertices[i + tmp].y = (float)(j * hstep);
			grid.vertices[i + tmp].z = 0.0f;

			if (i < w - 1 && j < h - 1)
			{
				grid.primitives[6 * i + hoff] = i + tmp;
				grid.primitives[6 * i + hoff + 1] = i + tmp + w;
				grid.primitives[6 * i + hoff + 2] = i + 1 + tmp;

				grid.primitives[6 * i + hoff + 3] = i + tmp + w;
				grid.primitives[6 * i + hoff + 4] = i + 1 + tmp;
				grid.primitives[6 * i + hoff + 5] = i + 1 + tmp + w;
			}
		}
	}

	return grid;
}