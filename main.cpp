#include "Character.h"
#include "Grid.h"
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>
clock_t clock_start = clock();
clock_t clock_end;
bool GameOver = false;

int debugger_int = 0;

#ifndef WIDTH
#define WIDTH 600		// window's width
#endif
#ifndef HEIGHT
#define HEIGHT 600		// window's height
#endif



#ifndef GRID_POINT_SIZE
#define GRID_POINT_SIZE 6.0f
#endif

#ifndef PATH_POINT_SIZE
#define PATH_POINT_SIZE 3.0f
#endif

#ifndef ZOMBIE_SPEED
#define ZOMBIE_SPEED 4
#endif



int zombie_move_count = 0;
int score;

Player P1(GRID_WIDTH/2, GRID_HEIGHT/2, 1, 0, 0);
vector<Character> Zombies;
Grid Territory(0.5, 0, 0.5, 1,GRID_POINT_SIZE);
Grid Path(0.5, 0, 0.5, 1, PATH_POINT_SIZE);
Grid Flood;
#define INIT_TERRITORY_LENGTH 3

void init()
{
	Territory.setRec(GRID_WIDTH/2+ INIT_TERRITORY_LENGTH, GRID_WIDTH / 2 - INIT_TERRITORY_LENGTH, 
		GRID_HEIGHT / 2 + INIT_TERRITORY_LENGTH, GRID_HEIGHT / 2 - INIT_TERRITORY_LENGTH, true);//�ʱ� ���� ����
	P1.setBR(GRID_WIDTH / 2 + INIT_TERRITORY_LENGTH, GRID_WIDTH / 2 - INIT_TERRITORY_LENGTH,
		GRID_HEIGHT / 2 + INIT_TERRITORY_LENGTH, GRID_HEIGHT / 2 - INIT_TERRITORY_LENGTH);//�ʱ� Boundary Rectangle ����
	Zombies.push_back(Character(GRID_WIDTH / 4, GRID_HEIGHT / 4, 0, 1, 0));
	Zombies.push_back(Character(GRID_WIDTH*3 / 4, GRID_HEIGHT / 4, 1, 0, 1));
	Zombies.push_back(Character(GRID_WIDTH / 4, GRID_HEIGHT*3 / 4, 0, 0, 1));
	Zombies.push_back(Character(GRID_WIDTH*3 / 4, GRID_HEIGHT*3 / 4, 1, 1, 0));


}
void renderBitmapCharacter(int x, int y,  void *font, string str)
{
	char *c;
	glRasterPos2i(x, y);
	int k = str.size();
	for (int i=0;i<k;i++)
	{
		glutBitmapCharacter(font, str.at(i));
	}
}


void renderScene() {
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	Territory.draw();
	Path.draw();
	P1.draw();
	int k = Zombies.size();
	for(int i = 0; i < k;i++)
		Zombies[i].draw();
	// make coordinate
	if (GameOver)
	{
			glColor3f(1, 1, 1);
			renderBitmapCharacter(GRID_WIDTH / 2, GRID_HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
	}

	glColor3f(1, 1, 1);
	string scr = to_string((score * 100) / (GRID_WIDTH*GRID_HEIGHT));
	scr.append("%");
	renderBitmapCharacter(GRID_WIDTH - 10, GRID_HEIGHT - 10, GLUT_BITMAP_TIMES_ROMAN_10, scr);
	renderBitmapCharacter(10, GRID_HEIGHT - 10, GLUT_BITMAP_TIMES_ROMAN_10, to_string(debugger_int));



	glutSwapBuffers();
}

void processSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (!P1.is_backward(NORTH))
			P1.setDir(NORTH);
		break;
	case GLUT_KEY_DOWN:
		if (!P1.is_backward(SOUTH))
			P1.setDir(SOUTH);
		break;
	case GLUT_KEY_LEFT:
		if (!P1.is_backward(WEST))
			P1.setDir(WEST);
		break;
	case GLUT_KEY_RIGHT:
		if (!P1.is_backward(EAST))
			P1.setDir(EAST);
		break;
	}
}

