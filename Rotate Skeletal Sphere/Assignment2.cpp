#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Angel-yjc.h"
using namespace std;

#define PI 3.14159

// Functions Used
void file_in();
void drawObj(GLuint, int);
void baseset();
void axisset();
void sphere();
void ballpath();
void init(void);
void display(void);
float distance(const vec3&, const vec3&);
int nextpt();
bool pointpass();
void idle(void);
void reshape(int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);
void demo_menu(int);
void mouseclickmenu();

// Variables Used
GLuint program; // Shader Program Object-Id
GLuint aspect;  // Viewport Aspect-Ratio
GLfloat fovy = 45;  // Field-of-View (in degrees)
GLfloat zNear = 0.5; // Near Plane
GLfloat zFar = 15.0; // Far Plane
vec4 vrp(7, 3, -10, 1);	//VRP
vec4 eye = vrp; //Eye at VRP
vec3 ballcolor(1, 0.84, 0); //Yellow Ball
vec3 basecolor(0, 1, 0); //Green Base
vec3 axiscolor[3] = {vec3(1, 0, 0),vec3(1, 0, 1),vec3(0, 0, 1)}; //Red, Magenta, Blue - X,Y,Z axes respectively
vec3 pathpoints[3] = { vec3(3, 1, 5),vec3(-2, 1, -2.5),vec3(2, 1, -4) }; //Ball Path
vec3 basevertex[4] = { vec3(5, 0, 8),vec3(5, 0, -4),vec3(-5, 0, -4),vec3(-5, 0, 8) }; //Base Coordinates
vec3 ballpathvar[3]; //Variable to allot path in function
vec3 axisrotate[3];
vec3 ballcenter;
vector<vec3> ballcoord;
int ballax = 0;
int ballfin = 3;
float radius = 1.0;
float angle = 0.0;
float rate = 0.4;
bool rolling;
bool started;
mat4 rotationmat(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));


typedef struct pixelpts
{
	vec3* pts;
	vec3* colors;
	GLuint buffer;
	int size;
}pixelpts;
pixelpts base;
pixelpts axis;
pixelpts ball;

//Read from file
void file_in()
{
	int sel;
	string fpath;
	string file1;
	ifstream file;
	cout << "Enter the no of triangles in Wireframe : \n";
	cout << "1. 8 Triangles : \n";
	cout << "2. 128 Triangles : \n";
	cout << "3. Enter Path : \n";
	xyz:
	cout << "Enter your choice : ";
	cin >> sel;
	if (sel == 1)
	{
		file.open("Sphere8.txt");
	}
	else if (sel == 2)
	{
		file.open("Sphere128.txt");
	}
	else if (sel == 3)
	{
		cout << "\n Path : ";
		cin >> fpath;
		file.open(fpath);
	}
	else
	{
		cout << "\n Incorrect Option, Try Again : ";
		goto xyz;
	}
	if (file.fail())
		cerr << "File did not Work" << endl;
	else
	{
		int temp, temp2;
		float x, y, z;
		file >> temp;
		for (int i = 0; i < temp; i++)
		{
			file >> temp2;
			for (int j = 0; j < temp2; j++)
			{
				file >> x >> y >> z;
				ballcoord.push_back(vec3(x, y, z));
			}
		}
	}
	file.close();
}


void drawObj(GLuint buffer, int num_vertices) 
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec3) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	   (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}

void baseset() 
{
	base.size = 6;
	base.pts = new vec3[base.size];
	base.colors = new vec3[base.size];
	base.pts[0] = basevertex[0]; base.colors[0] = basecolor;
	base.pts[1] = basevertex[1]; base.colors[1] = basecolor;
	base.pts[2] = basevertex[2]; base.colors[2] = basecolor;
	base.pts[3] = basevertex[0]; base.colors[3] = basecolor;
	base.pts[4] = basevertex[2]; base.colors[4] = basecolor;
	base.pts[5] = basevertex[3]; base.colors[5] = basecolor;
}

void axisset() 
{
	axis.size = 9;
	axis.pts = new vec3[axis.size];
	axis.colors = new vec3[axis.size];
	axis.pts[0] = vec3(0, 0, 0);  axis.colors[0] = axiscolor[0];
	axis.pts[1] = vec3(10, 0, 0); axis.colors[1] = axiscolor[0];
	axis.pts[2] = vec3(20, 0, 0); axis.colors[2] = axiscolor[0];
	axis.pts[3] = vec3(0, 0, 0);  axis.colors[3] = axiscolor[1];
	axis.pts[4] = vec3(0, 10, 0); axis.colors[4] = axiscolor[1];
	axis.pts[5] = vec3(0, 20, 0); axis.colors[5] = axiscolor[1];
	axis.pts[6] = vec3(0, 0, 0);  axis.colors[6] = axiscolor[2];
	axis.pts[7] = vec3(0, 0, 10); axis.colors[7] = axiscolor[2];
	axis.pts[8] = vec3(0, 0, 20); axis.colors[8] = axiscolor[2];
}

// Set ball using coordinates in vector
void sphere() 
{
	ball.size = ballcoord.size();
	ball.pts= &ballcoord[0];
	ball.colors = new vec3[ball.size];
	for (int i = 0; i < ball.size; i++) 
	{
		ball.colors[i] = ballcolor;
	}
}

