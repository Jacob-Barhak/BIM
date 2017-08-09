#ifdef _WIN32
	#pragma warning(disable:4786 4666)
#endif

// -*- Mode: c++ -*-
// $Id: cvec2.cpp,v 1.1.1.1 2003/10/29 22:14:18 guskov Exp $
// $Source: /home/cvsroot/graphics/base/common/cvec2.cpp,v $

// do not edit anything above this line
 
#include "stdafx.h"
#include <iostream>
#include <math.h>
#include <assert.h>
//#include <algobase.h>
  
#include "cvec2.h"
//#include "Dynamic/useful.h"

//#include "util.h" 
#if 1
void 
CVec2::print(ostream& os) const
{
  os << "(" << m_v[0] << ", " << m_v[1] << ")";
}

ostream&
operator<<( ostream& os, const CVec2& c )
{
  // c.print( os );	
	os << c.m_v[0] << " " << c.m_v[1];
  return os;
}

istream&
operator>>( istream& is, CVec2& f )
{
  return is >> f.m_v[0] >> f.m_v[1];
}


bool
CVec2::read( istream& is )  {
  return ( is >> m_v[0]) && (is >> m_v[1]);
}
#endif

CVec2
CVec2::dir( void ) const
{
  double a = l2();
  if( ( fabs(a)<1e-12 ) ) return *this;
  else return *this / a;
}

#if 0
int
CVec2::collinear( const CVec2& a, const CVec2& b ) const
{
  return zero( ( a - *this ).cross( a - b ) );
}
#endif
