#include "stdafx.h"
#include "MainFrm.h"
#include "Registration.h"
#include "SVD/jama_svd.h"
#include "meshbuilder.h"
#include "PointClouds.h"
#include <vector>
#include <math.h>
#include <stdio.h>
#include <ANN\ANN.h>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace JAMA;

// Calculates the distances from a point cloud to a mesh without doing any registration
void CalculateDistances(PartMeasurement& point_cloud, SimpleMeshT* BIMMesh, Options BIMOptions, CWnd* myWin)
{
	////////////////////////////////////////////////////////////////
	// Initialize mesh data structures outside loop.
	vector<SimpleMeshT::VertexCt::iterator> all_vertices;  // vector that holds ALL vertices in the mesh, will be used to get the nearest vertices.	
	ANNkd_tree* kd_tree;  // Pointer to kd_tree
	ANNpointArray mesh_data_points; // data_points for the 
	mesh_data_points = annAllocPts(BIMMesh->Verts().size(), DIMENSION); // Allocate the array that will hold our vertices in the kd_tree


	// Set std_dev_filter
	point_cloud.std_dev_filter = BIMOptions.ICP_std_dev_filter;
	point_cloud.NormalAgreementFilter = BIMOptions.ICP_normal_agreement_filter;
	// Initialize our initial mesh point arrays.
	int z = 0;
	for(SimpleMeshT::VertexCt::iterator vi = BIMMesh->VertsBegin(); vi != BIMMesh->VertsEnd(); vi++)
	{
		//push back iterator onto our vector that allows us to search the faces.
		all_vertices.push_back(vi);
		//initialize our ANN vertex list	

		mesh_data_points[z][0] = (*vi)->m_pos.x();
		mesh_data_points[z][1] = (*vi)->m_pos.y();
		mesh_data_points[z][2] = (*vi)->m_pos.z();
		z++;
	}

	// Have ANN construct our 3 Dimensional kd_tree
	kd_tree = new ANNkd_tree(
		mesh_data_points,
		BIMMesh->Verts().size(),
		DIMENSION);
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// this vector is the transformed point cloud
	vector<XVecf> transformed_pointcloud;
	float* mat = NULL;
	mat = point_cloud.PartMeasurementTransMatrix.Transpose();

	ANNpointArray transformed_pt_cld; // transformed pt cloud points for query points
	ANNidxArray idx_array; // Holds index into the mesh_data_points.  Holds multiple indices if we want multiple results
	ANNdistArray dist_array; // Holds distances to returned points.

	transformed_pt_cld = annAllocPts(point_cloud.PartMeasurementSampledPoints.BIMSampledPoints.size(), DIMENSION);
	idx_array = new ANNidx[BIMOptions.ICP_num_nearest_neighbor];
	dist_array = new ANNdist[BIMOptions.ICP_num_nearest_neighbor];

	// Initialize arrays that hold or transformed point cloud by applying the xform matrix to it.
	for(int i = 0 ; i < point_cloud.PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
	{
		XVecf pt(point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].x,
			point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].y,
			point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].z);

		XVecf temp = mult(mat, pt);

		transformed_pointcloud.push_back(temp);
		// Initialize query points for kd_tree
		transformed_pt_cld[i][0] = temp.x();
		transformed_pt_cld[i][1] = temp.y();
		transformed_pt_cld[i][2] = temp.z();
	}
	////////////////////////////////////////////////////////////

	// Release the memory for this matrix.
	delete[] mat;
	mat = NULL;

	double min_distance = 1e15;  // initially, make the min distance huge

	// This vector holds the pointers to all the closest vertices in the mesh.  These pointers
	// allow us to access the faces in constant time.
	vector<SimpleMeshT::VertexCt::iterator> nearest_iters;


	// Search for each nearest point in the mesh, and push back the corresponding iterator.
	for(int i = 0; i < transformed_pointcloud.size(); i++)
	{
		// Perform kd_tree search
		// We are looking for just the one closest result, so it will return just one idx. 
		kd_tree->annkSearch(transformed_pt_cld[i],
			BIMOptions.ICP_num_nearest_neighbor,
			idx_array,
			dist_array);
		// push back our returned index from the vector of ALL mesh iterators.
		for (int j = 0; j < BIMOptions.ICP_num_nearest_neighbor; j++)
			nearest_iters.push_back(all_vertices[idx_array[j]]);
	}


	// Loops to calculate Nearest points. (does the projection for me after finding the nearest vertices)
	vector<XVecfn> nearestPts;
	vector<XVecfn> nearestPtsNormal;
	vector<double> distances;

	for(int i = 0 ; i < (nearest_iters.size() / BIMOptions.ICP_num_nearest_neighbor); i++)
	{
		vector<XVecfn> closestPt;
		vector<XVecfn> closestPtNormal;
		closestPt.resize(BIMOptions.ICP_num_nearest_neighbor);
		closestPtNormal.resize(BIMOptions.ICP_num_nearest_neighbor);
		XVecfn actClosestPt;
		XVecfn actClosestPtNormal;

		for (int j = 0; j < BIMOptions.ICP_num_nearest_neighbor; j++)
		{
			SimpleMeshT::VertexCt::iterator vi = nearest_iters[i*BIMOptions.ICP_num_nearest_neighbor + j];
			SimpleMeshT::UmbrellaIt ui(*vi);

			SimpleDEdgeT de; 
			//				int valence = (*vi)->Valence();
			bool firststep = true;
			// set nearest point to vertex
			XVecf temp((*vi)->m_pos.x(),(*vi)->m_pos.y(),(*vi)->m_pos.z());
			XVecf tempNormal((*vi)->m_normal.x(),(*vi)->m_normal.y(),(*vi)->m_normal.z()); 
			closestPt[j].pt = temp;
			closestPt[j].rn = false;
			// for this version, the closest point normal will be set by the closets verstex normal. In later versions it should be calculated by a linear combination taking into account surrounding vertex normals
			closestPtNormal[j].pt = tempNormal; 

			// distance is now dis squared to vertex
			double dis = (closestPt[j].pt.x()-transformed_pointcloud[i].x()) * (closestPt[j].pt.x()-transformed_pointcloud[i].x())+
				(closestPt[j].pt.y()-transformed_pointcloud[i].y()) * (closestPt[j].pt.y()-transformed_pointcloud[i].y())+
				(closestPt[j].pt.z()-transformed_pointcloud[i].z()) * (closestPt[j].pt.z()-transformed_pointcloud[i].z());

			while(de=ui.Next())
			{
				// vertices on a face
				XVecf point[3];

				if(!de.Face() == NULL)
				{
					point[0] = de.Org()->Pos();
					point[1] = de.Dest()->Pos();
					point[2] = de.Enext().Dest()->Pos();

					// if you want the 3 vertices in CCW orientaion, please use the orientation
					// (point1,point3,point2)
					bool onFace;
					bool normal_reversed;
					XVecf returned_point;
					XVecf temp;
					// Perform BaryCentric coordinate projection.
					returned_point = FindNearestPointOnFace(transformed_pointcloud[i],point[0],point[2],point[1],onFace,normal_reversed);

					if(onFace)
					{
						double d;
						d = (returned_point.x()-transformed_pointcloud[i].x()) * (returned_point.x()-transformed_pointcloud[i].x())+
							(returned_point.y()-transformed_pointcloud[i].y()) * (returned_point.y()-transformed_pointcloud[i].y())+
							(returned_point.z()-transformed_pointcloud[i].z()) * (returned_point.z()-transformed_pointcloud[i].z());

						if (d < dis)
						{
							dis = d;
							closestPt[j].pt = returned_point;
							closestPt[j].rn = normal_reversed;
						}
					}
					else
					{
						double d;
						for (int z = 0; z < 3; z++)
						{
							XVecf pt;
							bool onEdge;

							pt = ProjectOntoEdges(transformed_pointcloud[i], point[z], point[((z+1)%3)], onEdge);

							d = (pt.x()-transformed_pointcloud[i].x()) * (pt.x()-transformed_pointcloud[i].x())+
								(pt.y()-transformed_pointcloud[i].y()) * (pt.y()-transformed_pointcloud[i].y())+
								(pt.z()-transformed_pointcloud[i].z()) * (pt.z()-transformed_pointcloud[i].z());

							if ( (d < dis)  && (true == onEdge))
							{
								dis = d;
								closestPt[j].pt = pt;
								closestPt[j].rn = false;
							}
						}
					}
				}
			}

			if (BIMOptions.ICP_2ringfaces == BST_UNCHECKED)
			{
				// This is a test piece of code to see how effective doing a second ring of faces does.
				SimpleMeshT::UmbrellaIt ut(*vi); // getting the 2nd ring vertices.
				SimpleDEdgeT dedge;

				// Loop through second ring vertices and project onto all their faces. (Some redundant calculations here)
				while (dedge = ut.Next())
				{
					SimpleMeshT::UmbrellaIt um(dedge.Dest());
					SimpleDEdgeT edge;
					while(edge = um.Next())
					{
						// vertices on a face
						XVecf point[3];

						if(!edge.Face() == NULL)
						{
							point[0] = edge.Org()->Pos();
							point[1] = edge.Dest()->Pos();
							point[2] = edge.Enext().Dest()->Pos();

							// if you want the 3 vertices in CCW orientaion, please use the orientation
							// (point1,point3,point2)
							bool onFace;
							bool normal_reversed;
							XVecf returned_point;
							XVecf temp;
							// Perform BaryCentric coordinate projection.
							returned_point = FindNearestPointOnFace(transformed_pointcloud[i],point[0],point[2],point[1],onFace, normal_reversed);

							if(onFace)
							{
								double d;
								d = (returned_point.x()-transformed_pointcloud[i].x()) * (returned_point.x()-transformed_pointcloud[i].x())+
									(returned_point.y()-transformed_pointcloud[i].y()) * (returned_point.y()-transformed_pointcloud[i].y())+
									(returned_point.z()-transformed_pointcloud[i].z()) * (returned_point.z()-transformed_pointcloud[i].z());

								if (d < dis)
								{
									dis = d;
									closestPt[j].pt = returned_point;
									closestPt[j].rn = normal_reversed;
								}
							}
							else
							{
								double d;
								for (int z = 0; z < 3; z++)
								{
									XVecf pt;
									bool onEdge;

									pt = ProjectOntoEdges(transformed_pointcloud[i], point[z], point[((z+1)%3)], onEdge);

									d = (pt.x()-transformed_pointcloud[i].x()) * (pt.x()-transformed_pointcloud[i].x())+
										(pt.y()-transformed_pointcloud[i].y()) * (pt.y()-transformed_pointcloud[i].y())+
										(pt.z()-transformed_pointcloud[i].z()) * (pt.z()-transformed_pointcloud[i].z());

									if ( (d < dis)  && (true == onEdge))
									{
										dis = d;
										closestPt[j].pt = pt;
										closestPt[j].rn = false;
									}
								}
							}
						}
					}
				}
			}
		}

		double dist = 1e15; // make this big for the first iteration.
		for(int k = 0; k < BIMOptions.ICP_num_nearest_neighbor; k++)
		{
			double d = (closestPt[k].pt.x()-transformed_pointcloud[i].x()) * (closestPt[k].pt.x()-transformed_pointcloud[i].x())+
				(closestPt[k].pt.y()-transformed_pointcloud[i].y()) * (closestPt[k].pt.y()-transformed_pointcloud[i].y())+
				(closestPt[k].pt.z()-transformed_pointcloud[i].z()) * (closestPt[k].pt.z()-transformed_pointcloud[i].z());
			if(d < dist)
			{
				actClosestPt = closestPt[k];
				actClosestPtNormal = closestPtNormal[k];
				dist=d;
			}
		}

		//Push back distances
		//double dist = sqrt(abs(dis));
		//distances.push_back(dist);
		// push back closest point
		nearestPts.push_back(actClosestPt);
		nearestPtsNormal.push_back(actClosestPtNormal);

	}


	// Set closest points 
	point_cloud.SetClosestPoints(nearestPts,nearestPtsNormal);
	// Set distances
	point_cloud.UpdateDistances();
	// calc std dev
	point_cloud.CalculateStdDev();

	// Set that this point cloud has been registered
	point_cloud.PartMeasurementFlags.Registered = true;

	// Update the color bar for this point cloud
	point_cloud.UpdatePartMeasurementColorBar();
	myWin->Invalidate(TRUE);
	myWin->UpdateWindow();

	delete[] kd_tree;
	annDeallocPts(mesh_data_points);
	return;

}

