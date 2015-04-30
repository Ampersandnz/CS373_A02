// Robot.cpp: implementation of the CRobot class.
// Copyright: (c) 2014 Burkhard Wuensche
//////////////////////////////////////////////////////////////////////

/*
Michael Lo
mlo450
5530588
*/

#include <time.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Robot.h"
#include "Geometry.h"
#include "Utilities.h"

#include <iostream>
#include <fstream>
using namespace std;

const double CRobot::rimRadius=0.02;
const double CRobot::basketRadius=0.2;
const double CRobot::basketHeight = 0.4;
const double CRobot::armLength = 0.6;
const double CRobot::bottomArmWidth = 0.2;
const double CRobot::topArmWidth = 0.1;

void CRobot::_calcHermitePoints() {
	CVec2df p1(basketHeight, 0); // start point 
	CVec2df p4(0, basketRadius); // end point 
	CVec2df r1(0, 2 * basketRadius); // tangent at start point 
	CVec2df r4(-2 * basketHeight*cos(2 * Pi*basketShapeAngle / 360.0), -2 * basketHeight*sin(2 * Pi*basketShapeAngle / 360.0)); // tangent at end point 
	CVec2df p;

	// For each of the NUM_CIRCUMFERENTIAL_VERTICES segments of the surface.
	const int numPoints = NUM_LONGITUDINAL_VERTICES;
	const int numSegments = NUM_CIRCUMFERENTIAL_VERTICES;
	float t;
	float s;

	for (int i = 0; i < numSegments; i++) {
		s = (float)i / (float)(numSegments - 1);

		// Curve in 2D:
		for (int j = 0; j < numPoints; j++) {
			t = (float) j / (float)(numPoints - 1);
			p = HermiteBasis1(t) * p1 + HermiteBasis2(t) * p4 + HermiteBasis3(t) * r1 + HermiteBasis4(t) * r4;
			
			float * coords = p.getArray();
			
			// Surface in 3D:
			float x = coords[0];
			float y = coords[1] * (sinf(2 * PI * s));
			float z = coords[1] * (cosf(2 * PI * s));
			
			// Store values in the 3d array vertices. 
			vertices[j][i][0] = x;
			vertices[j][i][1] = y;
			vertices[j][i][2] = z;
		}
	}
}

void CRobot::_initBasketSurface(){

	if (curveMode == HERMITE) {
		// Calculate the points on the curve for each of NUM_CIRCUMFERENTIAL_VERTICES segments around the y axis.
		_calcHermitePoints();
	}
	else {
		// Calculate the points on the curve for each of NUM_CIRCUMFERENTIAL_VERTICES segments around the y axis.
		for (int i = 0; i < NUM_CIRCUMFERENTIAL_VERTICES; i++) {
			//Calculate NUM_LONGITUDINAL_VERTICES + 1 points on the curve for this segment. Add 1 to make s span from 0 to 1.
			for (int j = 0; j < NUM_LONGITUDINAL_VERTICES; j++) {
				_calcSegmentPoint(i, j);
			}
		}
	}
}

void CRobot::_calcSegmentPoint(int segment, int vertex) {
	// Curve is in the xz plane, because it will be rotated into the xy plane when the torus is created and rotated.
	float x, y, z, a = basketRadius, b = basketHeight, xT, zT;

	float s = (float)segment / (NUM_CIRCUMFERENTIAL_VERTICES - 1);
	float t = (float)vertex / (NUM_LONGITUDINAL_VERTICES - 1);
	
	/* Curve in 2D:
	c(t) = (a(cos(pi(t) / 2)), 0, b(sin(pi(t))))
		 = (x(t)			 , 0, z(t))
	for 0 < t < 1 */
	xT = a * (cosf((PI / 2) * t));
	zT = b * (sinf((PI / 2) * t));

	/* Surface in 3D:
	p(s, t) = (x(t)cos(s)		  , x(t)sin(s)	 , z(t))
		    = (a(cos(pi(t)))cos(s), b(sin(pi(t))), a(cos(pi(t)))sin(s))
	for 0 < s < 1, 0 < t < 1 */
	x = xT * (sinf(2 * PI * s));
	y = xT * (cosf(2 * PI * s));
	z = zT;

	// Store values in the 3d array vertices.
	vertices[vertex][segment][0] = x;
	vertices[vertex][segment][1] = y;
	vertices[vertex][segment][2] = z;

}

CRobot::CRobot()
{
	// initialise material properties
	no_shininess[0]=5;
	mat_specular[0]=0.1;
	mat_specular[1]=0.1;
	mat_specular[2]=0.1;
	mat_specular[3]=1;
	mat_ambient_and_diffuse[0]=0.15f;
	mat_ambient_and_diffuse[1]=0.25f;
	mat_ambient_and_diffuse[2]=0.8f;
	mat_ambient_and_diffuse[3]=1;

	// initialise geometry
	thetaMin=10.0;
	thetaMax=90.0;
	basketShapeAngle=0.0;

	// initialise animation
	phi=0.0;
	theta=thetaMin;

	// initialise basket geometry
	_initBasketSurface();
}


