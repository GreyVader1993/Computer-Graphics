#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Angel-yjc.h"
#include "structdef.h"
using namespace std;

//Variables and Objects Used
GLfloat fovy = 45;  // Field-of-View (in degrees)
GLfloat zNear = 0.5; // Near Plane
GLfloat zFar = 15.0; // Far Plane
GLuint program;
GLuint aspect;
bool rolling = false;
bool started = false;
bool shadowdisplay = true;
bool shadingdisplay = false;
bool wireframe = false;
bool flagLight = true;
bool flagLightType = false;
vec4 vrp(7, 3, -10, 1); //VRP
vec4 eye = vrp; //Eye at VRP
vec4 ballcolor(1, 0.84, 0, 1);//Yellow Ball
vec4 basecolor(0, 1, 0, 1); //Green Base
vec4 ballshadowcolor(0.25, 0.25, 0.25, 0.65); //Shadow Color
vec4 basevertex[4] = { vec4(5, 0, 8, 1), vec4(5, 0, -4, 1),	vec4(-5, 0, -4, 1),	vec4(-5, 0, 8, 1) }; //Base Coordinates
vec4 axiscolor[3] = { vec4(1, 0, 0, 1), vec4(1, 0, 1, 1), vec4(0, 0, 1, 1) }; //Red, Magenta, Blue - X,Y,Z axes respectively
vec4 pathpoints[3] = { vec4(3, 1, 5, 1), vec4(-2, 1, -2.5, 1), vec4(2, 1, -4, 1) }; //Ball Path
vec4 ballpathvar[3]; //Variable to allot path in function
vec4 axisrotate[3];
vec4 ballcenter;
pixelpts base;
pixelpts axis;
pixelpts ball;
pixelpts ballshadow;
vector<vec4> ballcoord;
vector<vec4> shadowvertex;
vec4* ballflatshading;
vec4* ballsmoothshading;
int ballax = 0;
int ballfin = 3;
float radius = 1;
float rate = 0.5;
float angle = 0.0;
mat4 rotationmat(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));

lightsource ambientsource =
{
	ambientsource.position  = vec4(0),
	ambientsource.direction = vec4(0),
	ambientsource.ambient   = vec4(1.0, 1.0, 1.0, 1.0),
	ambientsource.diffuse   = vec4(0),
	ambientsource.specular  = vec4(0)
};

lightsource directionalsource =
{
	directionalsource.position  = vec4(0),
	directionalsource.direction = vec4(0.1, 0.0, -1.0, 0.0),
	directionalsource.ambient   = vec4(0.0, 0.0, 0.0, 1.0),
	directionalsource.diffuse   = vec4(0.8, 0.8, 0.8, 1.0),
	directionalsource.specular  = vec4(0.2, 0.2, 0.2, 1.0),
};

lightsource pointsource =
{
	pointsource.position			 = vec4(-14.0, 12.0, -3.0, 1.0),
	pointsource.direction			 = vec4(0),
	pointsource.ambient				 = vec4(0.0, 0.0, 0.0, 1.0),
	pointsource.diffuse				 = vec4(1.0, 1.0, 1.0, 1.0),
	pointsource.specular             = vec4(1.0, 1.0, 1.0, 1.0),
	pointsource.attenuationConstant  = 2.0,
	pointsource.attenuationLinear    = 0.01,
	pointsource.attenuationQuadratic = 0.01
};

lightsource spotlight =
{
	spotlight.position			   = vec4(-14.0, 12.0, -3.0, 1.0),
	spotlight.direction			   = vec4(-6.0, 0.0, -4.5, 1.0),
	spotlight.ambient			   = vec4(0.0, 0.0, 0.0, 1.0),
	spotlight.diffuse			   = vec4(1.0, 1.0, 1.0, 1.0),
	spotlight.specular			   = vec4(1.0, 1.0, 1.0, 1.0),
	spotlight.attenuationConstant  = 2.0,
	spotlight.attenuationLinear    = 0.01,
	spotlight.attenuationQuadratic = 0.01,
	spotlight.shininess			   = 0.0,
	spotlight.exponent			   = 15.0,
	spotlight.angle				   = 20.0
};