// RunICP()
// PartMeasurement& point_cloud: Part measurement we are registering
// SimpleMeshT* BIMMesh: CAD model we are trying to register point cloud to
// CWnd* myWin: MFC window object passed in to redraw window after every iteration of algorithm.
bool RunICP(PartMeasurement& point_cloud ,PartMesh &Part, CWnd* myWin, Options BIMOptions)
{	
	// Minimum distance error between averages
	double ratio = 100000;
	bool LastIteration = false;
	vector<double> errors;

	SimpleMeshT* BIMMesh = Part.GetMesh();

	int counter = 0;

	CMainFrame *pFrame;
	CString str;

	// Initialize mesh data structures outside loop.
	vector<SimpleMeshT::VertexCt::iterator> all_vertices;  // vector that holds ALL vertices in the mesh, will be used to get the nearest vertices.	
	ANNkd_tree* kd_tree;  // Pointer to kd_tree
	ANNpointArray mesh_data_points; // data_points for the 
	mesh_data_points = annAllocPts(BIMMesh->Verts().size(), DIMENSION); // Allocate the array that will hold our vertices in the kd_tree


	// Set std_dev_filter
	point_cloud.std_dev_filter = BIMOptions.ICP_std_dev_filter;
	point_cloud.NormalAgreementFilter = BIMOptions.ICP_normal_agreement_filter;

	// Initialize our initial mesh point arrays.
	int z = 0;
	for(SimpleMeshT::VertexCt::iterator vi = BIMMesh->VertsBegin(); vi != BIMMesh->VertsEnd(); vi++)
	{
		//push back iterator onto our vector that allows us to search the faces.
		all_vertices.push_back(vi);
		//initialize our ANN vertex list	

		mesh_data_points[z][0] = (*vi)->m_pos.x();
		mesh_data_points[z][1] = (*vi)->m_pos.y();
		mesh_data_points[z][2] = (*vi)->m_pos.z();
		z++;
	}

	// Have ANN construct our 3 Dimensional kd_tree
	kd_tree = new ANNkd_tree(
		mesh_data_points,
		BIMMesh->Verts().size(),
		DIMENSION);


	do{ 
		// perform iterations until the LastIterion flag is true
		
		// check whether this is the last iteration whose purpose is getting the distances right
		LastIteration = !(ratio > BIMOptions.ICP_epsilon && counter <= BIMOptions.ICP_num_iterations);

		// this vector is the transformed point cloud
		vector<XVecf> transformed_pointcloud;
		float* mat = NULL;
		mat = point_cloud.PartMeasurementTransMatrix.Transpose();

		ANNpointArray transformed_pt_cld; // transformed pt cloud points for query points
		ANNidxArray idx_array; // Holds index into the mesh_data_points.  Holds multiple indices if we want multiple results
		ANNdistArray dist_array; // Holds distances to returned points.

		transformed_pt_cld = annAllocPts(point_cloud.PartMeasurementSampledPoints.BIMSampledPoints.size(), DIMENSION);
		idx_array = new ANNidx[BIMOptions.ICP_num_nearest_neighbor];
		dist_array = new ANNdist[BIMOptions.ICP_num_nearest_neighbor];

		// Initialize arrays that hold or transformed point cloud by applying the xform matrix to it.
		for(int i = 0 ; i < point_cloud.PartMeasurementSampledPoints.BIMSampledPoints.size(); i++)
		{

			XVecf pt(point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].x,
				point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].y,
				point_cloud.PartMeasurementSampledPoints.BIMSampledPoints[i].z);

			XVecf temp = mult(mat, pt);

			transformed_pointcloud.push_back(temp);
			// Initialize query points for kd_tree
			transformed_pt_cld[i][0] = temp.x();
			transformed_pt_cld[i][1] = temp.y();
			transformed_pt_cld[i][2] = temp.z();
		}

		// Release the memory for this matrix.
		delete[] mat;
		mat = NULL;

		double min_distance = 1e15;  // initially, make the min distance huge

		// This vector holds the pointers to all the closest vertices in the mesh.  These pointers
		// allow us to access the faces in constant time.
		vector<SimpleMeshT::VertexCt::iterator> nearest_iters;


		// Search for each nearest point in the mesh, and push back the corresponding iterator.
		for(int i = 0; i < transformed_pointcloud.size(); i++)
		{
			// Perform kd_tree search
			// We are looking for just the one closest result, so it will return just one idx. 
			kd_tree->annkSearch(transformed_pt_cld[i],
				BIMOptions.ICP_num_nearest_neighbor,
				idx_array,
				dist_array);
			// push back our returned index from the vector of ALL mesh iterators.
			for (int j = 0; j < BIMOptions.ICP_num_nearest_neighbor; j++)
				nearest_iters.push_back(all_vertices[idx_array[j]]);
		}


		// Loops to calculate Nearest points. (does the projection for me after finding the nearest vertices)
		vector<XVecfn> nearestPts;
		vector<XVecfn> nearestPtsNormal;
		vector<double> distances;

		for(int i = 0 ; i < (nearest_iters.size() / BIMOptions.ICP_num_nearest_neighbor); i++)
		{
			vector<XVecfn> closestPt;
			vector<XVecfn> closestPtNormal;
			closestPt.resize(BIMOptions.ICP_num_nearest_neighbor);
			closestPtNormal.resize(BIMOptions.ICP_num_nearest_neighbor);
			XVecfn actClosestPt;
			XVecfn actClosestPtNormal;

			for (int j = 0; j < BIMOptions.ICP_num_nearest_neighbor; j++)
			{
				SimpleMeshT::VertexCt::iterator vi = nearest_iters[i*BIMOptions.ICP_num_nearest_neighbor + j];
				SimpleMeshT::UmbrellaIt ui(*vi);

				SimpleDEdgeT de; 
				//				int valence = (*vi)->Valence();
				bool firststep = true;
				// set nearest point to vertex
				XVecf temp((*vi)->m_pos.x(),(*vi)->m_pos.y(),(*vi)->m_pos.z());
				XVecf tempNormal((*vi)->m_normal.x(),(*vi)->m_normal.y(),(*vi)->m_normal.z()); 
				closestPt[j].pt = temp;
				closestPt[j].rn = false;
				// for this version, the closest point normal will be set by the closets verstex normal. In later versions it should be calculated by a linear combination taking into account surrounding vertex normals
				closestPtNormal[j].pt = tempNormal; 

				// distance is now dis squared to vertex
				double dis = (closestPt[j].pt.x()-transformed_pointcloud[i].x()) * (closestPt[j].pt.x()-transformed_pointcloud[i].x())+
					(closestPt[j].pt.y()-transformed_pointcloud[i].y()) * (closestPt[j].pt.y()-transformed_pointcloud[i].y())+
					(closestPt[j].pt.z()-transformed_pointcloud[i].z()) * (closestPt[j].pt.z()-transformed_pointcloud[i].z());

				while(de=ui.Next())
				{
					// vertices on a face
					XVecf point[3];

					if(!de.Face() == NULL)
					{
						point[0] = de.Org()->Pos();
						point[1] = de.Dest()->Pos();
						point[2] = de.Enext().Dest()->Pos();

						// if you want the 3 vertices in CCW orientaion, please use the orientation
						// (point1,point3,point2)
						bool onFace;
						bool normal_reversed;
						XVecf returned_point;
						XVecf temp;
						// Perform BaryCentric coordinate projection.
						returned_point = FindNearestPointOnFace(transformed_pointcloud[i],point[0],point[2],point[1],onFace,normal_reversed);

						if(onFace)
						{
							double d;
							d = (returned_point.x()-transformed_pointcloud[i].x()) * (returned_point.x()-transformed_pointcloud[i].x())+
								(returned_point.y()-transformed_pointcloud[i].y()) * (returned_point.y()-transformed_pointcloud[i].y())+
								(returned_point.z()-transformed_pointcloud[i].z()) * (returned_point.z()-transformed_pointcloud[i].z());

							if (d < dis)
							{
								dis = d;
								closestPt[j].pt = returned_point;
								closestPt[j].rn = normal_reversed;
							}
						}
						else
						{
							double d;
							for (int z = 0; z < 3; z++)
							{
								XVecf pt;
								bool onEdge;

								pt = ProjectOntoEdges(transformed_pointcloud[i], point[z], point[((z+1)%3)], onEdge);

								d = (pt.x()-transformed_pointcloud[i].x()) * (pt.x()-transformed_pointcloud[i].x())+
									(pt.y()-transformed_pointcloud[i].y()) * (pt.y()-transformed_pointcloud[i].y())+
									(pt.z()-transformed_pointcloud[i].z()) * (pt.z()-transformed_pointcloud[i].z());

								if ( (d < dis)  && (true == onEdge))
								{
									dis = d;
									closestPt[j].pt = pt;
									closestPt[j].rn = false;
								}
							}
						}
					}
				}

				if (BIMOptions.ICP_2ringfaces == BST_UNCHECKED)
				{
					// This is a test piece of code to see how effective doing a second ring of faces does.
					SimpleMeshT::UmbrellaIt ut(*vi); // getting the 2nd ring vertices.
					SimpleDEdgeT dedge;

					// Loop through second ring vertices and project onto all their faces. (Some redundant calculations here)
					while (dedge = ut.Next())
					{
						SimpleMeshT::UmbrellaIt um(dedge.Dest());
						SimpleDEdgeT edge;
						while(edge = um.Next())
						{
							// vertices on a face
							XVecf point[3];

							if(!edge.Face() == NULL)
							{
								point[0] = edge.Org()->Pos();
								point[1] = edge.Dest()->Pos();
								point[2] = edge.Enext().Dest()->Pos();

								// if you want the 3 vertices in CCW orientaion, please use the orientation
								// (point1,point3,point2)
								bool onFace;
								bool normal_reversed;
								XVecf returned_point;
								XVecf temp;
								// Perform BaryCentric coordinate projection.
								returned_point = FindNearestPointOnFace(transformed_pointcloud[i],point[0],point[2],point[1],onFace, normal_reversed);

								if(onFace)
								{
									double d;
									d = (returned_point.x()-transformed_pointcloud[i].x()) * (returned_point.x()-transformed_pointcloud[i].x())+
										(returned_point.y()-transformed_pointcloud[i].y()) * (returned_point.y()-transformed_pointcloud[i].y())+
										(returned_point.z()-transformed_pointcloud[i].z()) * (returned_point.z()-transformed_pointcloud[i].z());

									if (d < dis)
									{
										dis = d;
										closestPt[j].pt = returned_point;
										closestPt[j].rn = normal_reversed;
									}
								}
								else
								{
									double d;
									for (int z = 0; z < 3; z++)
									{
										XVecf pt;
										bool onEdge;

										pt = ProjectOntoEdges(transformed_pointcloud[i], point[z], point[((z+1)%3)], onEdge);

										d = (pt.x()-transformed_pointcloud[i].x()) * (pt.x()-transformed_pointcloud[i].x())+
											(pt.y()-transformed_pointcloud[i].y()) * (pt.y()-transformed_pointcloud[i].y())+
											(pt.z()-transformed_pointcloud[i].z()) * (pt.z()-transformed_pointcloud[i].z());

										if ( (d < dis)  && (true == onEdge))
										{
											dis = d;
											closestPt[j].pt = pt;
											closestPt[j].rn = false;
										}
									}
								}
							}
						}
					}
				}
			}


			double dist = 1e15; // make this big for the first iteration.
			for(int k = 0; k < BIMOptions.ICP_num_nearest_neighbor; k++)
			{
				double d = (closestPt[k].pt.x()-transformed_pointcloud[i].x()) * (closestPt[k].pt.x()-transformed_pointcloud[i].x())+
					(closestPt[k].pt.y()-transformed_pointcloud[i].y()) * (closestPt[k].pt.y()-transformed_pointcloud[i].y())+
					(closestPt[k].pt.z()-transformed_pointcloud[i].z()) * (closestPt[k].pt.z()-transformed_pointcloud[i].z());
				if(d < dist)
				{
					actClosestPt = closestPt[k];
					actClosestPtNormal = closestPtNormal[k];
					dist=d;
				}
			}
			//Push back distances
			//double dist = sqrt(abs(dis));
			//distances.push_back(dist);
			// push back closest point
			nearestPts.push_back(actClosestPt);
			nearestPtsNormal.push_back(actClosestPtNormal);
		}

		// Set closest points 
		point_cloud.SetClosestPoints(nearestPts,nearestPtsNormal);

		// Set distances
		point_cloud.UpdateDistances();

		// calc std dev
		point_cloud.CalculateStdDev();


		//vector<XVecf> temp_nearestPts;
		//for (int i = 0; i < nearestPts.size(); i++)
		//	temp_nearestPts.push_back(nearestPts[i].pt);

		vector<XVecf> temp_nearestPts;
		vector<XVecf> temp_TransformedPts;

		int PtsFiltered; 	
		PtsFiltered = point_cloud.FilterPointtsForICP(temp_TransformedPts,temp_nearestPts);

		// Perfoming all the matrix calculations to find the new transformation matrix
		float* matrix;
		float* Xmatrix;
		float* TransformMatrix;
		XVecf TranslationVector;
		// Construct H Matrix
		matrix = ConstructHMatrix(temp_TransformedPts, temp_nearestPts);
		// Pass to SVD
		Xmatrix = DecomposeHMatrix(matrix);

		//Do test
		//Not really calculating a determinant.
		//Using property of an orthogonal matrix, which is it is composed of
		//three perpindicular row vectors a,b,c.  det(a,b,c) = a dot (b cross c)
		//http://en.wikipedia.org/wiki/Cross_product
		float chck = CalculateDet(Xmatrix);
		if (!(chck <= 1.15 && chck >= 0.85)) // check a judicious range because its a float and won't always be EXACTLY 1, but very close
		{

			TRACE("Matrix is not a rotation matrix!");
			str.Format("Finish registering by ICP!");
			pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_wndStatusBar.SetPaneText(0,str);

			delete[] matrix;
			delete[] Xmatrix;
			annDeallocPts(transformed_pt_cld);
			delete[] kd_tree;
			annDeallocPts(mesh_data_points);
			delete[] idx_array;
			delete[] dist_array;

			MessageBeep(MB_ICONEXCLAMATION);
			return (false);
		}

		// Convert rotation matrix to 4x4
		TransformMatrix = convert3x3to4x4(Xmatrix);

		// Find translation vector
		TranslationVector = FindAverage(temp_nearestPts) - mult(TransformMatrix, FindAverage(temp_TransformedPts));

		//Get final transformation matrix
		TransformMatrix[3] = TranslationVector.x();
		TransformMatrix[7] = TranslationVector.y();
		TransformMatrix[11] = TranslationVector.z();
		TransformMatrix[15] = 1.0;

		// ************************************************************
		// Apply transform and iterate until convergence
		// ************************************************************
		if (!LastIteration) // If this the last iteration do not apply the new matrix as the purpose if the iteration is to calculate the distances
			point_cloud.PartMeasurementTransMatrix.multTransformMatrix(TransformMatrix);
		
		double error;
		error = CalculateError(TransformMatrix, temp_nearestPts, temp_TransformedPts);
		errors.push_back(error);

		// get ratio of errors
		if(errors.size() > 1)
			ratio = CalculateErrorRatio(errors[errors.size() - 1], errors[errors.size() - 2]);

		// increment counter , later check with number of iterations requested
		counter++;

		// Clean up memory
		delete[] matrix;
		delete[] Xmatrix;
		delete[] TransformMatrix;
		delete[] idx_array;
		delete[] dist_array;
		annDeallocPts(transformed_pt_cld);

		// Set that this point cloud has been registered
		point_cloud.PartMeasurementFlags.Registered = true;

		// Update the color bar for this point cloud
		point_cloud.UpdatePartMeasurementColorBar();
		myWin->Invalidate(TRUE);
		myWin->UpdateWindow();

		// NOTE: DE-ALLOCATE ALL HEAP MEMORY (kd_tree structures)
		
	} while(!LastIteration);

	delete[] kd_tree;
	annDeallocPts(mesh_data_points);

	TRACE("Finish registering by ICP!");
	str.Format("Finish registering by ICP!");
	pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.SetPaneText(0,str);

	MessageBeep(MB_ICONEXCLAMATION);

	// return success if the 
	double Diag = Part.PartMeshDisplayProperties.DisplayPropertiesBBox.DiagSize();
	// check if registration error is smaller than predefined success criteria of
	// Distance STD / BBox diag < AcceptAlignment threshold
	bool IsSuccess = (point_cloud.DistStd / Diag) < BIMOptions.AcceptAlignmentThreshold;

	return (IsSuccess);
}



