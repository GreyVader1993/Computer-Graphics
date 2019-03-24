#include <fstream>
#include <string>
#include <math.h>
#include <iostream>
#include "Angel-yjc.h"
using namespace std;

//Definitions and Variables
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
#define coordsqrt(x,y,z) sqrt(x*x+y*y+z*z)
#define ImageWidth  64
#define ImageHeight 64
#define	stripeImageWidth 32
GLubyte Image[ImageHeight][ImageWidth][4];
GLubyte stripeImage[4 * stripeImageWidth];

int corsa;
int ballax = 0;
int shadow = 1;
int flagfog = 1;
int ballfin = 3;
int lighting = 1;
int spotlight = 0;
int flatshade = 0;
int smoothshade = 1;
int pointsource = 1;
int latticenable = 1;
int flaganimation = 1;
int flagbasetexture = 1;
int ballverticescount;
float radius = 1.0;
float timcnt = 0.0;
float flagshade = 1.0;
bool flageye = false;
bool started = false;
bool rolling = false;
bool flagsolid = true;
bool flagobjectsp = true;
bool flagslanted = false;
bool flagfirework = true;
bool flagvertical = true;
bool flaglatticeup = false;
bool flagspheretext = true;
bool flagshadowblend = true;
bool flagspherelinetext = true;
bool flagspherespheretext = false;
GLuint fllightbuffer;
GLuint flbuffer;
GLuint lbuffer;
GLuint ballbuffer;
GLuint ballshbuffer;
GLuint sflatbuffer;
GLuint ssmoothbuffer;
GLuint texbuffer;
GLuint fwbuffer;
GLuint  ModelView, Projection;
GLuint  ModelView_2, Projection_2;
GLint aspect; // Viewport Aspect Ratio
GLuint program, programno2; //Shader Programs
GLfloat fovy = 45.0; // Field-of-view
GLfloat zNear = 0.5;
GLfloat zFar = 15.0;
GLfloat angle = 0.0; // Rotation Angle
GLfloat turnangle = 0.0;
GLfloat turnrate = 0.5;
static GLuint texturename;
static GLuint linetexture;
point4 *ball_shade;
point4 *ball_shadowinput;
point4 *ball_input;
point4 baselightpts[6];
point4 base_points[6];
color3 *ball_color;
color3 base_colors[6];
color3 *ball_shadowcolor;
vec3  basenormal[6];
vec3 *ballflatshade;
vec3 *ballsmoothshade;
vec4 vrp(7, 3, -10, 1); //VRP Initial Viewer Position
vec4 eye = vrp; // Eye at VRP Current Viewer Position
vec3 ballcolor(1, 0.84, 0);//Yellow Ball
vec3 basecolor(0, 1, 0); //Green Base
vec3 ballshadowcolor(0.25, 0.25, 0.25);
vec3 axiscolor[3] = { vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1) }; //Red, Magenta, Blue - X,Y,Z axes respectively
point4 basevertex[4] = { point4(5, 0,  8, 1), point4(5, 0,  -4, 1), point4(-5,  0,  -4, 1), point4(-5,  0,  8, 1) }; //Base Coordinates
point3 pathpoints[] = { point3(3.0, 1.0, 5.0), point3(-2.0, 1.0, -2.5), point3(2.0, 1.0, -4.0) }; //Ball Path
point3 ballcenter = pathpoints[ballax];
point3*ballpathvar;
point3*rotaxis;
point4 axis_points[9];
color3 axis_colors[9];
point3 firework_v[300];
point3 firework_c[300];
mat4 rotationmat(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
mat4 translatemat(12, 0, 0, 0, 14, 0, 3, -1, 0, 0, 12, 0, 0, 0, 0, 12);

point4 basevertices[4] =
{
	point4(5, 0,  8, 1.0),
	point4(5, 0,  -4, 1.0),
	point4(-5,  0,  -4, 1.0),
	point4(-5,  0,  8, 1.0),
};

point4 texturevertices[6] = {
	point4(-5,  0,  -4, 1),
	point4(-5,  0,  8, 1),
	point4(5, 0,  8, 1),
	point4(5, 0,  8, 1),
	point4(5, 0,  -4, 1),
	point4(-5,  0,  -4, 1),
};

point3 texturenormals[6] = 
{
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
	point3(0,  1,  0),
};

vec2 basetextcoord[6] = 
{
	vec2(0.0, 0.0),
	vec2(0.0, 6 / 4.0),
	vec2(5 / 4.0, 6 / 4.0),
	vec2(5 / 4.0, 6 / 4.0),
	vec2(5 / 4.0, 0.0),
	vec2(0.0, 0.0),
};

void pointlightsource(mat4 mv)
{
	color4 ambientlight(0, 0, 0, 1.0);
	color4 diffusedlight(1.0, 1.0, 1.0, 1.0);
	color4 specularlight(1.0, 1.0, 1.0, 1.0);
	float constatten = 2.0;
	float linearatten = 0.01;
	float quadatten = 0.001;
	point4 lightsourcepos(-14, 12.0, -3, 1.0);
	vec4 lightposeye = mv * lightsourcepos;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),1, lightposeye);
	glUniform1f(glGetUniformLocation(program, "ConstAtt"),constatten);
	glUniform1f(glGetUniformLocation(program, "LinearAtt"),linearatten);
	glUniform1f(glGetUniformLocation(program, "QuadAtt"),quadatten);
	glUniform4fv(glGetUniformLocation(program, "input_light_ambient"), 1,ambientlight);
	glUniform4fv(glGetUniformLocation(program, "input_light_diffuse"), 1,diffusedlight);
	glUniform4fv(glGetUniformLocation(program, "input_light_specular"), 1,specularlight);
	glUniform1f(glGetUniformLocation(program, "point_flag"), pointsource * 1.0);
	glUniform1f(glGetUniformLocation(program, "spot_flag"), 0.0);
}

