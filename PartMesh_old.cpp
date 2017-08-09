/************************************************

	This file contains the implementation for the
	PartMesh class.
	In this class, we have the following functions:

	default constructor
	destructor
	draw the PartMesh
	read the part mesh from file
	write the part mesh to a file

*************************************************/

#include "stdafx.h"
#include "PartMesh.h"
#include <string>
#include <iostream>
#include "meshbuilder.h"
#include "objfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// this static variable is a flag
// to determine whether we want to create
// a display list 



/***********************************************/
/*
	Default constructor
							modified by: Jun
							date: 01/26/05

*/
/***********************************************/
// Default constructor
PartMesh::PartMesh()
{
	FileName = "";
	Comments = "";
	m_mesh = 0;
	selected = false;
	m_meshReady = false;
	m_reinit = false; 
    m_edgesOn = false;
	m_edgesOnly = false;
    m_list_mesh =-1; 
    m_clip = false;
    m_smoothOn = true;
	redo = true;
}	




/***********************************************/
/*
	Destructor
							modified by: Jun
							date: 01/26/05
*/
/***********************************************/
// Destructor
PartMesh::~PartMesh()
{
	
	delete m_mesh;
}





/***********************************************/
/*
	Function to draw the mesh
	Will create a display list if needed
							modified by: Jun
							date: 01/26/05
*/
/***********************************************/
// Function to draw part 
void PartMesh::Draw()
{
	
	// Check if mesh has been loaded
	if(!m_mesh)
		return;
	
	// Check if user wants edges to be drawn
	if(m_edgesOn) {
		  glEnable( GL_POLYGON_OFFSET_LINE );
		  glEnable( GL_POLYGON_OFFSET_FILL ); 
    } else {
		  glDisable( GL_POLYGON_OFFSET_LINE );
		  glDisable( GL_POLYGON_OFFSET_FILL ); 
    }
	
	
	bool drawn = false;
	
	// if redo is true, means we need to make a new display
	// list
	if( redo ){
		
	  // check if there is a display list,
	  // if there is one, delete it first
      if( m_list_mesh != -1 ){ 
        glDeleteLists( m_list_mesh, 1 ); 
        m_list_mesh = -1; 
	  }
	  
	  // now create the display list
	  m_list_mesh = glGenLists( 1 );
      
	  // make display list
      glNewList( m_list_mesh, GL_COMPILE_AND_EXECUTE);
      glPushMatrix();

	  drawn = true;
      
      RenderMesh();
	
	  glPopMatrix();	 
	  glEndList();
     
  
      // rebuild display list
  	  redo = false;
	}
	
	// if not drawn yet, just call display list	
	if(!drawn)
		glCallList( m_list_mesh );
	
	
	return;
}




/***********************************************/
/*
	Function to read the mesh from a file
							modified by: Jun
							date: 10/14/04
*/
/***********************************************/
// Function to read the part from a file
// Reads .obj file and brings the mesh into the 
// workspace
void PartMesh::ReadPart(string BIMFile, char* buf)
{
	
	if(m_mesh) // already have a mesh, just return
	{	
		AfxMessageBox("You can only have one mesh loaded at a time!");
		return;
	}
	
	FileName = BIMFile;
	
	// load the mesh into m_mesh
	m_mesh = new SimpleMeshT;
	bm::MeshBuilderT<SimpleMeshT> builder(m_mesh);
	//OBJFileT::Read(buf, &builder);
	OBJFileT::Read(BIMFile.c_str(), &builder);

#if 0	
	bool success = ts::ReadFile(m_mesh, std::string(buf));
    if(success)
		AfxMessageBox("Successfully Loaded Part Mesh");
	m_mesh->RemoveUnusedVerts();
    m_mesh->InitFaces();
	m_mesh->ComputeVertexNormals();
	m_mesh->InitBBox();
#endif
	
	// set redo so that we create a display list for the mesh
	redo = true;
	
	// Calculate bouding box
	CalculateBoudingBox();
	


}
	


/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Function to write the part to a file
// Writes file, no manipulation of the mesh
void PartMesh::WritePart(string BIMFile)
{
}



