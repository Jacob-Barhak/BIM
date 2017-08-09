/************************************************
Declaration and 
implementation of the vector manipulation library.
i.e. dot product, cross product, addition, etc.
*************************************************/


#ifndef  __XVEC_H__
#define  __XVEC_H__

//#include <algorithm>
#ifndef _NO_IOSTREAMS
#include <iostream>
#endif
//#include <stddef.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
// column type 3 vectors

template<class real_t> 
class XVecT {
public:
  enum{ X = 0, Y = 1, Z = 2 };

  XVecT(void) {}
  XVecT(real_t f ) { m_v[X] = m_v[Y] = m_v[Z] = f; }
  
#if 0
  XVecT(const XVecT& c) { 
    m_v[X] = c.m_v[X];
    m_v[Y] = c.m_v[Y];
    m_v[Z] = c.m_v[Z]; 
  }
#endif

  template<class oreal_t> 
  explicit
  XVecT(const XVecT<oreal_t>& c) { 
    m_v[X] = c.x();
    m_v[Y] = c.y();
    m_v[Z] = c.z(); 
  }
  XVecT( const real_t& a, const real_t& b, const real_t& c ) { 
    m_v[X] = a;
    m_v[Y] = b;
    m_v[Z] = c; 
  }
  XVecT( const real_t* a ) { 
    m_v[X] = a[X];
    m_v[Y] = a[Y];
    m_v[Z] = a[Z]; 
  }

  operator real_t*( void ) { 
    return &m_v[0]; 
  }
  operator const real_t*( void ) const { 
    return &m_v[0]; 
  }

  bool operator==(const XVecT& c) const { 
    return m_v[X]==c.m_v[X] && m_v[Y]==c.m_v[Y] && m_v[Z]==c.m_v[Z];
  }

  XVecT& operator=( const XVecT& c ) { 
    m_v[X] = c.m_v[X];
    m_v[Y] = c.m_v[Y];
    m_v[Z] = c.m_v[Z]; 
    return *this; 
  }
  
  XVecT operator+( const XVecT& c ) const { 
    return XVecT( m_v[X] + c.m_v[X], m_v[Y] + c.m_v[Y], m_v[Z] + c.m_v[Z] ); 
  }
  XVecT operator-( const XVecT& c ) const { 
    return XVecT( m_v[X] - c.m_v[X], m_v[Y] - c.m_v[Y], m_v[Z] - c.m_v[Z] ); 
  }
  XVecT operator*( const real_t s ) const { 
    return XVecT( s * m_v[X], s * m_v[Y], s * m_v[Z] ); 
  }
  friend XVecT operator*( const real_t s, const XVecT& c ) { 
    return XVecT( s * c.m_v[X], s * c.m_v[Y], s * c.m_v[Z] ); 
  }
  XVecT operator/( const real_t s ) const { 
    return XVecT( m_v[X] / s, m_v[Y] / s, m_v[Z] / s ); 
  }

  XVecT& operator+=( const XVecT& c ) { 
    m_v[X] += c.m_v[X];
    m_v[Y] += c.m_v[Y];
    m_v[Z] += c.m_v[Z]; 
    return *this; 
  }
  XVecT& operator>>=( int k ) { 
    m_v[X] >>= k;
    m_v[Y] >>= k;
    m_v[Z] >>= k; 
    return *this; 
  }
  XVecT& operator-=( const XVecT& c ) { 
    m_v[X] -= c.m_v[X];
    m_v[Y] -= c.m_v[Y];
    m_v[Z] -= c.m_v[Z]; 
    return *this; 
  }
  XVecT& operator*=( const real_t s ) { 
    m_v[X] *= s;
    m_v[Y] *= s;
    m_v[Z] *= s; 
    return *this; 
  }
  XVecT& operator/=( const real_t s ) { 
    m_v[X] /= s;
    m_v[Y] /= s;
    m_v[Z] /= s; 
    return *this; 
  }
  XVecT operator-( void ) const { 
    return XVecT( -m_v[X], -m_v[Y], -m_v[Z] ); 
  }

  //stupid but useful
  XVecT operator*(const XVecT& c)const{
    return XVecT(m_v[X]*c.m_v[X], m_v[Y]*c.m_v[Y], m_v[Z]*c.m_v[Z]);
  }
  XVecT operator/(const XVecT& c)const{
    return XVecT(m_v[X]/c.m_v[X], m_v[Y]/c.m_v[Y], m_v[Z]/c.m_v[Z]);
  }

  real_t& x( void ) { return m_v[X]; }
  real_t x( void ) const { return m_v[X]; }
  real_t& y( void ) { return m_v[Y]; }
  real_t y( void ) const { return m_v[Y]; }
  real_t& z( void ) { return m_v[Z]; }
  real_t z( void ) const { return m_v[Z]; }
  real_t& operator() (const int i)      { return m_v[i]; }
  real_t operator() (const int i) const { return m_v[i]; }