void spotlightsource(mat4 mv)
{
	color4 ambientlight(0, 0, 0, 1.0);
	color4 diffusedlight(1.0, 1.0, 1.0, 1.0);
	color4 specularlight(1.0, 1.0, 1.0, 1.0);
	float constatten = 2.0;
	float linearatten = 0.01;
	float quadatten = 0.001;
	point4 lightsourcepos(-14, 12.0, -3, 1.0);
	float spotexpos = 15.0;
	float spotlightangle = cos(20.0 * M_PI / 180);
	vec4 lightposeye = mv * lightsourcepos;
	point4 spotlightdirection = mv * vec4(-6.0, 0.0, -4.5, 1.0);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),1, lightposeye);
	glUniform1f(glGetUniformLocation(program, "ConstAtt"),constatten);
	glUniform1f(glGetUniformLocation(program, "LinearAtt"),linearatten);
	glUniform1f(glGetUniformLocation(program, "QuadAtt"),quadatten);
	glUniform4fv(glGetUniformLocation(program, "input_light_ambient"), 1,ambientlight);
	glUniform4fv(glGetUniformLocation(program, "input_light_diffuse"), 1,diffusedlight);
	glUniform4fv(glGetUniformLocation(program, "input_light_specular"), 1,specularlight);
	glUniform1f(glGetUniformLocation(program, "point_flag"), 0.0);
	glUniform1f(glGetUniformLocation(program, "spot_flag"), 1.0);	
	glUniform4fv(glGetUniformLocation(program, "spotlightdirection"), 1,spotlightdirection);
	glUniform1f(glGetUniformLocation(program, "spotexpos"), spotexpos);
	glUniform1f(glGetUniformLocation(program, "spotlightangle"), spotlightangle);
}

void floorset()
{
	color4 ambientsourcemat(0.2, 0.2, 0.2, 1.0);
	color4 diffusesourcemat(0, 1.0, 0, 1);
	color4 speculatmat(0, 0, 0, 1);
	float  shininess = 125.0;
	glUniform4fv(glGetUniformLocation(program, "ini_material_ambient"), 1,ambientsourcemat);
	glUniform4fv(glGetUniformLocation(program, "ini_material_diffuse"), 1,diffusesourcemat);
	glUniform4fv(glGetUniformLocation(program, "ini_material_specular"), 1,speculatmat);
	glUniform1f(glGetUniformLocation(program, "Shininess"), 1.0);
}