bool RunAutoAlign(PartMeasurement& ptCloud, PartMesh& Part, CWnd* myWin, Options BIMOptions)
{

	// It is assumed that PartMeasurement is passed corectly, 
	// otherwise a check for this should be added.

	if(!Part.m_mesh) // if mesh does not exist, just return
	{	
		AfxMessageBox("Mesh does defined, cannot autoalign");
		return (false);
	}

	if(!Part.SigCalculated) // if signature not calculates exit
	{	
		AfxMessageBox("Singature not calcualted, please save a signature file to cause signature calculation");
		return (false);
	}

	// mark that there was an attempt to autoalign the point cloud
	ptCloud.PartMeasurementFlags.AutoAlignAttempted=true;
	double PartDiagonalSize = Part.PartMeshDisplayProperties.DisplayPropertiesBBox.DiagSize();

	PointClouds* PC = ptCloud.PC;

	/* Build the kdtree using method PointClouds::BuildKDTree 
	the first parameter is the number of points,
	the second parameter include the positions of these points
	the third parameter include the normals of these points,it could be
	a null array because normals may not be available.
	If you has the normal info, set the forth parameter to be "true", 
	otherwise, it's "false".
	*/
	//PC->BuildKDTree(pts.size(), pos, norms, true);
	//	PCs->BuildKDTree(pts.size(), pos, norms, false);

	/*  After build the kdtree, computer the radius used for smoothin */ 	
	PC->ComputeRadius();
	/*  If normals are not availble, use PointClouds::ComputeNormal to compute it 
	Note: this function is only good for scans, please do not use it to compute
	normals for obj files */
	//PC->ComputeNormal();
	/*  Also good for the scans */
	if (!BIMOptions.IncludeBoundaryFeatures)
		PC->FindBoundary();
	/*  Set the number of levels for smoothing*/
	PC->SetLevel(BIMOptions.SmoothingLevels);
	PC->SetStartRadius(BIMOptions.SmoothStartRadius*PartDiagonalSize);
	PC->SetSmoothStep(BIMOptions.SmoothStep);
	if (BIMOptions.UseFeatureRadiusForNeighborhood)
		PC->SetFeatureRadius(false,BIMOptions.FeatureRadius);
	else
		PC->SetFeatureRadius(true,BIMOptions.FeatureRadius*PartDiagonalSize);


	/*  Smooth the point clouds, and find the local maxima or minimal
	which are feature points */
	PC->SmoothPointCloud();
	/*  after find feature points, compute the principle curvature directions
	for these feature points	*/
	PC->ComputeCurvatures();
	/*	scale is the parameter you want to control the error for approximate matching
	it can be set as something like 5% of the bounding box */
	double scale = PC->GetSmoothRadius();
	scale = PC->GetScale();
	scale *= 0.05;
	// scale = BIMOptions.scale_level*sqrt(scale);

	//PC1->SetLevel(auto_align.smooth_levels);
	//PC1->SmoothPointCloud();
	//PC1->ComputeCurvatures();

	/*	You can use this function to match one pointcloud to the other one, like transform
	PCs to PCs1's coordinate here. Both of them should be smoothed*/
	float Trans[3][4];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				Trans[i][j] = 1.0;
			}
			else
			{
				Trans[i][j] = 0.0;
			}
		}
	}
	bool success = PC->MatchTagentFromFile( (char *) Part.SigFileName.c_str() , scale, Trans);

	if (!success)
	{
		TRACE("Failed Auto-align\n");
	}
	else{

		float* trans = new float[16];
		for (int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				trans[4*i + j] = Trans[i][j];
			}
		}

		trans[12] = 0.0;
		trans[13] = 0.0;
		trans[14] = 0.0;
		trans[15] = 1.0;

		ptCloud.PartMeasurementTransMatrix.multTransformMatrix(trans);

	}
	myWin->Invalidate(TRUE);
	myWin->UpdateWindow();

	TRACE("Finished Auto-Aligning!");
	CMainFrame *pFrame;
	CString str;
	str.Format("Finished Auto-Aligning!");
	pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.SetPaneText(0,str);

	MessageBeep(MB_OK);
	/*	Match the PointCloud PCs to another one (let name it PC1). 
	The information of the PC1 will be loaded from "test.txt" 
	The format of the file is:
	First line has two numbers: #of features N1 and  #of points N2
	From the second line to (N1+1)th line: all the information of feature points 
	From the (N1+2)th line: the positions fo all the points	
	*/
	//PCs->MatchTagentFromFile("test.txt", scale);
	return (success); // output the success
}



