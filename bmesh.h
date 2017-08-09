#pragma once

#ifdef _REMOVAL_SUPPORTED_BMESH
#include <list>
#else
#include <vector>
#endif

#include <assert.h>


#define _UMIT_CHECK

#ifdef _UMIT_CHECK
#include <hash_set>
#endif

#include "xvec.h"

namespace bm {

  /// The directed edge class.
  /// A directed edge class stores an edge pointer and an orientation bit 
  /// (0 - forward, 1 - backward). 
  template<class IFaceT, class IEdgeT, class IVertexT> 
  class DEdgeT {
    enum { DIR_MASK = 0x00000001, PE_MASK = 0xFFFFFFFC };
  public:

    /// Default constructor. 
    /** Construct a null directed edge.
    */
    DEdgeT() : m_p(0) {
    }
    
    /// A copy constructor.
    DEdgeT(const DEdgeT& _de) : m_p(_de.m_p) {
    }

    /// Assignment operator.
    DEdgeT& operator=(const DEdgeT& _de) {
      m_p = _de.m_p;
      return *this;
    }

    /// DEdge constructor. Takes an edge pointer and orientation. 
    /// Given an edge connecting vertices v0 and v1, and direction equal to 0, 
    /// this constructor will construct a directed edge from v0 to v1. 
    /// If the direction is 1, then a directed edge from v1 to v0 is constructed. 
    DEdgeT(IEdgeT* _e, int _dir) {
      m_p = (unsigned(_e) & PE_MASK) | (unsigned(_dir) & DIR_MASK);
    }
    
    /// Returns the edge pointer of this directed edge.
    IEdgeT* Edge() const {
      return (IEdgeT*)(m_p & PE_MASK);
    }
    
    /// Returns the orientation (0-forward, 1-backward) of this directed edge.
    int Dir() const {
      return (int)(m_p & DIR_MASK);
    }
    
    /// Changes the direction of this directed edge to its opposite.
    /// For non-null directed edge de, de.Flip().Flip() = de.
    /// If de12 = [v1,v2], then de12.Flip() = [v2,v1]
    /// This function returns a directed edge with the same edge pointer as the
    /// original, but will negate its direction bit.
    DEdgeT Flip() const {
      DEdgeT de(*this);
      de.m_p ^= DIR_MASK;
      return de;
    }

    /// Returns the next directed edge with respect to the associated face.
    /// For a boundary directed edge this function will return a null directed
    /// edge, and for a non-boundary directed edge the following considerations hold:
    /// The mesh is assumed to be orientable, and all the faces given in 
    /// a CCW orientation. Then a directed edge is associated to a face for which
    /// its vertex order is CCW. For instance, if face FA = [v0, v1, v2]
    /// and FB = [v2, v1, v3], then the directed edge de12 = [v1,v2] is associated 
    /// to the face FA, and de21 = [v2,v1] is associated to the face FB.
    /// Therefore, de12.Enext() will return the directed edge [v2,v0]; and 
    /// de21.Enext() will return the directed edge [v1,v3].
    /// The actual internal representation of a directed edge will store the 
    /// edge pointer and the direction bit, rather than a vertex pair however.
    DEdgeT Enext() const {
      return (*this ? Edge()->DEdge(Dir()) : DEdgeT());
    }

    /// Sets the neigborhood for the associated directed edge.
    /// Use this function when constructing mesh connectivity.
    void SetEnext(const DEdgeT& de) const {
      Edge()->DEdge(Dir()) = de;
    }

    /// Returns the result of applying the Enext operator k times.
    DEdgeT Enext(int k) const {
      k %= 3;
      switch(k) {
      case 0:
        return *this;
      case 1:
        return Enext();
      case 2:
        return Enext2();
      }
      assert(0);
      return DEdgeT();
    }

    /// Returns the origin vertex of this directed edge.
    IVertexT* Org() const {
      return (*this ? Edge()->Vertex(Dir()) : 0);
    }

    /// Sets the origin vertex for this directed edge.
    void SetOrg(IVertexT* v) const {
      Edge()->Vertex(Dir()) = v;
    }