void ballsetmat()
{
	color4 ambientsourcemat(0.2, 0.2, 0.2, 1.0);
	color4 diffusesourcemat(1.0, 0.84, 0, 1);
	color4 speculatmat(1.0, 0.84, 0, 1);
	float  shininess = 125;
	glUniform4fv(glGetUniformLocation(program, "ini_material_ambient"), 1,ambientsourcemat);
	glUniform4fv(glGetUniformLocation(program, "ini_material_diffuse"), 1,diffusesourcemat);
	glUniform4fv(glGetUniformLocation(program, "ini_material_specular"), 1,speculatmat);
	glUniform1f(glGetUniformLocation(program, "Shininess"), shininess);
}

void fogset()
{
	glUniform1f(glGetUniformLocation(program, "fog_flag"), flagfog * 1.0);
}

void baseset()
{
	base_points[0] = basevertex[0]; base_colors[0] = basecolor;
	base_points[1] = basevertex[1]; base_colors[1] = basecolor;
	base_points[2] = basevertex[2]; base_colors[2] = basecolor;
	base_points[3] = basevertex[0]; base_colors[3] = basecolor;
	base_points[4] = basevertex[3]; base_colors[4] = basecolor;
	base_points[5] = basevertex[2]; base_colors[5] = basecolor;
}

void axisset()
{
	axis_points[0] = point4(0, 0, 0, 1);  axis_colors[0] = axiscolor[0];
	axis_points[1] = point4(10, 0, 0, 1); axis_colors[1] = axiscolor[0];
	axis_points[2] = point4(20, 0, 0, 1); axis_colors[2] = axiscolor[0];
	axis_points[3] = point4(0, 0, 0, 1);  axis_colors[3] = axiscolor[1];
	axis_points[4] = point4(0, 10, 0, 1); axis_colors[4] = axiscolor[1];
	axis_points[5] = point4(0, 20, 0, 1); axis_colors[5] = axiscolor[1];
	axis_points[6] = point4(0, 0, 0, 1);  axis_colors[6] = axiscolor[2];
	axis_points[7] = point4(0, 0, 10, 1); axis_colors[7] = axiscolor[2];
	axis_points[8] = point4(0, 0, 20, 1); axis_colors[8] = axiscolor[2];
}

void basepath()
{
	vec4 u = basevertices[0] - basevertices[1];
	vec4 v = basevertices[2] - basevertices[1];
	vec3 normal = normalize(cross(v, u));
	basenormal[0] = normal; baselightpts[0] = basevertices[1];
	basenormal[1] = normal; baselightpts[1] = basevertices[0];
	basenormal[2] = normal; baselightpts[2] = basevertices[3];
	basenormal[3] = normal; baselightpts[3] = basevertices[1];
	basenormal[4] = normal; baselightpts[4] = basevertices[3];
	basenormal[5] = normal; baselightpts[5] = basevertices[2];
}

point3 ballpathcalc(point3 from, point3 to) 
{
	point3 vertex;
	vertex.x = to.x - from.x;
	vertex.y = to.y - from.y;
	vertex.z = to.z - from.z;
	float dist = coordsqrt(vertex.x, vertex.y, vertex.z);
	vertex.x = vertex.x / dist;
	vertex.y = vertex.y / dist;
	vertex.z = vertex.z / dist;
	return vertex;
}

point3 crosspro(point3 p1, point3 p2) 
{
	point3 nx;
	nx.x = p1.y * p2.z - p1.z * p2.y;
	nx.y = p1.z * p2.x - p1.x * p2.z;
	nx.z = p1.x * p2.y - p1.y * p2.x;
	return nx;
}