// Calculate XForm matrix for 2 sets of 3 points
// Used for the interfaces 3 point select and match function.
float* ThreePtSelectAndXForm(deque<XVecf> pairOne, deque<XVecf> pairTwo)
{
	vector<XVecf> pt_cloud;
	vector<XVecf> mesh;

	for (int i = 0; i < 3; i++)
	{
		pt_cloud.push_back(pairOne[i]);
		mesh.push_back(pairTwo[i]);
	}
	float* matrix;
	float* Xmatrix;
	float* TransformMatrix;
	XVecf TranslationVector;
	// Construct H Matrix
	matrix = ConstructHMatrix(pt_cloud, mesh);
	// Pass to SVD
	Xmatrix = DecomposeHMatrix(matrix);

	//Do test
	//Not really calculating a determinant.
	//Using property of an orthogonal matrix, which is it is composed of
	//three perpindicular row vectors a,b,c.  det(a,b,c) = a dot (b cross c)
	//http://en.wikipedia.org/wiki/Cross_product
	float chck = CalculateDet(Xmatrix);
	if (!(chck <= 1.15 && chck >= 0.85)) // check a judicious range because its a float and won't always be EXACTLY 1, but very close
	{
		AfxMessageBox("Matrix is not a rotation matrix!");
		delete[] matrix;
		delete[] Xmatrix;
		return NULL;
	}

	// Convert rotation matrix to 4x4
	TransformMatrix = convert3x3to4x4(Xmatrix);

	// Find translation vector
	TranslationVector = FindAverage(mesh) - mult(TransformMatrix, FindAverage(pt_cloud));

	//Get final transformation matrix
	TransformMatrix[3] = TranslationVector.x();
	TransformMatrix[7] = TranslationVector.y();
	TransformMatrix[11] = TranslationVector.z();
	TransformMatrix[15] = 1.0;

	delete[] matrix;
	delete[] Xmatrix;

	return TransformMatrix;
}