    /// Returns the destination vertex of this directed edge.
    /// Note that de.Flip().Org() = de.Dest()
    IVertexT* Dest() const {
      return (*this ? Edge()->Vertex(1-Dir()) : 0 );
    }

    /// Returns the result of applying Enext() operator twice. 
    /// Will crash for a boundary directed edge.
    DEdgeT Enext2() const {
      return Enext().Enext();
    }

    /// Returns the pointer to the face associated with this directed edge.
    /// For a boundary directed edge returns zero.
    /// The mesh is assumed to be orientable, and all the faces given in 
    /// a CCW orientation. Then a directed edge is associated to a face for which
    /// its vertex order is CCW. For instance, if face FA = [v0, v1, v2]
    /// and FB = [v2, v1, v3], then the directed edge de12 = [v1,v2] is associated 
    /// to the face FA, and de21 = [v2,v1] is associated to the face FB.
    IFaceT* Face() const {
      return (*this ? Edge()->Face(Dir()) : 0 );
    }

    /// Sets the face associated with a non-null directed edge.
    void SetFace(IFaceT* f) const {
      if(*this)
        Edge()->Face(Dir()) = f;
    }

    /// Returns false for a null directed edge, and true otherwise.
    operator bool() const {
      return m_p!=0;
    }

    /// Returns true for a null directed edge, and false otherwise.
    bool operator!() const {
      return m_p==0;
    }

    /// Returns true if this directed edge is the same as de.
    bool operator==(const DEdgeT& de) const {
      return m_p==de.m_p;
    }

    /// Returns true if this directed edge is not the same as de.
    bool operator!=(const DEdgeT& de) const {
      return m_p!=de.m_p;
    }
    
    /// Simple comparison operation useful for hashing purposes.
    bool operator<(const DEdgeT& de) const {
      return m_p<de.m_p;
    }

    /// Returns true for a boundary directed edge. 
    /// For an edge on the boundary of the mesh, one of the 
    /// associated directed edges will be a boundary edge since it
    /// will not have any associated face, compatible with its 
    /// vertex order.
    bool Boundary() const {
      return !Enext();
    }

    /// Returns the length of the edge
    /// This function will only work if the Pos() member of the vertex
    /// class returns the vertex position.
    float Length() const {
      return Org()->Pos().dist(Dest()->Pos());
    }

    size_t hash_value() const {
      return std::hash_value(m_p);
    }

    /// Computes the valence of the origin vertex.
    int Valence() const;

    /// Computes the index of this directed edge within the face associated with it.
    /// Returns an index from 0 to 2 for a non-boundary directed edge. 
    /// Otherwise returns -1. 
    /// For instance, for a face F=[v0,v1,v2] we have [v0,v1].SideIndex()==0, [v1,v2].SideIndex()==1,
    /// and [v2,v0].SideIndex()==2
    int SideIndex() const;
  private:
    unsigned m_p;
  };

  /// Outputs a directed edge info on the output stream.
  template<class IFaceT, class IEdgeT, class IVertexT> 
  inline std::ostream& operator<<(std::ostream& os, const DEdgeT<IFaceT, IEdgeT, IVertexT>& de) {
    os << "(" << de.Edge() << ":" << de.Dir() << "|" << de.Org() << "->" << de.Dest() << ")";
    return os;
  }

  template<class IFaceT, class IEdgeT, class IVertexT> 
  inline size_t hash_value(const DEdgeT<IFaceT, IEdgeT, IVertexT>& de)
	{	
    return de.hash_value();
	}


  /// The umbrella iterator.
  /// The umbrella iterator returns directed edges with the same origin
  /// in the CW order. If the vertex is on the boundary, the first returned 
  /// directed edge is the boundary directed edge (the one which is most CCW of all
  /// the directed edges with the same origin.) -- this will only hold if 
  /// the umbrella iterator is constructed with a vertex argument.
  /// The iterator will start returning null directed edges once
  /// all the edges are counted, or once it hits the boundary.
  template<class IFaceT, class IEdgeT, class IVertexT>
  class UmIt {
    typedef DEdgeT<IFaceT, IEdgeT, IVertexT> IDEdgeT;
#ifdef _UMIT_CHECK
    typedef std::hash_set<IDEdgeT> IDESetT;
#endif
  public:
    /// Constructs an umbrella iterator for a given vertex.
    UmIt(const IVertexT* v) : m_de(v->DEdge()), m_de0(v->DEdge())
    {
      m_bnd = m_de.Boundary();
    }

