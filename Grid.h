#pragma once

#include <iostream>
#include <GL/glut.h>

using namespace std;

#ifndef GRID_WIDTH
#define GRID_WIDTH 50		
#endif

#ifndef GRID_HEIGHT
#define GRID_HEIGHT 50		
#endif


class Grid
{
public:
	Grid();
	Grid(float r, float g, float b, int layer, float point_size);
	bool isEmpty();
	void empty();
	bool isGrid(int x, int y);
	void setPoint(int x, int y, bool b);
	void setRec(int max_x, int min_x, int max_y, int min_y,bool b);
	void draw();

private:
	bool grid[GRID_WIDTH][GRID_HEIGHT] = { 0 };
	bool emp=true;
	float color[3];
	int layer=1;
	float point_size;
};