void image_set_up(void)
{
	int i, j, c;
	for (i = 0; i < ImageHeight; i++)
		for (j = 0; j < ImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

			if (c == 1) /* white */
			{
				c = 255;
				Image[i][j][0] = (GLubyte)c;
				Image[i][j][1] = (GLubyte)c;
				Image[i][j][2] = (GLubyte)c;
			}
			else  /* green */
			{
				Image[i][j][0] = (GLubyte)0;
				Image[i][j][1] = (GLubyte)150;
				Image[i][j][2] = (GLubyte)0;
			}

			Image[i][j][3] = (GLubyte)255;
		}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (j = 0; j < stripeImageWidth; j++) {
		/* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
		   When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
		 */
		stripeImage[4 * j] = (GLubyte)255;
		stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
		stripeImage[4 * j + 2] = (GLubyte)0;
		stripeImage[4 * j + 3] = (GLubyte)255;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

}

void flatshadenormal()
{
	for (int i = 0; i < corsa; i++)
	{
		vec4 point1 = ball_shade[3 * i];
		vec4 point2 = ball_shade[3 * i + 1];
		vec4 point3 = ball_shade[3 * i + 2];
		vec4 p1 = point2 - point1;
		vec4 p2 = point3 - point1;
		vec3 normal = normalize(cross(p1, p2));
		ballflatshade[0] = normal;
		ballflatshade[1] = normal;
		ballflatshade[1] = normal;
	}
}

void smoothshadenormal()
{
	for (int i = 0; i < corsa * 3; i++)
	{
		vec4 point1 = ball_shade[i];
		vec3 temp1 = vec3(point1[0], point1[1], point1[2]);
		temp1 = normalize(temp1);
		ballsmoothshade[i] = temp1;
	}
}

// Reads the points from file
void file_in()
{
	int sel;
	string fpath;
	string file1;
	ifstream file;
	system("CLS");
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
		int points;
		float temp[3];
		int count = 0;
		file >> corsa;
		ball_input = new point4[corsa * 3];
		ball_shadowinput = new point4[corsa * 3];
		ball_color = new point3[corsa * 3];
		ball_shadowcolor = new point3[corsa * 3];
		ball_shade = new point4[corsa * 3];
		ballflatshade = new point3[corsa * 3];
		ballsmoothshade = new point3[corsa * 3];
		for (int i = 0; i < corsa; i++)
		{
			file >> points;
			for (int j = 0; j < points; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					file >> temp[k];
				}
				point4 tmp = vec4(temp[0], temp[1], temp[2], 1);
				point4 tmp2 = vec4(temp[0], temp[1], temp[2], 1);
				point4 tmp_shade = vec4(temp[0], temp[1], temp[2], 1);
				ball_input[count] = tmp;
				ball_shadowinput[count] = tmp2;
				ball_shade[count] = tmp_shade;
				count++;
			}
		}
		ballverticescount = corsa * 3;
	}
	file.close();
	flatshadenormal();
	smoothshadenormal();
}

void colorsphere()
{
	for (int i = 0; i < corsa * 3; i++) 
	{
		ball_color[i] = ballcolor;
		ball_shadowcolor[i] = ballshadowcolor;
	}
}

void textureinit()
{
	image_set_up();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texturename);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturename);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glGenBuffers(1, &texbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturevertices) + sizeof(texturenormals) + sizeof(basetextcoord),	NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texturevertices), texturevertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(texturevertices), sizeof(texturenormals), texturenormals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(texturevertices) + sizeof(texturenormals), sizeof(basetextcoord), basetextcoord);
}

void fireworkinit()
{
	for (int i = 0; i < 300; i++)
	{
		vec3 tmp_v = vec3(2.0 * ((rand() % 256) / 256.0 - 0.5), 2.4 * (rand() % 256) / 256.0, 2.0 * ((rand() % 256) / 256.0 - 0.5));
		firework_v[i] = tmp_v;
		vec3 tmp_c = vec3((rand() % 256) / 256.0, (rand() % 256) / 256.0, (rand() % 256) / 256.0);
		firework_c[i] = tmp_c;
	}
}

void init_line_tex()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Create and Initialize a texture object ---*/
	glGenTextures(1, &linetexture);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 0
	glBindTexture(GL_TEXTURE_1D, linetexture); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 32,
		0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
}

