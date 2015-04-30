// Lighting.h: interface for the CLighting and CLight class.
// Copyright: (c) 1998 Burkhard Wuensche
// This file contains subset of the original lighting class (c) 2002
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTING_H__6012A8D5_A57B_4EE6_BB19_048164137FFB__INCLUDED_)
#define AFX_LIGHTING_H__6012A8D5_A57B_4EE6_BB19_048164137FFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <gl/gl.h>


class CLight
{
public:
	friend class CLighting;
	CLight(int id);
	virtual ~CLight(){}
	void setPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	GLfloat* getPosition(){return light_position;}
	void init();
private:
	int glLightID;
	GLfloat light_position[4];
	GLfloat light_ambient[4];
	GLfloat light_diffuse[4];
	GLfloat light_specular[4];
};


class CLighting
{
public:
	CLighting();
	virtual ~CLighting(){ delete[] lights;}
	void enable() const;
	void disable() const;
	void init() const;
	CLight* getLight(int i){ return lights[i];}
private:
	int numLights;
	CLight **lights;
};


#endif // !defined(AFX_LIGHTING_H__6012A8D5_A57B_4EE6_BB19_048164137FFB__INCLUDED_)
