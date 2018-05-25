#include "extern.h"
#include "Character.h"
#include "Grid.h"

#ifndef CHARA_POINT_SIZE
#define CHARA_POINT_SIZE 6.0f
#endif

bool Character::is_backward(int di)
{
	if (dir == NORTH&&di == SOUTH)
		return true;
	if (dir == SOUTH&&di == NORTH)
		return true;
	if (dir == EAST&&di == WEST)
		return true;
	if (dir == WEST&&di == EAST)
		return true;
	return false;
}

bool Character::is_onTerritory()
{
	return Territory.isGrid(x, y);
}

bool Character::is_on(int i)
{
	switch (i)
	{
	case TERRITORY:
		return Territory.isGrid(x, y);
	case PATH:
		return Path.isGrid(x, y);
	}
}
int Character::getX()
{
	return x;
}

int Character::getY()
{
	return y;
}


void Character::setPos(int Pos_x, int Pos_y)
{
	x = Pos_x;
	y = Pos_y;
}

void Character::setColor(float r, float g, float b)
{
	Color[0] = r;
	Color[1] = g;
	Color[2] = b;
}

Character::Character(int Pos_x, int Pos_y, float r, float g, float b)
{
	setPos(Pos_x, Pos_y);
	setColor(r, g, b);
	dir = nulldir;
}

void Character::setDir(int di)
{
	dir = di;
}
int Character::getDir()
{
	return dir;
}

void Character::draw()
{
	glPointSize(CHARA_POINT_SIZE);
	glBegin(GL_POINTS);
	glColor3f(Color[0], Color[1], Color[2]);
	glVertex3f(x, y, 0);
	glEnd();
}


void Character::move()
{
	switch (dir)
	{
	case NORTH:
		if(y<GRID_WIDTH)
			setPos(x,y+1);
		break;
	case SOUTH:
		if(y>0)
			setPos(x, y - 1);
		break;
	case EAST:
		if (x<GRID_HEIGHT)
			setPos(x+1, y);
		break;
	case WEST:
		if (x>0)
			setPos(x - 1, y);
		break;
	default:
		break;
	}
}


bool Character::is_closed(Character& P, int i)
{
	switch (i)
	{
	case NORTH:
		if (P.getY() > y)
			return true;
		return false;
	case SOUTH:
		if (P.getY() < y)
			return true;
		return false;
	case WEST:
		if (P.getX() < x)
			return true;
		return false;
	case EAST:
		if (P.getX() > x)
			return true;
		return false;


	}
}


bool Character::is_blocked(int x, int y)
{
	if (Territory.isGrid(x, y) || x <= 0 || y <= 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT)
		return true;
	return false;
}
bool Character::is_blocked(int i)
{
	switch (i)
	{
	case NORTH:
		if (Territory.isGrid(x, y + 1) || y >= GRID_HEIGHT)
			return true;
		return false;
	case SOUTH:
		if (Territory.isGrid(x, y - 1) || y <= 0)
			return true;
		return false;
	case WEST:
		if (Territory.isGrid(x-1, y) || x <= 0)
			return true;
		return false;
	case EAST:
		if (Territory.isGrid(x + 1, y) || x >= GRID_WIDTH)
			return true;
		return false;
	}
}

int Character::distance(Character& P)
{
	return abs(x - P.getX()) + abs(y - P.getY());
}

int Character::distance(int X, int Y)
{
	return abs(x - X) + abs(y - Y);
}


Player::Player(int Pos_x, int Pos_y, float r, float g, float b) : Character(Pos_x,Pos_y,r,g,b)
{

}

void Player::drawPath()
{
	Path.setPoint(x,y,true);
}

void Player::processBR()
{
	BR_N = max(BR_N, y);
	BR_S = min(BR_S, y);
	BR_W = min(BR_W, x);
	BR_E = max(BR_E, x);
}

void Player::setBR(int E, int W,int N,int S)
{
	BR_E = E;
	BR_W = W;
	BR_N = N;
	BR_S = S;
}
void Player::move()
{
	Character::move();
	processBR();
}

int Player::getBRN()
{
	return BR_N;
}
int Player::getBRS()
{
	return BR_S;
}
int Player::getBRW()
{
	return BR_W;
}
int Player::getBRE()
{
	return BR_E;
}

