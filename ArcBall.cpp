// -*- c++-mode -*-
//
// ArcBall.cpp
// derived from original sources of
// Ken Shoemake, 1993
// hacked by Peter Schroeder and Andrei Khodakovsky among others

#include "stdafx.h"
#include <gl\gl.h>
#include <gl\glu.h>

#include "ArcBall.h"
//#include "util.h"

// number of segments
#define LG_NSEGS 4
#define NSEGS (1<<LG_NSEGS)

// some convenient helpers
// if you want to change the colors, do that here.
#define RIMCOLOR()    glColor3d(1.0, 1.0, 1.0)
#define FARCOLOR()    glColor3d(1.0, 0.125, 0.125)
#define NEARCOLOR()   glColor3d(1.0, 1.0, 0.25)
#define DRAGCOLOR()   glColor3d(0.5, 1.0, 1.0)
#define RESCOLOR()    glColor3d(0.75, 0.125, 0.125)

// your standard identity matrix...
ArcBall::HMatrix ArcBall::mId = 
{
  {1.0f,0.0f,0.0f,0.0f},
  {0.0f,1.0f,0.0f,0.0f},
  {0.0f,0.0f,1.0f,0.0f},
  {0.0f,0.0f,0.0f,1.0f}
};

ArcBall::HMatrix ArcBall::mZero =
{
  {0.0f,0.0f,0.0f,0.0f},
  {0.0f,0.0f,0.0f,0.0f},
  {0.0f,0.0f,0.0f,0.0f},
  {0.0f,0.0f,0.0f,0.0f}
};

ArcBall::Quat ArcBall::qOne( 0, 0, 0, 1 );
ArcBall::Quat ArcBall::qZero( 0, 0, 0, 0 );

static int zero(const float x) {
  return x==0.0;
}

void
ArcBall::Copy(HMatrix a, const HMatrix b)
{
  a[0][0]=b[0][0];a[0][1]=b[0][1];a[0][2]=b[0][2];a[0][3]=b[0][3];
  a[1][0]=b[1][0];a[1][1]=b[1][1];a[1][2]=b[1][2];a[1][3]=b[1][3];
  a[2][0]=b[2][0];a[2][1]=b[2][1];a[2][2]=b[2][2];a[2][3]=b[2][3];
  a[3][0]=b[3][0];a[3][1]=b[3][1];a[3][2]=b[3][2];a[3][3]=b[3][3];
}

// Construct rotation matrix from (possibly non-unit) quaternion.
// Assumes matrix is used to multiply column vector on the left:
// vnew = mat vold.  Works correctly for right-handed coordinate system
// and right-handed rotations.
void
ArcBall::Copy(HMatrix out, const Quat& q )
{
  double Nq = q.fx*q.fx + q.fy*q.fy + q.fz*q.fz + q.fw*q.fw;
  double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
  double xs = q.fx*s,	      ys = q.fy*s,	  zs = q.fz*s;
  double wx = q.fw*xs,	      wy = q.fw*ys,	  wz = q.fw*zs;
  double xx = q.fx*xs,	      xy = q.fx*ys,	  xz = q.fx*zs;
  double yy = q.fy*ys,	      yz = q.fy*zs,	  zz = q.fz*zs;
  out[X][X] = float(1.0 - (yy + zz)); out[Y][X] = float(xy + wz); out[Z][X] = float(xz - wy);
  out[X][Y] = float(xy - wz); out[Y][Y] = float(1.0 - (xx + zz)); out[Z][Y] = float(yz + wx);
  out[X][Z] = float(xz + wy); out[Y][Z] = float(yz - wx); out[Z][Z] = float(1.0 - (xx + yy));
  out[X][W] = out[Y][W] = out[Z][W] = out[W][X] = out[W][Y] = out[W][Z] = 0;
  out[W][W] = 1.0f;
}

ArcBall::ArcBall( void )
: center( qOne ), radius( .8f ),
  qNow( qOne ), qDown( qOne ), qDrag( qOne ),
  vNow( qZero ), vDown( qZero ), tDown( qZero ),
  vFrom( qZero ), vTo( qZero ),
  vrFrom( qZero ), vrTo( qZero ),
  showResult( FALSE ), dragging( FALSE ),
  axisSet( NoAxes ), axisIndex( -1 )
{
  Copy( mNow, mId ); Copy( mDown, mId );

  static float otherAxis[][4] = {{-0.48f, 0.80f, 0.36f, 1.0f}};

  sets[CameraAxes] = mId[X]; setSizes[CameraAxes] = 3;
  sets[BodyAxes] = mDown[X]; setSizes[BodyAxes] = 3;
  sets[OtherAxes] = otherAxis[X]; setSizes[OtherAxes] = 1;

  // this is a pure guess to make sure there is something in there. I don't really understand
  // how these things are managed, except that it gives me the willies that we are dealing with
  // these address pointers here... Yow.
  sets[NoAxes] = otherAxis[X]; setSizes[NoAxes] = -1;
}

