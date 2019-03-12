#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <GL/glut.h>

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
using namespace std;

int xmain, ymain, rmain, rstore[100];
void display(void);
void myinit(void);
void count_circle();
void circlepoint(int, int);
void draw_circle(int, int, int);
void call();
//void animate();
/* Function to handle file input; modification may be needed */
void file_in(void);

/*-----------------
The main function
------------------*/
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	/* Use both double buffering and Z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(XOFF, YOFF);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CS6533 Assignment 1");
	glutDisplayFunc(display);
	/* Function call to handle file input here */
	myinit();
	glutMainLoop();

	return 0;
}

/*----------
file_in(): file input function. Modify here.
------------*/
void file_in(void)
{
	int temp;
	int x, y, r;
	ifstream file;
	file.open("Text.txt");
	if (file.fail())
		cout << "File did not work";
	else
	{
		while (file >> temp)
			for (int i = 0; i < temp; i++)
			{
				file >> x;
				file >> y;
				file >> r;
				xmain = x;
				ymain = y;
				rmain = r;
				rstore[i] = r;
				int xuse = max(xmain + rmain, xmain - rmain);
				int yuse = max(ymain + rmain, ymain - rmain);
				int maxabs = max(xuse, yuse);
				if (maxabs > WINDOW_WIDTH/2)
				{
					xmain = (xmain*(WINDOW_WIDTH/2)) / maxabs;
					ymain = (ymain*(WINDOW_WIDTH/2)) / maxabs;
					rmain = (rmain*(WINDOW_WIDTH/2)) / maxabs;
				}
				draw_circle(xmain, ymain, rmain);
			}
	}
	file.close();
}

void count_circle()		//User Input for number of Circles to be drawn
{
	int count;
	cout << "\nEnter the number of circles to display : "; cin >> count;
	for (int i = 0; i < count; i++)
	{
		cout << "Enter the X-Coordinate of Circle " << i + 1 << " : "; cin >> xmain;
		cout << "Enter the Y-Coordinate of Circle " << i + 1 << " : "; cin >> ymain;
		cout << "Enter the Radius of Circle " << i + 1 << " : "; cin >> rmain;
		draw_circle(xmain, ymain, rmain);
	}
}

void circlepoint(int x, int y)	//Plot Points for Octants
{
	glBegin(GL_POINTS);
	glVertex2i(x + xmain, y + ymain);
	glVertex2i(x + xmain, -y + ymain);
	glVertex2i(-x + xmain, y + ymain);
	glVertex2i(-x + xmain, -y + ymain);
	glVertex2i(y + xmain, x + ymain);
	glVertex2i(y + xmain, -x + ymain);
	glVertex2i(-y + xmain, x + ymain);
	glVertex2i(-y + xmain, -x + ymain);
	glEnd();
}

void draw_circle(int xtemp, int ytemp, int rtemp)
{
	rtemp = rmain;	//Initialize Variables
	xtemp = 0;
	ytemp = rtemp;
	float d = 5 / 4 - rtemp;	//D-initial
	while (ytemp > xtemp)
	{
		circlepoint(xtemp, ytemp);
		if (d < 0)	//Select E
		{
			xtemp++;
			d = d + 2 * xtemp + 1;
		}
		else	//Select SE
		{
			ytemp--;
			xtemp++;
			d = d + 2 * (xtemp - ytemp) + 2;
		}
	}
}

void call()
{
	int a;
	cout << "1 = Draw Custom Circle : \n";
	cout << "2 = File Input Circle : \n";
	cout << "0 = Exit : \n";
	cout << "Enter Coordinate Input Mode : "; cin >> a;
	if (a == 1)
	{
		count_circle();
	}
	else if (a == 2)
	{
		file_in();
	}
	else if (a == 0)
		exit(0);
	else
	{
		cout << "Please re-enter";
		call();
	}
}



/*---------------------------------------------------------------------
display(): This function is called once for _every_ frame.
---------------------------------------------------------------------*/
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.84, 0);              /* draw in golden yellow */
	glPointSize(1.0);                     /* size of each point */
	call();
	glFlush();	                          /* render graphics */
	glutSwapBuffers();                    /* swap buffers */
}

/*---------------------------------------------------------------------
myinit(): Set up attributes and viewing
---------------------------------------------------------------------*/
void myinit()
{
	glClearColor(0.0, 0.0, 0.92, 0.0);    /* blue background*/
	/* Set-Up Viewing */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2);
	glMatrixMode(GL_MODELVIEW);
}
