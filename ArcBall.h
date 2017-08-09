// -*- c++-mode -*-
// ArcBall.h
// based on original code by Ken Shoemake.
// hacked by Peter Schroeder and Andrei Khodakovsky among others
//
#ifndef _H_ArcBall
#define _H_ArcBall

#include <math.h>
#include "xvec.h"

class ArcBall{
public:
  ArcBall( void );
  ~ArcBall( void ) {};

  enum AxisSet{
    NoAxes = 0,
    CameraAxes = 1,
    BodyAxes = 2,
    OtherAxes = 3,
    NSets = 4
  };

//  void Place(const Quat& center, const double radius);
  void Mouse(const float x, const float y);
  void UseSet(AxisSet axisSet);
  void ShowResult( void );
  void HideResult( void );
  void Update( void );
  void Value(float mNow[4][4]);
  void Set(float mNow[4][4]);
  void BeginDrag( void );
  void EndDrag( void );
  void BeginTrans( void );
  XVecf Trans( void );
  void Draw(float aspect);

private:
  // NOTE: This is a highly bastardized version of quaternions for this particular task only
  // NOTE: don't use for anything else unless you know precisely what you are doing...
  enum QuatPart{
    X = 0,
    Y = 1,
    Z = 2,
    W = 3,
    QuatLen = 4 
  };
 
  class Quat{
  public:
    Quat( void ) {}
    Quat( const float x, const float y, const float z, const float w = 0 )
      : fx( x ), fy( y ), fz( z ), fw( w ) {}
    Quat( const Quat& q ) : fx( q.fx ), fy( q.fy ), fz( q.fz ), fw( q.fw ) {}
    // construct a unit quaternion from two points on the sphere
    Quat( const Quat& from, const Quat& to )
      : fx( from.fy*to.fz - from.fz*to.fy ),
        fy( from.fz*to.fx - from.fx*to.fz ),
	fz( from.fx*to.fy - from.fy*to.fx ),
	fw( from.fx*to.fx + from.fy*to.fy + from.fz*to.fz ) {}
    ~Quat( void ) {}

    Quat Conj( void ) const { return Quat( -fx, -fy, -fz, fw ); }
    float Norm( void ) const { return fx*fx+fy*fy+fz*fz; }

    // Return unit magnitude vector in direction of v
    Quat Unit( void ) const {
      float vlen;
      return ( vlen = sqrtf( Norm() ) ) > 1e-6f ? Quat( fx/vlen, fy/vlen, fz/vlen ) : Quat( 0, 0, 0 );
    }
    Quat operator*( const float s ) const { return Quat( s*fx, s*fy, s*fz, fw ); }
    Quat operator*( const Quat& q ) const {
      return Quat( fw*q.fx+fx*q.fw+fy*q.fz-fz*q.fy,
	           fw*q.fy+fy*q.fw+fz*q.fx-fx*q.fz,
		   fw*q.fz+fz*q.fw+fx*q.fy-fy*q.fx,
		   fw*q.fw-fx*q.fx-fy*q.fy-fz*q.fz );
    }

    Quat operator-( void ) const { return Quat( -fx, -fy, -fz, fw ); }
    Quat operator+( const Quat v2 ) const { return Quat( fx+v2.fx, fy+v2.fy, fz+v2.fz ); }
    Quat operator-( const Quat v2 ) const { return Quat( fx-v2.fx, fy-v2.fy, fz-v2.fz ); }
    Quat Bisect( const Quat v1 ) const {
      Quat v = *this+v1; float Nv;
      return (Nv = v.Norm()) < 1e-5 ? Quat( 0, 0, 1 ) : v * (1/sqrtf(Nv));
    }
    float Dot( const Quat& v2 ) const { return fx*v2.fx+fy*v2.fy+fz*v2.fz; }
    Quat Cross( const Quat& v2 ) const { return Quat( fy*v2.fz-fz*v2.fy, fz*v2.fx-fx*v2.fz, fx*v2.fy-fy*v2.fx ); }
    
    void DrawHalfArc( void ) const;
    void DrawAnyArc(const Quat& vTo) const;

    static Quat ConstrainToAxis(const Quat& loose, const Quat& axis);
    static int NearestConstraintAxis(const Quat& loose, const Quat *axes, const int nAxes);
    static void BallPoints(const Quat& q, Quat& arcFrom, Quat& arcTo);

    float fx, fy, fz, fw;
  };
  
  typedef float HMatrix[QuatLen][QuatLen];
  static void Copy( HMatrix lhs, const HMatrix rhs );
  static void Copy( HMatrix lhs, const Quat& rhs );

  typedef float *ConstraintSet;
  
  Quat center;
  float radius;
  Quat qNow, qDown, qDrag;
  Quat tDown;
  Quat vNow, vDown, vFrom, vTo, vrFrom, vrTo;
  HMatrix mNow, mDown;
  BOOL showResult, dragging;
  ConstraintSet sets[NSets];
  int setSizes[NSets];
  AxisSet axisSet;
  int axisIndex;

  Quat MouseOnSphere(const Quat& mouse) const;
  
  static void Circ( void );
  void DrawOuterRing( void ) const;
  void DrawConstraints( void ) const;
  void DrawDragArc( void ) const;
  void DrawResultArc( void ) const;

  static HMatrix mId, mZero;
  static Quat qOne, qZero;

};
#endif /* _H_ArcBall */