// Set Ball Path between 3 points
void ballpath() 
{
	vec3 yaxis(0, 1, 0);
	ballpathvar[0] = normalize(pathpoints[1] - pathpoints[0]);
	axisrotate[0] = cross(yaxis, ballpathvar[0]);
	ballpathvar[1] = normalize(pathpoints[2] - pathpoints[1]);
	axisrotate[1] = cross(yaxis, ballpathvar[1]);
	ballpathvar[2] = normalize(pathpoints[0] - pathpoints[2]);
	axisrotate[2] = cross(yaxis, ballpathvar[2]);
	ballcenter = pathpoints[ballax];
}

// Create vertex buffer objects and initialize the shader
void init(void) 
{
	baseset();
	glGenBuffers(1, &base.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, base.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * base.size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * base.size, base.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * base.size, sizeof(vec3) * base.size, base.colors);
	axisset();
	glGenBuffers(1, &axis.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, axis.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * axis.size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * axis.size, axis.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * axis.size, sizeof(vec3) * axis.size, axis.colors);
	sphere();
	ballpath();
	glGenBuffers(1, &ball.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ball.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * ball.size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * ball.size, ball.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * ball.size, sizeof(vec3) * ball.size, ball.colors);
	program = InitShader("vshader42.glsl", "fshader42.glsl");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.0);
}

void display(void) 
{
	GLuint model_view; // model-view matrix uniform shader variable location
	GLuint projection; // projection matrix uniform shader variable location
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.529, 0.807, 0.920, 1.000);
	glUseProgram(program); // Use the shader program
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	/*---  Set up and pass on Projection matrix to the shader ---*/
	vec4 at(-7, -3, 10, 0); // VPN
	vec4 up(0, 1, 0, 0); //VUP

	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	/*---  Set up and pass on Projection matrix to the shader ---*/
	mat4 p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	// Base
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawObj(base.buffer, base.size);

	// Axes
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(axis.buffer, axis.size);

	// Sphere
	rotationmat = Rotate(rate, axisrotate[ballax].x, axisrotate[ballax].y, axisrotate[ballax].z) * rotationmat;
	mv = mv * (Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(ball.buffer, ball.size);

	glutSwapBuffers();
}


// Distance between two points
float distance(const vec3& a, const vec3& b) 
{
	vec3 dist = a - b;
	return sqrt(dot(dist, dist));
}

// Check if index passed
int nextpt() 
{
	int x = ballax + 1;
	if (x == ballfin)
		x = 0;
	return x;
}

//To check distance between current position, next point is greater between current track point,next point
bool pointpass() 
{
	int next = nextpt();
	float nx1 = distance(ballcenter, pathpoints[ballax]);
	float nx2 = distance(pathpoints[next], pathpoints[ballax]);
	return nx1 > nx2;
}

void idle(void) 
{
	angle = angle + rate;
	if (angle >= 360.0) 
	{
		angle = angle - 360.0;
	}
	float offset = (radius * rate * M_PI) / 180;
	ballcenter.x += ballpathvar[ballax].x * offset;
	ballcenter.y += ballpathvar[ballax].y * offset;
	ballcenter.z += ballpathvar[ballax].z * offset;

	// Sphere turn-edge point check
	if (pointpass()) 
	{
		ballax = nextpt();
		ballcenter = pathpoints[ballax];
	}
	glutPostRedisplay();
}

// Specify the Viewport
void reshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	aspect = (GLfloat)width / (GLfloat)height;
	glutPostRedisplay();
}

// Specify Mouse Click functions
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && started)
	{
		rolling = !rolling;
	}
	if (rolling)
	{
		glutIdleFunc(idle);
	}
	else
	{
		glutIdleFunc(NULL);
	}
}

//Specify keyboard button actions
void keyboard(unsigned char key, int x, int y) 
{
	if (key == 'b' || key == 'B') 
	{
		started = true;
		if (rolling) 
		{
			glutIdleFunc(idle);
		}
		else 
		{
			glutIdleFunc(NULL);
		}
	}
	if (key == 'x') 
		eye[0] = eye[0] - 1.0;
	if (key == 'X') 
		eye[0] = eye[0] + 1.0;
	if (key == 'y') 
		eye[1] = eye[1] - 1.0;
	if (key == 'Y') 
		eye[1] = eye[1] + 1.0;
	if (key == 'z') 
		eye[2] = eye[2] - 1.0;
	if (key == 'Z') 
		eye[2] = eye[2] + 1.0;
	if (key == ' ')
		eye = vrp;
}

// Assign function to drop down menu
void demo_menu(int n) 
{
	switch(n) 
	{
		case 1:
			eye = vrp;
			break;

		case 2:
			exit(0);
			break;
	}
}
 
// Create and Specify menu functions
void mouseclickmenu()
{
	glutCreateMenu(demo_menu);
	glutAddMenuEntry("Default View Point (Initial Position)", 1);
	glutAddMenuEntry("Quit/Exit", 2);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}

// Main Function
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	file_in();
	glutInitWindowSize(512, 512);
	glutCreateWindow("Assignment 2");	

#ifdef __APPLE__ // on macOS
	// Core profile requires to create a Vertex Array Object (VAO).
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
	/* Call glewInit() and error checking */
	int err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
		exit(1);
	}
#endif

	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	mouseclickmenu();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(NULL);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	init();
	glutMainLoop();
	return 0;
}
