/*
Block buster is a game of color removal.

Instructions:
Click on any block that has a nieghbor of the same color.
All connecting blocks that share the same color as the selected block will be removed.

The goal of the game is to remove as many blocks as possible

To increase or decrease the difficulty right click the screen at any time and
select your desired difficulty in the difficulty menu.

To restart the game right click and select new game.

Author: Dave Hoatlin
Date:   Feb 11, 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define boardSize 20
#define blockSize 25

#define RED 0
#define ORANGE 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define VIOLET 5
#define BLACK 9

#define EASIEST 2
#define EASY 3
#define NORMAL 4
#define HARD 5
#define HARDEST 6

#define NEW 0

// global color vector
GLfloat colorVec[3] = {0.0,0.0,0.0};

//the grid the blocks are stored on
int gameBoard[boardSize][boardSize];

//the current difficulty
int currentDifficulty = NORMAL;

// prototypes
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void setColor (int x, int y);
void drawSquare (int x, int y);
void makeGameBoard(int difficulty);
void drawGameBoard();
void setBlockColor(int color);
void breakBlocks(int x, int y);
int isBreakable(int x, int y);
void flood(int x, int y, int color);
void collapse();
void shift();
void menuOptions();


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Block Break");

	int difficultyMenu = glutCreateMenu(menuOptions);
	glutAddMenuEntry("Easiest", EASIEST);
	glutAddMenuEntry("Easy", EASY);
	glutAddMenuEntry("Normal", NORMAL);
	glutAddMenuEntry("Hard", HARD);
	glutAddMenuEntry("hardest", HARDEST);

	int mainMenu = glutCreateMenu(menuOptions);
	glutAddMenuEntry("New Game", NEW);
	glutAddSubMenu("Change Difficulty", difficultyMenu);
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	init();  
	glutDisplayFunc(display);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (mouse);
	glutMainLoop();

	return 0;
}


void init(void) 
{
// set background color, setup viewing, give instructions to user
   glClearColor (0.0, 0.0, 0.0, 0.0);

   glViewport(0, 0, 500, 500);
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   gluOrtho2D (0.0, 500.0, 0.0, 500.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();
   
   //seeding rand
   srand(time(NULL));
   //creating and drawing the game board. Initial difficult is 
   makeGameBoard(currentDifficulty);
   drawGameBoard();
}


void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   drawGameBoard();
   glFlush();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}


void mouse (int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		breakBlocks(x, y);
		collapse();
		shift();
	}
	glutPostRedisplay();
}


void setColor (int x, int y)
{
	if (x<=50 && y<50) {
		colorVec[0] = 1.0;
		colorVec[1] = 1.0;
		colorVec[2] = 1.0;
	}
}

void drawSquare (int x, int y)
{
	int xPix = x * 25;
	int yPix = 500 - (y * 25);
	glBegin(GL_POLYGON);
		glVertex2f(xPix, yPix);
		glVertex2f(xPix + blockSize, yPix);
		glVertex2f(xPix + blockSize, yPix - blockSize);
		glVertex2f(xPix, yPix - blockSize);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(xPix, yPix);
		glVertex2f(xPix + blockSize, yPix);
		glVertex2f(xPix + blockSize, yPix - blockSize);
		glVertex2f(xPix, yPix - blockSize);
	glEnd();
}

void makeGameBoard(int difficulty)
{
	int y, x;
	for(y = 0; y < boardSize; y++)
	{
		for(x = 0; x < boardSize; x++)
		{
			gameBoard[x][y] = rand() % difficulty;
			printf("%d ", gameBoard[x][y]);
		}
		printf("\n");
	}
}

void drawGameBoard()
{
	int i1, i2;
	for(i1 = 0; i1 < boardSize; i1++)
	{
		for(i2 = 0; i2 < boardSize; i2++)
		{
			setBlockColor(gameBoard[i1][i2]);
			glColor3fv(colorVec);
			drawSquare(i1, i2);
		}
	}
	printf("board drawn\n");
}

void setBlockColor(int color)
{
	switch(color)
	{
		case RED:
			colorVec[0] = 1.0;
			colorVec[1] = 0.0;
			colorVec[2] = 0.0;
			break;
		case ORANGE:
			colorVec[0] = 1.0;
			colorVec[1] = 0.65;
			colorVec[2] = 0.0;
			break;
		case YELLOW:
			colorVec[0] = 1.0;
			colorVec[1] = 1.0;
			colorVec[2] = 0.0;
			break;
		case GREEN:
			colorVec[0] = 0.0;
			colorVec[1] = 1.0;
			colorVec[2] = 0.0;
			break;
		case BLUE:
			colorVec[0] = 0.0;
			colorVec[1] = 0.0;
			colorVec[2] = 1.0;
			break;
		case VIOLET:
			colorVec[0] = 1.0;
			colorVec[1] = 0.0;
			colorVec[2] = 1.0;
			break;
		case BLACK:
			colorVec[0] = 0.0;
			colorVec[1] = 0.0;
			colorVec[2] = 0.0;
			break;
	}
}

void breakBlocks(int x, int y)
{
	int xloc = x / blockSize;
	int yloc = y / blockSize;
	printf("x: %d, y: %d ", xloc, yloc);
	if(isBreakable(xloc, yloc) == 1)
	{
		int breakColor = gameBoard[xloc][yloc];
		flood(xloc, yloc, breakColor);
	}
}

void flood(int x, int y, int color)
{
	//exit if a different color than what we want to delete
	if(gameBoard[x][y] != color)
	{
		return;
	}
	//set to black and recursively call in all directions
	else
	{
		gameBoard[x][y] = BLACK;
		if(x+1 < boardSize)
			flood(x+1, y, color);
		if(x-1 > -1)
			flood(x-1, y, color);
		if(y+1 < boardSize)
			flood(x, y+1, color);
		if(y-1 > -1)
			flood(x, y-1, color);
	}
}

int isBreakable(int x, int y)
{
	int retVal = 0;
	if(gameBoard[x][y] == BLACK)
		printf("no block here");
	else if(gameBoard[x][y] == gameBoard[x+1][y] && x+1 < boardSize)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x][y+1] && y+1 < boardSize)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x-1][y] && x-1 > -1)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x][y-1] && y-1 > -1)
		retVal = 1;
	if(retVal == 1)
		printf("breakable\n");
	else
		printf("\n");
	return retVal;
}

void collapse()
{
	int y, x, i;
	for(y = boardSize - 1; y > -1; y--)
	{
		for(x = 0; x < boardSize; x++)
		{
			if(gameBoard[x][y] == BLACK)
			{
				i = 0;
				while(1)
				{
					i++;
					if(y-i > -1)
					{
						if(gameBoard[x][y-i] != BLACK)
						{
							gameBoard[x][y] = gameBoard[x][y-i];
							gameBoard[x][y-i] = BLACK;
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

void shift()
{
	int x, y, i;
	int shiftBeg = -1;
	for(x = 0; x < boardSize; x++)
	{
		if(gameBoard[x][boardSize-1] == BLACK)
		{
			shiftBeg = x;
			break;
		}
	}
	if(shiftBeg > -1)
	{
		for(y = boardSize - 1; y > -1; y--)
		{
			for(x = shiftBeg; x < boardSize; x++)
			{
				if(gameBoard[x][y] == BLACK)
				{
					i = 0;
					while(1)
					{
						i++;
						if(x+i < boardSize)
						{
							if(gameBoard[x+i][y] != BLACK)
							{
								gameBoard[x][y] = gameBoard[x+i][y];
								gameBoard[x+i][y] = BLACK;
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
}

void menuOptions(int selection)
{
	switch(selection)
	{
		case EASIEST:
			currentDifficulty = EASIEST;
			makeGameBoard(currentDifficulty);
			break;
		case EASY:
			currentDifficulty = EASY;
			makeGameBoard(currentDifficulty);
			break;
		case NORMAL:
			currentDifficulty = NORMAL;
			makeGameBoard(currentDifficulty);
			break;
		case HARD:
			currentDifficulty = HARD;
			makeGameBoard(currentDifficulty);
			break;
		case HARDEST:
			currentDifficulty = HARDEST;
			makeGameBoard(currentDifficulty);
			break;
		case NEW:
			makeGameBoard(currentDifficulty);
			break;	
	}
	
	glutPostRedisplay();
}







