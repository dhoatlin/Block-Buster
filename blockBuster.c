/*
Block buster is a game of color removal.

Instructions:
Click on any block that has a neighbor of the same color.
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
// #include <Windows.h> //windows specific include for using Sleep()
#include <GL/glut.h>
#include <time.h>

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

//the player's score
int score = 0;

//blocks broken this click. used for score
int blocksBroken = 0;

// prototypes
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void reshape(int x, int y);
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
int checkWin();
void updateScore();
void gameOver();

int main(int argc, char **argv)
{
	int mainMenu, difficultyMenu;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Block Buster");

	difficultyMenu = glutCreateMenu(menuOptions);
		glutAddMenuEntry("Easiest", EASIEST);
		glutAddMenuEntry("Easy", EASY);
		glutAddMenuEntry("Normal", NORMAL);
		glutAddMenuEntry("Hard", HARD);
		glutAddMenuEntry("Hardest", HARDEST);

	mainMenu = glutCreateMenu(menuOptions);
		glutAddMenuEntry("New Game", NEW);
		glutAddSubMenu("Change Difficulty", difficultyMenu);
		glutAttachMenu (GLUT_RIGHT_BUTTON);

	init();  
	glutDisplayFunc(display);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (mouse);
	glutReshapeFunc(reshape);
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
/*
	Forces window size to remain at 500 500
*/
void reshape(int x, int y)
{
	glutReshapeWindow(500, 500);
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

/*
	Destroys the blocks you clicked and checks the win condition
*/
void mouse (int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		breakBlocks(x, y);
		collapse();
		shift();
		if(checkWin())
		{
			printf("Game Over. Score: %d\n", score);
			gameOver();
		}
	}
	glutPostRedisplay();
}

/*
	Draws a block for the puzzle
*/
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

/*
	Generates and prints out the array containing all the block data
*/
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

/*
	Draws the game board using the array as a reference 
*/
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
}

/*
	Sets the color vector depending on the int value
	for each color.
*/
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

/*
	Verifies a group of blocks can be destroyed
	then starts the flood function and finally
	updates the score
*/
void breakBlocks(int x, int y)
{
	int xloc = x / blockSize;
	int yloc = y / blockSize;
	if(isBreakable(xloc, yloc) == 1)
	{
		int breakColor = gameBoard[xloc][yloc];
		flood(xloc, yloc, breakColor);
		updateScore();
	}
}

/*
	Flood finds all neighboring blocks sharing the 
	same color.
*/
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
		blocksBroken++;
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

/*
	Updates the score and resets the block broken count
*/
void updateScore()
{
	score += blocksBroken * blocksBroken;
	blocksBroken = 0;
}

/*
	Compare block colors and return true if this block
	is breakable
*/
int isBreakable(int x, int y)
{
	int retVal = 0;
	if(gameBoard[x][y] == BLACK)
		printf("no block here\n");
	else if(gameBoard[x][y] == gameBoard[x+1][y] && x+1 < boardSize)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x][y+1] && y+1 < boardSize)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x-1][y] && x-1 > -1)
		retVal = 1;
	else if(gameBoard[x][y] == gameBoard[x][y-1] && y-1 > -1)
		retVal = 1;
	return retVal;
}

/*
	Push all black blocks to the top of the puzzle
*/
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

/*
	Slides entire puzzle to the left if an entire column
	has been destroyed
*/
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

/*
	handle which menu item was selected
*/
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

/*
	Check to see if all possible blocks have been
	destroyed
*/
int checkWin()
{
	int x, y;
	int win = 1;
	for(x = 0; x < boardSize; x++)
	{
		for(y = 0; y < boardSize; y++)
		{
			if(gameBoard[x][y] != BLACK && isBreakable(x, y))
			{
					win = 0;
			}
		}
	}
	return win;
}

/*
	Display the score once the game is over then restart
*/
void gameOver()
{
	/*
	currently not working correctly. Could not get text to color or display

	char gameOverString[256];
	int i;
	sprintf(gameOverString, "Game Over. Score: %d\0", score);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(100.0, 100.0);
	for(i = 0; gameOverString[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, gameOverString[i]);
	}
	Sleep(5000);
	*/
	score = 0;
	makeGameBoard(currentDifficulty);
}