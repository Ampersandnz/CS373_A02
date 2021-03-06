// Trackball2.cpp: implementation of the CTrackball2 class.
// Copyright: (c) Burkhard Wuensche, 2002
// Originally published in SIGGRAPH (1996?)
// This file contains a C++ implementation of a modification of the original algorithm 
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "Trackball.h"
#include <gl/gl.h> 
#include <gl/glu.h> 
#include <gl/glut.h> 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrackball::CTrackball()
{
	tb_angle = 0.0;
	tb_axis[0]=0.0;tb_axis[1]=0.0;tb_axis[2]=0.0;
	tb_tracking = GL_FALSE;
}

CTrackball::~CTrackball()
{
}

void CTrackball::_tbPointToVector(int x, int y, int width, int height, float v[3])
{
	float d, a;

	// project x, y onto a hemi-sphere centered within width, height. 
	v[0] = (float) ((2.0 * x - width) / width);
	v[1] = (float) ((height - 2.0 * y) / height);
	d = (float) (sqrt(v[0] * v[0] + v[1] * v[1]));
	v[2] = (float) (cos((3.14159265 / 2.0) * ((d < 1.0) ? d : 1.0)));
	a = (float) (1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void CTrackball::_tbStartMotion(int x, int y, int button, int time)
{
	tb_tracking = GL_TRUE;
	tb_lasttime = time;
	_tbPointToVector(x, y, tb_width, tb_height, tb_lastposition);
}


void CTrackball::_tbStopMotion(int button, unsigned time)
{
	tb_tracking = GL_FALSE;
	tb_angle=0.0;
}

void CTrackball::tbInit(GLuint button)
{
	tb_button = button;
	tb_angle = 0.0;

	// put the identity in the trackball transform 
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
			tb_transform[i][j]=0.0;
		tb_transform[i][i]=1.0;
	}
}

void CTrackball::tbMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(tb_angle, tb_axis[0], tb_axis[1], tb_axis[2]);	
	glMultMatrixf((GLfloat *)tb_transform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)tb_transform);
	glPopMatrix();
	glMultMatrixf((GLfloat *)tb_transform);
}

void CTrackball::tbReshape(int width, int height)
{
	tb_width  = width;
	tb_height = height;
}

void CTrackball::tbMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == tb_button)
      _tbStartMotion(x, y, button, glutGet(GLUT_ELAPSED_TIME));
    else if (state == GLUT_UP && button == tb_button)
      _tbStopMotion(button, glutGet(GLUT_ELAPSED_TIME));
}

void CTrackball::tbKeyboard(int key)
{
	if((key !=(int) 'x') && (key !=(int) 'y') && (key !=(int) 'z')) return;
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			tb_transform[i][j]=0.0;
	tb_transform[3][3]=1.0;
	switch (key)
	{
		case (int) 'z': tb_transform[0][0]=tb_transform[1][1]=tb_transform[2][2]=1.0; break;
		case (int) 'y': tb_transform[0][1]=tb_transform[1][2]=tb_transform[2][0]=1.0; break;
		case (int) 'x': tb_transform[0][2]=tb_transform[1][0]=tb_transform[2][1]=1.0; break;
		default:;
	}
	// remember to draw new position
	glutPostRedisplay();
}

void CTrackball::tbMotion(int x, int y)
{
	GLfloat current_position[3], dx, dy, dz;
	if (tb_tracking == GL_FALSE) return;
	_tbPointToVector(x, y, tb_width, tb_height, current_position);

	// calculate the angle to rotate by (directly proportional to the
	// length of the mouse movement 
	dx = current_position[0] - tb_lastposition[0];
	dy = current_position[1] - tb_lastposition[1];
	dz = current_position[2] - tb_lastposition[2];
	tb_angle = (float) (90.0 * sqrt(dx * dx + dy * dy + dz * dz));

	// calculate the axis of rotation (cross product)
	tb_axis[0] = tb_lastposition[1] * current_position[2] - 
				 tb_lastposition[2] * current_position[1];
	tb_axis[1] = tb_lastposition[2] * current_position[0] - 
				 tb_lastposition[0] * current_position[2];
	tb_axis[2] = tb_lastposition[0] * current_position[1] - 
				 tb_lastposition[1] * current_position[0];

	// reset for next time
	tb_lasttime = glutGet(GLUT_ELAPSED_TIME);
	tb_lastposition[0] = current_position[0];
	tb_lastposition[1] = current_position[1];
	tb_lastposition[2] = current_position[2];

	// remember to draw new position
	glutPostRedisplay();
}

