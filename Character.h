#pragma once
#include <iostream>
#include <GL/glut.h>



#ifndef GRID_WIDTH
#define GRID_WIDTH 50
#endif

#ifndef GRID_HEIGHT
#define GRID_HEIGHT 50		
#endif



using namespace std;


enum { NORTH, EAST, SOUTH, WEST };

enum { TERRITORY, PATH };

#define nulldir -1

class Character
{
public:
	Character(int x, int y, float r, float g, float b);
	bool is_backward(int di);
	virtual void setDir(int di);
	void setColor(float r, float g, float b);
	void setPos(int pos_x, int pos_y);
	int getX();
	int getY();
	int getDir();
	virtual void move();
	bool is_onTerritory();
	bool is_on(int i);
	bool is_blocked(int i);
	bool is_blocked(int x, int y);
	bool is_closed(Character& P, int i);
	int distance(Character& P);
	int distance(int x, int y);
	void draw();

protected:
	int x = 0;
	int y = 0;
	int dir = nulldir;
	int pre_dir = nulldir;
	float Color[3];
};



class Player : public Character
{
public:
	Player(int x, int y, float r, float g, float b);
	void drawPath();//Player의 좌표를 Path Grid에 포함시킨다.
	void processBR();//Player를 포함하는 Boundary Rectangle 갱신한다.
	void setBR(int E, int W, int N, int S);
	int getBRN();
	int getBRS();
	int getBRW();
	int getBRE();
	bool is_inBR(int x, int y);
	bool is_inBR(Character& P);
	void move();
	void flood(int x, int y);
	void FloodtoTerritory();
	void floodChecking(int BR_Start, int BR_End, int BR_Side, bool isHorizontal);
	void PathToTer();//경로를 영토로 바꾸는 함수
	int Depth_in_BR(int x, int y);//Boundary Rectangle 안의 점의 깊이를 반환하는 함수
private:
	int BR_N;//Boundary Rectangle, North
	int BR_S;
	int BR_E;
	int BR_W;
	bool PREV_B_EMPTY;
};
