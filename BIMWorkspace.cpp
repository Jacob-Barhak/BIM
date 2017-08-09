/************************************************

	This file contains the implementation for the
	BIMWorkspace class.
	In this class, we have the following functions:
	
	reading	.bim files
	writing .bim files
	adding new PartMeasurements to workspace
	andding new PartMesh to workspace.
	deleting a PartMesh
	deleting PartMeasurements
	process a selection
	draw the workspace
	run ICP algorithm (registration)
	run PCA algorithm (registration)
	calculate the global color map
	calculate bounding box for workspace

	History: Created	9/20/04		Jun
			 Modified	9/22/04		Geoff	desc: Made this the MFC Document Class


*************************************************/

#include "stdafx.h"
#include "BIM.h"
#include "MainFrm.h"
#include "BIMWorkspace.h"
#include "Registration.h"
#include "3DPoint.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(BIMWorkspace, CDocument)

BEGIN_MESSAGE_MAP(BIMWorkspace, CDocument)
END_MESSAGE_MAP()

/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/

BIMWorkspace::BIMWorkspace()
{
	SelectedPart = 0;
	SelectedPartMeasurement = false;
	SelectedPartMesh = false;
	
	pt_select = false;
}


/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by: Jacob
							date: 9/23/2005
*/
/***********************************************/

BIMWorkspace::~BIMWorkspace() 
{

	// Remove memory
	// remove part
	DeletePartMesh();

	// Remove measurements
	//for (int i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++){
	//	DeletePartMeasurement(i);
	//}

	// xinju
	for (int i = BIMPartMeasurements.BIMPartMeasurements.size()-1; i >=0; i--){
		DeletePartMeasurement(i);
	}
	// end xinju

}


/***********************************************/
/*
	Reads in a BIM workspace file.
							modified by: Geoff
							date: 10/27/04
*/
/***********************************************/

void BIMWorkspace::ReadBIMWorkspace(string BIMFile)
{
	FILE* bimFile;

	// Open the file for reading.
	bimFile = fopen(BIMFile.c_str(), "r");

	char* temp = new char[1024];

	while(feof(bimFile) == 0)
	{

		// Read in mesh path and load it.
		fscanf(bimFile, "%s", temp);
		if (!strcmp(temp, "<mesh>"))
		{
			// got file path
			fgets(temp, 1024, bimFile);
			string path(temp);
			path.resize(path.size() - 1);
			path = path.substr(1, path.size());
			
			AddNewPartMesh(path);
		}
		else if (!strcmp(temp, "<signature>"))
		{
			// got file path
			fgets(temp, 1024, bimFile);
			string path(temp);
			path.resize(path.size() - 1);
			path = path.substr(1, path.size());
			
			AddNewSignature(path);
		}
		else if (!strcmp(temp, "<part>"))
		{
			// get file path
			fgets(temp, 1024, bimFile);
			string path(temp);
			path.resize(path.size() - 1);
			path = path.substr(1, path.size());
			TRACE("PATH: %s\n", path.c_str());
			AddNewPartMeasurements(path);

			//read <part_trans_matrix>
			fscanf(bimFile, "%s", temp);
			//read in transMatrix vals
			int i = BIMPartMeasurements.BIMPartMeasurements.size() - 1;
			TRACE("Part: %d\n", i);
			float* mat = &BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementTransMatrix.TransformationMatrix[0];
			fscanf(bimFile, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
				&mat[0], &mat[1], &mat[2], &mat[3],
				&mat[4], &mat[5], &mat[6], &mat[7],
				&mat[8], &mat[9], &mat[10], &mat[11],
				&mat[12], &mat[13], &mat[14], &mat[15]);
			TRACE("READ XFORM MAT\n");
			// update 
		}
		else
		{
			// fail queitly
			TRACE("File is invalid\n");
		}
	}
	fclose(bimFile);
		
	delete [] temp;
}


/***********************************************/
/*
	Saves out the BIM Workspace in a pseudo
	XML file format.
							modified by: Geoff Blake
							date: 10/27/04
*/
/***********************************************/