  const real_t& ref() const {
    return m_v[0];
  }

  XVecT Min( const XVecT& o ) const
  { 
#ifdef _MSC_VER
		real_t a = __min( m_v[X], o.m_v[X] );
    real_t b = __min( m_v[Y], o.m_v[Y] );
    real_t c = __min( m_v[Z], o.m_v[Z] );
#else
		real_t a = ::Min( m_v[X], o.m_v[X] );
    real_t b = ::Min( m_v[Y], o.m_v[Y] );
    real_t c = ::Min( m_v[Z], o.m_v[Z] );
#endif
    return XVecT( a, b, c ); 
	}
  
  XVecT Max( const XVecT& o ) const
  {    
#ifdef _MSC_VER
		real_t a = __max( m_v[X], o.m_v[X] );
    real_t b = __max( m_v[Y], o.m_v[Y] );
    real_t c = __max( m_v[Z], o.m_v[Z] );
#else
		real_t a = ::Max( m_v[X], o.m_v[X] );
    real_t b = ::Max( m_v[Y], o.m_v[Y] );
    real_t c = ::Max( m_v[Z], o.m_v[Z] );
#endif
    return XVecT( a, b, c ); 
	}

  void bbox( XVecT& cmin, XVecT& cmax ) const {
    if( m_v[X] < cmin.m_v[X] ) cmin.m_v[X] = m_v[X];
    if( m_v[X] > cmax.m_v[X] ) cmax.m_v[X] = m_v[X];
    if( m_v[Y] < cmin.m_v[Y] ) cmin.m_v[Y] = m_v[Y];
    if( m_v[Y] > cmax.m_v[Y] ) cmax.m_v[Y] = m_v[Y];
    if( m_v[Z] < cmin.m_v[Z] ) cmin.m_v[Z] = m_v[Z];
    if( m_v[Z] > cmax.m_v[Z] ) cmax.m_v[Z] = m_v[Z];
  }

  real_t dot( const XVecT& c ) const { 
    return m_v[X] * c.m_v[X] + m_v[Y] * c.m_v[Y] + m_v[Z] * c.m_v[Z]; 
  }
  real_t dot( void ) const { 
    return m_v[X] * m_v[X] + m_v[Y] * m_v[Y] + m_v[Z] * m_v[Z]; 
  }
  XVecT cross( const XVecT& c ) const { 
    return XVecT( m_v[Y] * c.m_v[Z] - m_v[Z] * c.m_v[Y],
                  m_v[Z] * c.m_v[X] - m_v[X] * c.m_v[Z],
                  m_v[X] * c.m_v[Y] - m_v[Y] * c.m_v[X] ); 
  }
  real_t l1( void ) const { 
    real_t a = fabs( m_v[X] ); 
    a += fabs( m_v[Y] ); 
    return a + fabs( m_v[Z] ); 
  }
  real_t l2( void ) const { 
    return sqrtf( dot() ); 
  }

  real_t dist( const XVecT& c ) const {
    return ( *this - c ).l2(); 
  }

  real_t dist_squared( const XVecT& c ) const {
    return ( *this - c ).dot(); 
  }
  int normalize( void ) { 
    real_t mag = l2(); return (fabs( mag )<1e-25) ? 0 : ( *this *= 1 / mag, 1 ); 
  }
  real_t linfty( void ) const
  { real_t a = abs( m_v[X] ); a = ::Max( a, abs( m_v[Y] ) );
    return ::Max( a, abs( m_v[Z] ) ); }

  size_t max_index() const {
    return ( m_v[0]>=m_v[1] ? ( m_v[0]>=m_v[2] ? 0 : 2 ) : ( m_v[1]>=m_v[2] ? 1 : 2 ) );
  }

  size_t min_index() const {
    return ( m_v[0]<m_v[1] ? ( m_v[0]<m_v[2] ? 0 : 2 ) : ( m_v[1]<m_v[2] ? 1 : 2 ) );
  }

  size_t minabs_index() const {
    return XVecf(fabs(m_v[0]),fabs(m_v[1]), fabs(m_v[2])).min_index();
  }

protected:
  real_t  m_v[3];
};

#ifndef _NO_IOSTREAMS
template<class real_t> 
std::ostream& operator<<( std::ostream& os, const XVecT<real_t>& c )
{
  os << c.x() << " " << c.y() << " " << c.z();
  return os;
}

template<class real_t> 
std::istream&
operator>>( std::istream& is, XVecT<real_t>& f )
{
  return is >> f.x() >> f.y() >> f.z();
}
#endif

template<class real_t>
XVecT<real_t> mult(float* mat, XVecT<real_t>& pt) {
  // assuming 3 by 4 matrix stored row-wise
  XVecT<real_t> res(real_t(0));
  for(int i=0; i<3; ++i) {
    res(i) = mat[4*i+3];
    for(int j=0; j<3; ++j) 
      res(i) += mat[4*i+j]*pt[j];
  }
  return res;
}

