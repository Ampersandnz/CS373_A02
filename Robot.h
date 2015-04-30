// Robot.h: interface for the robot class.
// Copyright: (c) 2014 Burkhard Wuensche
//////////////////////////////////////////////////////////////////////

/*
Michael Lo
mlo450
5530588
*/

#if !defined(AFX_ROBOT_H__CFA6100C_622B_478B_B2F1_E5DDF89FE34D__INCLUDED_)
#define AFX_ROBOT_H__CFA6100C_622B_478B_B2F1_E5DDF89FE34D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

const int NUM_LONGITUDINAL_VERTICES=10;
const int NUM_CIRCUMFERENTIAL_VERTICES=16;

#define PI 3.14159265
#define PARAMETRIC 0
#define HERMITE 1

class CRobot  
{
public:
	CRobot();
	virtual ~CRobot(){}
	void draw();
	void setTexture(GLuint i){ texName=i; _initTexture(); }
	void incrementPhi(double time){ phi+=0.3*(1+0.02*time); if (phi>360) phi-=360;}
	void decrementPhi(double time){ phi -= 0.3*(1 + 0.02*time); if (phi<0) phi += 360; }
	void incrementTheta(double time){ theta += 0.3*(1 + 0.02*time); if (theta>thetaMax) theta = thetaMax; }
	void decrementTheta(double time){ theta -= 0.3*(1 + 0.02*time); if (theta<thetaMin) theta = thetaMin; }
	void incrementBasketShapeAngle(){ basketShapeAngle++; }
	// geometry
	static const double rimRadius;
	static const double basketRadius;
	static const double basketHeight;
	static const double armLength;
	static const double bottomArmWidth;
	static const double topArmWidth;
	double basketShapeAngle;
	void _initBasketSurface();
	void updateBasketPosition();
	float basketCentre[3];
	int curveMode;
private:
	// Define Hermite basis functions
	double HermiteBasis1(double t){ return t * t * (2 * t - 3) + 1; }	// B1(t)
	double HermiteBasis2(double t){ return (-2 * t + 3) * t * t; }		// B2(t)
	double HermiteBasis3(double t){ return t * (t * (t - 2) + 1); }		// B3(t)
	double HermiteBasis4(double t){ return t * t * (t - 1); }			// B4(t)
	// material properties
	GLfloat no_shininess[1];
	GLfloat mat_specular[4];
	GLfloat mat_ambient_and_diffuse[4];
	// animation parameters
	double thetaMin, thetaMax;
	double phi, theta;	// angles for rotating robot arms
	double timeLastStep;
	// attributes and methods for the basket at the tip of the robot arm
	void _calcSegmentPoint(int segment, int vertex);
	void _calcHermitePoints();
	void _drawBasket();
	float vertices[NUM_LONGITUDINAL_VERTICES][NUM_CIRCUMFERENTIAL_VERTICES][3];
	// texture parameters
	void _initTexture();
	float texture_coordinates[NUM_LONGITUDINAL_VERTICES][NUM_CIRCUMFERENTIAL_VERTICES][2];
	GLuint texName;
	GLubyte *texture;
	int textureWidth, textureHeight;
};

#endif // !defined(AFX_ROBOT_H__CFA6100C_622B_478B_B2F1_E5DDF89FE34D__INCLUDED_)