lightsource baselight =
{
	baselight.position  = vec4(0),
	baselight.direction = vec4(0),
	baselight.ambient   = vec4(0.2, 0.2, 0.2, 1),
	baselight.diffuse   = vec4(0.0, 1.0, 0.0, 1.0),
	baselight.specular  = vec4(0.0, 0.0, 0.0, 1.0)
};

lightsource balllight =
{
	balllight.position			   = vec4(0),
	balllight.direction			   = vec4(0),
	balllight.ambient			   = vec4(0.2, 0.2, 0.2, 1),
	balllight.diffuse			   = vec4(1, 0.84, 0, 1),
	balllight.specular			   = vec4(1, 0.84, 0, 1),
	balllight.attenuationConstant  = 0,
	balllight.attenuationLinear    = 0,
	balllight.attenuationQuadratic = 0,
	balllight.shininess			   = 125.0
};

//Functions Used
void file_in(vector<vec4>&, vector<vec4>&);
void drawObj(GLuint, int, GLuint);
void baseset();
void axisset();
void sphere();
void ballpath();
void shadow();
void init(void);
void light(mat4, const lightsource& , const lightsource& , const lightsource& , const lightsource&);
void display(void);
float distance(const vec4&, const vec4&);
int nextpt();
bool pointpass();
void idle(void);
void reshape(int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);
void shademenu(int);
void demo_menu(int);
void mouseclickmenu();

// Reads the points from file
void file_in(vector<vec4>& spherePoints, vector<vec4>& shadowPoints)
{
	int sel;
	string fpath;
	string file1;
	ifstream file;
	cout << "Enter the no of triangles in Wireframe : \n";
	cout << "1. 256 Triangles : \n";
	cout << "2. 1024 Triangles : \n";
	cout << "3. Enter Path : \n";
xyz:
	cout << "Enter your choice : ";
	cin >> sel;
	if (sel == 1)
	{
		file.open("sphere.256");
	}
	else if (sel == 2)
	{
		file.open("sphere.1024");
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
				spherePoints.push_back(vec4(x, y, z, 1));
				shadowPoints.push_back(vec4(x, y, z, 1));
			}
		}
	}
	file.close();
}

void drawObj(GLuint buffer, int num_vertices, GLuint program) {
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4) * num_vertices));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4) * num_vertices * 2));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	   (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(vNormal);
}

// Base Set
void baseset()
{
	base.size = 6;
	base.pts = new vec4[base.size];
	base.colors = new vec4[base.size];
	base.normals = new vec4[base.size];

	base.pts[0] = basevertex[0]; base.colors[0] = basecolor; 
	base.pts[2] = basevertex[2]; base.colors[1] = basecolor; 
	base.pts[3] = basevertex[0]; base.colors[2] = basecolor; 
	base.pts[1] = basevertex[1]; base.colors[3] = basecolor; 
	base.pts[4] = basevertex[2]; base.colors[4] = basecolor; 
	base.pts[5] = basevertex[3]; base.colors[5] = basecolor;

	vec4 normal = normalize(cross(base.pts[2] - base.pts[3],base.pts[0] - base.pts[1]));
	base.normals[0] = normal;
	base.normals[1] = normal;
	base.normals[2] = normal;
	base.normals[3] = normal;
	base.normals[4] = normal;
	base.normals[5] = normal;
}

// Draw Axis
void axisset() 
{
	axis.size = 9;
	axis.pts = new vec4[axis.size];
	axis.colors = new vec4[axis.size];

	axis.pts[0] = vec4(0, 0, 0, 1);  axis.colors[0] = axiscolor[0];
	axis.pts[1] = vec4(10, 0, 0, 1); axis.colors[1] = axiscolor[0];
	axis.pts[2] = vec4(20, 0, 0, 1); axis.colors[2] = axiscolor[0];
	axis.pts[3] = vec4(0, 0, 0, 1);  axis.colors[3] = axiscolor[1];
	axis.pts[4] = vec4(0, 10, 0, 1); axis.colors[4] = axiscolor[1];
	axis.pts[5] = vec4(0, 20, 0, 1); axis.colors[5] = axiscolor[1];
	axis.pts[6] = vec4(0, 0, 0, 1);  axis.colors[6] = axiscolor[2];
	axis.pts[7] = vec4(0, 0, 10, 1); axis.colors[7] = axiscolor[2];
	axis.pts[8] = vec4(0, 0, 20, 1); axis.colors[8] = axiscolor[2];
}