template<class real_t>
XVecT<real_t> mult3x3(float* mat, XVecT<real_t>& pt) {
  // assuming 3 by 4 matrix stored row-wise
  XVecT<real_t> res(real_t(0));
  for(int i=0; i<3; ++i) {
    res(i) = real_t(0);
    for(int j=0; j<3; ++j) 
      res(i) += mat[4*i+j]*pt[j];
  }
  return res;
}

template<class real_t>
void complement_basis(const XVecT<real_t>& vec, XVecT<real_t>& t0, XVecT<real_t>& t1) {
  // figure out smallest
  XVecf d(0,0,0);
  d[vec.minabs_index()] = 1.0f;

  t0 = d.cross(vec);
  t1 = vec.cross(t0);

  t0.normalize();
  t1.normalize();
}


// assumes two 3x3 matrices, stored as 1d
template<class real_t>
real_t* mult3x3(real_t* mat1, real_t* mat2)
{
	real_t* res;
	res = new real_t[9];

	/*
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			int x = 3*i + j; //current index
			res[x] = mat1[x]*mat2[x] + mat1[x+1]*mat2[x+3] + mat1[x+2]*mat2[x+6];
		}
	}*/
	// I think this is the correct math . Jun
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			int x = 3*i + j; //current index
			res[x] = mat1[3*i]*mat2[j] + mat1[3*i+1]*mat2[j+3] + mat1[3*i+2]*mat2[j+6];
		}
	}
	return res;
}

// assumes two 4x4 matrices, stored as 1d
template<class real_t>
real_t* mult4x4(real_t* mat1, real_t* mat2)
{
	real_t* res;
	res = new real_t[16];

	/*
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			int x = 4*i + j; //current index
			res[x] = mat1[x]*mat2[x] + mat1[x+1]*mat2[x+4] + mat1[x+2]*mat2[x+8] + mat1[3]*mat2[x+12];
		}
	}
	*/
	// I think this is the correct math . Jun
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			int x = 4*i + j; //current index
			res[x] = mat1[4*i]*mat2[j] + mat1[4*i+1]*mat2[j+4] + mat1[4*i+2]*mat2[j+8] + mat1[4*i+3]*mat2[j+12];
		}
	}
	
	return res;
}

// adds mat2 to mat1, modifies mat1 data.
// assumes 2 3x3 matrices
template<class real_t>
void add3x3(real_t* mat1, real_t* mat2)
{
	for(int i = 0; i < 9; i++)
	{
		mat1[i] = mat1[i] + mat2[i];
	}
}

// Takes the outer product of 2 vectors.
// returns a 3x3 matrix
template<class real_t>
real_t* outerproduct(XVecT<real_t>& p, XVecT<real_t>& q)
{
	real_t* res;
	res = new real_t[9];
	for(int i = 0; i < 9; i++) res[i] = 0;
	for(int j = 0; j < 3; j++)
	{
		for(int k = 0; k < 3; k++)
		{
			res[3*j + k] = p[j] * q[k];
		}
	}
	return res;
}

// Converts a 3x3 to a standard 4x4 matrix
template<class real_t>
real_t* convert3x3to4x4(real_t* matrix)
{
	real_t* converted;
	converted = new real_t[16];
	for(int i = 0; i < 15; i++) converted[i] = 0;
	converted[15] = 1;

	for(int j = 0; j < 3; j++)
	{
		for(int k = 0; k < 3; k++)
		{
			converted[4*j+k] = matrix[3*j+k];
		}
	}
	return converted;
}

// Transposes a 4x4 matrix
template<class real_t>
void transpose(real_t* matrix)
{
	// make a copy of the current matrix . Jun
	real_t* copy;
	copy = new real_t[16];
	for(int i = 0 ; i < 16; i++)
		copy[i] = matrix[i];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[4*i + j] = copy[i + 4*j];
		}
	}
	// clean up memory
	delete[] copy;
}

// Transposes a 3x3 matrix
template<class real_t>
void transpose3x3(real_t* matrix)
{
	// make a copy of the current matrix . Jun
	real_t* copy;
	copy = new real_t[9];
	for(int i = 0 ; i < 9; i++)
		copy[i] = matrix[i];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[3*i + j] = copy[i + 3*j];
		}
	}
	// clean up memory
	delete[] copy;
}

typedef XVecT<double> XVecd;
typedef XVecT<float> XVecf;
typedef XVecT<int> XVeci;
typedef XVecT<unsigned int> XVecu;

typedef XVecd XVec3d;
typedef XVecf XVec3f;
typedef XVeci XVec3i;
typedef XVecu XVec3u;

#endif  /* __CVECD_H__ */