    /// Constructs an umbrella iterator for a given directed edge.
    /// This iterator will return directed edges with the same origin
    /// as de, in the CW order until it hits the boundary or encounters de again.
    UmIt(const IDEdgeT& de) : m_de(de), m_de0(de) {
      m_bnd = m_de.Boundary();
    }

    /// Resets the umbrella iterator, similar to calling the constructor from a vertex.
    void Reset(const IVertexT* v) {
      m_de = v->DEdge();
      m_de0 = v->DEdge();
      m_bnd = m_de.Boundary();
      m_visited.clear();
    }

    /// Returns the current directed edge and advances the iterator clockwise, or null directed edge.
    IDEdgeT Next() {
      if(!m_de)
        return m_de;
      
      IDEdgeT der = m_de;
      m_de = m_de.Flip().Enext();

      if(m_de==m_de0)
        m_de = IDEdgeT();
      
#ifdef _UMIT_CHECK
      std::pair<typename IDESetT::iterator, bool> ib = m_visited.insert(der);
      if(!ib.second) {
        m_de = IDEdgeT();
        return m_de;
      }
#endif

      return der;
    }

    /// Checks if a boundary was hit during the current traversal.
    bool Boundary() const {
      return m_bnd;
    }

    /// Returns the current directed edge without advancing.
    IDEdgeT Current() const {
      return m_de;
    }
  private:
    IDEdgeT m_de, m_de0;
    bool m_bnd;
#ifdef _UMIT_CHECK
    IDESetT m_visited;
#endif
  };

  template<class IFaceT, class IEdgeT, class IVertexT> 
  int DEdgeT<IFaceT, IEdgeT, IVertexT>::Valence() const {
    UmIt<IFaceT, IEdgeT, IVertexT> ui(*this);
    DEdgeT<IFaceT, IEdgeT, IVertexT> de;
    int count = 0;
    while(de=ui.Next())
      ++count;
    return count;
  }

  /// The vertex class.
  /// The only connectivity information stored in the vertex is the 
  /// associated directed edge whose origin is the vertex itself.
  /// For an inside (non-boundary) vertex, this directed edge can
  /// be chosen arbitrarily from the set of directed edges emanating
  /// from the vertex. For a boundary vertex, the stored directed edge
  /// must be the only boundary directed edge emanating from the vertex.
  template<class IFaceT, class IEdgeT, class IVertexT> 
  class VertexT {
    typedef DEdgeT<IFaceT, IEdgeT, IVertexT> IDEdgeT;
    typedef UmIt<IFaceT, IEdgeT, IVertexT> IUmIt;
  public:
    /// Default constructor.
    VertexT() {}

    /// Returns the associated directed edge whose origin is this vertex.
    IDEdgeT DEdge() const {
      return m_de;
    }

    /// Returns the reference to the associated directed edge.
    IDEdgeT& DEdge() {
      return m_de;
    }

    /// Returns true is the vertex is on the boundary.
    bool Boundary() const {
      return m_de.Boundary();
    }

    /// Returns the valence of the vertex, that is, the number of adjacent edges.
    int Valence() const { // number of adjacent edges
      int val = 0;
      IUmIt ui((IVertexT*)this);
      IDEdgeT de;
      while(de=ui.Next()) {
        ++val;
      }
      return val;
    }

    /// Returns the number of adjacent faces. 
    /// For an inside vertex, the Valence and FValence are the same.
    /// For a boundary vertex, FValence = Valence-1.
    int FValence() const { // number of adjacent polygons
      int val = 0;
      IUmIt ui((IVertexT*)this);
      IDEdgeT de;
      while(de=ui.Next()) {
        ++val;
      }
      if(ui.Boundary())
        return val-1;
      else
        return val;
    }