#if 0
// Set the center and size of the controller
void
ArcBall::Place( const Quat& cnt, const double rad )
{
  center = cnt;
  radius = rad;
}
#endif

// Incorporate new mouse position
void
ArcBall::Mouse( const float x, const float y )
{
  vNow = Quat( x, y, 0 );
}

// Choose a constraint set, or none
void
ArcBall::UseSet(AxisSet aSet)
{
  if(!dragging) axisSet = aSet;
}

// Begin drawing arc for all drags combined
void
ArcBall::ShowResult( void )
{
  showResult = TRUE;
}

// Stop drawing arc for all drags combined
void
ArcBall::HideResult( void )
{
  showResult = FALSE;
}

// Convert window coordinates to sphere coordinates
ArcBall::Quat
ArcBall::MouseOnSphere(const Quat& mouse) const
{
  Quat ballMouse( float( (mouse.fx - center.fx) / radius ),
                  float( (mouse.fy - center.fy) / radius ),
		  0, 0 );
  double mag = ballMouse.fx*ballMouse.fx + ballMouse.fy*ballMouse.fy;
  if(mag > 1.0){
    float scale = float(1.0/sqrt(mag));
    ballMouse.fx *= scale; ballMouse.fy *= scale;
  }else{
    ballMouse.fz = float(sqrt(1 - mag));
  }

  return ballMouse;
}

// Force sphere point to be perpendicular to axis
ArcBall::Quat
ArcBall::Quat::ConstrainToAxis(const ArcBall::Quat& loose, const ArcBall::Quat& ax)
{
  Quat onPlane = loose - ax * ax.Dot( loose );
  float norm = onPlane.Norm();
  if(norm > 0){
    if(onPlane.fz < 0) onPlane = -onPlane;
    return onPlane * float(1/sqrt(norm));
  }else{
    return fabsf( ax.fz - 1 ) < 1.e-6f ? Quat(1,0,0) : Quat(-ax.fy, ax.fx, 0).Unit();
  }
}

// Find the index of nearest arc of axis set
int
ArcBall::Quat::NearestConstraintAxis(const ArcBall::Quat& loose,
				     const ArcBall::Quat *axes,
				     const int nAxes)
{
  float max = -1;
  int nearest = 0;

  for( int i=0; i<nAxes; i++){
    Quat onPlane = ConstrainToAxis(loose, axes[i]);
    float dot = onPlane.Dot( loose );
    if(dot>max){ max = dot; nearest = i; }
  }

  return nearest;
}

// Convert a unit quaternion to two points on unit sphere
void
ArcBall::Quat::BallPoints(const ArcBall::Quat& q,
			  ArcBall::Quat& arcFrom,
			  ArcBall::Quat& arcTo)
{
  double s = sqrt(q.fx*q.fx + q.fy*q.fy);
  if( zero( float( s ) ) ) arcFrom = Quat(0,1,0);
  else arcFrom = Quat( float(-q.fy/s), float(q.fx/s), 0);
  
  arcTo.fx = q.fw*arcFrom.fx - q.fz*arcFrom.fy;
  arcTo.fy = q.fw*arcFrom.fy + q.fz*arcFrom.fx;
  arcTo.fz = q.fx*arcFrom.fy - q.fy*arcFrom.fx;
  if(q.fw < 0) arcFrom = Quat(-arcFrom.fx, -arcFrom.fy, 0);
}


// Using vDown, vNow, dragging, and axisSet, compute rotation etc
void 
ArcBall::Update( void )
{
  int setSize = setSizes[axisSet];
  Quat *set = (Quat *)(sets[axisSet]);
  vFrom = MouseOnSphere(vDown);
  vTo = MouseOnSphere(vNow);
  if(dragging){
    if(axisSet!=NoAxes){
      vFrom = Quat::ConstrainToAxis(vFrom, set[axisIndex]);
      vTo = Quat::ConstrainToAxis(vTo, set[axisIndex]);
    }
    qDrag = Quat(vFrom, vTo);
    qNow = qDrag * qDown;
  }else{
    if(axisSet!=NoAxes) axisIndex = Quat::NearestConstraintAxis(vTo, set, setSize);
  }
  Quat::BallPoints(qDown, vrFrom, vrTo);
  Copy( mNow, qNow.Conj() ); // Gives transpose for GL
}

