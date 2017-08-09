#ifndef  __XVECDIM_H__
#define  __XVECDIM_H__

//#include <algorithm>
#include <iostream>
#include <assert.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// column type 3 vectors

template<int dim, class real_t> 
class XVecdimT {
public:
  enum{ X = 0, Y = 1, Z = 2 };

  XVecdimT(void) {
  }
  XVecdimT(real_t f ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] = f;
  }
  XVecdimT(real_t f0, real_t f1 ) { 
    assert(dim>1);
    m_v[0] = f0;
    m_v[1] = f1;
  }
  XVecdimT(real_t f0, real_t f1, real_t f2) { 
    assert(dim>2);
    m_v[0] = f0;
    m_v[1] = f1;
    m_v[2] = f2;
  }
  XVecdimT(real_t f0, real_t f1, real_t f2, real_t f3) { 
    assert(dim>3);
    m_v[0] = f0;
    m_v[1] = f1;
    m_v[2] = f2;
    m_v[3] = f3;
  }
  XVecdimT(const XVecdimT& c) { 
    for(int i=0; i<dim; ++i)
      m_v[i] = c.m_v[i];
  }
  explicit XVecdimT( const real_t* a ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] = a[i];
  }

  operator real_t*( void ) { 
    return &m_v[0]; 
  }
  operator const real_t*( void ) const { 
    return &m_v[0]; 
  }

  bool operator==(const XVecdimT& c) const { 
    for(int i=0; i<dim; ++i)
      if(m_v[i]!=c.m_v[i])
        return false;
    return true;
  }

  bool operator!=(const XVecdimT& c) const { 
    return !((*this)==c);
  }

  bool operator<(const XVecdimT& c) const {
    for(int i=0; i<dim; ++i) {
      if(m_v[i]<c.m_v[i])
        return true;
      else if(m_v[i]>c.m_v[i])
        return false;
    }
    return false;
  }

  XVecdimT& operator=( const XVecdimT& c ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] = c.m_v[i];
    return *this; 
  }
  
  XVecdimT operator+( const XVecdimT& c ) const { 
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] += c.m_v[i];
    return pt;
  }

  XVecdimT operator-( const XVecdimT& c ) const { 
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] -= c.m_v[i];
    return pt;
  }
  XVecdimT operator*( const real_t s ) const { 
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] *= s;
    return pt;
  }

  friend XVecdimT operator*( const real_t s, const XVecdimT& c ) { 
    XVecdimT pt(c);
    for(int i=0; i<dim; ++i)
      pt[i] *= s;
    return pt;
  }
  
  XVecdimT operator/( const real_t s ) const { 
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] /= s;
    return pt;
  }

  XVecdimT& operator+=( const XVecdimT& c ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] += c.m_v[i];
    return *this; 
  }

  XVecdimT& operator-=( const XVecdimT& c ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] -= c.m_v[i];
    return *this; 
  }
  XVecdimT& operator*=( const real_t s ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] *= s;
    return *this; 
  }
  XVecdimT& operator/=( const real_t s ) { 
    for(int i=0; i<dim; ++i)
      m_v[i] /= s;
    return *this; 
  }
  XVecdimT operator-( void ) const { 
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] = -pt[i];
    return pt;
  }

  //stupid but useful
  XVecdimT operator*(const XVecdimT& c)const{
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] *= c.m_v[i];
    return pt;
  }
  XVecdimT operator/(const XVecdimT& c)const{
    XVecdimT pt(*this);
    for(int i=0; i<dim; ++i)
      pt[i] /= c.m_v[i];
    return pt;
  }

  real_t& operator() (const int i)      { return m_v[i]; }
  real_t operator() (const int i) const { return m_v[i]; }

  const real_t& ref() const {
    return m_v[0];
  }

  XVecdimT Min( const XVecdimT& o ) const
  { 
    XVecdimT pt;

    for(int i=0; i<dim; ++i) 
#ifdef _MSC_VER
      pt[i] = __min( m_v[i], o.m_v[i] );
#else
		  pt[i] = ::Min( m_v[i], o.m_v[i] );
#endif

    return pt;
	}
  
  XVecdimT Max( const XVecdimT& o ) const
  {    
    XVecdimT pt;

    for(int i=0; i<dim; ++i) 
#ifdef _MSC_VER
      pt[i] = __max( m_v[i], o.m_v[i] );
#else
		  pt[i] = ::Max( m_v[i], o.m_v[i] );
#endif

    return pt;
	}


  void bbox( XVecdimT& cmin, XVecdimT& cmax ){
    for(int i=0; i<dim; ++i) {
      if( m_v[i] < cmin.m_v[i] ) 
        cmin.m_v[i] = m_v[i];
      if( m_v[i] > cmax.m_v[i] ) 
        cmax.m_v[i] = m_v[i];
    }
  }

  real_t dot( const XVecdimT& c ) const { 
    real_t d = 0;
    for(int i=0; i<dim; ++i) 
      d += m_v[i] * c.m_v[i];
    return d;
  }

  real_t dot( void ) const { 
    real_t d = 0;
    for(int i=0; i<dim; ++i) 
      d += m_v[i] * m_v[i];
    return d;
  }

  real_t l1( void ) const { 
    real_t d = 0;
    for(int i=0; i<dim; ++i) 
      d += fabs( m_v[i] );
    return d;
  }
  real_t l2( void ) const { 
    return sqrtf( dot() ); 
  }

  real_t dist( const XVecdimT& c ) const {
    return ( *this - c ).l2(); 
  }
  int normalize( void ) { 
    real_t mag = l2(); return (fabs( mag )<1e-25) ? 0 : ( *this *= 1 / mag, 1 ); 
  }
  real_t linfty( void ) const
  { 
    real_t d = fabs( m_v[0] );
    for(int i=1; i<dim; ++i) 
      d = ::Max( a, fabs( m_v[i] ) ) );
    return d;
  }

protected:
  real_t  m_v[dim];
};

template<int dim, class real_t> 
std::ostream& operator<<( std::ostream& os, const XVecdimT<dim, real_t>& c )
{
  for(int i=0; i<dim; ++i) 
    os << c(i) << " ";
  return os;
}

template<int dim, class real_t> 
std::istream&
operator>>( std::istream& is, XVecdimT<dim, real_t>& f )
{
  return is >> f.x() >> f.y() >> f.z();
}

typedef XVecdimT<2, double> XVec2d;
typedef XVecdimT<2, float> XVec2f;
typedef XVecdimT<2, int> XVec2i;

typedef XVecdimT<4, double> XVec4d;
typedef XVecdimT<4, float> XVec4f;
typedef XVecdimT<4, int> XVec4i;
typedef XVecdimT<4, short> XVec4s;

#endif  /* __XVECDIM_H__ */