void init()
{
	textureinit();
	init_line_tex();
	colorsphere();
	ballfin = sizeof(pathpoints) / sizeof(point3);
	ballpathvar = new point3[ballfin];
	rotaxis = new point3[ballfin];
	for (int i = 0; i < ballfin - 1; i++) 
	{
		ballpathvar[i] = ballpathcalc(pathpoints[i], pathpoints[i + 1]);
	}
	ballpathvar[ballfin - 1] = ballpathcalc(pathpoints[ballfin - 1], pathpoints[0]);
	point3 y_axis(0, 1, 0);
	for (int i = 0; i < ballfin; i++) {
		rotaxis[i] = crosspro(y_axis, ballpathvar[i]);
	}
	baseset();
	glGenBuffers(1, &flbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, flbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(base_points) + sizeof(base_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(base_points), base_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(base_points), sizeof(base_colors),base_colors);
	basepath();
	glGenBuffers(1, &fllightbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, fllightbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(baselightpts) + sizeof(basenormal), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(baselightpts), baselightpts);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(baselightpts),sizeof(basenormal), basenormal);
	glGenBuffers(1, &sflatbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sflatbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4) + 3 * corsa * sizeof(color3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * corsa * sizeof(point4), ball_shade);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4), 3 * corsa * sizeof(color3), ballflatshade);
	glGenBuffers(1, &ssmoothbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ssmoothbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4) + 3 * corsa * sizeof(color3),	NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * corsa * sizeof(point4), ball_shade);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4),	3 * corsa * sizeof(color3), ballsmoothshade);
	axisset();
	glGenBuffers(1, &lbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_points) + sizeof(axis_colors),NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axis_points), axis_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(axis_points), sizeof(axis_colors),axis_colors);
	fireworkinit();
	glGenBuffers(1, &fwbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, fwbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(firework_v) + sizeof(firework_c),NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(firework_v), firework_v);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(firework_v), sizeof(firework_c),firework_c);
	glGenBuffers(1, &ballbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ballbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4) + 3 * corsa * sizeof(color3),NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * corsa * sizeof(point4), ball_input);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4), 3 * corsa * sizeof(color3),ball_color);
	glGenBuffers(1, &ballshbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ballshbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4) + 3 * corsa * sizeof(color3),	NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * corsa * sizeof(point4), ball_shadowinput);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * corsa * sizeof(point4), 3 * corsa * sizeof(color3),ball_shadowcolor);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.0);
	program = InitShader("vshader53.glsl", "fshader53.glsl");
	programno2 = InitShader("my_vshader.glsl", "my_fshader.glsl");
}

