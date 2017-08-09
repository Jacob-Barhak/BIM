/************************************************

	This file contains the implementation for the
	PartMeasurements class.

*************************************************/

#include "stdafx.h"
#include "PartMeasurements.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/***********************************************/
/*
	This function calculates the bounds
	for all the PartMeasurements
							modified by: Jun
							date: 10/19/04
*/
/***********************************************/
void PartMeasurements::CalculatePartMeasurementsBounds()
{
	// initiliaze bmin to be huge
	bmin = XVecf(1e15);
    
	// initialize bmax to be very small
	bmax = XVecf(-1e15);
		

	// now look for the real bmin and bmax
	for(int i = 0 ; i < BIMPartMeasurements.size(); i++)
	{
		BIMPartMeasurements[i].CalculateBoundingBox();
		BBox  temp = BIMPartMeasurements[i].PartMeasurementDisplayProperties.DisplayPropertiesBBox;
		bmin = bmin.Min(temp.bmin);
		bmax = bmax.Max(temp.bmax);
	}
}


/***********************************************/
/*
	Function to delete a PartMeasurement based on its name
	from our collection of PartMeasurements
							modified by: Jun
							date: 10/29/04
*/
/***********************************************/
void PartMeasurements::DeletePartMeasurement(int name)
{
	//CString str;
	//str.Format("need to delete point cloud %d",name);
	//AfxMessageBox(str);

	int counter = 0;

	// end xinju
	vector<PartMeasurement>::iterator iter;
	counter = BIMPartMeasurements.size();
	counter = 0;
	for(iter = BIMPartMeasurements.begin(); iter != BIMPartMeasurements.end() && counter!= name; iter++)
		counter++;

	if(iter == BIMPartMeasurements.end())
		AfxMessageBox("Cannot find PartMeasurement to delete!");
	else
	{
		CString str;
		str.Format("counter is %d",counter);
		BIMPartMeasurements.erase(iter);
	}

}


/***********************************************/
/*
	Function to update the display properties
	(point size and point type) of all part
	measurements
							modified by: Jun
							date: 01/15/05
*/
/***********************************************/
void PartMeasurements::UpdateDisplayProperties()
{
	for(int i = 0 ; i < BIMPartMeasurements.size(); i++)
	{
		BIMPartMeasurements[i].PartMeasurementDisplayProperties.PointSize = PartMeasurementsDisplayProperties.PointSize;
		BIMPartMeasurements[i].PartMeasurementDisplayProperties.PointType = PartMeasurementsDisplayProperties.PointType;
		

	}

}

/***********************************************/
/*
	Function to update the color bar based on
	all the part measurements
							modified by: Jun
							date: 01/21/05
*/
/***********************************************/
void PartMeasurements::UpdatePartMeasurementsColorBar()
{
	double min(1e15);
	double max(1e-15);

	for(int i = 0 ; i < BIMPartMeasurements.size(); i++)
	{
		if(BIMPartMeasurements[i].PartMeasurementFlags.Registered)
		{
			if(BIMPartMeasurements[i].PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.max > max)
				max = BIMPartMeasurements[i].PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.max;

			if(BIMPartMeasurements[i].PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.min < min)
				min = BIMPartMeasurements[i].PartMeasurementDisplayProperties.DisplayPropertiesColorToolBarOptions.min;
		}
	}

	PartMeasurementsDisplayProperties.DisplayPropertiesColorToolBarOptions.max = max;
	PartMeasurementsDisplayProperties.DisplayPropertiesColorToolBarOptions.min = min;


}

// Saves out any point clouds that do have a filename associated with them
void PartMeasurements::SaveScans(CWnd* myWin)
{
	for(int i = 0; i < BIMPartMeasurements.size(); i++)
	{
		if(BIMPartMeasurements[i].FileName.size() == 0)
		{
			string filePath;
			string fileExt;

			BIMPartMeasurements[i].PartMeasurementFlags.Selected == true;
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

			BIMPartMeasurements[i].FileName = filePath;
			BIMPartMeasurements[i].WritePartMeasurement(filePath);
		}
	}

	return;
}