/***********************************************/
/*
	Function to render the mesh
							modified by: Jun
							date: 01/26/05
*/
/***********************************************/
void PartMesh::RenderMesh()
{
	

#ifdef _GRAY_PALETTE
  GLfloat mat_ambient[] = { .5, .5, .5, 1 };
  GLfloat mat_diffuse[] = { .8f, .8f, .8f, 1 };
  GLfloat mat_shininess[] = { 128 };
  glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
  glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
  glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );
  
#endif
  glFrontFace(GL_CW);
  glPolygonOffset(2.0f, 10.0f);

  SimpleMeshT::FaceCt::const_iterator fi;
  SimpleDEdgeT de;

 

  {
   
	if(selected == false)
		glColor3f(0.8f, 0.2f, 0.8f);
	else if(selected == true)
		glColor4d(0.0, 1.0, 0.0, 1.0);
	
	// check if we want a the mesh to have a smooth/flat shading
	if(!m_edgesOnly)
	{
		if(!m_smoothOn) {
		glEnable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);
			for(fi=m_mesh->Faces().begin(); fi!=m_mesh->Faces().end(); ++fi) {
			de = (*fi)->DEdge();

			glNormal3fv((*fi)->Normal());

			glVertex3fv(de.Org()->Pos());
			glVertex3fv(de.Dest()->Pos());
			glVertex3fv(de.Enext().Dest()->Pos());
			}
		glEnd();
		} else {
		glBegin(GL_TRIANGLES);
			for(fi=m_mesh->Faces().begin(); fi!=m_mesh->Faces().end(); ++fi) {
			de = (*fi)->DEdge();

			glNormal3fv(de.Org()->Normal());
			glVertex3fv(de.Org()->Pos());
			glNormal3fv(de.Dest()->Normal());
			glVertex3fv(de.Dest()->Pos());
			glNormal3fv(de.Enext().Dest()->Normal());
			glVertex3fv(de.Enext().Dest()->Pos());
			}
		glEnd();
		}
	}
  }

  glDisable(GL_LIGHTING);

  // check if user wants to display the edges of the mesh
  if(m_edgesOn) {
    glBegin(GL_LINES);  
    float color[3] = {1.0f, 1.0f, 0.5f};
    glColor3fv(color);

    SimpleMeshT::EdgeCt::const_iterator ei;
    for(ei=m_mesh->Edges().begin(); ei!=m_mesh->Edges().end(); ++ei) {
      glVertex3fv((*ei)->Vertex(0)->Pos());
      glVertex3fv((*ei)->Vertex(1)->Pos());
    }
    glEnd();
  }

  
  glEnable(GL_LIGHTING);

}
  
  


/***********************************************/
/*
	This function calculates the bouding box
	for the mesh
							modified by: Jun
							date: 01/26/05
*/
/***********************************************/
void PartMesh::CalculateBoudingBox()
{
	

	XVecf bmin,bmax;

	bmin = XVecf(1e15);
    bmax = XVecf(-1e15);


	if(m_mesh == 0)
		return;
	
	
	for(SimpleMeshT::VertexCt::iterator vi=m_mesh->m_vs.begin(); vi!=m_mesh->m_vs.end(); ++vi) {
    bmin = bmin.Min((*vi)->Pos());
    bmax = bmax.Max((*vi)->Pos());
	}
	
	PartMeshDisplayProperties.DisplayPropertiesBBox.bmin = bmin;
	PartMeshDisplayProperties.DisplayPropertiesBBox.bmax = bmax;
	
	

}




/***********************************************/
/*
	This function deletes the part mesh if
	it is loaded
							modified by: Jun
							date: 01/26/05
*/
/***********************************************/
void PartMesh::DeletePartMesh()
{
	if(m_mesh)
		m_mesh = 0;
	
	selected = false;
	m_meshReady = false;
	m_reinit = false; 
    m_edgesOn = false; 
    m_list_mesh =-1; 
    m_clip = false;
    m_smoothOn = false;
	redo = true;

	PartMeshDisplayProperties.DisplayPropertiesBBox.bmin = 1e15;
	PartMeshDisplayProperties.DisplayPropertiesBBox.bmax = 1e-15;
	
}