void drawObj(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(num_vertices * sizeof(point4)));

	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	   (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

void drawObj_2(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}

void drawObj_3(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(num_vertices * sizeof(point4)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(num_vertices * (sizeof(point4) + sizeof(point3))));

	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	   (using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	glDisableVertexAttribArray(vTexCoord);
	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

void drawObj_4(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint velocity = glGetAttribLocation(programno2, "velocity");
	glEnableVertexAttribArray(velocity);
	glVertexAttribPointer(velocity, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(programno2, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(velocity);
	glDisableVertexAttribArray(vColor);
}

void setmenuflags()
{
	flagshade = 0;
	if (flagsolid == 0 || lighting == 0)
	{
		flatshade = 0;
		smoothshade = 0;
	}
	if (lighting == 1)
	{
		flagshade = 1.0;
	}
	glUniform1f(glGetUniformLocation(program, "shade_flag"), flagshade * 1.0);
	glUniform1i(glGetUniformLocation(program, "text_ground_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "f_sphere_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "vertical_flag"), 1);
	glUniform1i(glGetUniformLocation(program, "slant_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "eye_space_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "object_space_flag"), 1);

}

void basedraw(mat4 mv, mat4 eye_frame)
{
	if (lighting == 0)
	{
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawObj_2(flbuffer, 6);  // draw the floor
	}
	else
	{
		if (pointsource)
		{
			pointlightsource(eye_frame);
		}
		else
		{
			spotlightsource(eye_frame);
		}
		floorset();
		mat4  model_view = mv;
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		mat3 normal_matrix = NormalMatrix(model_view, 1);
		glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (flagbasetexture == 1)
		{
			glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
			glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
			glUniform1i(glGetUniformLocation(program, "text_ground_flag"), 1);
			drawObj_3(texbuffer, 6);
		}
		else
		{
			drawObj(fllightbuffer, 6);
		}
		glUniform1i(glGetUniformLocation(program, "text_ground_flag"), 0);
	}
}

void shadowdraw(mat4 mv)
{
	glUniform1i(glGetUniformLocation(program, "draw_shadow_lat"), 1);
	glUniform1i(glGetUniformLocation(program, "f_draw_shadow_lat"), 1);
	glUniform1i(glGetUniformLocation(program, "upright_lat_flag"), flaglatticeup);
	if (shadow == 1)
	{
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv);
		if (flagsolid == false)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (flagshadowblend == true)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(glGetUniformLocation(program, "shade_flag"), 0);
			glUniform1f(glGetUniformLocation(program, "shadow_semi_flag"), 1.0);
			drawObj_2(ballshbuffer, ballverticescount);
			glDisable(GL_BLEND);
		}
		else
		{
			glUniform1f(glGetUniformLocation(program, "shade_flag"), 0);
			glUniform1f(glGetUniformLocation(program, "shadow_semi_flag"), 0);
			drawObj_2(ballshbuffer, ballverticescount);
		}
	}
	glUniform1i(glGetUniformLocation(program, "draw_shadow_lat"), 0);
	glUniform1i(glGetUniformLocation(program, "f_draw_shadow_lat"), 0);
}

void settextureflags()
{
	glUniform1i(glGetUniformLocation(program, "vertical_flag"), flagvertical);
	glUniform1i(glGetUniformLocation(program, "slant_flag"), flagslanted);
	glUniform1i(glGetUniformLocation(program, "eye_space_flag"), flageye);
	glUniform1i(glGetUniformLocation(program, "object_space_flag"), flagobjectsp);
	glUniform1i(glGetUniformLocation(program, "sphere_line_flag"), flagspherelinetext);
	glUniform1i(glGetUniformLocation(program, "f_sphere_flag"), flagspherelinetext);
	glUniform1i(glGetUniformLocation(program, "sphere_check_flag"), flagspherespheretext);
	glUniform1i(glGetUniformLocation(program, "f_sphere_check_flag"), flagspherespheretext);
	glUniform1i(glGetUniformLocation(program, "upright_lat_flag"), flaglatticeup);
	glUniform1i(glGetUniformLocation(program, "enable_lat"), latticenable);
};

void ballflagrem()
{
	glUniform1i(glGetUniformLocation(program, "f_sphere_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "f_sphere_check_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "eye_space_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "object_space_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "sphere_line_flag"), 0);
	glUniform1i(glGetUniformLocation(program, "sphere_check_flag"), 0);
}

void ball_create(mat4 mv, mat4 eye_frame)
{

	if (flatshade == 1)
	{
		if (pointsource)
		{
			pointlightsource(eye_frame);
		}
		else
		{
			spotlightsource(eye_frame);
		}
		ballsetmat();
		mat4  model_view = mv;
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		mat3 normal_matrix = NormalMatrix(model_view, 1);
		glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (flagspheretext == true)
		{
			settextureflags();
			glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
			glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
			drawObj(sflatbuffer, corsa * 3);
			ballflagrem();
		}
		else
		{
			drawObj(sflatbuffer, corsa * 3);
		}
	}

	else if (smoothshade == 1)
	{
		if (pointsource)
		{
			pointlightsource(eye_frame);
		}
		else
		{
			spotlightsource(eye_frame);
		}
		ballsetmat();
		mat4  model_view = mv;
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		mat3 normal_matrix = NormalMatrix(model_view, 1);
		glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (flagspheretext == true)
		{
			settextureflags();
			glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
			glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
			glUniform1i(glGetUniformLocation(program, "my_texture_2D"), 2);
			drawObj(ssmoothbuffer, corsa * 3);
			ballflagrem();
		}
		else
		{
			drawObj(ssmoothbuffer, corsa * 3);
		}
	}

	else
	{
		glUniform1f(glGetUniformLocation(program, "shade_flag"), 0);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv);
		if (flagsolid == false)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		drawObj_2(ballbuffer, ballverticescount);
	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.529, 0.807, 0.92, 0.0);
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	vec4  at(0.0, 0.0, 0.0, 1.0);
	vec4  up(0.0, 1.0, 0.0, 0.0);
	mat4  mv = LookAt(eye, at, up);
	mat4 eye_frame = LookAt(eye, at, up);
	glUseProgram(program);
	setmenuflags();
	fogset();
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	glUniformMatrix4fv(Projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
	glEnable(GL_DEPTH_TEST);
	if (flagshadowblend == true)
	{
		glDepthMask(GL_FALSE);
		basedraw(mv, eye_frame);
		mv = LookAt(eye, at, up) * translatemat * Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat;
		if (eye[1] > 0) shadowdraw(mv);

		glDepthMask(GL_TRUE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		mv = LookAt(eye, at, up);
		basedraw(mv, eye_frame);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else
	{
		glDepthMask(GL_FALSE);
		basedraw(mv, eye_frame);
		glDepthMask(GL_TRUE);
		mv = LookAt(eye, at, up) * translatemat * Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat;
		if (eye[1] > 0) shadowdraw(mv);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		mv = LookAt(eye, at, up);
		basedraw(mv, eye_frame);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	glUniform1f(glGetUniformLocation(program, "shade_flag"), 0);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, eye_frame);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj_2(lbuffer, 9);
	if (lighting == 1) glUniform1f(glGetUniformLocation(program, "shade_flag"), 1.0);
	rotationmat = Rotate(turnrate, rotaxis[ballax].x, rotaxis[ballax].y, rotaxis[ballax].z) * rotationmat;
	mv = LookAt(eye, at, up) * Translate(ballcenter.x, ballcenter.y, ballcenter.z) * rotationmat;
	ball_create(mv, eye_frame);
	glUseProgram(programno2);
	ModelView_2 = glGetUniformLocation(programno2, "model_view");
	Projection_2 = glGetUniformLocation(programno2, "projection");
	glUniformMatrix4fv(Projection_2, 1, GL_TRUE, p);
	glUniformMatrix4fv(ModelView_2, 1, GL_TRUE, eye_frame);
	glPointSize(3.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	float t = glutGet(GLUT_ELAPSED_TIME);
	int time = int((t - timcnt)) % 5000;
	if (started == true)
		glUniform1f(glGetUniformLocation(programno2, "time"), time);
	else
		glUniform1f(glGetUniformLocation(programno2, "time"), 0);
	if (flagfirework == true) drawObj_4(fwbuffer, 300);
	glutSwapBuffers();
}

// Distance between two points
float distance(point3 a, point3 b) 
{
	float dist_x = a.x - b.x;
	float dist_y = a.y - b.y;
	float dist_z = a.z - b.z;
	return coordsqrt(dist_x, dist_y, dist_z);
}

// Check if index passed
int nextpt() 
{
	int x = ballax + 1;
	if (x == ballfin)
		x = 0;
	return x;
}

//To check distance between current position, next point is greater between current pathpoints point,next point
bool pointpass() 
{
	int next = nextpt();
	float nx1 = distance(ballcenter, pathpoints[ballax]);
	float nx2 = distance(pathpoints[next], pathpoints[ballax]);
	return nx1 > nx2;
}

void idle(void)
{
	//Adjusts the rotation speed and rotates by constant speed
	turnangle = turnangle + turnrate;
	if (turnangle > 360.0)
	{
		turnangle = turnangle - 360.0;
	}
	float offset = (radius * turnrate * M_PI) / 180;
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

// Specify the Viewport Aspect Ratio
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	aspect = (GLfloat)w / (GLfloat)h;
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
		timcnt = glutGet(GLUT_ELAPSED_TIME);
		started = true;
		glutIdleFunc(idle);
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
	if (key == 'q' || key == 'Q')
	{
		exit(0);
	}
	if (key == 'v' || key == 'V')
	{
		flagvertical = true;
		flagslanted = false;
	}
	if (key == 's' || key == 'S')
	{
		flagvertical = false;
		flagslanted = true;
	}
	if (key == 'o' || key == 'O')
	{
		flagobjectsp = true;
		flageye = false;
	}
	if (key == 'e' || key == 'E')
	{
		flagobjectsp = false;
		flageye = true;
	}
	if (key == 'u' || key == 'U')
		flaglatticeup = false;
	if (key == 't' || key == 'T')
		flaglatticeup = true;
	if (key == 'l' || key == 'L')
		latticenable = 1 - latticenable;
	if (key == 'a' || key == 'A')
	{
		flaganimation = 1 - flaganimation;
		if (flaganimation == 1)
			glutIdleFunc(idle);
		else
			glutIdleFunc(NULL);
	}
	glutPostRedisplay();
}
//Specify mouse button actions
void demomenu(int index)
{
	switch (index)
	{
		case 0:	eye = vrp;
					break;
		case 1:
		{
			exit(0);
			break;
		}
		case 2:
		{
			flagsolid = !flagsolid;
			break;
		}
	}
	display();
}
void shadowtoggle(int index) 
{
	if (index == 1)
		shadow = false;
	else
		shadow = true;
	display();
}
void shadingtoggle(int index) 
{
	if (index == 1)
		flatshade = true;
	else
		flatshade = false;
	if (flatshade) 
	{
		flatshade = flatshade;
	}
	else 
	{
		smoothshade = !smoothshade;
		if (smoothshade == 1) 
			flatshade = false;
	}
	flagsolid = true;
	display();
}
void lightingtoggle(int index) 
{
	if (index == 1)
		lighting = 0;
	else 
		lighting = 1;
	display();
}
void lightsourcetoggle(int index) 
{
	if (index == 1)
		spotlight = 1;
	else 
		spotlight = 0;
	if (spotlight) 
	{
		spotlight = 1;
		pointsource = 0;
	}
	else
	{
		spotlight = 0;
		pointsource = 1;
	}
	display();
}
void fogtoggle(int index)
{
	if (index >= 0) 
		flagfog = index;
	else 
		flagfog = 1;
	display();
}
void groundtexturetoggle(int index)
{
	if (index == 2)
		flagbasetexture = 1;
	else 
		flagbasetexture = 0;
	display();
}
void spheretexturetoggle(int index)
{
	if (index == 1)
	{
		flagspheretext = false;
	}
	if (index == 2)
	{
		flagspheretext = true;
		flagspherelinetext = true
;
		flagspherespheretext = false;
	}
	if (index == 3)
	{
		flagspheretext = true;
		flagspherelinetext = false;
		flagspherespheretext = true;
	}
	display();
}
void blendshadowtoggle(int index)
{
	if (index == 2)
		flagshadowblend = true;
	else
		flagshadowblend = false;
	display();
}
void fireworktoggle(int index)
{
	if (index == 2)
		flagfirework = true;
	else 
		flagfirework = false;
	display();
}
//Specify mouseclick button actions
void mouseclickmenu() 
{
	int shadow = glutCreateMenu(shadowtoggle);
	glutAddMenuEntry("No Shadow", 1);
	glutAddMenuEntry("Display Shadow", 2);
	int shade = glutCreateMenu(shadingtoggle);
	glutAddMenuEntry("Flat Shading", 1);
	glutAddMenuEntry("Smooth Shading", 2);
	int lighting = glutCreateMenu(lightingtoggle);
	glutAddMenuEntry("Lighting Disable", 1);
	glutAddMenuEntry("Enable Lighting", 2);
	int groundtexture = glutCreateMenu(groundtexturetoggle);
	glutAddMenuEntry("No Ground Texture", 1);
	glutAddMenuEntry("Display Ground Texture", 2);
	int spheretexture = glutCreateMenu(spheretexturetoggle);
	glutAddMenuEntry("No Decals", 1);
	glutAddMenuEntry("Contour Lines", 2);
	glutAddMenuEntry("Checkerboard", 3);
	int shadowblend = glutCreateMenu(blendshadowtoggle);
	glutAddMenuEntry("No Blending Shadow", 1);
	glutAddMenuEntry("Display Blending Shadow", 2);
	int fireworkbutton = glutCreateMenu(fireworktoggle);
	glutAddMenuEntry("No Fireworks", 1);
	glutAddMenuEntry("Fireworks Display", 2);
	int lightsourcebutton = glutCreateMenu(lightsourcetoggle);
	glutAddMenuEntry("Spot light", 1);
	glutAddMenuEntry("Point light", 2);
	int fogenable = glutCreateMenu(fogtoggle);
	glutAddMenuEntry("No fog", 1);
	glutAddMenuEntry("Linear fog", 2);
	glutAddMenuEntry("Exponential fog", 3);
	glutAddMenuEntry("Exponential square fog", 4);

	glutCreateMenu(demomenu);
	glutAddMenuEntry("Default View Point", 0);
	glutAddMenuEntry("Wire-Frame Sphere", 2);
	glutAddSubMenu("Enable Lighting", lighting);
	glutAddSubMenu("Shadow", shadow);
	glutAddSubMenu("Shading", shade);
	glutAddSubMenu("Lighting", lightsourcebutton);
	glutAddSubMenu("Fog", fogenable);
	glutAddSubMenu("Texture-Mapped Ground", groundtexture);
	glutAddSubMenu("Texture-Mapped Sphere", spheretexture);
	glutAddSubMenu("Blend-Shadow", shadowblend);
	glutAddSubMenu("Fireworks", fireworkbutton);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}

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
	glutCreateWindow("Assignment 4");

#ifdef __APPLE__ // on macOS
	// Core profile requires to create a Vertex Array Object (VAO).
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
	/* Call glewInit() and error checking */
	int err = glewInit();
	if (GLEW_OK != err) {
		printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
		exit(1);
	}
#endif
	mouseclickmenu();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	init();
	glutMainLoop();
	return 0;
}
