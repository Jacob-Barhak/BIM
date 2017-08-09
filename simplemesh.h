#pragma once 

#include "bmesh.h"
#include "flagged.h"

#include "xvec.h"
#include "xvecdim.h"
#include "geom.h"

class SimpleEdgeT;
class SimpleVertexT;
class SimpleFaceT;

class SimpleMeshT : public bm::MeshT<SimpleFaceT, SimpleEdgeT, SimpleVertexT> {
public:
  SimpleMeshT() {}
  ~SimpleMeshT() {
  }

  void InitBBox();
  void ComputeVertexNormals();
  void Smooth();
  void InitFaces();


  float BBDiag() const {
    return m_bbmin.dist(m_bbmax);
  }

  const XVecf& BBMin() const {
    return m_bbmin;
  }

  const XVecf& BBMax() const {
    return m_bbmax;
  }

private:
  XVecf m_bbmin, m_bbmax;
};


class SimpleVertexT : public bm::VertexT<SimpleFaceT, SimpleEdgeT, SimpleVertexT>, 
                 public bm::StoredT<bm::MeshT<SimpleFaceT, SimpleEdgeT, SimpleVertexT>::VertexCt> {
public:
  SimpleVertexT() {}

  XVecf& Pos() {
    return m_pos;
  }

  const XVecf& Pos() const {
    return m_pos;
  }

  XVecf& Normal() {
    return m_normal;
  }
  const XVecf& Normal() const {
    return m_normal;
  }

  float Area();

  XVecf m_pos, m_normal;
};

typedef bm::DEdgeT<SimpleFaceT, SimpleEdgeT, SimpleVertexT> SimpleDEdgeT;

class SimpleFaceT : public bm::FaceT<SimpleFaceT, SimpleEdgeT, SimpleVertexT>,
  public bm::StoredT<SimpleMeshT::FaceCt> {
public:
  const XVecf& Normal() const {
    return m_normal;
  }
  void Normal(const XVecf& n) {
    m_normal = n;
  }
  void ComputeNormal();
  float Area() const {
    return m_area;
  }
private:
  XVecf m_normal;
  float m_area;
};

class SimpleEdgeT 
  : public bm::EdgeT<SimpleFaceT, SimpleEdgeT, SimpleVertexT>, 
    public bm::StoredT<bm::MeshT<SimpleFaceT, SimpleEdgeT, SimpleVertexT>::EdgeCt> {
};