// Set ball using coordinates in vector
void sphere()
{
	ball.size = ballcoord.size();
	ball.pts = &ballcoord[0];
	ball.colors = new vec4[ball.size];
	ballflatshading = new vec4[ball.size];
	ballsmoothshading = new vec4[ball.size];
	ball.normals = ballflatshading;

	for (int i = 0; i < ball.size; i++)
	{
		ball.colors[i] = ballcolor;
	}

	for (int i = 0; i < ball.size; i = i + 3)
	{
		vec4 flatNormal = normalize(cross(ball.pts[i + 1] - ball.pts[i], ball.pts[i + 2] - ball.pts[i]));
		ballflatshading[i] = flatNormal;
		ballflatshading[i + 1] = flatNormal;
		ballflatshading[i + 2] = flatNormal;
		ballsmoothshading[i] = normalize(ball.pts[i]);;
		ballsmoothshading[i + 1] = normalize(ball.pts[i + 1]);;
		ballsmoothshading[i + 2] = normalize(ball.pts[i + 2]);;
	}
}

// Set Ball Path between 3 points
void ballpath()
{
	vec4 yaxis(0, 1, 0, 0);
	ballpathvar[0] = normalize(pathpoints[1] - pathpoints[0]);
	axisrotate[0] = cross(yaxis, ballpathvar[0]);
	ballpathvar[1] = normalize(pathpoints[2] - pathpoints[1]);
	axisrotate[1] = cross(yaxis, ballpathvar[1]);
	ballpathvar[2] = normalize(pathpoints[0] - pathpoints[2]);
	axisrotate[2] = cross(yaxis, ballpathvar[2]);
	ballcenter = pathpoints[ballax];
}

//Set up Ball Shadow
void shadow() 
{
	ballshadow.size = ballcoord.size();
	ballshadow.pts = &ballcoord[0];
	ballshadow.colors = new vec4[ballshadow.size];
	for (int i = 0; i < ballshadow.size; i++) 
	{
		ballshadow.colors[i] = ballshadowcolor;
	}
}

// Create vertex buffer objects and initialize the shader
void init(void) 
{
	baseset();
	glGenBuffers(1, &base.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, base.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * base.size * 3, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * base.size, base.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * base.size, sizeof(vec4) * base.size, base.colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * base.size * 2, sizeof(vec4) * base.size, base.normals);
	axisset();
	glGenBuffers(1, &axis.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, axis.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * axis.size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * axis.size, axis.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * axis.size, sizeof(vec4) * axis.size, axis.colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * axis.size * 2, sizeof(vec4) * axis.size, axis.normals);
	sphere();
	ballpath();
	glGenBuffers(1, &ball.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ball.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * ball.size * 3, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * ball.size, ball.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ball.size, sizeof(vec4) * ball.size, ball.colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ball.size * 2, sizeof(vec4) * ball.size, ball.normals);
	shadow();
	glGenBuffers(1, &ballshadow.buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ballshadow.buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * ballshadow.size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * ballshadow.size, ballshadow.pts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ballshadow.size, sizeof(vec4) * ballshadow.size, ballshadow.colors);
	program = InitShader("vshader53.glsl", "fshader53.glsl");
	glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);
	glUniform1f(glGetUniformLocation(program, "shadingdisplay"), shadingdisplay);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.0);
}