bool Player::is_inBR(int x, int y)
{
	if (x<BR_E&&x>BR_W&&y<BR_N&&y>BR_S)
		return true;
	return false;
}
bool Player::is_inBR(Character& P)
{
	int X = P.getX();
	int Y = P.getY();
	if (X<BR_E&&X>BR_W&&Y<BR_N&&Y>BR_S)
		return true;
	return false;
}


void Player::flood(int x, int y)
{
	if (Flood.isGrid(x, y))
		return;
	if (x > BR_E || x<BR_W || y>BR_N || y < BR_S)
		return;
	if (Path.isGrid(x, y))
		return;
	if (Territory.isGrid(x, y))
		return;
	Flood.setPoint(x, y, true);
	flood(x + 1, y);
	flood(x - 1, y);
	flood(x, y+1);
	flood(x, y-1);


}


void Player::PathToTer()//경로를 영토로 바꾸는 함수
{
	if (Path.isEmpty())
		return;
	PREV_B_EMPTY = false;
	floodChecking(BR_W, BR_E, BR_N, false);//위쪽의 수평 경계를 왼쪽에서 오른쪽으로 따라가며 check. 수직이 아니므로 마지막 인자는 false.
	floodChecking(BR_N, BR_S, BR_E, true);//오른쪽의 수직 경계를 위쪽에서 아래쪽으로 내려가며 check.수직이므로 true
	floodChecking(BR_E, BR_W, BR_S, false);
	floodChecking(BR_S, BR_N, BR_W, true);
	FloodtoTerritory();

	Flood.empty();
	Path.empty();
}

void Player::FloodtoTerritory()
{
	score=0;
	for (int i = BR_W; i <= BR_E; i++)
	{
		for (int j = BR_S; j <= BR_N; j++)
		{
			Territory.setPoint(i,j,!Flood.isGrid(i,j));
			if (!Flood.isGrid(i, j))
				score++;
		}
	}
}


//BR_SIDE쪽의 경계를 BR_START부터 BR_END까지 돌면서 flood를 가하는 동시에 경계의 emptiness를 check 하는 함수
void Player::floodChecking(int BR_START, int BR_END, int BR_SIDE, bool isHorizontal)
{
	if (!isHorizontal)
	{
		for (int i = BR_START; ; )
		{
			if (!PREV_B_EMPTY && !Territory.isGrid(i, BR_SIDE) && !Path.isGrid(i, BR_SIDE))
			{
				flood(i, BR_SIDE);//flooding을 가한다.
				PREV_B_EMPTY = true;//빈 경계임을 표현함.
				continue;
			}
			if (Territory.isGrid(i, BR_SIDE) || Path.isGrid(i, BR_SIDE))//경계가 비어있지 않으면
				PREV_B_EMPTY = false;//경계가 비어있지 않았음을 표현함.
			if (BR_START > BR_END)
				i--;
			if (BR_END > BR_START)
				i++;
			if (BR_START > BR_END&&i < BR_END)
				break;
			if (BR_END > BR_START&&i > BR_END)
				break;


		}
	}
	else
	{
		for (int i = BR_START;;)
		{
			if (!PREV_B_EMPTY && !Territory.isGrid(BR_SIDE, i) && !Path.isGrid(BR_SIDE, i))//경계가 비어있지 않으면
			{
				flood(BR_SIDE, i);//flooding을 가한다.
				PREV_B_EMPTY = true;//빈 경계임을 표현함.
				continue;
			}
			if (Territory.isGrid(BR_SIDE, i) || Path.isGrid(BR_SIDE, i))//경계가 비어있지 않으면
				PREV_B_EMPTY = false;//경계가 비어있지 않았음을 표현함.
			if (BR_START > BR_END)
				i--;
			if (BR_END > BR_START)
				i++;
			if (BR_START > BR_END&&i < BR_END)
				break;
			if (BR_END > BR_START&&i > BR_END)
				break;


		}

	}
}

int Player::Depth_in_BR(int X, int Y)//Boundary Rectangle 안의 점의 깊이를 반환하는 함수
{
	if (X > BR_E | X < BR_W) return 0;
	if (Y > BR_N | Y < BR_S) return 0;
	return min(abs(BR_E - X), abs(BR_W - X)) + min(abs(BR_N - Y), abs(BR_S - Y));
}