    /// Returns the directed edge connecting this vertex to the vertex v, or null directed edge if that is not possible.
    IDEdgeT DEdgeTo(IVertexT* v) {
      IUmIt ui((IVertexT*)this);
      IDEdgeT de;
      while(de=ui.Next()) {
        if(de.Dest()==v)
          return de;
      }
      return de;
    }

    /// Assigns the associated directed edge with a proper behaviour on the boundary.
    /// The origin of de must be this vertex.
    void AssignDEdge(const IDEdgeT& de0) {
      IDEdgeT de = de0;
      assert(de.Org()==this);
      while(!de.Boundary()) {
        assert(de.Org()==this);
        de = de.Enext2().Flip();
        if(de==de0)
          break;
      }
      m_de = de;
    }
  private:
    IDEdgeT m_de;
  };


  /// The face class.
  /// The only connectivity information stored in the face [v0,v1,v2] is
  /// the directed edge [v0,v1]. The vertices of the face are found by
  /// using operations on this stored directed edge.
  template<class IFaceT, class IEdgeT, class IVertexT> 
  class FaceT {
    typedef DEdgeT<IFaceT, IEdgeT, IVertexT> IDEdgeT;
    typedef UmIt<IFaceT, IEdgeT, IVertexT> IUmIt;
  public:
    /// Default constructor
    FaceT() {}

    /// Returns the directed edge connecting Vertex(0) and Vertex(1)
    IDEdgeT DEdge() const {
      return m_de;
    }

    /// Returns the reference to the stored directed edge
    IDEdgeT& DEdge() {
      return m_de;
    }

    /// Assigns the associated directed edge
    void AssignDEdge(const IDEdgeT& de0) {
      m_de = de0;
    }

    /// Returns the k-th vertex of the face.
    IVertexT* Vertex(int k) {
      switch(k) {
      case 0:
        return m_de.Org();
      case 1:
        return m_de.Dest();
      case 2:
        return m_de.Enext().Dest();
      default:
        return 0;
      }
    }
  private:
    IDEdgeT m_de;
  };

  template<class IFaceT, class IEdgeT, class IVertexT> 
  int DEdgeT<IFaceT, IEdgeT, IVertexT>::SideIndex() const {
    IFaceT* f = Face();
    if(f->DEdge()==*this) {
      return 0;
    } 
    DEdgeT<IFaceT, IEdgeT, IVertexT> den = Enext();
    if(f->DEdge()==den) {
      return 2;
    } 
    if(f->DEdge()==den.Enext()) {
      return 1;
    } 
    return -1;
  }

  /// The edge class.
  /// Most of the mesh connectivity information is stored in this class.
  /// In particular, the neighboring edges and adjacent vertices are stored within
  /// this class. 
  template<class IFaceT, class IEdgeT, class IVertexT> 
  class EdgeT {
    typedef DEdgeT<IFaceT, IEdgeT, IVertexT> IDEdgeT;
  public:
    /// Default constructor.
    /// Creates an isolated edge.
    EdgeT() {
      m_vs[0] = m_vs[1] = 0;
      m_fs[0] = m_fs[1] = 0;
    }

    /// Returns a neighboring directed edge in the _dir direction.
    /// Be careful! This does not return DEdgeT(this, _dir),
    /// rather it returns DEdgeT(this, _dir).Enext(). This function
    /// should be renamed, to something like NeiDEdge().
    /// The returned directed edge will be null if the edge is on the 
    /// boundary and 
    IDEdgeT DEdge(int _dir) const {
      return m_des[_dir];
    }
    
    /// Returns a reference to the neighboring directed edge 
    IDEdgeT& DEdge(int _dir) {
      return m_des[_dir];
    }

    /// Returns a pointer to the adjacent vertex in the _dir direction.
    /// DEdge(0).Org() == Vertex(1), and DEdge(1).Org() == Vertex(0)
    IVertexT* Vertex(int _dir) const {
      return m_vs[_dir];
    }

    /// Returns a reference to the pointer to the adjacent vertex in the _dir direction.
    IVertexT*& Vertex(int _dir) {
      return m_vs[_dir];
    }

    /// Returns a pointer to the adjacent face in the _dir direction.
    /// DEdge(this, 0).Face() == Face(0), and DEdge(this, 1).Face() == Face(1)
    IFaceT* Face(int _dir) const {
      return m_fs[_dir];
    }