void BIMWorkspace::WriteBIMWorkspace(string BIMFile)
{
	FILE* bimFile;

	// Create a new file to write to.
	bimFile = fopen(BIMFile.c_str(), "w");

	// Write out current file path to mesh if it exists.
	if(BIMPartMesh.FileName.size() > 0)
	{
		fprintf(bimFile, "<mesh> ");
		fprintf(bimFile, "%s\n", BIMPartMesh.FileName.c_str());
	}

	// Write out current file path to signature if it exists.
	if(BIMPartMesh.SigFileName.size() > 0)
	{
		fprintf(bimFile, "<signature> ");
		fprintf(bimFile, "%s\n", BIMPartMesh.SigFileName.c_str());
	}


	// Loop through part measurements and save out path
	// trans matrix and some other assorted data.
	for(int i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++)
	{
		// Write out path
		fprintf(bimFile, "<part> ");
		fprintf(bimFile, "%s\n", BIMPartMeasurements.BIMPartMeasurements[i].FileName.c_str());

		// Write out trans matrix
		fprintf(bimFile, "<part_trans_matrix> ");
		for (int j = 0; j < 16; j++)
		{
			fprintf(bimFile, "%f ", 
				BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementTransMatrix.TransformationMatrix[j]);
		}
		fprintf(bimFile, "\n");
		//Other data may be needed, but for now this is all I want to work with.
	}
	fclose(bimFile);
}


/***********************************************/
/*
	Add a new part measurement to the bim
	workspace
*/
/***********************************************/

void BIMWorkspace::AddNewPartMeasurements(string BIMFile)
{
	// Read in partmeasurement
	PartMeasurement p;
	// xinju
	// solve the problem of duplicate
#if 0
	BIMPartMeasurements.BIMPartMeasurements.push_back(p);
	int n = BIMPartMeasurements.BIMPartMeasurements.size()-1;
	bool success = BIMPartMeasurements.BIMPartMeasurements[n].ReadPartMeasurement(BIMFile);
	BIMPartMeasurements.BIMPartMeasurements[n].InitPointCloud();
	if ( !success ){
		BIMPartMeasurements.BIMPartMeasurements.pop_back();
	}
	// end xinju
#endif
#if 1
	bool success = p.ReadPartMeasurement(BIMFile);
//	p.InitPointCloud();

	// if we successfully read it in, push it back onto the vector of measurements
	if (success == true)
	{
		BIMPartMeasurements.BIMPartMeasurements.push_back(p);
	}
#endif
	// set partmeasurement properties
	for(int i = 0 ; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++)
	{
		BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementDisplayProperties.PointSize = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointSize;
		BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementDisplayProperties.PointType = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointType;
		// The following two lines were changed by Jacob. Calculate Normal and InitPointCloud were changed as Normals have not been set and InitPointCloud does not set normals
		//BIMPartMeasurements.BIMPartMeasurements[i].InitPointCloud();
		BIMPartMeasurements.BIMPartMeasurements[i].CalculateNormals();
	}
	// calculate measurment normals
//	BIMPartMeasurements.BIMPartMeasurements[BIMPartMeasurements.BIMPartMeasurements.size() - 1].CalculateNormals();
}


/***********************************************/
/*
	Function to add a new part mesh
	(Import .obj file)
							modified by: Jun
							date: 10/14/04
*/
/***********************************************/

void BIMWorkspace::AddNewPartMesh(string BIMFile)
{

	// read new part mesh
	BIMPartMesh.ReadPart(BIMFile);

	// Allocate a vector of vertices for mesh for selection of points.
	for(SimpleMeshT::VertexCt::iterator vi = BIMPartMesh.GetMesh()->VertsBegin(); vi != BIMPartMesh.GetMesh()->VertsEnd(); vi++)
	{
		XVecf tmp((*vi)->m_pos.x(), 
			(*vi)->m_pos.y(), 
			(*vi)->m_pos.z());
		mesh_vertices.push_back(tmp);
	}
	
}

//This function adds a new signature from file
// By jacob
// modified 9/23/2005
void BIMWorkspace::AddNewSignature(string BIMFile)
{

	BIMPartMesh.ReadSignature(BIMFile);
	
}



/***********************************************/
/*
	Deletes the part mesh
							modified by: Jun
							date: 10/29/04
*/
/***********************************************/

void BIMWorkspace::DeletePartMesh()
{
	BIMPartMesh.DeletePartMesh();
	
	// Clear the vector of mesh vertices
	mesh_vertices.clear();
}


