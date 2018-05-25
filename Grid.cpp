#include "Grid.h"

#ifndef GRID_POINT_SIZE
#define GRID_POINT_SIZE 6.0f
#endif

#ifndef PATH_POINT_SIZE
#define PATH_POINT_SIZE 5.0f
#endif
Grid::Grid()
{
}

Grid::Grid(float r, float g, float b, int lay, float psize)
{
	layer = lay;
	color[0] = r;
	color[1] = g;
	color[2] = b;
	point_size = psize;
}

bool Grid::isGrid(int x, int y)
{
	return grid[x][y];
}

bool Grid::isEmpty()
{
	return emp;
}

void Grid::draw()
{
	glPointSize(point_size);

	glBegin(GL_POINTS);
	glColor3f(color[0], color[1], color[2]);

	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			if (grid[i][j] == true)
			{
				glVertex3i(i,j,-1);
			}
		}
	}
	glEnd();
}

void Grid::setPoint(int x, int y, bool b)
{
	if (b)
		emp = false;
	grid[x][y] = b;
}

void Grid::setRec(int max_x, int min_x, int max_y, int min_y, bool b)
{
	if (b)
		emp = false;
	for (int i = min_x; i <= max_x; i++)
	{
		for (int j = min_y; j <= max_y; j++)
		{
			grid[i][j] = b;
		}
	}
}

void Grid::empty()
{
	setRec(GRID_WIDTH - 1, 0, GRID_HEIGHT - 1, 0, false);
	emp = true;
}