void ZombieMoveCloser(int i)
{
	if (!Zombies[i].is_blocked(NORTH) && Zombies[i].is_closed(P1, NORTH))
		Zombies[i].setDir(NORTH);
	else if (!Zombies[i].is_blocked(SOUTH) && Zombies[i].is_closed(P1, SOUTH))
		Zombies[i].setDir(SOUTH);
	else if (!Zombies[i].is_blocked(EAST) && Zombies[i].is_closed(P1, EAST))
		Zombies[i].setDir(EAST);
	else if (!Zombies[i].is_blocked(WEST) && Zombies[i].is_closed(P1, WEST))
		Zombies[i].setDir(WEST);
	else
		Zombies[i].setDir(nulldir);
}


void ZombieMoveCloser(int i, int X, int Y)
{
	if (!Zombies[i].is_blocked(NORTH) && Zombies[i].getY()<Y)
		Zombies[i].setDir(NORTH);
	else if (!Zombies[i].is_blocked(SOUTH) && Zombies[i].getY()>Y)
		Zombies[i].setDir(SOUTH);
	else if (!Zombies[i].is_blocked(EAST) && Zombies[i].getX()<X)
		Zombies[i].setDir(EAST);
	else if (!Zombies[i].is_blocked(WEST) && Zombies[i].getX()>X)
		Zombies[i].setDir(WEST);
	else
		Zombies[i].setDir(nulldir);
}


#define CLOSE GRID_WIDTH/5+GRID_HEIGHT/5



void ZombiePathFinder(int i)//Path�� ã�� ���
{

	//�ݰ� CLOSE �̳��� Path���� Ž���Ѵ�.
	int i1 = max(0, Zombies[i].getX() - CLOSE);
	int k1 = min(GRID_WIDTH-1, Zombies[i].getX() + CLOSE);
	int i2 = max(0, Zombies[i].getY() - CLOSE);
	int k2 = min(GRID_HEIGHT - 1, Zombies[i].getY() + CLOSE);
	int X=-1;
	int Y=-1;
	int dis = 999;
	for (i1=0; i1 <= GRID_WIDTH-1; i1++)
	{
		for (i2=0; i2 <= GRID_HEIGHT-1; i2++)
		{
			//�����ϴ� Path�� �� �Ÿ��� ���� ����� ���� ã�´�.
			if (Path.isGrid(i1, i2)&&Zombies[i].distance(i1,i2)<dis)
			{
				X = i1;
				Y = i2;
				dis = Zombies[i].distance(i1, i2);
			}
		}
	}
	if (X != -1 && Y != -1)
		ZombieMoveCloser(i, X, Y);//���� ����� ��ġ�� Path�� �޸���.
	else
		ZombieMoveCloser(i);
}

void ZombieCheckRisk(int i)
{

}



void ZombieThink()
{
	int k = Zombies.size();
	for (int i = 0; i < k; i++)
	{
		ZombieCheckRisk(i);
		ZombiePathFinder(i);
	}


}

void processIdle()
{
	if (GameOver)
		return;
	clock_end = clock();
	if (clock_end - clock_start > 1000 / 20)
	{

		zombie_move_count++;
		ZombieThink();
		P1.move();
		int k = Zombies.size();
		for (int i = 0; i < k; i++)
		{
			if (zombie_move_count == ZOMBIE_SPEED)
			{
				Zombies[i].move();
			}
			if (P1.is_on(PATH))
			{
				GameOver = true;
			}
			if (Zombies[i].is_on(PATH))
			{
				GameOver = true;
			}
			if (Zombies[i].is_on(TERRITORY))
			{
				Zombies.erase(Zombies.begin()+i);
				k = Zombies.size();
			}
		}
		if (zombie_move_count == ZOMBIE_SPEED)
			zombie_move_count = 0;
		if (!P1.is_onTerritory())
		{
			P1.drawPath();
		}
		else
		{
			P1.PathToTer();
		}
		clock_start = clock_end;
	}
	glutPostRedisplay();
}

void main(int argc, char **argv) {
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(650, 200);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Splix.io DEMO by KIM WOO CHANG");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, GRID_WIDTH, 0, GRID_HEIGHT);

	init();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutSpecialFunc(processSpecialKey);
	glutIdleFunc(processIdle);


	// enter GLUT event processing cycle
	glutMainLoop();
}