/***********************************************/
/*
	Delete the PartMeasurement based on its
	name
							modified by:Jun
							date:10/28/04
*/
/***********************************************/

void BIMWorkspace::DeletePartMeasurement(int name)
{
	BIMPartMeasurements.DeletePartMeasurement(name);
}


// Draws the vertices of the Mesh a second time for picking during 3 pt selection.
void BIMWorkspace::DrawMeshSelectionVerts()
{
	double size = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointSize;
	int type = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointType;

	CrossHatchPoint CrossPt(size);
	TetraHedronPoint TetraPt(size);
	DoubleTetraHedronPoint DoubleTetraPt(size);
	
	int j = -1;
	glPushName((GLuint)j);
	for(int i = 0; i < mesh_vertices.size(); i++)
	{
		glPushName((GLuint)i);
		// Just draw a point
		glBegin(GL_POINTS);
			glVertex3f(mesh_vertices[i].x(), mesh_vertices[i].y(), mesh_vertices[i].z());
		glEnd();
		glPopName();
	}
	glPopName();
}
/***********************************************/
/*
	Draws bimworkspace objects, ie:meshes, measurements
*/
/***********************************************/

void BIMWorkspace::Draw()
{	
	int i;

	glDisable(GL_LIGHTING);
	for(i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++)
	{
		
		glPushName((GLuint)i);
		if ((SelectedPartMeasurement == true) && (SelectedPart == i))
		{
			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Selected = true;
		}
		else
		{
			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Selected = false;
		}
		BIMPartMeasurements.BIMPartMeasurements[i].Draw();
		glPopName();
	}
	glEnable(GL_LIGHTING);

	
	// Jun added this part

	i = -1;
	glPushName((GLuint)i);
	
	if((SelectedPartMesh == true) && (SelectedPart==i))
		BIMPartMesh.selected = true;
	else
		BIMPartMesh.selected = false;
		
	// draw part mesh
	BIMPartMesh.Draw();
	
	glPopName();


	double size = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointSize;
	int type = BIMPartMeasurements.PartMeasurementsDisplayProperties.PointType;
	
	CrossHatchPoint CrossPt(2*size);
	TetraHedronPoint TetraPt(2*size);
	DoubleTetraHedronPoint DoubleTetraPt(2*size);
	
	glDisable(GL_LIGHTING);
	if(pt_select == true)
	{
		// Draw selected points
		for (int i = 0; i < selectedMeasurementPts.size(); i++)
		{
			// Draw these points
			glColor4f(0.0, 1.0, 0.0, 1.0);
			switch(type)
			{
				case 0:
					CrossPt.Draw(selectedMeasurementPts[i].x(),
					selectedMeasurementPts[i].y(),
					selectedMeasurementPts[i].z());
					break;
				case 1:
					TetraPt.Draw(selectedMeasurementPts[i].x(),
					selectedMeasurementPts[i].y(),
					selectedMeasurementPts[i].z());
					break;
				case 2:
					DoubleTetraPt.Draw(selectedMeasurementPts[i].x(),
					selectedMeasurementPts[i].y(),
					selectedMeasurementPts[i].z());
					break;
				default:
					AfxMessageBox("Cannot draw such a point type!");
			}
		}
		for (int i = 0; i < selectedMeshPts.size(); i++)
		{
			// Draw these points
			glColor4f(0.0, 1.0, 0.0, 1.0);
			switch(type)
			{
				case 0:
					CrossPt.Draw(selectedMeshPts[i].x(),
					selectedMeshPts[i].y(),
					selectedMeshPts[i].z());
					break;
				case 1:
					TetraPt.Draw(selectedMeshPts[i].x(),
					selectedMeshPts[i].y(),
					selectedMeshPts[i].z());
					break;
				case 2:
					DoubleTetraPt.Draw(selectedMeshPts[i].x(),
					selectedMeshPts[i].y(),
					selectedMeshPts[i].z());
					break;
				default:
					AfxMessageBox("Cannot draw such a point type!");
			}
		}
	}
	glEnable(GL_LIGHTING);

	// Get bounds for bounding box
	XVecf temp_bmin,temp_bmax, bmin,bmax;
	bmin = XVecf(1e15);
	bmax = XVecf(1e-15);
				
	if(BIMPartMeasurements.BIMPartMeasurements.size()!=0)
	{
		temp_bmin = BIMPartMeasurements.GetMinBound();
		temp_bmax = BIMPartMeasurements.GetMaxBound();
		bmin = bmin.Min(temp_bmin);
		bmax = bmin.Max(temp_bmax);
	}
	
	
	if(BIMPartMesh.m_mesh!=0)
	{
		BBox temp = BIMPartMesh.PartMeshDisplayProperties.DisplayPropertiesBBox;
		bmin = bmin.Min(temp.bmin);
		bmax = bmax.Max(temp.bmax);
	}

	XVecf middle = (bmin + bmax) /2;
	glPushMatrix();
	glTranslatef(-middle.x(),-middle.y(),-middle.z());
	glPushMatrix();
	
	glPopMatrix();
	glPopMatrix();

}


