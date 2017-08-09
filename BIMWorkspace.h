/************************************************

	This file is the header file for the 
	BIMWorkspace class

*************************************************/


#ifndef BIMWORKSPACE_H
#define BIMWORKSPACE_H

#include <string>
#include <vector>
#include <deque>

#include "BBox.h"
#include "PartMeasurements.h"
#include "PartMesh.h"
#include "Options.h"
#include "ViewParameters.h"
#include "Arcball.h"
#include "xvec.h"


class BIMWorkspace : public CDocument
{
	protected: // create from serialization only
	DECLARE_DYNCREATE(BIMWorkspace)
	DECLARE_MESSAGE_MAP()

	public:
	
	// Default constructor
	BIMWorkspace();
	
	// Destructor
	~BIMWorkspace();
	
	// Function to read the BIM Workspace from file
	void ReadBIMWorkspace(string BIMFile);
	
	// Function to write the BIM Workspace to file
	void WriteBIMWorkspace(string BIMFile);
	
	// Function to add new PartMeasurements from a file
	// to the BIM workspace
	void AddNewPartMeasurements(string BIMFile);
	
	// Function to add new PartMesh from a file
	// to the BIM workspace
	void AddNewPartMesh(string BIMFile);
	
	// Function to add new signature from a file
	// to the BIM workspace
	void AddNewSignature(string BIMFile);


	// Function to delete PartMesh from the BIM Workspace
	void DeletePartMesh();
	
	// Function to delete a selected
	// PartMeasurement from the BIM
	// Workspace based on its name
	void DeletePartMeasurement(int name);
	
	// Function to draw whole workspace
	void Draw();
	
	// Function that runs registration using ICP algorithm.
	void RegisterByICP(CWnd* myWin);

	// Auto align function
	void AutoAlign(CWnd* myWin);

	// Function that runs registration using PCA algorithm.
	void RegisterByPCA();

	// Loop through all measurements in workspace,
	// Autoalign and refine each one report back the number of succesful alignments
	int MultiAutoAlignAndRefine (CWnd* myWin );

	
	// Function to calculate the global color map, color map will be based
	// on either snr, total, etc.
	void CalculateColorMap(enum VALUE/*total,snr,etc.*/);
	
	// Function to calculate bounding box of the whole BIM Workspace
	void CalulateBoundingBox();

	void SetArcball(ArcBall ball){my_arcball=ball;}
	
	// Draws vertices of the mesh, only in selection mode though.
	void DrawMeshSelectionVerts();

	void SaveScans(CWnd* myWin);

	// Transform pt cloud to mesh
	void PointTransformation();

	void MeasureCurrentDistances(CWnd* myWin);
	
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif

	// member variables
	bool SelectedPartMesh;
	bool SelectedPartMeasurement;
	// FOR NOW: Lets say negative numbers for meshes, positive for pt clouds.
	// Sure hope 2 billion for each is enough.
	int SelectedPart;
	BBox BIMWorkspaceBBox;
	// File to save to, starts off as null, then is initialized on a saveas.
	string fileSavePath;
	ViewParameters BIMViewParameters;
	PartMeasurements BIMPartMeasurements;
	PartMesh BIMPartMesh;
	Options BIMOptions;

	// Vector of vertices for selecting on mesh
	vector<XVecf> mesh_vertices;
	
	// Set of vectors that will be passed to Xinju's aligning code.
	// Can probably use these to draw pts over original points.
	bool pt_select;
	deque<XVecf> selectedMeasurementPts;
	deque<XVecf> selectedMeshPts;

	vector<XVeci> highLightedPts;

	ArcBall my_arcball;

};

#endif