// Calculates the final error between part and point cloud.
double CalculateError(float* TransformMatrix, vector<XVecf> nearestPts, vector<XVecf>transformed_pointcloud)
{
	XVecf errorVec;
	double squared_value;
	double sum_squared;
	sum_squared = 0;

	for(int i = 0; i < transformed_pointcloud.size(); i++)
	{
		errorVec = nearestPts[i] - mult(TransformMatrix, transformed_pointcloud[i]);
		squared_value = errorVec.dot();
		sum_squared += squared_value;
	}
	return sum_squared;
}

// function to find closest vertex on a mesh from a point cloud
// NOTE: Not being used.
XVecf FindClosestPoint(SampledPoint point, SimpleMeshT* BIMMesh)
{
	XVecf temp;

	return temp;
}

//??????
// Function to project the sample point onto the mesh?
// NOTE: Not being used
void Projection()
{

}

// Function to construct the matrix H
float* ConstructHMatrix(vector<XVecf> ptCloud, vector<XVecf> nearestPts)
{
	XVecf* qi;
	XVecf* qiprime;
	float* matrix;

	matrix = new float[9];
	qi = FindQi(ptCloud);
	qiprime = FindQi(nearestPts);

	// initialize matrix
	for(int i = 0; i < 9; i++) matrix[i] = 0;

	for(int i = 0; i < ptCloud.size(); i++)
	{
		// Construct H Matrix
		float* res = outerproduct(qi[i], qiprime[i]);
		add3x3(matrix, res);
		delete[] res;
	}

	//clean up our memory.
	delete[] qi;
	delete[] qiprime;

	return matrix;
}

