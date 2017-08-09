/************************************************

	This file contains the implementation for the
	PartMeasurement class.
	In this class, we have the following functions:

	default constructor
	destructor
	read a PartMeasurement from file
	write a PartMeasurement from file
	draw the PartMeasurement
	reset flags after a transformation
	register by ICP (registration)
	register by PCA (registration)
	calculate the color map for the PartMeasurement
	apply the color map
	get color map range - TBD


*************************************************/

#include "stdafx.h"
#include "PartMeasurement.h"
#include "3DPoint.h"
#include "ColorConversion.h"
#include "BasePoints.h"
#include "PointClouds.h"
#include <stdio.h>
#include ".\partmeasurement.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	
/***********************************************/
/*
	TO DO:
	add description
	add comments
							modified by:
							date:
*/
/***********************************************/
// Default constructor , - set matrix to unit matrix
PartMeasurement::PartMeasurement()
{
	XVecf temp1(0,0,0);
	XVecf temp2(0,0,1);

	PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.max = 1e-15;
	PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.min = 1e15;
	pov.ViewPoint = temp1;
	pov.ViewDir = temp2;
	std_dev_filter = 1e99; //Really high std dev filter.
	NormalAgreementFilter = -1e99; //Really low normal agreement filter
	PC = NULL;
}

