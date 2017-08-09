#include "stdafx.h"

#ifdef _WIN32
	#pragma warning(disable:4786 4666)
#endif

#include "xvec.h"
#include "geom.h"
#include <assert.h>

XVecf ComputeNormal(const XVecf& p1, const XVecf& p2, const XVecf& p3) {
	XVecf v(p2 - p1);
	v.normalize();
	XVecf v13(p3-p1);
	XVecf normal = v.cross(v13);
  //crossprod(p1, p2, p3, normal);
  normal.normalize();
	return normal;
}

/// KV ////////////////////////////////////////////////////////////////
// computes the angle formed by (p2 - p1) and (p3 - p1)
/// KV ////////////////////////////////////////////////////////////////
double ComputeAngle(const XVecf& p1, const XVecf& p2, const XVecf& p3) {
	XVecf va(p2 - p1);
	XVecf vb(p3 - p1);
  double vavb = va.l2() * vb.l2();
  if(vavb<1e-10)
    return 0.0;
  double arg = va.dot(vb) / vavb;
  assert(fabs(arg)<=1.0000001);
	if(arg>=1.0f)
		arg = 1.0f;
	else if(arg<=-1.0f)
		arg = -1.0f;
	double ac =  acos( arg );
  assert(ac>=0.0);
  return ac;
}

double 
CalcRoundness(XVecf& p0, XVecf& p1, XVecf& p2) {
  double peri, len, lmax;

	XVecf v(p1 - p0);
	XVecf normal = v.cross(p2-p0);
  double area = .5 * normal.l2();

	len = p0.dist(p1);
  peri = len;
  lmax = len;

	len = p1.dist(p2);
  peri += len;
  if(len>lmax) lmax = len;

	len = p2.dist(p0);
  peri += len;
  if(len>lmax) lmax = len;

  if(area<1e-10)
    return 1000.0;
  else
    return peri * lmax / area;
}

double 
CalcRoundnessAndNormal(XVecf& p0, XVecf& p1, XVecf& p2, XVecf& normal) {
  double peri, len, lmax;

	XVecf v(p1 - p0);
	normal = v.cross(p2-p0);
  double area = .5 * normal.l2();

	len = p0.dist(p1);
  peri = len;
  lmax = len;

	len = p1.dist(p2);
  peri += len;
  if(len>lmax) lmax = len;

	len = p2.dist(p0);
  peri += len;
  if(len>lmax) lmax = len;

	normal.normalize();

  if(area<1e-10)
    return 100.0;
  else
    return peri * lmax / area;
}

double CalcArea(const XVecf& p0, const XVecf& p1, const XVecf& p2) {
	XVecf v(p1 - p0);
	XVecf cross = v.cross(p2-p0);
  return .5 * cross.l2();
}

double CalcArea2d(double* xy0, double* xy1, double* xy2) {
	double xy01[2], xy02[2];

	xy01[0] = xy1[0] - xy0[0];
	xy01[1] = xy1[1] - xy0[1];

	xy02[0] = xy2[0] - xy0[0];
	xy02[1] = xy2[1] - xy0[1];

	return xy01[0]*xy02[1] - xy01[1]*xy02[0];
}

double ComputeArea(const CVec2& par0, const CVec2& par1, const CVec2& par2) {
	return (par1-par0).cross(par2-par0);
}

void ComputeBary(XVecf& bary, const CVec2& par, 
								 const CVec2& par0, const CVec2& par1, const CVec2& par2) {
	bary(0) = ComputeArea(par, par1, par2);
	bary(1) = ComputeArea(par, par2, par0);
	bary(2) = ComputeArea(par, par0, par1);
	bary /= (bary(0)+bary(1)+bary(2));
}

double dist2D(double *p, double *q) {
  return sqrt((p[0] - q[0])*(p[0] - q[0]) 
		+ (p[1] - q[1])*(p[1] - q[1]));
}



