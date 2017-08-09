/************************************************

	This file is the header file for the
	PartMesh class

*************************************************/


#ifndef PARTMESH_H
#define PARTMESH_H


#include <string>
#include "Mesh.h"
#include "DisplayProperties.h"
#include "TransMatrix.h"
#include "simplemesh.h"
#include "PointClouds.h"
#include "Options.h"

class PartMesh
{

	public:
	
	// Default constructor
	PartMesh();
	
	// Destructor
	~PartMesh();
	
	// Function to draw part 
	void Draw();
	
	// Function to read the part from a file
	// Reads .obj file and brings the mesh into the 
	// workspace
	void ReadPart(string BIMFile);
	
	// Function to write the part to a file
	// Writes file, no manipulation of the mesh
	void WritePart(string BIMFile);
	
	// Function to render mesh
	void RenderMesh();

	// Function to calculate bouding box for mesh
	void CalculateBoudingBox();
	
	// Function to delete the part mesh
	void DeletePartMesh();

	// Compute Normals
	void SetVertsAndNorms();
	void ConstructPointClouds();

	// Function that returns the mesh
	SimpleMeshT* GetMesh(){return m_mesh;}

	// Signature relatd functions:
	// Function to Read the signature file
	void ReadSignature(string BIMFile);
	
	// Function to write the signature file 
	void WriteSignature(string BIMFile);

	// Function to calculate the signature, without saving to file
	void CalculateSignature(Options Ops);

	// member variables
	string FileName;
	string Comments;
	Mesh PartMeshMesh;
	DisplayProperties PartMeshDisplayProperties;
	TransMatrix PartMeshTransMatrix;
	bool selected;
	
	//signature related variables
	bool SigCalculated;
	string SigFileName;
	

	SimpleMeshT* m_mesh;
//	PointClouds surfaceMatching;
	int N;
	float* vertices;
	float* normals;
	PointClouds PC1;
	bool m_meshReady, m_reinit;
	bool m_edgesOn, m_smoothOn, m_edgesOnly; 
	int m_list_mesh;
	bool m_clip;
	bool redo;
	
};

#endif