// Copy Constructor, transfers the Auto-align structure correctly
PartMeasurement::PartMeasurement(const PartMeasurement& rhs)
{
	closestPoints = rhs.closestPoints;
	Comments = rhs.Comments;
	distances = rhs.distances;
	NormalAgreement = rhs.NormalAgreement;
	FileName = rhs.FileName;
	middle = rhs.middle;
	pov = rhs.pov;
	PartMeasurementDisplayProperties = rhs.PartMeasurementDisplayProperties;
	PartMeasurementFlags = rhs.PartMeasurementFlags;
	PartMeasurementSampledPoints = rhs.PartMeasurementSampledPoints;
	PartMeasurementTransMatrix = rhs.PartMeasurementTransMatrix;
	PartMeasurementFlags = rhs.PartMeasurementFlags;
	std_dev_filter = rhs.std_dev_filter;
	NormalAgreementFilter = rhs.NormalAgreementFilter;
	StdDevToFilerRatios = rhs.StdDevToFilerRatios;
	PC = NULL;

	if (rhs.PC){
	    PC = new PointClouds;
		*PC = * rhs.PC;
	}
	

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
// Destructor
PartMeasurement::~PartMeasurement()
{
	if (PC != NULL){
		delete PC;
		PC = NULL;
	}
}
	


/***********************************************/
/*
	bool ReadPartMeasurement(string BIMFile)
	Reads a .SNR file and places the points into
	the PartMeasurement vector. Returns true
	on a successful read, false otherwise.
	TODO: Get RAW data reading working?
							modified by: Geoff Blake
							date: 9/23/04
*/
/***********************************************/
// Function to read part measurement from a file
// and populates the SampledPoints
bool PartMeasurement::ReadPartMeasurement(string BIMFile)
{
	FILE* file;
	SampledPoint p;

	// empty string, don't read the file
	if (BIMFile.empty() == true)
	{
		return false;
	}

	// open file for reading
	file = fopen(BIMFile.c_str(), "r");

	// No file found, return an error
	if(file == NULL)
	{
		return false;
	}

	// Test if file is empty and return an error if it is.
	if(feof(file) != 0)
	{
		return false;
	}
	
	// Testing whether file is a pts or snr file
	char junk[100];
	if(fgets(junk, 100, file) != NULL)
	{
		double x,y,z,snr;
		if(sscanf(junk, "%lf %lf %lf %lf", &snr, &x, &y, &z) == 4)
		{
			// rewind file
			fseek(file, 0, SEEK_SET);
			// While not at EOF, read in 4 doubles per line.
			while(fscanf(file, "%lf %lf %lf %lf", &p.SNR, &p.x, &p.y, &p.z) != EOF)
			{
				p.PointValid = true;
				p.highlight = false;
				PartMeasurementSampledPoints.BIMSampledPoints.push_back(p);
			}
		}
		else if(sscanf(junk, "%lf %lf %lf %lf", &snr, &x, &y, &z) == 3)
		{
			// rewind file
			fseek(file, 0, SEEK_SET);
			// While not at EOF, read in 4 doubles per line.
			while(fscanf(file, "%lf %lf %lf", &p.x, &p.y, &p.z) != EOF)
			{
				p.PointValid = true;
				p.highlight = false;
				PartMeasurementSampledPoints.BIMSampledPoints.push_back(p);
			}
		}
	}

	// save path name.
	FileName = BIMFile;
	CalculateBoundingBox();

	fclose(file);
	return true;
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
// Function to write part measurement to a file
void PartMeasurement::WritePartMeasurement(string BIMFile)
{
	FILE* file;

	file = fopen(BIMFile.c_str(), "w");

	// No file found, return an error
	if(file == NULL)
	{
		return;
	}

	for(int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		fprintf(file, "%lf %lf %lf %lf\n", 
			PartMeasurementSampledPoints.BIMSampledPoints[i].SNR, 
			PartMeasurementSampledPoints.BIMSampledPoints[i].x, 
			PartMeasurementSampledPoints.BIMSampledPoints[i].y, 
			PartMeasurementSampledPoints.BIMSampledPoints[i].z);
	}

	fclose(file);
	return;
}



/***********************************************/
/*
	void Draw()
	Draws the point cloud points to the display area.
							modified by: Geoff Blake
							date: 9/23/04
*/
/***********************************************/
// Function to draw part measurement
void PartMeasurement::Draw()
{
	// TODO: Make this variable, not hardwired.
	
	double PointSize = PartMeasurementDisplayProperties.PointSize;
	int PointType    = PartMeasurementDisplayProperties.PointType;

	
	
	CrossHatchPoint CrossPt(PointSize);
	TetraHedronPoint TetraPt(PointSize);
	DoubleTetraHedronPoint DoubleTetraPt(PointSize);

	glPushMatrix();
	glMultMatrixf(PartMeasurementTransMatrix.TransformationMatrix);
	for(int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		// Add names to individual points
		glPushName((GLuint)i);
		if( PartMeasurementFlags.Selected == false && 
			PartMeasurementFlags.Registered == false && 
			PartMeasurementFlags.Active == true && 
			PartMeasurementSampledPoints.BIMSampledPoints[i].highlight == false)
		{
			glColor4d((GLdouble)PartMeasurementSampledPoints.BIMSampledPoints[i].ColorAttributes[0],
				(GLdouble)PartMeasurementSampledPoints.BIMSampledPoints[i].ColorAttributes[1],
				(GLdouble)PartMeasurementSampledPoints.BIMSampledPoints[i].ColorAttributes[2],
				(GLdouble)PartMeasurementSampledPoints.BIMSampledPoints[i].ColorAttributes[3]);
		}
		else if(PartMeasurementFlags.Selected == true)
		{
			glColor4d(0.0, 1.0, 0.0, 1.0);
		}
		else if(PartMeasurementFlags.Active == false)
		{
			glColor4d(1.0, 1.0, 0.0, 1.0);
		}
		else if(PartMeasurementFlags.Registered == true)
		{
			if ( (StdDevToFilerRatios[i] < std_dev_filter) && (NormalAgreement[i] >= NormalAgreementFilter) )
			{
				double H = GetHueFromDistance(distances[i], PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.min, 
					PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.max);
				double R,G,B;
				ConvertHSVtoRGB(H, 1, 1, R, G, B);
				glColor4d(R,G,B,1.0);
			}
			else
			{
				glColor4d(0, 0, 0, 1.0);
			}
		}
		else if(PartMeasurementSampledPoints.BIMSampledPoints[i].highlight == true)
		{
			glColor4d(0.0, 1.0, 0.0, 1.0);
		}

		switch(PointType)
		{
			case 0:
				CrossPt.Draw(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
					PartMeasurementSampledPoints.BIMSampledPoints[i].y,
					PartMeasurementSampledPoints.BIMSampledPoints[i].z);
				break;
			case 1:
				TetraPt.Draw(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
					PartMeasurementSampledPoints.BIMSampledPoints[i].y,
					PartMeasurementSampledPoints.BIMSampledPoints[i].z);
				break;
			case 2:
				DoubleTetraPt.Draw(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
					PartMeasurementSampledPoints.BIMSampledPoints[i].y,
					PartMeasurementSampledPoints.BIMSampledPoints[i].z);
				break;
			default:
					AfxMessageBox("Cannot draw such a point type!");
		}
		glPopName();
	}
	glPopMatrix();

	// Draw lines to closest point.
	// Can help bring out errors in registration.
	if(PartMeasurementFlags.Registered == true)
	{
		float* mat;
		mat = PartMeasurementTransMatrix.Transpose();

		for(int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
		{
		
			glPushMatrix();
			glColor4d(0.0, 1.0, 0.0, 1.0);
			XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
					PartMeasurementSampledPoints.BIMSampledPoints[i].y,
					PartMeasurementSampledPoints.BIMSampledPoints[i].z);
			pt = mult(mat, pt);
			glBegin(GL_LINES);
				glVertex3f(pt.x(),pt.y(),pt.z());
				glVertex3f(closestPoints[i].pt.x(),closestPoints[i].pt.y(),closestPoints[i].pt.z());
			glEnd();
			glPopMatrix();
		}
		delete[] mat;
	}	
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
// Function to set the flags and reset the from the mesh
void PartMeasurement::ResetPropAfterTrans(enum value)
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
// Function to run ICP algorithm for registering the part measurement
// to the mesh
void PartMeasurement::RegisterByICP(Mesh* BIMMesh)
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
// Function to run PCA algorithm for registering the part measurement
// to the mesh
void PartMeasurement::RegisterByPCA(Mesh* BIMMesh)
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
// Function to calculate color map for the
// PartMeasuremnt
void PartMeasurement::CalculateColorMap(int VALUE/*total,snr,etc.*/)
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
// Function to apply color map
void PartMeasurement::ApplyColorMap()
{
}


/***********************************************/
/*
	This function calculates the bounding box 
	for a part measurement
							modified by: Jun
							date: 10/15/04
*/
/***********************************************/
void PartMeasurement::CalculateBoundingBox()
{
	XVecf bmin,bmax;
	float* mat = NULL;

	bmin = XVecf(1e15);
    bmax = XVecf(-1e15);

    int i;
	mat = PartMeasurementTransMatrix.Transpose();
	for(i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size();i++)
	{
		XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].y,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].z);

		XVecf temp = mult(mat, pt);
		
		if (i == 0)
		{
			TRACE("PT: %f, %f, %f, TEMP: %f, %f, %f\n", pt.x(), pt.y(), pt.z(), temp.x(), temp.y(), temp.z());
		}
	
		bmin = bmin.Min(temp);
		bmax = bmax.Max(temp);
	}

	PartMeasurementDisplayProperties.DisplayPropertiesBBox.bmin = bmin;
	PartMeasurementDisplayProperties.DisplayPropertiesBBox.bmax = bmax;

	middle = (bmax + bmin)/2.0;
	delete [] mat;
}