    /// Returns a reference to the pointer to the adjacent face in the _dir direction.
    IFaceT*& Face(int _dir) {
      return m_fs[_dir];
    }

    /// Returns if the edge is adjacent to the boundary.
    bool Boundary() const {
      return (!m_des[0]) || (!m_des[1]);
    }

    /// Returns the index of the edge in the face Face(_dir)
    int FaceIndex(int _dir) {
      return IDEdgeT((IEdgeT*)this, _dir).SideIndex();
    }

  private:
    IDEdgeT m_des[2];
    IVertexT* m_vs[2];
    IFaceT* m_fs[2];
  };

  /// Mesh class.
  /// If _REMOVAL_SUPPORTED_BMESH is defined,
  /// the primitive classes should have the StoredT interface,
  /// so that each primitive can be removed from the mesh in O(1) time.
  /// If _REMOVAL_SUPPORTED_BMESH is not defined, the removal is 
  /// not supported.
  template<class IFaceT, class IEdgeT, class IVertexT> 
  class MeshT {
  protected:
    typedef DEdgeT<IFaceT, IEdgeT, IVertexT> IDEdgeT;
  public:
    /// The directed edge class used by the mesh.
    typedef IDEdgeT DEdgeT;

    /// The edge class used by the mesh.
    typedef IEdgeT EdgeT;

    /// The vertex class used by the mesh.
    typedef IVertexT VertexT;

    /// The face class used by the mesh.
    typedef IFaceT FaceT;

    /// The umbrella iterator class used by the mesh.
    typedef UmIt<IFaceT, IEdgeT, IVertexT> UmbrellaIt;

  public:
#ifdef _REMOVAL_SUPPORTED_BMESH
    /// The face container class used by the mesh.
    typedef std::list<IFaceT*> FaceCt;
    /// The edge container class used by the mesh.
    typedef std::list<IEdgeT*> EdgeCt;
    /// The vertex container class used by the mesh.
    typedef std::list<IVertexT*> VertexCt;
#else
    /// The face container class used by the mesh.
    typedef std::vector<IFaceT*> FaceCt;
    /// The edge container class used by the mesh.
    typedef std::vector<IEdgeT*> EdgeCt;
    /// The vertex container class used by the mesh.
    typedef std::vector<IVertexT*> VertexCt;
#endif
    /// Face iterator type
    typedef typename FaceCt::iterator FaceIt;
    /// Edge iterator type
    typedef typename EdgeCt::iterator EdgeIt;
    /// Vertex iterator type
    typedef typename VertexCt::iterator VertexIt;
  public:
    /// Default constructor. Creates an empty mesh.
    MeshT() {}

    /// Destructor. Deallocates all the primitives: faces, edges, and vertices.
    ~MeshT() {
      for(FaceCt::iterator fi=m_fs.begin(); fi!=m_fs.end(); ++fi)
        delete *fi;
      for(EdgeCt::iterator ei=m_es.begin(); ei!=m_es.end(); ++ei)
        delete *ei;
      for(VertexCt::iterator vi=m_vs.begin(); vi!=m_vs.end(); ++vi)
        delete *vi;
    }

    /// Returns a const reference to the face container.
    const FaceCt& Faces() const {
      return m_fs;
    }

    /// Returns the begin iterator for face container.
    FaceIt FacesBegin() {
      return m_fs.begin();
    }

    /// Returns the end iterator for face container.
    FaceIt FacesEnd() {
      return m_fs.end();
    }

    /// Returns a const reference to the edge container.
    const EdgeCt& Edges() const {
      return m_es;
    }

    /// Returns the begin iterator for edge container.
    EdgeIt EdgesBegin() {
      return m_es.begin();
    }

    /// Returns the end iterator for edge container.
    EdgeIt EdgesEnd() {
      return m_es.end();
    }

    /// Returns a const reference to the vertex container.
    const VertexCt& Verts() const {
      return m_vs;
    }

    /// Returns the begin iterator for vertex container.
    VertexIt VertsBegin() {
      return m_vs.begin();
    }