// Returns an array of points: qi = pi - p
XVecf* FindQi(vector<XVecf> pts)
{
	XVecf p;
	XVecf* qi;

	p = FindAverage(pts);
	qi = new XVecf[pts.size()];
	for (int i = 0; i < pts.size(); i++)
	{
		qi[i] = pts[i] - p;
	}
	return qi;
}

// Take a vector of points and finds the numerical mean of the points.
XVecf FindAverage(vector<XVecf> points)
{
	XVecf sum(0,0,0);
	XVecf ave(0,0,0);

	for (int i = 0; i < points.size(); i++)
	{
		sum += points[i];
	}

	ave = sum / points.size();

	return ave;
}

// Use single value decomposition (SVD) on the matrix H
float* DecomposeHMatrix(float* HMatrix)
{
	float* res;
	Array1D<float> S;
	// create a 3x3 matrix
	Array2D<float> my2Darray(3,3); 
	Array2D<float> U, V_transposed;

	// Initialize our array
	for(int i = 0; i < 3; i++)
	{
		for(int k = 0; k < 3; k++)
		{
			float temp = HMatrix[3*i+k];
			my2Darray[i][k] = HMatrix[3*i + k];
		}
	}

	//my2Darray = ConvertTransMatrixToArray2D(HMatrix);
	SVD<float> mySVD(my2Darray);
	mySVD.getSingularValues(S); // we will not use this
	mySVD.getU(U); // we will need this
	mySVD.getV(V_transposed); // we will need this also

	float U_1D[9]; 
	float V_transposed_1D[9];
	//U_1D = new float[9];
	//V_transposed_1D = new float[9];

	// get our 3x3 arrays into a 1D array we can work with.
	for(int m = 0; m < 3; m++)
	{
		for(int n = 0; n < 3; n++)
		{
			float temp1 = U[m][n];
			float temp2 = V_transposed[m][n];
			U_1D[3*m + n] = U[m][n];
			V_transposed_1D[3*m + n] = V_transposed[m][n];
		}
	}
	//U_1D = ConvertArray2DToTransMatrix(U);
	//V_transposed_1D = ConvertArray2DToTransMatrix(V_transposed);
	transpose3x3(U_1D);
	// SVD Was already returning V, not V transposed.
	//transpose3x3(V_transposed_1D);

	res = mult3x3(V_transposed_1D, U_1D);

	//delete[] U_1D;
	//delete[] V_transposed_1D;

	return res;
}