/***************************************/
/*
	This function calculates the center of mass for the
	point cloud to properly perform rotations.
*/
/***************************************/
XVecf PartMeasurement::CalculateCenterOfMass()
{
	float* mat = NULL;
	XVecf center(0,0,0);

	mat = PartMeasurementTransMatrix.Transpose();

	for(int i=0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].y,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].z);

		XVecf temp = pt;//mult(mat, pt);

		center += temp;
	}
	delete[] mat;
	mat = NULL;

	center = center / PartMeasurementSampledPoints.BIMSampledPoints.size();
	return center;
}

/***************************************/
/*
*/
/***************************************/
void PartMeasurement::SetClosestPoints(vector<XVecfn> closestPts, vector<XVecfn> closestPtsNrml)
{
	closestPoints = closestPts;
	closestPointsNormal = closestPtsNrml;
	return;
}

/***************************************/
/*
	Function to update the distances
	between points transformed
	and its closest points
	It also updates Normal Agreements
						Modified by Jun
						01/21/05
						Modified by Jacob
						11/17/05

*/
/***************************************/
void PartMeasurement::UpdateDistances()
{
	float* mat = NULL;
	double dtemp, dist, NormalAgreementRatio;
	mat = PartMeasurementTransMatrix.Transpose();
	distances.clear();
	NormalAgreement.clear();

	for(int i = 0; i < closestPoints.size(); i++)
	{
		XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].y,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].z);

		XVecf temp = mult(mat, pt);
        dtemp = (temp.x()-closestPoints[i].pt.x()) * (temp.x()-closestPoints[i].pt.x())+
				(temp.y()-closestPoints[i].pt.y()) * (temp.y()-closestPoints[i].pt.y())+
				(temp.z()-closestPoints[i].pt.z()) * (temp.z()-closestPoints[i].pt.z());
		if (!closestPoints[i].rn)
			dist = sqrt(dtemp);
		else
			dist = -sqrt(dtemp);

		XVecf TmpNormal=PartMeasurementSampledPoints.BIMSampledPoints[i].normal;
		// Transform the normal by the matrix - rotation part only wothout translation
		XVecf TmpNormal1 = mult3x3(mat, TmpNormal);
		XVecf TmpNormal2 = closestPointsNormal[i].pt;
		
		//Normalize just in case it is not a unit normal
		TmpNormal1.normalize();
		TmpNormal2.normalize();
		NormalAgreementRatio = (TmpNormal1.dot(TmpNormal2));
		//TRACE ("NormalAgreementRatio %lf\n",NormalAgreementRatio);
		distances.push_back(dist);
		NormalAgreement.push_back(NormalAgreementRatio);
	}

	// De-allocating memory
	delete[] mat;
	mat = NULL;
	
	return;
}

