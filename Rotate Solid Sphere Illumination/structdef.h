#ifndef INTERACTIVE_CG_STRUCTDEF_H
#define INTERACTIVE_CG_STRUCTDEF_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Angel-yjc.h"
using namespace std;

typedef struct pixelpts 
{
	vec4* pts;
	vec4* colors;
	vec4* normals;
	GLuint buffer;
	int size;
} pixelpts;

typedef struct lightsource 
{
	vec4 position;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	float shininess;
	float exponent;
	float angle;
} lightsource;

#endif
