#ifndef _GEOM_H
#define _GEOM_H

//#include "Dynamic/cvecd.h"
#include "cvec2.h"

XVecf ComputeNormal(const XVecf& p1, const XVecf& p2, const XVecf& p3);

double 
CalcRoundness(XVecf& p0, XVecf& p1, XVecf& p2);

double 
CalcArea(const XVecf& p0, const XVecf& p1, const XVecf& p2);

double 
CalcArea2d(double* xy0, double* xy1, double* xy2);

double ComputeArea(const CVec2& par0, const CVec2& par1, const CVec2& par2);

double ComputeAngle(const XVecf& p1, const XVecf& p2, const XVecf& p3);

double dist2D(double *p, double *q);

void ComputeBary(XVecf& bary, const CVec2& par, 
								 const CVec2& par0, const CVec2& par1, const CVec2& par2);

#ifdef _WIN32
//	const double M_PI = 3.14159265358979323846f;
	//const double M_PI_2 = 1.57079632679489661923f;
#endif
//#define M_PI 3.14159265358979323846f

double 
CalcRoundnessAndNormal(XVecf& p0, XVecf& p1, XVecf& p2, XVecf& normal);


#endif
