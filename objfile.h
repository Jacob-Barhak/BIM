#pragma once

#include <fstream>
#include <stdio.h>
#include <string>

#include "meshbuilder.h"

struct OBJFileT {
  template<class IMeshT>
  static bool Read(const char* filename, bm::MeshBuilderT<IMeshT>* builder) {
    std::ifstream ist(filename);    
    std::string line;
    int countv = 0;
    while(ist.good()) {
      std::getline(ist, line);
      if(line.length()<2)
        continue;
      if(line[0]=='v') {
        if(line[1]=='n') {
          // read normal here
        } else {
          // read position here
          XVecf pos;
          int nr = sscanf(line.c_str(), "v %f %f %f", &pos.x(), &pos.y(), &pos.z());
          if(nr!=3) {
            std::cerr << "cannot read position: " << line << std::endl;
            continue;
          }
          IMeshT::VertexT* v = builder->AddVertex(pos);
          //v->m_index = countv++;
        }
      } else if(line[0]=='f') {
        // read face here. only triangular
        XVeci tri;
        int nr = sscanf(line.c_str(), "f %d %d %d", &tri.x(), &tri.y(), &tri.z());
        if(nr!=3) {
          std::cerr << "cannot read face indices: " << line << std::endl;
          continue;
        }
        tri -= 1;
        builder->AddTriangle(tri);
      }
    }
    builder->Finalize();
    return true;
  }

  template<class IMeshT>
  static bool Save(const char* filename, IMeshT* m) {
    if(m==0)
      return false;

    std::ofstream ost(filename);    
    
    if(!ost.good())
      return false;

    int count = 1;
    for(IMeshT::VertexCt::const_iterator vi=m->Verts().begin(); vi!=m->Verts().end(); ++vi) {
      ost << "v " << (*vi)->m_pos << std::endl;
      (*vi)->m_aux = count++;
    }

    for(IMeshT::FaceCt::const_iterator fi=m->Faces().begin(); fi!=m->Faces().end(); ++fi) {
      ost << "f " << (*fi)->Vertex(0)->m_aux << " " 
        << (*fi)->Vertex(1)->m_aux << " " 
        << (*fi)->Vertex(2)->m_aux << " " << std::endl;
    }

    return true;
  }
};
