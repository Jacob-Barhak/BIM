// -*- Mode: c++ -*-
// $Id: cvec2.h,v 1.1.1.1 2003/10/29 22:14:18 guskov Exp $
// $Source: /home/cvsroot/graphics/base/common/cvec2.h,v $

#ifndef	__CVEC2_H__
#define	__CVEC2_H__

// Do not change anything above this line
#ifdef _WIN32
#undef min
#undef max
#endif

#include <stddef.h>
#include <math.h>
//#include "Dynamic/useful.h"

using namespace std;

class CVec2;
istream& operator>>( istream&, CVec2& );
ostream& operator<<( ostream&, const CVec2& );

// column type 2 vectors
class CVec2{
 public:
  enum{
    X = 0,
    Y = 1
    };
  CVec2( void ) {}
  CVec2( const double f ) { m_v[X] = m_v[Y] = f; }
  CVec2( const CVec2& c )
    { m_v[X] = c.m_v[X]; m_v[Y] = c.m_v[Y]; }
  CVec2( const double& a, const double& b)
    { m_v[X] = a; m_v[Y] = b; }
  CVec2( const double* a )
    { m_v[X] = a[X]; m_v[Y] = a[Y]; }
  CVec2( const float* a )
    { m_v[X] = double( a[X] ); m_v[Y] = double( a[Y] ); }
  operator double*( void )
    { return &m_v[X]; }
  operator const double*( void ) const
    { return &m_v[X]; }
  // ~CVec2( void ) {}
  
  friend istream& operator>>( istream&, CVec2& );
  friend ostream& operator<<( ostream&, const CVec2& );
  void print( ostream& os ) const;
  bool read ( istream& is );
  
  bool operator==(const CVec2& c)const
    { return m_v[X]==c.m_v[X] && m_v[Y]==c.m_v[Y];}
  bool operator<(const CVec2& c)const 
    { return unsigned(this)<unsigned(&c); }
  
  CVec2& operator=( const CVec2& c )
    { m_v[X] = c.m_v[X]; m_v[Y] = c.m_v[Y]; return *this; }
  CVec2 operator+( const CVec2& c ) const
    { return CVec2( m_v[X] + c.m_v[X], m_v[Y] + c.m_v[Y]); }
  CVec2 operator-( const CVec2& c ) const
    { return CVec2( m_v[X] - c.m_v[X], m_v[Y] - c.m_v[Y]); }
  CVec2 operator*( const double s ) const
    { return CVec2( s * m_v[X], s * m_v[Y] ); }
  friend CVec2 operator*( const double s, const CVec2& c )
    { return CVec2( s * c.m_v[X], s * c.m_v[Y] ); }
  CVec2 operator/( const double s ) const
    { return CVec2( m_v[X] / s, m_v[Y] / s ); }
  
  CVec2& operator+=( const CVec2& c )
    { m_v[X] += c.m_v[X], m_v[Y] += c.m_v[Y]; return *this; }
  CVec2& operator-=( const CVec2& c )
    { m_v[X] -= c.m_v[X], m_v[Y] -= c.m_v[Y]; return *this; }
  CVec2& operator*=( const double s )
    { m_v[X] *= s, m_v[Y] *= s; return *this; }
  CVec2& operator/=( const double s )
    { m_v[X] /= s, m_v[Y] /= s; return *this; }
  CVec2 operator-( void ) const
    { return CVec2( -m_v[X], -m_v[Y] ); }
  
  //stupid but useful
  CVec2 operator*(const CVec2& c)const{
    return CVec2(m_v[X]*c.m_v[X], m_v[Y]*c.m_v[Y] );
  }
  CVec2 operator/(const CVec2& c)const{
    return CVec2(m_v[X]/c.m_v[X], m_v[Y]/c.m_v[Y]);
  }
  
  double& x( void ) { return m_v[X]; }
  double x( void ) const { return m_v[X]; }
  double& y( void ) { return m_v[Y]; }
  double y( void ) const { return m_v[Y]; }
  double& operator() (const int i)      { return m_v[i]; }
  double operator() (const int i) const { return m_v[i]; }
  
  CVec2 lerp( const CVec2& v1, const double t ) const
    { return CVec2( m_v[X] + t * ( v1.m_v[X] - m_v[X] ),
		   m_v[Y] + t * ( v1.m_v[Y] - m_v[Y] ) ); }

  CVec2 min( const CVec2& o ) const
  { double a = __min( m_v[X], o.m_v[X] );
    double b = __min( m_v[Y], o.m_v[Y] );
    return CVec2( a, b ); }
  CVec2 max( const CVec2& o ) const
  {		double a = __max( m_v[X], o.m_v[X] );
      double b = __max( m_v[Y], o.m_v[Y] );
      return CVec2( a, b); }
  void bbox( CVec2& min, CVec2& max ){
    if( m_v[X] < min.m_v[X] ) min.m_v[X] = m_v[X];
    else if( m_v[X] > max.m_v[X] ) max.m_v[X] = m_v[X];

    if( m_v[Y] < min.m_v[Y] ) min.m_v[Y] = m_v[Y];
    else if( m_v[Y] > max.m_v[Y] ) max.m_v[Y] = m_v[Y];
  }

  double dot( const CVec2& c ) const
    { return m_v[X] * c.m_v[X] + m_v[Y] * c.m_v[Y]; }
  double dot( void ) const
    { return m_v[X] * m_v[X] + m_v[Y] * m_v[Y]; }
  double cross( const CVec2& c ) const
    { return m_v[X] * c.m_v[Y] - m_v[Y] * c.m_v[X]; }
  double l1( void ) const
    { double a = abs( m_v[X] ); a += abs( m_v[Y] ); return a; }
  double linfty( void ) const
    { double a = abs( m_v[X] ); a = __max( a, abs( m_v[Y] ) );
      return a; }
  double l2( void ) const { return sqrtf( dot() ); }
  
  CVec2 dir( void ) const;

  int normalize( void )
    { double mag = l2(); return ( fabs(mag)<1e-12 ) ? 0 : ( *this *= 1 / mag, 1 ); }
  double dist( const CVec2& c ) const { return ( *this - c ).l2(); }
  int collinear( const CVec2& a, const CVec2& b ) const;
  //CVec2 perp( void ) const;
  
 protected:
 private:
  double	m_v[2];
};

typedef CVec2* CVec2p;

// extern defs for non-member outline functions go here
// externally visible constant defs go here

#endif	/* __CVEC_H__ */