// Return rotation matrix defined by controller use
void 
ArcBall::Value(float m[4][4])
{
  Copy( m, mNow );
}

// Return rotation matrix defined by controller use
void 
ArcBall::Set(float m[4][4])
{
  Copy( mNow, m );
}

// Begin drag sequence
void
ArcBall::BeginDrag( void )
{
  dragging = TRUE; vDown = vNow;
}

void
ArcBall::BeginTrans( void )
{
  tDown = vNow;
}

XVecf
ArcBall::Trans( void )
{
  return XVecf( vNow.fx - tDown.fx, vNow.fy - tDown.fy, 0 );
}

// Stop drag sequence
void
ArcBall::EndDrag( void )
{
 	dragging = FALSE; qDown = qNow; Copy( mDown, mNow );
}

// Draw the controller with all its arcs
void
ArcBall::Draw(float aspect)
{
  glDisable(GL_LIGHTING);
  glDepthFunc(GL_ALWAYS);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  if( aspect > 1 ){
    gluOrtho2D(-aspect/radius, aspect/radius, -1/radius, 1/radius);
  }else{
    gluOrtho2D(-1/radius, 1/radius, -1/aspect/radius, 1/aspect/radius);
  }
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  DrawOuterRing();
  // DrawResultArc();
  DrawConstraints();
  DrawDragArc();
  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );

  glDepthFunc(GL_LESS);
  glEnable(GL_LIGHTING);
}

void
ArcBall::Circ( void )
{
  ArcBall::Quat p( 1, 0, 0 ), m( 0, 1, 0 );
  p.DrawAnyArc(m); m.DrawAnyArc(-p);
  (-p).DrawAnyArc(-m); (-m).DrawAnyArc(p);
}

// Draw an arc defined by its ends
void
ArcBall::Quat::DrawAnyArc( const Quat& vTo ) const
{
  Quat pts[NSEGS+1];
  pts[0] = *this; pts[1] = pts[NSEGS] = vTo;
  for( int i=0; i<LG_NSEGS; i++ ) pts[1] = pts[0].Bisect(pts[1]);

  float dot = 2.f*pts[0].Dot(pts[1]);
  for( i=2; i<NSEGS; i++) pts[i] = pts[i-1]*dot - pts[i-2];

  glBegin(GL_LINES);
  for( i=0; i<=NSEGS; i++) glVertex3fv((float *)&pts[i]);
  glEnd();
}

// Draw the arc of a semi-circle defined by its axis
void
ArcBall::Quat::DrawHalfArc( void ) const
{
  Quat p = fz != 1.f ? Quat( fy, -fx, 0 ).Unit() : Quat( 0, 1, 0 );
  Quat m = p.Cross(*this);
  p.DrawAnyArc(m); m.DrawAnyArc(-p);
}

// Draw all constraint arcs
void
ArcBall::DrawConstraints( void ) const
{
  if(axisSet==NoAxes) return;

  // what an incredible mess... (not my responsibility PS)
  int setSize = setSizes[axisSet];
  ConstraintSet set = sets[axisSet];
  for( int axisI=0; axisI<setSize; axisI++ ){
    if(axisIndex!=axisI){
      if(dragging) continue;
      FARCOLOR();
    }else NEARCOLOR();

    // would you believe this. Gross.
    Quat axis = *(Quat *)&set[4*axisI];
    if( zero( axis.fz-1 ) ) Circ();
    else axis.DrawHalfArc();
  }
}

void
ArcBall::DrawOuterRing( void ) const
{
  if( axisSet == BodyAxes ){ RIMCOLOR(); Circ(); }
}

// Draw "rubber band" arc during dragging
void
ArcBall::DrawDragArc( void ) const
{
  if(dragging){ DRAGCOLOR();
    vFrom.DrawAnyArc(vTo); }
}

// Draw arc for result of all drags
void
ArcBall::DrawResultArc( void ) const
{
  if(showResult){ RESCOLOR(); vrFrom.DrawAnyArc(vrTo); }
}
