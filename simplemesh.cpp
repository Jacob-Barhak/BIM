#include "stdafx.h"
#include "simplemesh.h"

void SimpleMeshT::Smooth() {
  for(SimpleMeshT::VertexCt::iterator vi=m_vs.begin(); vi!=m_vs.end(); ++vi) {
    if((*vi)->Boundary())
      continue;
    SimpleMeshT::UmbrellaIt ui(*vi);
    SimpleDEdgeT de;
    XVecf pt(0.0f);
    int count = 0;
    while(de=ui.Next()) {
      pt += de.Dest()->Pos();
      ++count;
    }
    pt /= float(count);
    (*vi)->Pos() = 0.95f*(*vi)->Pos() + 0.05f*pt;
  }
}

void SimpleMeshT::InitBBox() {
  m_bbmin = XVecf(1e30);
  m_bbmax = XVecf(-1e30);
  for(VertexCt::iterator vi=m_vs.begin(); vi!=m_vs.end(); ++vi) {
    m_bbmin = m_bbmin.Min((*vi)->Pos());
    m_bbmax = m_bbmax.Max((*vi)->Pos());
  }
}

void SimpleMeshT::InitFaces() {

  FaceCt::iterator fi;
  for(fi=m_fs.begin(); fi!=m_fs.end(); ++fi) {
    (*fi)->ComputeNormal();
  }

}

void SimpleMeshT::ComputeVertexNormals() {
  SimpleMeshT::VertexCt::iterator vi;
  for(vi=m_vs.begin(); vi!=m_vs.end(); ++vi) {
    (*vi)->Normal() = XVecf(0.0f);
  }

  SimpleMeshT::FaceCt::iterator fi;
  for(fi=m_fs.begin(); fi!=m_fs.end(); ++fi) {
    float len2[3];
    len2[0] = (*fi)->Vertex(0)->Pos().dist_squared((*fi)->Vertex(1)->Pos());
    len2[1] = (*fi)->Vertex(1)->Pos().dist_squared((*fi)->Vertex(2)->Pos());
    len2[2] = (*fi)->Vertex(2)->Pos().dist_squared((*fi)->Vertex(0)->Pos());

    (*fi)->Vertex(0)->Normal() += ((*fi)->Area() / ( len2[0] * len2[2] ))*(*fi)->Normal();
    (*fi)->Vertex(1)->Normal() += ((*fi)->Area() / ( len2[1] * len2[0] ))*(*fi)->Normal();
    (*fi)->Vertex(2)->Normal() += ((*fi)->Area() / ( len2[2] * len2[1] ))*(*fi)->Normal();
  }

  for(vi=m_vs.begin(); vi!=m_vs.end(); ++vi) {
    (*vi)->Normal().normalize();
  }
}

void SimpleFaceT::ComputeNormal() {
  if(!DEdge())
    return;

  XVecf t1 = DEdge().Dest()->Pos() - DEdge().Org()->Pos();
  XVecf t2 = DEdge().Enext().Dest()->Pos() - DEdge().Org()->Pos();

  m_normal = t1.cross(t2);
  m_normal.normalize();

  m_area = ::CalcArea(DEdge().Org()->Pos(), DEdge().Dest()->Pos(), DEdge().Enext().Dest()->Pos());
}

