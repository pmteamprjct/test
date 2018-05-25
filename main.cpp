#include "Character.h"
#include "Zombie.h"
#include "Grid.h"
#include <time.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <algorithm>
clock_t clock_start = clock();
clock_t clock_end;
bool GameOver = false;
bool GameWin = false;
//int select = -1;
//Windows.h와 충돌하여 제외했습니다
int debugger_int = 0;
int life = 3;

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



int zombie_move_count = 0;
int score;

Player P1(GRID_WIDTH/2, GRID_HEIGHT/2, 1, 0, 0);
vector<Zombie> Zombies;
Grid Territory(0.5, 0, 0.5, 1,GRID_POINT_SIZE);
Grid Path(0.5, 0, 0.5, 1, PATH_POINT_SIZE);
Grid Flood;
#define INIT_TERRITORY_LENGTH 3


void init()
{	
	Territory.setRec(GRID_WIDTH/2+ INIT_TERRITORY_LENGTH, GRID_WIDTH / 2 - INIT_TERRITORY_LENGTH, 
		GRID_HEIGHT / 2 + INIT_TERRITORY_LENGTH, GRID_HEIGHT / 2 - INIT_TERRITORY_LENGTH, true);//초기 영토 지정
	P1.setBR(GRID_WIDTH / 2 + INIT_TERRITORY_LENGTH, GRID_WIDTH / 2 - INIT_TERRITORY_LENGTH,
		GRID_HEIGHT / 2 + INIT_TERRITORY_LENGTH, GRID_HEIGHT / 2 - INIT_TERRITORY_LENGTH);//초기 Boundary Rectangle 지정
	
	Zombies.push_back(Zombie(GRID_WIDTH / 4, GRID_HEIGHT / 4, 0, 1, 0));
	Zombies.push_back(Zombie(GRID_WIDTH*3 / 4, GRID_HEIGHT / 4, 1, 0, 1));
	Zombies.push_back(Zombie(GRID_WIDTH / 4, GRID_HEIGHT*3 / 4, 0, 0, 1));
	Zombies.push_back(Zombie(GRID_WIDTH*3 / 4, GRID_HEIGHT*3 / 4, 1, 1, 0));
}


//새로 정의한 함수
//게임 오버시 재도전 processSpecialKeys에서 호출
void reset() {
	GameOver = false;
	GameWin = false;
	debugger_int = 0;
	life = 3;
	score = 0;
	Zombies.erase(Zombies.begin(), Zombies.end());
	P1.setPos(GRID_WIDTH / 2, GRID_HEIGHT / 2);
	P1.setDir(nulldir);
	Path.empty();
	Territory.empty();
	init();
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

	glColor3f(1, 1, 1);
	int score_percent = (score * 100) / (GRID_WIDTH*GRID_HEIGHT);
	if (score_percent >= 60)
		GameWin = true;
	string scr = to_string(score_percent);
	scr.append("%");
	renderBitmapCharacter(GRID_WIDTH - 10, GRID_HEIGHT - 10, GLUT_BITMAP_TIMES_ROMAN_10, scr);
	renderBitmapCharacter(10, GRID_HEIGHT - 10, GLUT_BITMAP_TIMES_ROMAN_10, "kill: " + to_string(debugger_int));
	renderBitmapCharacter(10, GRID_HEIGHT - 11, GLUT_BITMAP_TIMES_ROMAN_10, "life: " + to_string(life));

	// GameWin 상황에 글자가 제대로 표시되지 않는 것 같아 순서를 바꿔보았습니다.

	if (GameOver)
	{
			glColor3f(1, 1, 1);
			renderBitmapCharacter(GRID_WIDTH / 2, GRID_HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
			renderBitmapCharacter(GRID_WIDTH / 2, GRID_HEIGHT / 2-1, GLUT_BITMAP_TIMES_ROMAN_10, "Press INSERT to retry");
	}
	if (GameWin)
	{
		glColor3f(1, 1, 1);
		renderBitmapCharacter(GRID_WIDTH / 2, GRID_HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24, "GAME WIN");
	}

	

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
	case GLUT_KEY_INSERT:
		if (GameOver)
			reset();
		break;
	}

}







void Zombies_Think()
{
	int k = Zombies.size();
	for (int i = 0; i < k; i++)
	{
		Zombies[i].ZombieCheckRisk();
		
		if (Zombies[i].getRisk() < 15)
		{
			Zombies[i].ZombiePathFinder();
			for (int j = 0; j < k; j++)
			{
				if (j == i)
					continue;
				if (Zombies[i].distance(Zombies[j]) < 7)
				{
					Zombies[i].ZombieMoveAwayFrom(Zombies[j]);
					break;
				}
			}
		}
		else
		{
			Zombies[i].ZombieMoveOutFromBR();
		}
		for (int j = 0; j < k; j++)
		{
			if (j == i)
				continue;
			if (Zombies[i].distance(Zombies[j]) < 1)
			{
				Zombies[i].ZombieMoveAwayFrom(Zombies[j]);
				break;
			}
		}

	}


}

void processIdle()
{
	if (GameWin)
		return;
	if (GameOver)
		return;
	clock_end = clock();
	if (clock_end - clock_start > 1000 / 15)
	{

		zombie_move_count++;
		Zombies_Think();
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
				(life>0) ? life-- : life = 0;  // 목숨 감소
				if (life == 0)
					GameOver = true;
				else {
					Path.empty();
					P1.setPos(GRID_WIDTH / 2, GRID_HEIGHT / 2);
					P1.setDir(nulldir);
					Sleep(500); //Windows.h 함수
					// 경로, 위치, 진행방향 초기화 후 0.5초 정지
				}

			}
			if (Zombies[i].is_on(PATH))
			{
				(life>0) ? life-- : life = 0;  // 목숨 감소
				if (life == 0)
					GameOver = true;
				else {
					Path.empty();
					P1.setPos(GRID_WIDTH / 2, GRID_HEIGHT / 2);
					P1.setDir(nulldir);
					Sleep(500); // Windows.h 함수
					// 경로, 위치, 진행방향 초기화 후 0.5초 정지
				}
			}
			if (Zombies[i].is_on(TERRITORY))
			{
				Zombies.erase(Zombies.begin()+i);
				k = Zombies.size();
				debugger_int++;
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
