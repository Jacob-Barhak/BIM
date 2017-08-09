#pragma once

#include <vector>

#include <map>

namespace bm {

  template<class IMeshT>
  class MeshBuilderT {

    typedef typename IMeshT::VertexT IVertexT;
    typedef typename IMeshT::EdgeT IEdgeT;
    typedef typename IMeshT::FaceT IFaceT;

    typedef typename IMeshT::DEdgeT IDEdgeT;

    typedef std::map< std::pair<IVertexT*, IVertexT*>, IEdgeT*> IVVEMapT;
  
  public:

    MeshBuilderT(IMeshT* _mesh) : m_mesh(_mesh) {}

    IVertexT* AddVertex(const XVecf& pos) {
      IVertexT* v = new IVertexT;
      v->Pos() = pos;
      m_verts.push_back(v);
      m_mesh->AddVertex(v);
      return v;
    }

    IDEdgeT AddEdge(IVertexT* v0, IVertexT* v1) {
      IVertexT* vmin;
      IVertexT* vmax;

      int dir;
      if(v0<v1) {
        dir = 0;
        vmin = v0;
        vmax = v1;
      } else {
        dir = 1;
        vmin = v1;
        vmax = v0;
      }

      std::pair<typename IVVEMapT::iterator, bool> mi_b = 
        m_vv2e.insert(make_pair(make_pair(__min(v0,v1), __max(v0,v1)), (IEdgeT*)0));

      if(mi_b.second) {
        (*mi_b.first).second = new IEdgeT;
        assert((*mi_b.first).second);
        (*mi_b.first).second->Vertex(0) = vmin;
        (*mi_b.first).second->Vertex(1) = vmax;
        m_mesh->AddEdge((*mi_b.first).second);
      }

      return IDEdgeT((*mi_b.first).second, dir);
    }

    IDEdgeT AddTriangle(int* tri) {
      
      IFaceT* f = new IFaceT;
      assert(f);

      m_mesh->AddFace(f);

      IDEdgeT de01 = AddEdge(m_verts[tri[0]], m_verts[tri[1]]);
      IDEdgeT de12 = AddEdge(m_verts[tri[1]], m_verts[tri[2]]);
      IDEdgeT de20 = AddEdge(m_verts[tri[2]], m_verts[tri[0]]);

      de01.SetEnext(de12);
      de12.SetEnext(de20);
      de20.SetEnext(de01);

      de01.SetFace(f);
      de12.SetFace(f);
      de20.SetFace(f);

      f->AssignDEdge(de01);

      return de01;
    }
    void Finalize() {
#ifdef _REMOVAL_SUPPORTED_BMESH
      cerr << "removing unused verts..." << endl;
      m_mesh->RemoveUnusedVerts();
#endif
      m_mesh->AssignVertEdges();
      m_mesh->InitFaces();
      m_mesh->ComputeVertexNormals();
      m_mesh->InitBBox();

      cerr << "done." << endl;
    }
  private:
    IVVEMapT m_vv2e;
    std::vector<IVertexT*> m_verts;
    IMeshT* m_mesh;
  };

}