// Function to calculate the "determinant" of a matrix
// Assumes RMatrix is a 3x3 orthogonal matrix.
float CalculateDet(float* RMatrix)
{
	float res;
	XVecf a(RMatrix[0], RMatrix[1], RMatrix[2]);
	XVecf b(RMatrix[3], RMatrix[4], RMatrix[5]);
	XVecf c(RMatrix[6], RMatrix[7], RMatrix[8]);

	res = a.dot((b.cross(c)));
	return res;
}

// Function to calculate the transformation matrix, T
void CalculateT()
{


}


// Function to apply the transformation T to the 
// part measurement
void ApplyTransform(PartMeasurement* PointCloud)
{


}


XVecf ProjectOnFacesAroundVertex(XVecf vertex, XVecf point_to_project)
{
	XVecf projected_point;

	return projected_point;
}


void FindNearestPointToFace(XVecf point/*,Face face*/)
{

}


void FindNearestPointToEdge(XVecf point, XVecf A, XVecf B)
{

}

void CalculateBarycentricCoords(XVecf &u, XVecf &v, XVecf &w, XVecf point, XVecf A, XVecf B, XVecf C)
{

}

double CalculateErrorRatio(double current_error,double previous_error)
{
	double ratio;
	ratio = (abs(current_error-previous_error))/previous_error;
	return ratio;

}