    /// Returns the end iterator for vertex container.
    VertexIt VertsEnd() {
      return m_vs.end();
    }

    /// Adds the given vertex to the mesh. 
    /// Assigns the back reference (iterator) to the vertex container.
    void AddVertex(IVertexT* v) {
      v->Iter(m_vs.insert(m_vs.end(), v));
    }

    /// Adds the given edge to the mesh. 
    /// Assigns the back reference (iterator) to the edge container.
    void AddEdge(IEdgeT* e) {
      e->Iter(m_es.insert(m_es.end(), e));
    }

    /// Adds the given face to the mesh. 
    /// Assigns the back reference (iterator) to the face container.
    void AddFace(IFaceT* f) {
      f->Iter(m_fs.insert(m_fs.end(), f));
    }

    /// A procedure that properly assigns all the DEdges for all the vertices.
    void AssignVertEdges() {
      EdgeCt::const_iterator ei;
      for(ei=Edges().begin(); ei!=Edges().end(); ++ei) {
        (*ei)->Vertex(0)->DEdge() = DEdgeT(*ei, 0);
        (*ei)->Vertex(1)->DEdge() = DEdgeT(*ei, 1);
      }
      for(ei=Edges().begin(); ei!=Edges().end(); ++ei) {
        for(int dir=0; dir<2; ++dir) {
          DEdgeT de(*ei, dir);
          if(de.Boundary()) {
            if(de.Org()->DEdge() && de.Org()->DEdge().Boundary() && de.Org()->DEdge()!=de)
              std::cerr << "nontopo" << std::endl;
            de.Org()->DEdge() = de;
          }
        }
      }
    }

#ifdef _REMOVAL_SUPPORTED_BMESH
    /// Removes the given vertex to the mesh. 
    /// Removes the vertex from the vertex container. Does not delete the vertex.
    void RemoveVertex(IVertexT* v) {
      m_vs.erase(v->Iter());
    }

    /// Removes the edge vertex to the mesh. 
    /// Removes the edge from the edge container. Does not delete the edge.
    void RemoveEdge(IEdgeT* e) {
      m_es.erase(e->Iter());
    }

    /// Removes the face vertex to the mesh. 
    /// Removes the face from the face container. Does not delete the face.
    void RemoveFace(IFaceT* f) {
      m_fs.erase(f->Iter());
    }


    /// Removes the vertices that are not used in any faces.
    void RemoveUnusedVerts() {
      VertexCt::const_iterator vi;
      for(vi=Verts().begin(); vi!=Verts().end(); ++vi) {
        (*vi)->m_aux = 0;
      }
      EdgeCt::const_iterator ei;
      for(ei=Edges().begin(); ei!=Edges().end(); ++ei) {
        (*ei)->Vertex(0)->m_aux = 1;
        (*ei)->Vertex(1)->m_aux = 1;
      }
      std::vector<IVertexT*> to_remove;
      for(vi=Verts().begin(); vi!=Verts().end(); ++vi) {
        if((*vi)->m_aux==0)
          to_remove.push_back(*vi);
      }
      int count_removed = 0;
      for(std::vector<IVertexT*>::iterator vri=to_remove.begin(); vri!=to_remove.end(); ++vri) {
        RemoveVertex(*vri);
        ++count_removed;
      }
      std::cerr << "Removed " << count_removed << " unused vertices" << std::endl;
    }

#endif

  protected:
    /// Face container.
    FaceCt m_fs;
    /// Edge container.
    EdgeCt m_es;
    /// Vertex container.
    VertexCt m_vs;  
  };

  /// StoredT class.
  /// Allows to store and retrieve the iterator into the container, where an element is stored.
  template<class ICtT>
	class StoredT {
    typedef typename ICtT::iterator iterator;
	public:
    /// Default constructor.
    StoredT() {}
    /// Destructor.
    ~StoredT() {}
    /// Returns the stored iterator within the container.
    typename ICtT::iterator Iter() const {
			return m_iter;
		}	

    /// Assigns the iterator.
    void Iter(typename ICtT::iterator _iter) {
			m_iter = _iter;
		}	
	private:
		typename ICtT::iterator m_iter;
	};

}