void CRobot::_initTexture()
{	// load net texture
	ifstream textureFile;
	char* fileName="Net.pgm";
	
	textureFile.open(fileName, ios::in);
	if (textureFile.fail())
		displayMessage(ERROR_MESSAGE, "initGround(): could not open file %s", fileName);

	skipLine(textureFile);
	textureFile >> textureWidth;
	textureFile >> textureHeight;
	int numGreyscaleValues;
	textureFile >> numGreyscaleValues;
	texture = new GLubyte[textureWidth * textureHeight * 4];

	int i, j, v;
	for (i = 0; i < textureHeight; i++)
		for (j = 0; j < textureWidth; j++)
		{
			textureFile >> v;
			texture[(i * textureWidth + j) * 4] = (GLubyte)v;
			texture[(i * textureWidth + j) * 4 + 1] = (GLubyte)v;
			texture[(i * textureWidth + j) * 4 + 2] = (GLubyte)v;
			if (v == 255) {
				texture[(i * textureWidth + j) * 4 + 3] = (GLubyte)0;
			}
			else {
				texture[(i * textureWidth + j) * 4 + 3] = (GLubyte)255;
			}
		}
	textureFile.close();

	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
}

void CRobot::updateBasketPosition()
{
	float radius=2*armLength*sin(theta*Pi/180.0f)+basketRadius;
	basketCentre[0]=radius*sin((phi-90)*Pi/180.0f);
	basketCentre[1]=armLength;
	basketCentre[2]=radius*cos((phi-90)*Pi/180.0f);
}

void CRobot::_drawBasket()
{
	glShadeModel(GL_FLAT); 
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);

	float texX, texY;
	int moddedIPlusOne;

	for (int i = 0; i < NUM_CIRCUMFERENTIAL_VERTICES; i++) {
		glBegin(GL_QUAD_STRIP);
		//Mod (i + 1) because the final value is NUM_CIRCUMFERENTIAL_VERTICES + 1 and we need that to wrap around and join with i = 0
		moddedIPlusOne = (i + 1) % NUM_CIRCUMFERENTIAL_VERTICES;

		for (int j = 0; j < NUM_LONGITUDINAL_VERTICES; j++) {
			// Whole texture spans one circumference of the basket, so no tiling needed. 0 < texX, texY < 1 for all values used.
			texX = ((float)i / (NUM_CIRCUMFERENTIAL_VERTICES - 1));
			texY = ((float)j / (NUM_LONGITUDINAL_VERTICES - 1));

			glTexCoord2f(texX, texY);
			
			glVertex3f(vertices[j][i][0], vertices[j][i][1], vertices[j][i][2]);

			texX = ((float)moddedIPlusOne / (NUM_CIRCUMFERENTIAL_VERTICES - 1));
			texY = ((float)j / (NUM_LONGITUDINAL_VERTICES - 1));

			glTexCoord2f(texX, texY);

			glVertex3f(vertices[j][moddedIPlusOne][0], vertices[j][moddedIPlusOne][1], vertices[j][moddedIPlusOne][2]);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glShadeModel(GL_SMOOTH);
}

void CRobot::draw()
{
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);

	glPushMatrix();
	glRotatef(phi - 90, 0, 1, 0);

		// Draw the base
		glPushMatrix();
			glTranslatef(0, armLength / 2, 0);
			glScalef(bottomArmWidth, armLength, bottomArmWidth);
			glutSolidCube(1.0);
		glPopMatrix();

		// Draw the centre arm
		glPushMatrix();
			glTranslatef(0, armLength, 0);
			glRotatef(theta, 1, 0, 0);

			// Don't apply these operations to the basket arm.
			glPushMatrix();
				glTranslatef(0, armLength / 2, 0);
				glScalef(topArmWidth, armLength, topArmWidth);
				glutSolidCube(1.0);
			glPopMatrix();

			// Draw the basket arm
			glPushMatrix();
				glTranslatef(0, armLength, 0);

				glRotatef(180 - 2 * theta, 1, 0, 0);

				glTranslatef(0, armLength / 2, 0);

				// Don't apply these operations to the basket rim.
				glPushMatrix();
					glScalef(topArmWidth, armLength, topArmWidth);
					glutSolidCube(1.0);
				glPopMatrix();

				// Draw the torus
				glTranslatef(0, (armLength / 2) + (sin(theta*PI / 180) * basketRadius), -cos(theta*PI / 180) * basketRadius);
				glRotatef(theta - 90, 1, 0, 0);
				glutSolidTorus(rimRadius, basketRadius, 15, 15);

				if (curveMode == HERMITE) {
					glRotatef(-90, 0, 1, 0);
				}
				_drawBasket();

			glPopMatrix();

		glPopMatrix();

	glPopMatrix();

	glDisable(GL_NORMALIZE);
	glFlush();
}