// Calculate each point's std dev from the model.
void PartMeasurement::CalculateStdDev()
{
	StdDevToFilerRatios.clear();
	// calculate std dev, then find how many std devs each point is away from the center.
	// Points that thair normal agreement with thier closest points is 
	// below NormalAgreementFilter are filtered out and do not participate in calculating the average and std dev

	DistAvg = 0;
	int num=0;
	for(int i = 0; i < distances.size(); i++)
		if (NormalAgreement[i] >= NormalAgreementFilter){
			DistAvg += distances[i];
			num++;
		}
	DistAvg /= num;

	double sqrd_var = 0;
	for(int i = 0; i < distances.size(); i++)
		if (NormalAgreement[i] >= NormalAgreementFilter){
			sqrd_var += (distances[i] - DistAvg) * (distances[i] - DistAvg);
		}
	sqrd_var /= num;
	DistStd = sqrt(sqrd_var);

	// Now show how many StdDevToFilerRatios each point is away from mean.
	double tmp;
	for(int i = 0; i < distances.size(); i++){
		tmp = distances[i] - DistAvg;
		tmp /= DistStd;
		tmp = abs(tmp);  // take absolute value because we can have outliers on both ends.
		StdDevToFilerRatios.push_back(tmp);
	}

	return;
}



/***********************************************/
/*
	Function to update the color bar
	for the part measurement
							modified by: Jun
							date: 01/21/05
*/
/***********************************************/
void PartMeasurement::UpdatePartMeasurementColorBar()
{
	double min(1e15);
	double max(1e-15);
		

	for(int i = 0 ; i < distances.size(); i++)
	{
		if(distances[i] > max && (StdDevToFilerRatios[i] < std_dev_filter) && (NormalAgreement[i] >= NormalAgreementFilter))
			max = distances[i];

		if(distances[i] < min && (StdDevToFilerRatios[i] < std_dev_filter) && (NormalAgreement[i] >= NormalAgreementFilter))
			min = distances[i];
	}

	PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.max = max;
	PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.min = min;
	
	return;

}

// Exports the transformed data points to a file
void PartMeasurement::ExportTransformedDataPts(int i)
{
	FILE* file;
	string new_fileNameExt;
	string new_fileName;
	float* mat = NULL;
	mat = PartMeasurementTransMatrix.Transpose();

	// make a new filename
	new_fileNameExt += ".exported";
	if (FileName.size() > 0)
	{
		new_fileName += FileName;
		new_fileName += new_fileNameExt;
	}
	else
	{
		char buffer[15];
		itoa(i, buffer, 15);
		new_fileName += buffer;
		new_fileName += new_fileNameExt;
	}

	file = fopen(new_fileName.c_str(), "w+");

	for (int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].y,
				   PartMeasurementSampledPoints.BIMSampledPoints[i].z);

		XVecf temp = mult(mat, pt);

		fprintf(file, "%lf %lf %lf %lf\n", temp.x(), temp.y(), temp.z(), distances[i]);
	}

	fclose(file);
	delete [] mat;

	return;
}