/***********************************************/
/*
	Function that runs the ICP algorithm on
	the selected point cloud
							modified by: Jun
							date: 11/18/04
*/
/***********************************************/

void BIMWorkspace::RegisterByICP(CWnd* myWin)
{
	//PartMeasurement* point_cloud;
	SimpleMeshT* BIMMesh = BIMPartMesh.GetMesh();
	
	// check if a point cloud has been selected
	if(SelectedPartMeasurement && BIMMesh)
	{
		RunICP(BIMPartMeasurements.BIMPartMeasurements[SelectedPart],BIMPartMesh, myWin, BIMOptions);
	}
	else 
	{
		if(!SelectedPartMeasurement && !BIMMesh)
			AfxMessageBox("No mesh and no point cloud selected");
		else if(!SelectedPartMeasurement && BIMMesh)
			AfxMessageBox("You need to select a point cloud to register");
		else
			AfxMessageBox("No mesh in workspace");
	}	
	
	// Update Color Bar for all part measurements
	BIMPartMeasurements.UpdatePartMeasurementsColorBar();
}

// Auto-align code
void BIMWorkspace::AutoAlign(CWnd* myWin)
{
	if (SelectedPartMeasurement)
	{
		RunAutoAlign(BIMPartMeasurements.BIMPartMeasurements[SelectedPart], BIMPartMesh, myWin, BIMOptions);
	}
	else
	{
		AfxMessageBox("You need to select a point cloud to register");
	}

	return;
}

// Takes the 2 deques of points and transforms the point cloud accordingly.
void BIMWorkspace::PointTransformation()
{
	float* newXformMatrix;

	if (selectedMeasurementPts.size() == 3 && selectedMeshPts.size() == 3)
	{
		newXformMatrix = ThreePtSelectAndXForm(selectedMeasurementPts, selectedMeshPts);
		if(newXformMatrix != NULL)
		{
			BIMPartMeasurements.BIMPartMeasurements[SelectedPart].PartMeasurementTransMatrix.multTransformMatrix(newXformMatrix);
			delete[] newXformMatrix;
		}
	}
	
}

