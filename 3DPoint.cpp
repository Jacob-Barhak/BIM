#include "stdafx.h"
#include "3DPoint.h"

// default ctor for CrossHatchpoint
CrossHatchPoint::CrossHatchPoint()
{
	points[0][0] = 0.25;
	points[0][1] = 0.0;
	points[0][2] = 0.0;

	points[1][0] = -0.25;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = 0.25;
	points[2][2] = 0.0;

	points[3][0] = 0.0;
	points[3][1] = -0.25;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = 0.0;
	points[4][2] = 0.25;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -0.25;
}

// ctor that sets the diameter of the point
CrossHatchPoint::CrossHatchPoint(double size)
{
	points[0][0] = size/2.0;
	points[0][1] = 0.0;
	points[0][2] = 0.0;

	points[1][0] = -size/2.0;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = size/2.0;
	points[2][2] = 0.0;

	points[3][0] = 0.0;
	points[3][1] = -size/2.0;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = 0.0;
	points[4][2] = size/2.0;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -size/2.0;
}

// draws the point translated by x,y,z
void CrossHatchPoint::Draw(double x, double y, double z)
{
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	glBegin(GL_LINES);
		glVertex3d(
			points[0][0],
			points[0][1],
			points[0][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);

		glVertex3d(
			points[2][0],
			points[2][1],
			points[2][2]);
		glVertex3d(
			points[3][0],
			points[3][1],
			points[3][2]);

		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
		glVertex3d(
			points[5][0],
			points[5][1],
			points[5][2]);
	glEnd();
	glPopMatrix();
}

// default ctor for TetrahedronPoint
TetraHedronPoint::TetraHedronPoint()
{
	points[0][0] = 0.0;
	points[0][1] = 0.0;
	points[0][2] = 0.25;

	points[1][0] = 0.25;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = 0.25;
	points[2][2] = 0.0;

	points[3][0] = -0.25;
	points[3][1] = 0.0;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = -0.25;
	points[4][2] = 0.0;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -0.25;
}

// ctor that sets the diameter of the point
TetraHedronPoint::TetraHedronPoint(double size)
{
	points[0][0] = 0.0;
	points[0][1] = 0.0;
	points[0][2] = size/2.0;

	points[1][0] = size/2.0;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = size/2.0;
	points[2][2] = 0.0;

	points[3][0] = -size/2.0;
	points[3][1] = 0.0;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = -size/2.0;
	points[4][2] = 0.0;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -size/2.0;
}

// draws the point translated by x,y,z
void TetraHedronPoint::Draw(double x, double y, double z)
{
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	glBegin(GL_TRIANGLE_FAN);
		glVertex3d(
			points[0][0],
			points[0][1],
			points[0][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);
		glVertex3d(
			points[2][0],
			points[2][1],
			points[2][2]);
		glVertex3d(
			points[3][0],
			points[3][1],
			points[3][2]);
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3d(
			points[0][0],
			points[0][1],
			points[0][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();

	glPopMatrix();
}


// default ctor for DoubleTetrahedronPoint
DoubleTetraHedronPoint::DoubleTetraHedronPoint()
{
	points[0][0] = 0.0;
	points[0][1] = 0.0;
	points[0][2] = 0.25;

	points[1][0] = 0.25;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = 0.25;
	points[2][2] = 0.0;

	points[3][0] = -0.25;
	points[3][1] = 0.0;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = -0.25;
	points[4][2] = 0.0;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -0.25;
}

// ctor that sets the diameter of the point
DoubleTetraHedronPoint::DoubleTetraHedronPoint(double size)
{
	points[0][0] = 0.0;
	points[0][1] = 0.0;
	points[0][2] = size/2.0;

	points[1][0] = size/2.0;
	points[1][1] = 0.0;
	points[1][2] = 0.0;

	points[2][0] = 0.0;
	points[2][1] = size/2.0;
	points[2][2] = 0.0;

	points[3][0] = -size/2.0;
	points[3][1] = 0.0;
	points[3][2] = 0.0;

	points[4][0] = 0.0;
	points[4][1] = -size/2.0;
	points[4][2] = 0.0;

	points[5][0] = 0.0;
	points[5][1] = 0.0;
	points[5][2] = -size/2.0;
}

// draws the point translated by x,y,z
void DoubleTetraHedronPoint::Draw(double x, double y, double z)
{
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	glBegin(GL_TRIANGLE_FAN);
		glVertex3d(
			points[0][0],
			points[0][1],
			points[0][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);
		glVertex3d(
			points[2][0],
			points[2][1],
			points[2][2]);
		glVertex3d(
			points[3][0],
			points[3][1],
			points[3][2]);
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();

	
	glBegin(GL_TRIANGLES);
		glVertex3d(
			points[0][0],
			points[0][1],
			points[0][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		glVertex3d(
			points[5][0],
			points[5][1],
			points[5][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);
		glVertex3d(
			points[2][0],
			points[2][1],
			points[2][2]);
		glVertex3d(
			points[3][0],
			points[3][1],
			points[3][2]);
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();
	
	glBegin(GL_TRIANGLES);
		glVertex3d(
			points[5][0],
			points[5][1],
			points[5][2]);
		glVertex3d(
			points[1][0],
			points[1][1],
			points[1][2]);	
		glVertex3d(
			points[4][0],
			points[4][1],
			points[4][2]);
	glEnd();
	glPopMatrix();
}