void light(mat4 mv, const lightsource& global, const lightsource& dLight, const lightsource& pLight, const lightsource& material) 
{
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	// global
	glUniformMatrix4fv(glGetUniformLocation(program, "globalLight"), 1, GL_TRUE, global.ambient * material.ambient);

	// directional
	glUniform4fv(glGetUniformLocation(program, "dPosition"), 1, dLight.position);
	glUniform4fv(glGetUniformLocation(program, "dDirection"), 1, dLight.direction);

	glUniform4fv(glGetUniformLocation(program, "dAmbientProduct"), 1, dLight.ambient * material.ambient);
	glUniform4fv(glGetUniformLocation(program, "dDiffuseProduct"), 1, dLight.diffuse * material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "dSpecularProduct"), 1, dLight.specular * material.specular);

	glUniform1f(glGetUniformLocation(program, "dConstAtt"), dLight.attenuationConstant);
	glUniform1f(glGetUniformLocation(program, "dLinearAtt"), dLight.attenuationLinear);
	glUniform1f(glGetUniformLocation(program, "dQuadAtt"), dLight.attenuationQuadratic);

	// positional
	glUniform4fv(glGetUniformLocation(program, "pPosition"), 1, mv * pLight.position);
	glUniform4fv(glGetUniformLocation(program, "pDirection"), 1, mv * pLight.direction);

	glUniform4fv(glGetUniformLocation(program, "pAmbientProduct"), 1, pLight.ambient * material.ambient);
	glUniform4fv(glGetUniformLocation(program, "pDiffuseProduct"), 1, pLight.diffuse * material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "pSpecularProduct"), 1, pLight.specular * material.specular);

	glUniform1f(glGetUniformLocation(program, "pConstAtt"), pLight.attenuationConstant);
	glUniform1f(glGetUniformLocation(program, "pLinearAtt"), pLight.attenuationLinear);
	glUniform1f(glGetUniformLocation(program, "pQuadAtt"), pLight.attenuationQuadratic);

	glUniform1f(glGetUniformLocation(program, "pExponent"), pLight.exponent);
	glUniform1f(glGetUniformLocation(program, "pAngle"), cos(pLight.angle * (M_PI / 180)));
}

void display(void) 
{
	GLuint modelView; // Modelview matrix uniform shader variable location
	GLuint projection; // projection matrix uniform shader variable location
	GLuint normalMatrix; // Normal matrix uniform shader variable location
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//** Important: glUseProgram() must be called *before* any shader variable
//              locations can be retrieved. This is needed to pass on values to
//              uniform/attribute variables in shader ("variable binding" in 
//              shader).
	glClearColor(0.529, 0.807, 0.92, 1); // Background Color
	glUseProgram(program);
	// Retrieve transformation uniform variable locations
	// ** Must be called *after* glUseProgram().
	modelView = glGetUniformLocation(program, "modelView");
	projection = glGetUniformLocation(program, "projection");
	normalMatrix = glGetUniformLocation(program, "normalMatrix");

	// Set up and pass Projection matrix to the shader
	vec4 at(0, 0, 0, 1); // VPN 
	vec4 up(0, 1, 0, 0); // VUP 

	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(modelView, 1, GL_TRUE, mv);
	mat3 nm = NormalMatrix(mv, 1);
	glUniformMatrix3fv(normalMatrix, 1, GL_TRUE, nm);
	mat4 p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	// Draw Base in Frame Buffer with Lighting
	glDepthMask(GL_FALSE);
	if (flagLightType) 
	{
		light(mv, ambientsource, directionalsource, pointsource, baselight);
	} 
	else 
	{
		light(mv, ambientsource, directionalsource, spotlight, baselight);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawObj(base.buffer, base.size, program);
	glDepthMask(GL_TRUE);

	// Axis
	glUniform1i(glGetUniformLocation(program, "flagLight"), false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(axis.buffer, axis.size, program);
	glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);

	// Sphere
	rotationmat = Rotate(rate, axisrotate[ballax].x, axisrotate[ballax].y, axisrotate[ballax].z) * rotationmat;
	mv *= Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat;
	
	glUniformMatrix4fv(modelView, 1, GL_TRUE, mv);
	glUniformMatrix3fv(normalMatrix, 1, GL_TRUE, NormalMatrix(mv, 1));
	if (flagLightType) 
	{
		light(mv, ambientsource, directionalsource, pointsource, balllight);
	} 
	else 
	{
		light(mv, ambientsource, directionalsource, spotlight, balllight);
	}
	if (wireframe) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(glGetUniformLocation(program, "flagLight"), false);
		drawObj(ball.buffer, ball.size, program);
		glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);
	} 
	else 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawObj(ball.buffer, ball.size, program);
	}

	// Shadow
	if (shadowdisplay and eye.y>0)
	{
		glUniform1i(glGetUniformLocation(program, "flagLight"), false);
		mat4 shadowMatrix(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, -1 / spotlight.position.y, 0, 0));
		mv = LookAt(eye, at, up) * Translate(spotlight.position) * shadowMatrix * Translate(-spotlight.position) * Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat;
		glUniformMatrix4fv(modelView, 1, GL_TRUE, mv);
		if (wireframe) 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} 
		else 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		drawObj(ballshadow.buffer, ballshadow.size, program);
		glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);
	}

	// Draw Base to z buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	if (flagLightType) 
	{
		light(mv, ambientsource, directionalsource, pointsource, baselight);
	} 
	else 
	{
		light(mv, ambientsource, directionalsource, spotlight, baselight);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawObj(base.buffer, base.size, program);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glutSwapBuffers();
}