// Calculate the normals of the point cloud.
void PartMeasurement::CalculateNormals()
{
	if (PC == NULL)
		PC = new PointClouds;
	float* points;
	
	points = new float[PartMeasurementSampledPoints.BIMSampledPoints.size()*3];

	for (int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		points[3*i] = PartMeasurementSampledPoints.BIMSampledPoints[i].x;
		points[3*i + 1] = PartMeasurementSampledPoints.BIMSampledPoints[i].y;
		points[3*i + 2] = PartMeasurementSampledPoints.BIMSampledPoints[i].z;
	}

	//PC.SetCameraData(pov.ViewPoint, pov.ViewDir);
	PC->BuildKDTree(PartMeasurementSampledPoints.BIMSampledPoints.size(), points, NULL, false);
	PC->ComputeRadius();
	PC->ComputeNormal();

	for (int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		PartMeasurementSampledPoints.BIMSampledPoints[i].normal = PC->PointsCloud[i].norm;
	}

	delete[] points;

	return;
}
// clears the closet points and propogates the update for distances and all other variables
void PartMeasurement::ClearClosestPointsAndUpdate(void)
{
	closestPoints.clear();
	closestPointsNormal.clear();
	UpdateDistances();
	CalculateStdDev();
	UpdatePartMeasurementColorBar();

}


// This function claculates the transformed point cloud and the nearest points after filtering for Normals and StdDev
// These points are returned in the RetTransPoints and the RetClosestPoints variables. 
// Information in these vectors is erased and then filed by the function .
// The function returns the number of filtered points.
int PartMeasurement::FilterPointtsForICP ( vector<XVecf> &RetTransPoints, vector<XVecf> &RetClosestPoints){

	int NumFiltered=0;
	float* mat = NULL;
	mat = PartMeasurementTransMatrix.Transpose();
	RetTransPoints.clear();
	RetClosestPoints.clear();
	//TRACE ("%i, %i \n",StdDevToFilerRatios.size(),NormalAgreement.size());
	for(int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
		if (   (StdDevToFilerRatios[i] < std_dev_filter) && (NormalAgreement[i] >= NormalAgreementFilter) ){
			XVecf pt(PartMeasurementSampledPoints.BIMSampledPoints[i].x,
					PartMeasurementSampledPoints.BIMSampledPoints[i].y,
					PartMeasurementSampledPoints.BIMSampledPoints[i].z);
			XVecf TransPt = mult(mat, pt);

			XVecf ClosePt;
			ClosePt = closestPoints[i].pt;

			RetTransPoints.push_back(TransPt);
			RetClosestPoints.push_back(ClosePt);
		}
		else{
			NumFiltered++;
		}

	delete [] mat;

	return (NumFiltered);
}


void PartMeasurement::InitPointCloud(void)
{

	if ( ! PC)
        PC = new PointClouds;
	float* points;
	float* normals;
		
		// Transfer Xinju's Auto-align structure over properly
		points = new float[PartMeasurementSampledPoints.BIMSampledPoints.size()*3];
		normals = new float[PartMeasurementSampledPoints.BIMSampledPoints.size()*3];

		for (int i = 0; i < PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
		{
			points[3*i] = PartMeasurementSampledPoints.BIMSampledPoints[i].x;
			points[3*i + 1] = PartMeasurementSampledPoints.BIMSampledPoints[i].y;
			points[3*i + 2] = PartMeasurementSampledPoints.BIMSampledPoints[i].z;

			normals[3*i] = PartMeasurementSampledPoints.BIMSampledPoints[i].normal.x();
			normals[3*i + 1] = PartMeasurementSampledPoints.BIMSampledPoints[i].normal.y();
			normals[3*i + 2] = PartMeasurementSampledPoints.BIMSampledPoints[i].normal.z();
		}

		//PC.SetCameraData(pov.ViewPoint, pov.ViewDir);
		PC->BuildKDTree(PartMeasurementSampledPoints.BIMSampledPoints.size(), points, normals, false);
		PC->ComputeRadius();
		PC->ComputeNormal();

		delete[] points;
		delete[] normals;
		points = NULL;
		normals = NULL;

}