// Measure current distance from selected point cloud to mesh.
void BIMWorkspace::MeasureCurrentDistances(CWnd* myWin)
{

	CString str;

	SimpleMeshT* BIMMesh = BIMPartMesh.GetMesh();
	if	(BIMMesh){
		if(SelectedPartMeasurement) // if a point cloud is selected calculate distances for the selection only
		{
			CalculateDistances(BIMPartMeasurements.BIMPartMeasurements[SelectedPart] ,BIMMesh, BIMOptions, myWin);
			str.Format("Updated distances for measurement #%i",SelectedPart);
		}
		else
		{ // otherwise, loop through all part measurements
			for(int i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++){
				CalculateDistances(BIMPartMeasurements.BIMPartMeasurements[i] ,BIMMesh, BIMOptions, myWin);
			}
			str.Format("Updated distances for %d measurements",BIMPartMeasurements.BIMPartMeasurements.size());
		}
	}
	else 
	{
		AfxMessageBox("No mesh in workspace");
	}	

	// Update Color Bar for all part measurements
	BIMPartMeasurements.UpdatePartMeasurementsColorBar();
	myWin->Invalidate(TRUE);
	myWin->UpdateWindow();

	CMainFrame *pFrame;
	pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.SetPaneText(0,str);

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

void BIMWorkspace::RegisterByPCA()
{
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

void BIMWorkspace::CalculateColorMap(enum VALUE)
{
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

void BIMWorkspace::CalulateBoundingBox()
{
}

/////////////////////////////////////////////////
// Needed functions for MFC                    //
/////////////////////////////////////////////////
BOOL BIMWorkspace::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CBIMDoc serialization

void BIMWorkspace::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

// Saves any scans with no names. IE: scans that are not loaded from a file,
// but imported by the machine, or split off from a previous point cloud.
void BIMWorkspace::SaveScans(CWnd* myWin)
{
	for(int i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++)
	{
		if(BIMPartMeasurements.BIMPartMeasurements[i].FileName.size() == 0)
		{
			string filePath;
			string fileExt;

			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Selected = true;
			SelectedPartMeasurement = true;
			SelectedPart = i;
			myWin->Invalidate(TRUE);
			myWin->UpdateWindow();
			// Open save dialog
			CFileDialog* fileSave = new CFileDialog(
				FALSE,
				"*.snr",
				NULL,
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"PointCloud (*.snr)|*.snr||\0",
				NULL,
				0);

			// Display FileDialog box
			fileSave->DoModal();
			// Get path and extension of file to be imported
			filePath = fileSave->GetPathName();
			fileExt = fileSave->GetFileExt();

			if (filePath.size() > 0)
			{
				BIMPartMeasurements.BIMPartMeasurements[i].FileName = filePath;
				BIMPartMeasurements.BIMPartMeasurements[i].WritePartMeasurement(filePath);
			}

			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Selected = false;
			SelectedPartMeasurement = false;
			SelectedPart = 0;
			myWin->Invalidate(TRUE);
			myWin->UpdateWindow();
		}
	}

	return;
}

// Loop through all measurements in workspace,
// Autoalign and refine each one report back the number of succesful alignments
int BIMWorkspace::MultiAutoAlignAndRefine (CWnd* myWin ){

	bool AutoAlignSuccess;
	bool ICPSuccess;
	int SuccessCounter=0;
	clock_t Start, Finish;
	double Duration;


	SimpleMeshT* BIMMesh = BIMPartMesh.GetMesh();

	if (!BIMMesh){
		AfxMessageBox("No mesh in workspace");
		return (-1); // indicates an error
	}

	Start = clock();

	// loop through all part measurements
	for(int i = 0; i < BIMPartMeasurements.BIMPartMeasurements.size(); i++){
		TransMatrix UndoTransMatrix = BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementTransMatrix;
		// First autoalign
		AutoAlignSuccess = RunAutoAlign(BIMPartMeasurements.BIMPartMeasurements[i], BIMPartMesh, myWin, BIMOptions);
		if (AutoAlignSuccess){ // if successfull refine solution using ICP
			ICPSuccess = RunICP(BIMPartMeasurements.BIMPartMeasurements[i] ,BIMPartMesh, myWin, BIMOptions);
		}
		if (!AutoAlignSuccess || !ICPSuccess){ //alignment or refinement unsuccessful
			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementTransMatrix = UndoTransMatrix;
			BIMPartMeasurements.BIMPartMeasurements[i].ClearClosestPointsAndUpdate();
			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Active=false; // mark the measurement as inactive to 
			BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Registered=false; // mark the measurement as not registered
		}
		else
		{ 
			// success
			SuccessCounter++;
		}

		// Update Color Bar for all part measurements and redraw screen
		BIMPartMeasurements.UpdatePartMeasurementsColorBar();
		myWin->Invalidate(TRUE);
		myWin->UpdateWindow();

	}

	Finish = clock();
	Duration = (double)(Finish - Start) / CLOCKS_PER_SEC;
	
	CMainFrame *pFrame;
	CString str;
	str.Format("Succeeded auto-align and refine of %d measurements out of %d , in %.1lf seconds",SuccessCounter,BIMPartMeasurements.BIMPartMeasurements.size(),Duration);
	pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.SetPaneText(0,str);

	AfxMessageBox(str);
	//MessageBeep(MB_ICONEXCLAMATION);

	return (SuccessCounter);
}




// CBIMDoc diagnostics
#ifdef _DEBUG
void BIMWorkspace::AssertValid() const
{
	CDocument::AssertValid();
}

void BIMWorkspace::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
