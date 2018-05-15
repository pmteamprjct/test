#include "Character.h"
#include "extern.h"
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


void Player::PathToTer()//��θ� ����� �ٲٴ� �Լ�
{
	if (Path.isEmpty())
		return;
	PREV_B_EMPTY = false;
	floodChecking(BR_W, BR_E, BR_N, false);//������ ���� ��踦 ���ʿ��� ���������� ���󰡸� check. ������ �ƴϹǷ� ������ ���ڴ� false.
	floodChecking(BR_N, BR_S, BR_E, true);//�������� ���� ��踦 ���ʿ��� �Ʒ������� �������� check.�����̹Ƿ� true
	floodChecking(BR_E, BR_W, BR_S, false);
	floodChecking(BR_S, BR_N, BR_W, true);
	FloodtoTerritory();

	Flood.empty();
	Path.empty();
}

void Player::FloodtoTerritory()
{
	for (int i = BR_W; i <= BR_E; i++)
	{
		for (int j = BR_S; j <= BR_N; j++)
		{
			Territory.setPoint(i,j,!Flood.isGrid(i,j));
		}
	}
}


//BR_SIDE���� ��踦 BR_START���� BR_END���� ���鼭 flood�� ���ϴ� ���ÿ� ����� emptiness�� check �ϴ� �Լ�
void Player::floodChecking(int BR_START, int BR_END, int BR_SIDE, bool isHorizontal)
{
	if (!isHorizontal)
	{
		for (int i = BR_START; ; )
		{
			if (!PREV_B_EMPTY && !Territory.isGrid(i, BR_SIDE) && !Path.isGrid(i, BR_SIDE))
			{
				flood(i, BR_SIDE);//flooding�� ���Ѵ�.
				PREV_B_EMPTY = true;//�� ������� ǥ����.
				continue;
			}
			if (Territory.isGrid(i, BR_SIDE) || Path.isGrid(i, BR_SIDE))//��谡 ������� ������
				PREV_B_EMPTY = false;//��谡 ������� �ʾ����� ǥ����.
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
			if (!PREV_B_EMPTY && !Territory.isGrid(BR_SIDE, i) && !Path.isGrid(BR_SIDE, i))//��谡 ������� ������
			{
				flood(BR_SIDE, i);//flooding�� ���Ѵ�.
				PREV_B_EMPTY = true;//�� ������� ǥ����.
				continue;
			}
			if (Territory.isGrid(BR_SIDE, i) || Path.isGrid(BR_SIDE, i))//��谡 ������� ������
				PREV_B_EMPTY = false;//��谡 ������� �ʾ����� ǥ����.
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