// Distance between two points
float distance(const vec4& a, const vec4& b)
{
	vec4 dist = a - b;
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
	float offset = radius * rate * (M_PI / 180);
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
void reshape(int w, int h) 
{
	glViewport(0, 0, w, h);
	aspect = (GLfloat) w / (GLfloat) h;
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
		rolling = !rolling;
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
		glutPostRedisplay();
}

// Assign function to drop down menu
void demo_menu(int n) 
{
	switch (n) 
	{
		case 0:	eye = vrp;
				break;
		case 1:	wireframe = !wireframe;
				break;
		case 2:	shadowdisplay = true;
				break;
		case 3: shadowdisplay = false;
				break;
		case 4: flagLight = true;
				glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);
				break;
		case 5: flagLight = false;
				glUniform1i(glGetUniformLocation(program, "flagLight"), flagLight);
				break;
		case 6:	flagLightType = false;
				glUniform1f(glGetUniformLocation(program, "flagLightType"), flagLightType);
				break;
		case 7:	flagLightType = true;
				glUniform1f(glGetUniformLocation(program, "flagLightType"), flagLightType);
				break;
		case 8: shadingdisplay = false;
				ball.normals = ballflatshading;
				glUniform1f(glGetUniformLocation(program, "shadingdisplay"), shadingdisplay);
				glBindBuffer(GL_ARRAY_BUFFER, ball.buffer);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ball.size * 2, sizeof(vec4) * ball.size, ball.normals);
				break;
		case 9:	shadingdisplay = true;
				ball.normals = ballsmoothshading;
				glUniform1f(glGetUniformLocation(program, "shadingdisplay"), shadingdisplay);
				glBindBuffer(GL_ARRAY_BUFFER, ball.buffer);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * ball.size * 2, sizeof(vec4) * ball.size, ball.normals);
				break;
		case 10:exit(0);
				break;
	}
	display();
}

void mouseclickmenu()
{
	GLuint shadowmenu = glutCreateMenu(demo_menu);
	glutAddMenuEntry("Yes, Display Shadow", 2);
	glutAddMenuEntry("No Shadow", 3);
	GLuint lightmenu = glutCreateMenu(demo_menu);
	glutAddMenuEntry("Yes, Lighting On", 4);
	glutAddMenuEntry("No Lighting", 5);
	GLuint lighttype = glutCreateMenu(demo_menu);
	glutAddMenuEntry("Spot-Light", 6);
	glutAddMenuEntry("Point-Sourced Light", 7);
	GLuint shademenu = glutCreateMenu(demo_menu);
	glutAddMenuEntry("Flat Shading", 8);
	glutAddMenuEntry("Smooth Shading", 9);	
	glutCreateMenu(demo_menu);
	glutAddMenuEntry("Return to Default View Point", 0);
	glutAddMenuEntry("Wire Frame Sphere Toggle", 1);
	glutAddSubMenu("Display Shadows for Sphere", shadowmenu);
	glutAddSubMenu("Enable Lighting", lightmenu);
	glutAddSubMenu("Light Source", lighttype);
	glutAddSubMenu("Type of Shading", shademenu);
	glutAddMenuEntry("Exit", 10);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}

int main(int argc, char** argv) 
{
	glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	file_in(ballcoord, shadowvertex);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Assignment 3");

#ifdef __APPLE__ // on macOS
	// Core profile requires to create a Vertex Array Object (VAO).
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
	/* Call glewInit() and error checking */
	int err = glewInit();
	if (GLEW_OK != err) {
		printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err));
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
