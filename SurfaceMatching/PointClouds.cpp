#include ".\pointclouds.h"
#include "mp/meschach_plus.h"
#include <stdio.h>
#include <math.h>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <map>

using namespace std;
using namespace mes;

PointClouds::PointClouds(void){
	m_data = NULL;
	m_kdtree = NULL;
	m_nidx = new ANNidx[50];   // allocate near neigh indices
	m_dists = new ANNdist[50];
	dim = 3;
	m_eps = 0.0;
	m_radius = 0.0;
	m_radius2 = 0.0;
	NumOfPoints = 0;

	// for smoothing
	m_level = 0;
	level_radius = NULL; 
	smooth_step = sqrt(2.0);
	accuracy = 1e-8;
	numOfExtrema = 0;
	is_smooth = false;
	minmax = NULL;
	TDirection = NULL;
	Normals = NULL;
	Positions = NULL;
	mCurvatures = NULL;
// some parameter to play with
	startRadius = 0;
	smoothStep = false;
	isFeatureRadius = false;
	featureRadius = 6;
}

PointClouds::PointClouds(const PointClouds &_pc){
	dim = _pc.dim;
	m_eps = _pc.m_eps;
	m_radius = _pc.m_radius;
	m_radius2 = _pc.m_radius2;
	NumOfPoints = _pc.NumOfPoints;
	m_level = _pc.m_level;
	smooth_step = _pc.smooth_step;
	accuracy = _pc.accuracy;
	numOfExtrema = _pc.numOfExtrema;
	is_smooth = _pc.is_smooth;
	minmax = _pc.minmax;
	if ( !m_nidx ){
        m_nidx = new ANNidx[50];   // allocate near neigh indices
        m_dists = new ANNdist[50];
	}
	m_data = NULL;
	m_kdtree = NULL;
	level_radius = NULL;
	minmax = NULL;
	TDirection = NULL;
	Normals = NULL;
	Positions = NULL;
	mCurvatures = NULL;
#if 0
	if (_pc.m_data){
		m_data = annAllocPts(NumOfPoints,dim);
		// copy the data for kd_tree
		for ( int i=0; i<NumOfPoints; i++){
			for ( int k=0; k<dim; k++)
				m_data[i][k] = _pc.m_data[i][k];
			BasePoints *BP;
			BP = new BasePoints;
			BP->pos = _pc.PointsCloud[i].pos;
			BP->norm = _pc.PointsCloud[i].norm;
			PointsCloud.push_back(*BP);
		}
		m_kdtree = new ANNkd_tree(			// build search structure
			m_data,			// the data points
			NumOfPoints,			// number of points
			dim);
		ComputeRadius();
		ComputeNormal();
		FindBoundary();
		if(is_smooth){
			SmoothPointCloud();
		}
	}
#endif	
	startRadius = _pc.startRadius;
	smoothStep = _pc.smoothStep;
	isFeatureRadius = _pc.isFeatureRadius;
	featureRadius = _pc.featureRadius;
}

PointClouds& PointClouds::operator = (const PointClouds &_pc){
	dim = _pc.dim;
	m_eps = _pc.m_eps;
	m_radius = _pc.m_radius;
	m_radius2 = _pc.m_radius2;
	NumOfPoints = _pc.NumOfPoints;
	m_level = _pc.m_level;
	smooth_step = _pc.smooth_step;
	accuracy = _pc.accuracy;
	numOfExtrema = _pc.numOfExtrema;
	is_smooth = _pc.is_smooth;
	minmax = _pc.minmax;
	if ( !m_nidx){
        m_nidx = new ANNidx[50];   // allocate near neigh indices
        m_dists = new ANNdist[50];
	}
		m_data = NULL;
	m_kdtree = NULL;
	level_radius = NULL;
	minmax = NULL;
	TDirection = NULL;
	Normals = NULL;
	Positions = NULL;
	mCurvatures = NULL;
#if 0
	if (_pc.m_data){
		m_data = annAllocPts(NumOfPoints,dim);
		// copy the data for kd_tree
		for ( int i=0; i<NumOfPoints; i++){
			for ( int k=0; k<dim; k++)
				m_data[i][k] = _pc.m_data[i][k];
			BasePoints *BP;
			BP = new BasePoints;
			BP->pos = _pc.PointsCloud[i].pos;
			BP->norm = _pc.PointsCloud[i].norm;
			PointsCloud.push_back(*BP);
		}
		m_kdtree = new ANNkd_tree(			// build search structure
			m_data,			// the data points
			NumOfPoints,			// number of points
			dim);
		ComputeRadius();
		ComputeNormal();
		FindBoundary();
		if(is_smooth){
			SmoothPointCloud();
		}
	}
#endif	

	startRadius = _pc.startRadius;
	smoothStep = _pc.smoothStep;
	isFeatureRadius = _pc.isFeatureRadius;
	featureRadius = _pc.featureRadius;
	return *this;
}

PointClouds::~PointClouds(void)
{

	if(m_data)
		annDeallocPts(m_data);
	if ( m_kdtree )
		delete m_kdtree;
	m_kdtree = NULL;
	if ( m_dists )
		delete []m_dists;
	m_dists = NULL;
	if ( m_nidx )
		delete []m_nidx;
	m_nidx = NULL;
	if (level_radius)
		delete []level_radius;
	level_radius = NULL;
	if ( minmax ){
		delete []minmax;
		delete []TDirection;
		delete [] Normals;
		delete []Positions;
		delete []mCurvatures;
		minmax = NULL;
		TDirection = NULL;
		Normals = NULL;
		Positions = NULL;
		mCurvatures = NULL;
	}
//	PointsCloud.clear();
}

bool PointClouds::BuildKDTree(int N, float *Pos, float *Norm, bool has_normal){
	
	if ( N<=0 )
		return false;

	NumOfPoints = N;
	PointsCloud.clear();
	for ( int i=0; i<N; i++){
		//BasePoints *BP = new BasePoints;
		BasePoints BP;
		PointsCloud.push_back(BP);
//		BasePoints BP;
//		PointsCloud.push_back(BP);
	}

	// read position
	m_data = annAllocPts(NumOfPoints,dim);
	for ( int i=0; i<N; i++){
		XVecf pos=XVecf(0, 0, 0);
		for ( int j=0; j<3; j++){
			pos(j) = Pos[i*3+j];
			m_data[i][j] = Pos[i*3+j];
		}
		PointsCloud[i].pos = pos;
	}

	// read normal if available
	if (has_normal){
		for ( int i=0; i<N; i++){
			XVecf normal = XVecf(0, 0, 0);
			for (int j=0; j<3; j++){
				normal(j) = Norm[i*3+j];
			}
			PointsCloud[i].norm = normal;
		}		
	}

	// construct the kd_tree

	if ( !m_kdtree){

		m_kdtree = new ANNkd_tree(			// build search structure
			m_data,			// the data points
			NumOfPoints,			// number of points
			dim);

	}

	return true;
}

void PointClouds::SetCameraData(XVecf center, XVecf direction){
	CamCenter = center;
	CamDir = direction;
}

bool PointClouds::ComputeNormal(){
	if ( NumOfPoints<=0 )
		return false;
	ANNpoint		query_pt;
	int num = 15, n;
	for ( int i=0; i<NumOfPoints; i++){
		query_pt = m_data[i];
		m_kdtree->annkSearch(
			query_pt,
			num,
			m_nidx,
			m_dists,
			0.0);		
	
		mes::MatrixT M(3,3), MQ(3,3);
		mes::VectorT v(3);

		for ( int k=0; k<3; k++)
			for (int l=0; l<3; l++)
				M(k,l) = 0;

	    XVecf P_avg = XVecf(0, 0, 0);
        for ( int j=0; j<num; j++){
            n = m_nidx[j];
            P_avg += PointsCloud[n].pos;
		}
		P_avg /= num;
		for ( int j=0; j<num; j++){
			n = m_nidx[j];
			XVecf detP = PointsCloud[n].pos - P_avg;
			for ( int k=0; k<3; k++){
				for ( int l=0; l<3; l++){
					M(k,l) += detP[k]*detP[l];
				}
			}
		}


		mes::SymEigen(M, MQ, v);
	
		float min = v(0);
		n = 0;
		for ( int j=0; j<3; j++){
			if ( v(j) <min ){
				min = v(j);
				n = j;
			}
		}

		XVecf N;
		float d;
		for ( int j=0; j<3; j++){
			N(j) = MQ(j,n);
			d = N(j);
		}

		XVecf Cam = XVecf(0, 0, 0);
		Cam(2) = -1* m_data[i][2];

//		XVecf Cam = PointsCloud[i].pos - CamCenter;
		if ( N.dot(Cam) < 0)
			N *= -1;
		PointsCloud[i].norm = N;

	}

	return true;
}

void PointClouds::FindBoundary(){
	if ( NumOfPoints<=0){
		return;
	}
	ANNpoint query_pt;
	int num = 32, nn, j;
	float scale = 25 * m_radius;
	XVecf Pi, Pj, detP, Ni, Nj;
	float t, w;	
	float x1, x2, x3;
	float y1, y2, y3;

	int count = 0;
	for (int i=0; i<NumOfPoints; i++ ){
		Pi = PointsCloud[i].pos;
		Ni = PointsCloud[i].norm;

		query_pt = m_data[i];
		std::vector<float> dists;
		std::vector<int> idx;
		num = m_kdtree->annSearchR(
			query_pt,
			idx,
			dists,
			scale,
			0.0);
		
		if ( num< 4){
			PointsCloud[i].is_boundary = BasePoints::BOUNDARY;
			continue;
		}
		//Pi = PointsCloud[i].
		// if they are on the boundary
		std::set<float> angels;
		set <float>::iterator angels_I1, angels_I2;

		angels.insert(0);
		if ( i == idx[0] ){
			nn = idx[1];
			Pj = PointsCloud[nn].pos;
			detP = Pj - Pi;
			t = Ni.dot(detP);
			detP = detP - t*Ni;
			j = 2;
		} else {
			nn = idx[0];
			Pj = PointsCloud[nn].pos;
			detP = Pj - Pi;
			t = Ni.dot(detP);
			detP = detP - t*Ni;
			j = 1;
		}
		detP.normalize();

		for ( j; j<num; j++){
			nn = idx[j];
			if ( nn == i)
				continue;
			
			x1 = Pj(0);
			x2 = Pj(1);
			x3 = Pj(2);

			float d1, d2;
			Pj = PointsCloud[nn].pos;
			Nj = Pj - Pi;			
			t = Nj.dot(Ni);
			
			Nj = Nj - t*Ni;
			Nj.normalize();
			d1 = Ni.l2();
			d2 = detP.l2();
			d1 = Nj.l2();
			d1 = Ni.dot( detP.cross(Nj) );
			if ( d1 > 0 ){
				d2 = detP.dot(Nj);
				angels.insert( acos(d2));
			}else{
				d2 = detP.dot(Nj);
				angels.insert(2*3.1415926-acos(d2));
			}			
		}

		angels.insert(2*3.1415926);
//		cerr<<angels.size()<<endl;

		angels_I1 = angels.begin();
		angels_I2 = angels.begin();
		angels_I1 ++;
		int nnn = angels.size();
		for ( j=1; j<angels.size(); j++){
			x1 = *angels_I1;

			float del_angel = *angels_I1 - *angels_I2;
			if ( abs(del_angel) > 2.0 ){
				PointsCloud[i].is_boundary = BasePoints::BOUNDARY;
			}
			angels_I1 ++;
			angels_I2 ++;
		}

	}

	num = 6.0;
	count = 0;
	for ( int i=0; i<NumOfPoints; i++ ){
		if(PointsCloud[i].is_boundary == BasePoints::BOUNDARY){
			count ++;
			continue;
		}
		query_pt = m_data[i];
		m_kdtree->annkSearch(
			query_pt,
			num,
			m_nidx,
			m_dists,
			0.0);
		for ( j=0; j<num; j++){
			nn = m_nidx[j];
			if ( PointsCloud[nn].is_boundary == BasePoints::BOUNDARY ){
				PointsCloud[i].is_boundary = BasePoints::NEARP;
				break;
			}
		}
	}
	cerr<<"# of boundary points "<<count<<endl;
	count = 0;
	for ( int i=0; i<NumOfPoints; i++ ){
		if(PointsCloud[i].is_boundary != BasePoints::INNERP){
			count ++;
		}
	}
	cerr<<"# of near boundary points "<<count<<endl;
}

float PointClouds::ComputeRadius(){
	if ( NumOfPoints<=0){
        return -1;
	}
	ANNpoint		query_pt;
	float *distances = new float[NumOfPoints];
	for ( int i=0; i<NumOfPoints; i++){
		query_pt = m_data[i];
		m_kdtree->annkSearch(
			query_pt,
			2,
			m_nidx,
			m_dists,
			0.0);
		distances[i] = m_dists[1];
	}	
	m_radius = MedianOfArray(distances, NumOfPoints);
	// now round to the neareast 2's power
	m_radius = log(m_radius)/log(2.0);
	int is = (int(m_radius - 0.5));
	m_radius = pow(2.0, is);
	cerr<<"m_radius is:		"<<sqrt(m_radius)<<endl;
	delete []distances;
	distances = NULL;
	return m_radius;
}

// With normals, smooth the surface to find features
void PointClouds::SmoothPointCloud(){
	// for every points, more points
	if ( m_level <= 3 ){
		std::cerr<<"Not enough levels: Levels = "<<m_level<<endl;
		return;
	}
	if ( NumOfPoints<=0){
		std::cerr<<"No points"<<endl;
        return;
	}
	level_radius = new float [m_level];
	for(int i=0; i<NumOfPoints; i++){
		PointsCloud[i].SetLevel(m_level);
		PointsCloud[i].m_max = new unsigned int [m_level];
		PointsCloud[i].pos1 = new XVecf[m_level];
		PointsCloud[i].norm1 = new XVecf[m_level];
		PointsCloud[i].diff1 = new float[m_level];
	}

//	m_radius2 = 4.0 * m_radius;
	if ( startRadius>0 )
		m_radius2 = startRadius*startRadius;
	else
		m_radius2 = 2*smooth_step * m_radius;

	cerr<<"Smoothing with starting radius: "<<sqrt(m_radius2)<<endl;
	for ( int l=0; l<m_level; l++){
		level_radius[l] = m_radius2;
		cerr<<"m_radius at level: "<<l<<"  is:  "<<sqrt(m_radius2)<<endl;
		m_radius2 *= smooth_step;
	}

	// now smoothing:
	ANNpoint		query_pt;
	std::vector <int> indices[maxLevels];
	std::vector <float> dist[maxLevels];
	std::vector<int> idx1;
	std::vector<float> dists1;

	numOfExtrema = 0;
#if 0
	for ( int i=0; i<NumOfPoints; i++){
           //SmoothOnePoint(i, l);
#if 0
		if ( PointsCloud[i].is_boundary != BasePoints::INNERP ){
			for ( int l=0; l<m_level; l++){
				PointsCloud[i].pos1[l] = PointsCloud[i].pos;
				PointsCloud[i].norm1[l] = PointsCloud[i].norm;
				PointsCloud[i].diff1[l] = 0;
			}
			continue;
		}
#endif
		m_radius2 = level_radius[m_level-1];
		query_pt = m_data[i];
		for ( int l=0; l<m_level; l++){
			indices[l].clear();
		}
		int num = m_kdtree->annSearchR(
			query_pt,
			idx1,
			dists1,
			m_radius2,
			m_eps);
		// collect the points
		for ( int j=0; j<num; j++){
			for ( int l=0; l<m_level; l++){
				if ( dists1[j] <= level_radius[l]){
					indices[l].push_back(idx1[j]);
					dist[l].push_back(dists1[j]);
				}
			}
		}
		for ( int l=0; l<m_level; l++){
			idx.clear();
			dists.clear();
			for ( int j=0; j<indices[l].size(); j++){
				idx.push_back( indices[l][j]);
				dists.push_back( dist[l][j]);
			}
			SmoothOnePoint(i, l);
//			cerr<<PointsCloud[i].pos<<" "<<PointsCloud[i].pos1[l]<<endl;
//			cerr<<PointsCloud[i].norm<<" "<<PointsCloud[i].norm1[l]<<endl;
		}
 	}
#endif
	for ( int l=0; l<m_level; l++){
		m_radius2 = level_radius[l] ;
		cerr<<"smoothing level: "<<l<<endl;
        for ( int i=0; i<NumOfPoints; i++){
			SmoothOnePoint(i,l);
		}
	}

	// find local extrema
	for ( int i=0; i<NumOfPoints; i++){
		FindLocalMaxMin(i);
	}
	int numOfMax, numOfMin;
	numOfMax = 0;
	numOfMin = 0;
	for ( int l=0; l<m_level; l++){
		for ( int i=0; i<NumOfPoints; i++){
			if ( PointsCloud[i].m_max[l] == 1 )
				numOfMax++;
			else if (PointsCloud[i].m_max[l] == 2 )
				numOfMin++;
			else if (PointsCloud[i].m_max[l] == 3)
				cerr<<"Wrong "<<endl;
		}
	}
	numOfExtrema = numOfMax + numOfMin;
	if ( numOfExtrema <= 0){
		cerr<<"No feature points availabel"<<endl;
	} else
  		cerr<<"Number of features: "<<numOfExtrema<<endl;
	// compute curvatures
	minmax = new float[numOfExtrema];
	TDirection = new XVecf[numOfExtrema];
	Normals = new XVecf[numOfExtrema];
	Positions = new XVecf[numOfExtrema];
	mCurvatures = new float[numOfExtrema];
	is_smooth = true;

}

void PointClouds::ComputeCurvatures( ){
	ANNpoint		query_pt;
	query_pt = annAllocPt(dim, 0);
	int i;

	float sqrt2 = sqrt(2.0), scale, h2;
	int dl, l, j, n, index, k, num, k1, k2;
	int count = 0, count1 = 0;
	XVecf Ni, Nj, Pi, Pj, T1, T2, Tj, Pc, Nc, Pij;
	float dis, u1, u2, tanj, kj, wj;

	for ( i=0; i<NumOfPoints; i++){
		for ( l=0; l<3; l++){
			query_pt[l] = m_data[i][l];
		}
		for ( l=1; l<m_level-1; l++){
			m_radius2 = level_radius[l];
			h2 = m_radius2;
			idx.clear();
			dists.clear();
			if ( PointsCloud[i].m_max[l] > 0 ){
				minmax[count] = PointsCloud[i].m_max[l];
				Normals[count] = PointsCloud[i].norm;
				Positions[count] = PointsCloud[i].pos;

				num = m_kdtree->annSearchR(
					query_pt,
					idx,
					dists,
					m_radius2,
					m_eps);
#if 0
				num = 16;
				m_kdtree->annkSearch(
					query_pt,
					num,
					m_nidx,
					m_dists,
					0.0);
				idx.clear();
				dists.clear();
				for ( int k=0; k<10; k++){
					idx.push_back(m_nidx[k]);
					dists.push_back(m_dists[k]);
				}

#endif	
				Ni = PointsCloud[i].norm;
				Pi = PointsCloud[i].pos;
				float u0, u1;
				int nn = 0;
				mes::MatrixT B_taubin(3,3);
				mes::MatrixT BQ(3,3);
				mes::VectorT Bv(3);
				float sum = 0.0;
				B_taubin.Zero();
				int num1 = 0;
				for ( j=0; j<num; j++){
					n = idx[j];
					if ( n==i )
						continue;
					if(dists[j]<1e-8)
						continue;
					Pj = PointsCloud[n].pos;
					Nj = PointsCloud[n].norm;
					if ( Nj.dot(Ni)<-0.5)
						continue;
					num1++;
					Pij = Pj - Pi;
					kj = Ni.dot(Pij);
					Tj = kj*Ni;
					Tj = Pij - Tj;
					Tj.normalize();
					T1 = Ni.cross(Tj);
					T1.normalize();
					T2 = T1.dot(Nj)*T1;
					T2 = Nj - T2;
					T2.normalize();
					kj = Ni.dot(T2);
					kj = abs(kj);
					if ( kj<0)
						cerr<<"be careful about Page's"<<endl;
					if ( kj>=1.0)
						continue;

					kj = acos(kj);
					kj /= sqrt(dists[j]);
					wj = -1.0*dists[j]/h2;
					wj = exp(wj);
//					wj = float(1.0)/num;
					sum += wj;
					for ( k1=0; k1<3; k1++){
						for ( k2=0; k2<3; k2++){
							B_taubin(k1,k2) += wj*kj*Tj(k1)*Tj(k2);
						}
					}
				}
				for ( k1=0; k1<3; k1++)
					for ( k2=0; k2<3; k2++)
						if (sum != 0)
							B_taubin(k1,k2) /= sum;
//				cerr<<B_taubin<<endl;
				mes::SymEigen(B_taubin, BQ, Bv);
				k2 = 0; 
				sum = abs(Bv(0));
				for ( k1=0; k1<3; k1++){
					if ( abs(Bv(k1))<sum ){
						sum = abs(Bv(k1));
						k2 = k1;
					}
				}
				k1 = (k2+1)%3;
				k2 = (k2+2)%3;
				if ( Bv(k1)>Bv(k2)){
					int k = k2;
					k2 = k1;
					k1 = k;
				}
				mCurvatures[count] = 0.5 * ( Bv(k1) + Bv(k2) );
				for (k1=0; k1<3; k1++){
					TDirection[count][k1] = BQ(k1,k2);
				}				
				count++;
			}
		}
	}

	cerr<<count<<endl;
	annDeallocPt(query_pt);
}

void PointClouds::SaveMatchTagent(char *fname){
	ofstream ost(fname);
	ost<<numOfExtrema<<" "<<NumOfPoints<<endl;
	for ( int i=0; i<numOfExtrema; i++){
		ost<<minmax[i]<<" "<<Positions[i]<<" "<<endl<<Normals[i]<<" "<<TDirection[i]<<endl;
	}
	for ( int i=0; i<NumOfPoints; i++){
		ost<<PointsCloud[i].pos<<endl;
	}
	ost.close();
}

void PointClouds::SmoothOnePoint(int i, int l){

	int j, n, num = 0;
	float w, t=0, sum = 0, dis, wei;
	float h2 = level_radius[l]/4.0;
	XVecf Pi, Ni, Pj, Nj, P, N;
	XVecf P_avg, P_wei;
	if ( l==0){
        Pi = PointsCloud[i].pos;
        Ni = PointsCloud[i].norm;
	} else{
        Pi = PointsCloud[i].pos1[l-1];
        Ni = PointsCloud[i].norm1[l-1];
	}
	PointsCloud[i].m_max[l] = 3;
	ANNpoint		query_pt;	
	query_pt = m_data[i];
	PointsCloud[i].diff1[l] = 0;

	num = m_kdtree->annSearchR(
		query_pt,
		idx,
		dists,
		m_radius2,
		m_eps);

	// compute the tagent plane
	int count = 0;
	N = XVecf(0, 0, 0);
	for (j=0; j<num; j++){
		n = idx[j];
		if ( l==0)
            Nj = PointsCloud[n].norm;
		else
			Nj = PointsCloud[n].norm1[l-1];
		if ( Nj.dot(Ni) <= -0.5)
			continue;
		wei = Nj.dot(Ni);
		wei = -1.0*(wei-1)*(wei-1);
		wei = exp(wei);
		float dis = -1.0*dists[j]/h2;
		dis = exp(dis);
		N += dis*Nj;
		N += wei*dis*Nj;
		count ++;
	}
	
	if ( count <= 4){
		PointsCloud[i].norm1[l] = Ni;
		PointsCloud[i].pos1[l] = Pi;
		PointsCloud[i].diff1[l] = 0;
		return;
	}
	if ( N.l2() == 0){
		cerr<<" The smoothed plane is not good"<<endl;
		PointsCloud[i].norm1[l] = Ni;
		PointsCloud[i].pos1[l] = Pi;
		return;
	}
	N.normalize();

	// now start iterations
	XVecf detP;
	dis = 0;
	float J0, J1, J, ft1, ft2, t1;
	t1 = t;
	J0 = J1 = J = ft1 = ft2 = 0;
	int k;
	XVecf FunT, P0;
	count = 0;
	P_wei = Pi;

	// J is the function we try to minimize
	for ( int j=0; j<num; j++){
		n = idx[j];
		
		if ( l==0 ){
			Nj = PointsCloud[n].norm;
			FunT = PointsCloud[n].pos - P_wei;
		}
		else{
			Nj = PointsCloud[n].norm1[l-1];
			FunT = PointsCloud[n].pos1[l-1] - P_wei;
		}
		if ( Nj.dot(Ni) < -0.5)
			continue;

		ft1 = FunT.dot(N);
		ft2 =  - 1.0*( FunT(0)*FunT(0) + FunT(1)*FunT(1) + FunT(2)*FunT(2)) /h2;
		ft2 = exp(ft2);
		J0 += ft1*ft1*ft2;
	}

	while(true){
		// find a new position by moving along the normal direction
		k = 0; t = 0; sum = 0;
		for ( j=0; j<num; j++){
			n = idx[j];
			if ( l==0 ){
				Nj = PointsCloud[n].norm;
				detP = PointsCloud[n].pos - P_wei;
			} else {
				Nj = PointsCloud[n].norm1[l-1];
				detP = PointsCloud[n].pos1[l-1] - P_wei;
			}
			if ( Nj.dot(Ni)<-0.5)
				continue;
			float dis1 = detP(0) * detP(0) + detP(1) * detP(1) + detP(2) * detP(2);
#if 0
			dis = detP.dot(N);
			dis1 = dis*dis - dis1;
			if (dis1>0)
				dis1 = 0;
			dis1/= h2;
			dis1 = exp(dis1);
			t += dis*dis1;
			sum += dis1;
#endif
			dis = -1.0*dis1;
			dis /= h2;
			dis = exp(dis);
			dis1 = detP.dot(Nj) + 2*detP.dot(N);
			t += dis*dis1;
			dis1 = N.dot(Nj) + 2;
			sum += dis*dis1;
		}
		t /= sum;
		if ( abs(t)<accuracy )
			break;
		P0 = P_wei;
		P_wei = P_wei + t*N;
		J = 0;
		for ( int j=0; j<num; j++){
			n = idx[j];
			if ( l==0 ){
				Nj = PointsCloud[n].norm;
				FunT = PointsCloud[n].pos - P_wei;
			} else{
				Nj = PointsCloud[n].norm1[l-1];
				FunT = PointsCloud[n].pos1[l-1]-P_wei;
			}
			if ( Nj.dot(Ni) <= -.5)
				continue;
			ft1 = FunT.dot(N);		// distance
			ft2 = -1.0* ( FunT(0)*FunT(0) + FunT(1)*FunT(1) + FunT(2)*FunT(2) )/h2;
			ft2 = exp(ft2);
			J += ft1*ft1*ft2;
		}
		if ( J>J0){
			P_wei = P0;
			break;
		}

		// if the new direction good (better)?
		// compute the new normal
		w=0; sum=0;
		XVecf N1 = XVecf(0, 0, 0);
		for (j=0; j<num; j++){
			n = idx[j];
			if ( l == 0 ){
				Nj = PointsCloud[n].norm;
				detP = PointsCloud[n].pos - P_wei;
			} else{
				Nj = PointsCloud[n].norm1[l-1];
				detP = PointsCloud[n].pos1[l-1] - P_wei;
			}
			if ( Nj.dot(Ni) <=-.5)
				continue;
			dis = detP(0)*detP(0) + detP(1)*detP(1) + detP(2)*detP(2);
			dis = -1*dis/h2;
			dis = exp(dis);
			Nj = PointsCloud[n].norm;
			N1 += dis*Nj;
		}
		if ( N1.l2() == 0){
			cerr<<" after moving, the average tagent plane is bad"<<endl;
			break;
		}
		N1.normalize();

		// the new position/normal is better?
		J1 = 0;
		for ( int j=0; j<num; j++){
			n = idx[j];
			if ( l==0 ){
				Nj = PointsCloud[n].norm;
				FunT = PointsCloud[n].pos - P_wei;
			} else{
				Nj = PointsCloud[n].norm1[l-1];
				FunT = PointsCloud[n].pos1[l-1] - P_wei;
			}
			if ( Nj.dot(Ni) < -.05)
				continue;
			ft1 = FunT.dot(N1);
			ft2 = - 1.0*(FunT(0)*FunT(0) + FunT(1)*FunT(1) + FunT(2)*FunT(2)) /h2;
			ft2 = exp(ft2);
			J1 += ft1*ft1*ft2;
		}
		if ( J1>J)		// new position is not good;
			break;
		if ( abs(J-J1)<accuracy)
			break;
		if ( J1<accuracy){
			N = N1;
			break;
		}
		if (count>50)
			break;
		//if ( abs(J0-J1)/J0 < accuracy || count > 45)
		//	break;
		J0 = J1;
		N = N1;
		count ++;
	}
	PointsCloud[i].norm1[l] = N;
	PointsCloud[i].pos1[l] = P_wei;
	if ( l==0){
		PointsCloud[i].diff1[l] = N.dot(P_wei - PointsCloud[i].pos) ;
	} else{
        PointsCloud[i].diff1[l] = N.dot(P_wei - PointsCloud[i].pos1[l-1]) ;
	}
}

#if 0
void PointClouds::FindLocalMaxMin(int i){

	ANNpoint		query_pt;
	if ( PointsCloud[i].is_boundary != BasePoints::INNERP ){
		for ( int l=0; l<m_level; l++){
			PointsCloud[i].m_max[l] = 0;;
		}
		return;
	}

	typedef std::vector<float> diff_vector;
	float scale;
	diff_vector *diffs;
	diffs = new diff_vector[m_level];
	query_pt = m_data[i];
	float size = 4.0;
	scale = level_radius[m_level-1]/size;
	idx.clear();
	dists.clear();
	
	int num = m_kdtree->annSearchR(
		query_pt,
		idx,
		dists,
		scale,
		m_eps);

	for(int l=0; l<m_level; l++){

		float f = PointsCloud[i].diff1[l];
		if ( f<0 )
			PointsCloud[i].m_max[l] = 1;
		else if ( f>0 )
			PointsCloud[i].m_max[l] = 2;
		else{
			PointsCloud[i].m_max[l]= 0;
			continue;
		}
	
		diffs[l].push_back(f);
		for ( int j=0; j<num; j++){
			int n = idx[j];
			if ( n==i)
				continue;
			if ( dists[j] > level_radius[l]/size )
				continue;
//			if ( PointsCloud[n].is_boundary != BasePoints::INNERP )
//				PointsCloud[i].m_max[l] = 0;
			//else 
//			if ( PointsCloud[i].m_max[l] != 0){
				f = PointsCloud[n].diff1[l];
				diffs[l].push_back(f);
//			}
		}
	}
//	for ( int l=0; l<m_level; l++){
//		cerr<<"the size: "<<diffs[l].size()<<endl;
//	}


	num = 6;	// num is the minimal # of neighbors for 
	// finding features
	for ( int l=0; l<m_level; l++){
		if ( PointsCloud[i].m_max[l] == 0)
			continue;
		if ( diffs[l].size()<num ){
			diffs[l].clear();
			float f = PointsCloud[i].diff1[l];
			diffs[l].push_back(f);
			m_kdtree->annkSearch(
				query_pt,
				num,
				m_nidx,
				m_dists,
				m_eps);
			idx.clear();
			dists.clear();
			for ( int j=0; j<num; j++){
				int n = m_nidx[j];
				idx.push_back(n);
				dists.push_back(m_dists[j]);
				if ( n == i )
					continue;
				if ( PointsCloud[n].is_boundary != BasePoints::INNERP
					|| PointsCloud[n].diff1[l] == 0)
					PointsCloud[i].m_max[l] = 0;
				f = PointsCloud[n].diff1[l];
				diffs[l].push_back(f);
			}
		}
	}
//	for ( int l=0; l<m_level; l++){
//		cerr<<"the size  aa: "<<diffs[l].size()<<endl;
//	}

	PointsCloud[i].m_max[0] = 0;
	PointsCloud[i].m_max[m_level-1] = 0;
	for ( int l=1; l<m_level-1; l++){
		float f = PointsCloud[i].diff1[l];
		if (PointsCloud[i].m_max[l] == 0)
			continue;

		// compare with previous level
		scale = diffs[l-1].size();
		for ( int j=0; j<diffs[l-1].size(); j++){
			float ff = diffs[l-1][j];
			if ( f>0 && ff > f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
			if ( f<0 && ff < f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
		}
		// compare with current level
		scale = diffs[l].size();
		for ( int j=0; j<diffs[l].size(); j++){
			float ff = diffs[l][j];
			if ( f>0 && ff > f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
			if ( f<0 && ff < f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
		}
		// compare with next level
		scale = diffs[l+1].size();
		for ( int j=0; j<diffs[l+1].size(); j++){
			float ff = diffs[l+1][j];
			if ( f>0 && ff > f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
			if ( f<0 && ff < f ){
				PointsCloud[i].m_max[l] = 0;
				continue;
			}
		}
		//cerr<<PointsCloud[i].m_max[l]<<endl;
	}
	for ( int l=1; l<m_level-1; l++){
		if ( PointsCloud[i].m_max[l]>0 )
			numOfExtrema ++;
	}
	delete []diffs;
	diffs = NULL;
}

#endif
void PointClouds::FindLocalMaxMin(int i){

	ANNpoint		query_pt;
	if ( PointsCloud[i].is_boundary != BasePoints::INNERP ){
		for ( int l=0; l<m_level; l++){
			PointsCloud[i].m_max[l] = 0;;
		}
		return;
	}

	query_pt = m_data[i];
#if 0
	int num = 6;
	m_kdtree->annkSearch(
		query_pt,
		num+1,
		m_nidx,
		m_dists,
		m_eps);
#endif
	int num;
	if ( isFeatureRadius ){
		if ( featureRadius<= 0 ){
			for ( int l=0; l<m_level; l++){
				PointsCloud[i].m_max[l] = 0;;
			}
			return;
		}
		num = m_kdtree->annSearchR(
			query_pt,
			idx,
			dists,
			featureRadius*featureRadius,
			m_eps);
		if ( num <5 ){
			for ( int l=0; l<m_level; l++){
				PointsCloud[i].m_max[l] = 0;;
			}
			return;
		}
		for ( int k=0; k<num; k++){
			m_nidx[k] = idx[k];
			m_dists[k] = dists[k];
		}
		num = num-1;
	}else{
		num = (int)(featureRadius+0.01) + 1;
		m_kdtree->annkSearch(
			query_pt,
			num+1,
			m_nidx,
			m_dists,
			m_eps);

	}
	int k=0;
	for ( ; k<=num; k++){
		if ( m_dists[k] > 50*m_radius){
			for ( int l=0; l<m_level; l++){
				PointsCloud[i].m_max[l] = 0;
			}
			return;
		}
	}
	XVecf detP, Ni, N;
	float max1, min1;
	int maxk, mink, n;
	bool *sign = new bool[m_level];
	float *scale = new float[m_level];
	float scale1;
	float pFuzzy = 1.000;
	float nFuzzy = 1.0;
	for ( int l=1; l<m_level-1; l++){
		if ( PointsCloud[i].m_max[l] == 0)
			continue;
		detP = PointsCloud[i].pos1[l] - PointsCloud[i].pos1[l-1];
		Ni = PointsCloud[i].norm1[l];
		scale[l] = detP.dot(Ni);
		if ( scale[l] == 0){
			PointsCloud[i].m_max[l] = 0;
			continue;
		}else if ( scale[l] < 0)
			PointsCloud[i].m_max[l] = 1;
		else
			PointsCloud[i].m_max[l] = 2;
		for ( int j=0; j<num+1; j++){
			n = m_nidx[j];
			// previous level;
			if ( l==1 ){
				detP = PointsCloud[n].pos1[l-1] - PointsCloud[n].pos;
				N = PointsCloud[n].norm1[l-1];
			}else{
				detP = PointsCloud[n].pos1[l-1] - PointsCloud[n].pos1[l-2];
				N = PointsCloud[n].norm1[l-1];			
			}
			scale1 = detP.dot(N);
			if ( scale[l] < pFuzzy*0 ){
				sign[l] = 0;
				if (scale[l] > scale1)
					PointsCloud[i].m_max[l] = 0;
			} else {
				sign[l] = 1;
				if ( scale[l] <scale1)
					PointsCloud[i].m_max[l] = 0;
			}
			// current level 
			detP = PointsCloud[n].pos1[l] - PointsCloud[n].pos1[l-1];
			N = PointsCloud[n].norm1[l];
			scale1 = detP.dot(N);

			if ( scale[l] <0 ){
				if ( scale[l] > scale1)
					PointsCloud[i].m_max[l] = 0;
			} else{
				if ( scale[l] < scale1)
					PointsCloud[i].m_max[l] = 0;
			}

			// next level;
			detP = PointsCloud[n].pos1[l+1] - PointsCloud[n].pos1[l];
			N = PointsCloud[n].norm1[l+1];
			scale1 = detP.dot(N);

			if ( scale[l] <0 ){
				if ( scale[l] > scale1)
					PointsCloud[i].m_max[l] = 0;
			} else{
				if ( scale[l] < scale1)
					PointsCloud[i].m_max[l] = 0;
			}
//			cerr<<detP<<" "<<N<<endl;
		}
	}

	detP = PointsCloud[i].pos1[m_level-1] - PointsCloud[i].pos1[m_level-2];
	Ni = PointsCloud[i].norm1[m_level-1];
	scale1 = detP.dot(Ni);
	detP = PointsCloud[i].pos1[0] - PointsCloud[i].pos;
	Ni = PointsCloud[i].norm1[0];

	scale1 = detP.dot(Ni);
	PointsCloud[i].m_max[0] = 0;
	PointsCloud[i].m_max[m_level-1] = 0;
	delete []sign;
	delete []scale;
	scale = NULL;
	sign = NULL;
	return;
}

bool PointClouds::MatchTagent(PointClouds *PC1, float scale, float Trans[3][4]){
	bool ready1 = PC1->IfSmoothed(); 
	if ( !ready1 ){
		return false;
	}
	int num = PC1->GetNumOfExtrema();
	if ( num <= 0 ){
		return false;
	}
	XVecf T1, T2, N;
	XVecf db_T1, db_N, db_T2, db_P;
	float Rot[3][3], db_R[3][3], R[3][3];
	float t[3], t1[3];
	int max1, max2, count_out, count_inner;
	ANNpoint		query_pt, query_pt1;
	query_pt = annAllocPt(dim);
	query_pt1 = annAllocPt(dim);

	for ( int i=0; i< numOfExtrema; i++){
		max1 = minmax[i];
		N =  Normals[i];
		T1 = TDirection[i];
		T2 = N.cross(T1);
		T2.normalize();
		for ( int k=0; k<3; k++){
			R[0][k] = T1(k);
			R[1][k] = T2(k);
			R[2][k] = N(k);
			t1[k] = Positions[i][k];
		}
		for ( int j=0; j<num; j++){
			if ( PC1->minmax[j] != max1 )
				continue;
			db_T1 = PC1->TDirection[j];
			db_N = PC1->Normals[j];
			db_T2 = db_N.cross(db_T1);
			db_T2.normalize();
			db_P = PC1->Positions[j];

			for ( int k=0;k<3;k++){
				db_R[k][0] = db_T1(k);
				db_R[k][1] = db_T2(k);
				db_R[k][2] = db_N(k);
			}
			for ( int k1 =0; k1<3; k1++){
				for ( int k2=0; k2<3; k2++){
					Rot[k1][k2] = 0;
					for ( int  k=0; k<3; k++)
						Rot[k1][k2] += db_R[k1][k]*R[k][k2];
				}
			}
			for ( int k1=0; k1<3; k1++){
				t[k1] = 0;
				for (int k2=0; k2<3; k2++)
					t[k1] += Rot[k1][k2] *t1[k2];
				t[k1] = db_P[k1] - t[k1];
			}
			count_out = 0;
			count_inner = 0;
			ready1 = true;
			for ( int k=0; k<numOfExtrema; k++){
				for ( int k1=0; k1<3; k1++)
                    query_pt1[k1] = Positions[k][k1];
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Rot[k1][k2]*query_pt1[k2];
					}
					query_pt[k1] += t[k1];
				}
				float error = PC1->NearestNg(query_pt);
				if ( error > scale ){
					ready1 = false;
					break;
				}
				count_out ++;
			}

// IF FIND A RIGHT ONE	
			if (ready1==true && count_out!=0){
				for ( int k1=0; k1<3; k1++){
					for ( int k2=0; k2<3; k2++)
						Trans[k1][k2] = Rot[k1][k2];
					Trans[k1][3] = t[k1];
				}
				return true;
			}

		}
	}
	annDeallocPt(query_pt);
	annDeallocPt(query_pt1);
}

bool PointClouds::MatchTagentFromFile(char *fname, float scale, float Trans[3][4]){

	bool returnvalue = false;
	int num1, num2;
	std::string s;
	ifstream ist(fname);
	
	if ( !ist.good() ){
		return returnvalue;
	}
	getline(ist, s);
	num1 = num2 = 0;
	int n = sscanf(s.c_str(), "%d %d", &num1, &num2);
	int *max;
	XVecf *PN;
	XVecf *TD;
	XVecf *NM;
	float x, y, z, nx, ny, nz;
	max = new int[num1];
	PN = new XVecf[num1];
	TD = new XVecf[num1];
	NM = new XVecf[num1];
	ANNpointArray	m_data1;     // store the coordinates
	m_data1 = annAllocPts(num2,dim);

	int max1, max2;
	ANNkd_tree      *m_kdtree1;
	ANNpoint		query_pt, query_pt1;
	query_pt = annAllocPt(dim);
	query_pt1 = annAllocPt(dim);
	
	XVecf T1, T2, N;
	XVecf db_T1, db_N, db_T2, db_P;
	float Rot[3][3], db_R[3][3], R[3][3];
	float t[3], t1[3];
	int count_out, count_inner;
	bool ready1;

	float aa;
	// load the file
	for ( int i=0; i<num1; i++){
		getline(ist,s);
		int n = sscanf(s.c_str(), "%d %f %f %f", &max1, &x, &y, &z);
		if ( n<4 )
			continue;
		max[i] = max1;
		PN[i] = XVecf(x, y, z);
		getline(ist,s);
		n = sscanf(s.c_str(), "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
		if ( n<4 )
			continue;
		NM[i] = XVecf(x, y, z);
		TD[i] = XVecf(nx, ny, nz);
	}

	for ( int i=0; i<num2; i++){
		getline(ist,s);
		n = sscanf(s.c_str(), "%f %f %f", &x, &y, &z);
		m_data1[i][0] = x;
		m_data1[i][1] = y;
		m_data1[i][2] = z;
	}
	m_kdtree1 = new ANNkd_tree(
		m_data1,
		num2,
		dim);

	float error, max_error, old_error = 0;;
	XVecf norm1, normRt;
	int numOfBestResult = 0;
	int stepOfVerify = NumOfPoints/100;
	for ( int i=0; i<numOfExtrema; i++){
		max1 = minmax[i];
//		cerr<<max1<<endl;
		cerr<<i<<endl;
		N =  Normals[i];
		T1 = TDirection[i];
		T2 = N.cross(T1);
		T2.normalize();
		for ( int k=0; k<3; k++){
			R[0][k] = T1(k);
			R[1][k] = T2(k);
			R[2][k] = N(k);
			t1[k] = Positions[i][k];
		}
		for ( int j=0; j<num1; j++){
			if ( max[j] != max1 )
				continue;
			db_T1 = TD[j];
			db_N = NM[j];
			db_T2 = db_N.cross(db_T1);
			db_T2.normalize();
			db_P = PN[j];
			for ( int k=0;k<3;k++){
				db_R[k][0] = db_T1(k);
				db_R[k][1] = db_T2(k);
				db_R[k][2] = db_N(k);
			}
			for ( int k1 =0; k1<3; k1++){
				for ( int k2=0; k2<3; k2++){
					Rot[k1][k2] = 0;
					for ( int  k=0; k<3; k++)
						Rot[k1][k2] += db_R[k1][k]*R[k][k2];
				}
			}
			for ( int k1=0; k1<3; k1++){
				t[k1] = 0;
				for (int k2=0; k2<3; k2++)
					t[k1] += Rot[k1][k2] *t1[k2];
				t[k1] = db_P[k1] - t[k1];
			}
#if 0
			for ( int k1=0; k1<3; k1++){
				for ( int k2=0; k2<3; k2++)
					cerr<<Rot[k1][k2]<<" ";
				cerr<<t[k1]<<endl;
			}
#endif
			count_out = 0;
			count_inner = 0;
			ready1 = true;
			max_error = 0;
			for ( int k=0; k<NumOfPoints; k = k + stepOfVerify){
				if( PointsCloud[k].is_boundary == BasePoints::BOUNDARY)
					continue;
				for ( int k1=0; k1<3; k1++)
                    query_pt1[k1] = PointsCloud[k].pos[k1];
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Rot[k1][k2]*query_pt1[k2];
					}
					query_pt[k1] += t[k1];
				}
				m_kdtree1->annkSearch(
					query_pt,
					1,
					m_nidx,
					m_dists,
					0.0);
				error = sqrt(m_dists[0]);
				if ( error > old_error && numOfBestResult > 0){
					ready1 = false;
					break;
				}
				if ( error>max_error)
					max_error = error;
				if ( error > scale ){
					ready1 = false;
					break;
				}
				count_out ++;
			}

#if 0
			for ( int k=0; k<numOfExtrema; k++){
				for ( int k1=0; k1<3; k1++){
                    query_pt1[k1] = Positions[k][k1];
					norm1(k1) = Normals[k][k1];
					normRt(k1) = 0;
				}
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Rot[k1][k2]*query_pt1[k2];
						normRt(k) += Rot[k1][k2] * norm1(k);
					}
					query_pt[k1] += t[k1];
				}
				m_kdtree1->annkSearch(
					query_pt,
					1,
					m_nidx,
					m_dists,
					0.0);
				error = sqrt(m_dists[0]);
				if ( error > old_error && numOfBestResult > 0){
					ready1 = false;
					break;
				}
				if ( error>max_error)
					max_error = error;
				if ( error > scale ){
					ready1 = false;
					break;
				}
				count_out ++;
			}


#endif
			// IF FIND A RIGHT ONE	
			if (ready1==true && count_out!=0 ){
			
				for ( int k1=0; k1<3; k1++){
					for ( int k2=0; k2<3; k2++){
						cerr<<Rot[k1][k2]<<" ";
					}
					cerr<<t[k1]<<endl;
				}

				if ( numOfBestResult == 0 ){
					cerr<<"old_error:  "<<old_error<<"  max_error:  "<<max_error<<endl;
					old_error = max_error;
					numOfBestResult ++;
				}else if ( max_error > old_error )
                    continue;

				returnvalue = true;
				cerr<<"old_error:  "<<old_error<<"  max_error:  "<<max_error<<endl;
				old_error = max_error;

				for ( int k1=0; k1<3; k1++){
					for ( int k2=0; k2<3; k2++){
						Trans[k1][k2] = Rot[k1][k2];
						cerr<<Trans[k1][k2]<<" ";
					}
					Trans[k1][3] = t[k1];
					cerr<<Trans[k1][3]<<endl;
				}
			} 
		}
	}
#if 0
	if ( returnvalue ){
        int rand_file = rand();
        char buffer[65];
        itoa( rand_file, buffer, 10 );
		strcat(buffer,".obj");
        ofstream ost(buffer);
		for ( int k=0; k<NumOfPoints; k ++){
				for ( int k1=0; k1<3; k1++)
                    query_pt1[k1] = PointsCloud[k].pos[k1];
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Trans[k1][k2]*query_pt1[k2];
					}
					query_pt[k1] += Trans[k1][3];
				}
				ost<<"v ";
				for ( int k1=0; k1<3; k1++)
					ost<<query_pt[k1]<<" ";
				ost<<endl;

				for ( int k1=0; k1<3; k1++)
                    query_pt1[k1] = PointsCloud[k].norm[k1];
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Trans[k1][k2]*query_pt1[k2];
					}
				}
				ost<<"vn ";
				for ( int k1=0; k1<3; k1++)
					ost<<query_pt[k1]<<" ";
				ost<<endl;
		}
	}



#endif
	ist.close();
	delete []max;
	delete[] PN;
	delete[]TD;
	delete []NM;
	max = NULL;
	PN = TD = NM = NULL;
	delete m_kdtree1;
	m_kdtree1 = NULL;
	annDeallocPts(m_data1);
	annDeallocPt(query_pt);
	return returnvalue;
}


#if 0
void PointClouds::CurvaturePrinceton(){
	ANNpoint		query_pt;
	query_pt = annAllocPt(dim, 0);
	int i;

	mes::MatrixT II(3,3);
	mes::MatrixT SecondT(2,2), SQ(2,2);
	mes::VectorT Bs(3), Sv(2), BB(3);
	mes::MatrixT M(3,3), MQ(3,3);

	float sqrt2 = sqrt(2.0), scale, h2, w, t, u, v;
	float uj, vj, b1j, b2j;
	int dl, l, j, n, index, k, num, k1, k2;
	int count = 0, count1 = 0;
	XVecf Ni, Pi, T1, T2, ej, Pj, Nij, Nj;
	float dis, u1, u2, tanj, kj, wj;
	float a, b, c, det;
	num = 0;
	for ( i=0; i<NumOfPoints; i++){
		for ( l=0; l<3; l++){
			query_pt[l] = m_data[i][l];
		}
		Ni = PointsCloud[i].norm;
		Pi = PointsCloud[i].pos;
		T2 = ComputeTagentPlane(Ni); 
		T1 = Ni.cross(T2);
		for ( l=1; l<m_level-1; l++){
			m_radius2 = level_radius[l];
			h2 = m_radius2;
			idx.clear();
			dists.clear();
			if ( PointsCloud[i].m_max[l] <= 0 )
				continue;
			if ( PointsCloud[i].m_max[l] > 0 ){
				// initial 
#if 0
				num = m_kdtree->annSearchR(
					query_pt,
					idx,
					dists,
					m_radius2,
					m_eps);
#endif
				num = 16;
				idx.clear();
				dists.clear();
				m_kdtree->annkSearch(
					query_pt,
					num,
					m_nidx,
					m_dists,
					m_eps);
				for ( int k=0; k<num; k++){
					idx.push_back(m_nidx[k]);
					dists.push_back(m_dists[k]);
				}
				if ( num < 8 ){
					PointsCloud[i].m_max[l] = 0;
					continue;
				}
				for (int k=0; k<3; k++){
					for (int m =0; m<3; m++){
						II(k,m) = 0;
					}
					Bs(k) = 0;
				}
				minmax[count] = PointsCloud[i].m_max[l];
				Normals[count] = PointsCloud[i].norm;
				Positions[count] = PointsCloud[i].pos;
				mes::VectorT v1(num);
				for ( int jj=0; jj<num; jj++){
					int index = idx[jj];
					if ( index == i )
						continue;
					Pj = PointsCloud[index].pos;
					Nij = PointsCloud[index].norm;
					if ( Nij.dot(Nj) < -0.5 )
						continue;
					ej = Pj - Pi;
					Nj = Nij - Ni;
					float uj, vj, b1j, b2j;
					uj = ej.dot(T1);
					vj = ej.dot(T2);
					b1j = T1.dot(Nj);
					b2j = T2.dot(Nj);
					w = -1.0*dists[jj]/m_radius2;
					w = exp(w);
					II(0,0) += w*uj*uj;
					II(0,1) += w*uj*vj;
					II(2,2) += w*vj*vj;
					Bs(0) += w*uj*b1j;
					Bs(1) += w*(vj*b1j+uj*b2j);
					Bs(2) += w*vj*b2j;
				}

				II(0,2) = 0;
				II(2,0) = 0;
				II(1,0) = II(0,1);
				II(1,2) = II(0,1);
				II(2,1) = II(0,1);
				II(1,1) = II(0,0)+II(2,2);
				a = II(0,0);
				b = II(0,1);
				c = II(2,2);
				det = (a*c-b*b)*(a+c);
				if ( det==0 ){
					cerr<<"error for curvature"<<endl;
					cerr<<ej<<endl;
					cerr<<T1<<endl;
					cerr<<Ni<<endl;
					cerr<<Bs<<II;
					continue;
				}
			}
			float e=0, f=0, g=0;

			e = c*(a+c)*Bs(0) - b*c*Bs(1) + b*b*(Bs(2)-Bs(0));
			f = a*c*Bs(1) - b*c*Bs(0) - a*b*Bs(2);
			g = a*(a+c)*Bs(2) + b*b*(Bs(0)-Bs(2)) - a*b*Bs(1);
			e /= det;
			f /= det;
			g /= det;
			SecondT(0,0) = e;
			SecondT(0,1) = SecondT(1,0) = f;
			SecondT(1,1) = g; 
			mes::SymEigen(SecondT, SQ, Sv);
			XVecf TV1, TV2;
			if ( abs(Sv(0)) >= abs(Sv(1)) ){
				TV1 = SQ(0,0)*T1 + SQ(1,0)*T2;
				TV2 = SQ(0,1)*T1 + SQ(1,1)*T2;
			}else{
				TV2 = SQ(0,0)*T1 + SQ(1,0)*T2;
				TV1 = SQ(0,1)*T1 + SQ(1,1)*T2;
			}	
			TV1.normalize();
			for (k1=0; k1<3; k1++){
				TDirection[count][k1] = TV1(k1);
			}	
			mCurvatures[count] = 0.5*(Sv(1)+Sv(0));
			count++;
		}
	}
	numOfExtrema = count;
	cerr<<" #of signatures "<<count<<endl;
	annDeallocPt(query_pt);
}

void PointClouds::SaveMatchCurvature(char *fname){
	ofstream ost(fname);
	ost<<numOfExtrema<<" "<<NumOfPoints<<endl;
	for ( int i=0; i<numOfExtrema; i++){
		ost<<minmax[i]<<" "<<mCurvatures[i]<<" "<<Positions[i]<<" "<<endl<<Normals[i]<<" "<<TDirection[i]<<endl;
	}
	for ( int i=0; i<NumOfPoints; i++){
		ost<<PointsCloud[i].pos<<endl;
	}
	ost.close();
}
bool PointClouds::MatchCurvature(char *fname, float scale, float Trans[3][4]){
	bool returnvalue = false;
	int num1, num2;
	std::string s;
	ifstream ist(fname);	
	if ( !ist.good() ){
		return returnvalue;
	}
	getline(ist, s);
	num1 = num2 = 0;
	int n = sscanf(s.c_str(), "%d %d", &num1, &num2);
	int *max;
	XVecf *PN;
	XVecf *TD;
	XVecf *NM;
	float x, y, z, nx, ny, nz;
	max = new int[num1];
	PN = new XVecf[num1];
	TD = new XVecf[num1];
	NM = new XVecf[num1];
	ANNpointArray	m_data1;     // store the coordinates
	m_data1 = annAllocPts(num2,dim);

	ANNpointArray	m_data2; 
	m_data2 = annAllocPts(num1,1);
	ANNpoint  query_curvature;
	query_curvature = annAllocPt(1);

	int max1, max2;
	float curvature;
	ANNkd_tree      *m_kdtree1;
	ANNkd_tree      *m_kdcurvature;
	ANNpoint		query_pt, query_pt1;
	query_pt = annAllocPt(dim);
	query_pt1 = annAllocPt(dim);
	float maxCur, minCur;
	maxCur = -1.0e+8;
	minCur = 1.0e+8;
	
	XVecf T1, T2, N;
	XVecf db_T1, db_N, db_T2, db_P;
	float Rot[3][3], db_R[3][3], R[3][3];
	float t[3], t1[3];
	int count_out, count_inner;
	bool ready1;
//	std::map<float, int> mapCurvatureIndex;
//	std::map<float, int>::iterator cur_Iter1, cur_Iter2;
//	typedef pair<float, int> curPair;

	// load the file
	for ( int i=0; i<num1; i++){
		getline(ist,s);
		int n = sscanf(s.c_str(), "%d %f %f %f %f", &max1, &curvature, &x, &y, &z);
		if ( n<4 )
			continue;
		max[i] = max1;
		PN[i] = XVecf(x, y, z);
//		cur_Iter1 = mapCurvatureIndex.find(curvature);
//		mapCurvatureIndex.insert(curPair(curvature+,i));
		m_data2[i][0] = curvature;
		if ( curvature > maxCur )
			maxCur = curvature;
		if ( curvature < minCur )
			minCur = curvature;
		getline(ist,s);
		n = sscanf(s.c_str(), "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
		if ( n<4 )
			continue;
		NM[i] = XVecf(x, y, z);
		TD[i] = XVecf(nx, ny, nz);
	}

//	cerr<<"num1: "<<num1<<" #of curvature: "<<mapCurvatureIndex.size()<<endl;
	for ( int i=0; i<num2; i++){
		getline(ist,s);
		n = sscanf(s.c_str(), "%f %f %f", &x, &y, &z);
		m_data1[i][0] = x;
		m_data1[i][1] = y;
		m_data1[i][2] = z;
	}
	m_kdtree1 = new ANNkd_tree(
		m_data1,
		num2,
		dim);

	m_kdcurvature = new ANNkd_tree(
		m_data2,
		num1,
		1);

	float cRadius;
	cRadius = 0.05*(maxCur - minCur);
	cerr<<"min "<<minCur<<" max "<<maxCur<<" "<<cRadius<<endl;
	cRadius *= cRadius;
	float error, max_error, old_error = 0;;
	XVecf norm1, normRt;
	int numOfBestResult = 0;
	int numOfNearest = num1/10;
	int stepOfVerify = NumOfPoints/100;
	
	ANNdistArray    mdists; 
	ANNidxArray     midx;  
	midx = new ANNidx[numOfNearest+1];   // allocate near neigh indices
	mdists = new ANNdist[numOfNearest+1];
	
	for ( int i=0; i<numOfExtrema; i++){
//		cerr<<i<<endl;
		max1 = minmax[i];
		query_curvature[0] = mCurvatures[i];
		cerr<<query_curvature[0]<<endl;
		cRadius = query_curvature[0]*query_curvature[0];
		numOfNearest = m_kdcurvature->annSearchR(
			query_curvature,
			idx,
			dists,
			cRadius,
			0.0);
#if 0
		m_kdcurvature->annkSearch(
			query_curvature,
			numOfNearest,
			midx,
			mdists,
			0.0);
#endif
		N =  Normals[i];
		T1 = TDirection[i];
		T2 = N.cross(T1);
		T2.normalize();
		for ( int k=0; k<3; k++){
			R[0][k] = T1(k);
			R[1][k] = T2(k);
			R[2][k] = N(k);
			t1[k] = Positions[i][k];
		}
		cerr<<"numOfNearest: "<<numOfNearest<<endl;
		for ( int nn=0; nn<numOfNearest; nn++){
//		for ( int nn=0; nn<num1; nn++){
			int j = idx[nn];
//			int j = nn;
			if ( max[j] != max1 )
				continue;
//			cerr<<query_curvature[0]<<"matched to "<<m_data2[j][0]<<endl;
			db_T1 = TD[j];
			db_N = NM[j];
			db_T2 = db_N.cross(db_T1);
			db_T2.normalize();
			db_P = PN[j];
//			cerr<<db_P<<endl;
			for ( int k=0;k<3;k++){
				db_R[k][0] = db_T1(k);
				db_R[k][1] = db_T2(k);
				db_R[k][2] = db_N(k);
			}
			for ( int k1 =0; k1<3; k1++){
				for ( int k2=0; k2<3; k2++){
					Rot[k1][k2] = 0;
					for ( int  k=0; k<3; k++)
						Rot[k1][k2] += db_R[k1][k]*R[k][k2];
				}
			}
			for ( int k1=0; k1<3; k1++){
				t[k1] = 0;
				for (int k2=0; k2<3; k2++)
					t[k1] += Rot[k1][k2] *t1[k2];
				t[k1] = db_P[k1] - t[k1];
			}
#if 0
			cerr<<"Rt:   "<<endl;
			for ( int k1=0; k1<3; k1++){
				for ( int k2=0; k2<3; k2++)
					cerr<<Rot[k1][k2]<<" ";
				cerr<<t[k1]<<endl;
			}
#endif
			count_out = 0;
			count_inner = 0;
			ready1 = true;
			max_error = 0;
			for ( int k=0; k<NumOfPoints; k = k + stepOfVerify){
				if( PointsCloud[k].is_boundary == BasePoints::BOUNDARY)
					continue;
//				cerr<<"org: "<<endl;
				for ( int k1=0; k1<3; k1++){
                    query_pt1[k1] = PointsCloud[k].pos[k1];
//					cerr<<query_pt1[k1]<<" ";
				}
//				cerr<<endl;
//				cerr<<"after: "<<endl;
				for (int k1=0; k1<3; k1++){
					query_pt[k1] = 0;
					for (int k2=0; k2<3; k2++){
						query_pt[k1] += Rot[k1][k2]*query_pt1[k2];
					}
					query_pt[k1] += t[k1];
//					cerr<<query_pt[k1]<<" ";
				}
//				cerr<<endl;
				m_kdtree1->annkSearch(
					query_pt,
					1,
					m_nidx,
					m_dists,
					0.0);
				error = sqrt(m_dists[0]);
				if ( error > old_error && numOfBestResult > 0){
					ready1 = false;
					break;
				}
				if ( error>max_error)
					max_error = error;
				if ( error > scale ){
					ready1 = false;
					break;
				}
				count_out ++;
			}

			// IF FIND A RIGHT ONE	
			if (ready1==true && count_out!=0 ){
//				cerr<<" a good match: "<<endl;
//				cerr<<mCurvatures[i]<<"  and  "<<m_data2[nn][0]<<endl;
				cerr<<"cRadius and distance  "<<cRadius<<"  "<<sqrt(dists[nn])<<endl;
			
				for ( int k1=0; k1<3; k1++){
					for ( int k2=0; k2<3; k2++){
						cerr<<Rot[k1][k2]<<" ";
					}
					cerr<<t[k1]<<endl;
				}

				if ( numOfBestResult == 0 ){
					cerr<<"old_error:  "<<old_error<<"  max_error:  "<<max_error<<endl;
					old_error = max_error;
					numOfBestResult ++;
				}else if ( max_error > old_error )
                    continue;

				returnvalue = true;
				cerr<<"old_error:  "<<old_error<<"  max_error:  "<<max_error<<endl;
				old_error = max_error;

				for ( int k1=0; k1<3; k1++){
					for ( int k2=0; k2<3; k2++){
						Trans[k1][k2] = Rot[k1][k2];
						cerr<<Trans[k1][k2]<<" ";
					}
					Trans[k1][3] = t[k1];
					cerr<<Trans[k1][3]<<endl;
				}
			} 
		}
	}

	delete []max;
	delete[] PN;
	delete[]TD;
	delete []NM;
	annDeallocPts(m_data1);
	max = NULL;
	PN = TD = NM = NULL;
	delete []midx;
	delete []mdists;
	delete m_kdtree1;
	annDeallocPts(m_data2);
	delete m_kdcurvature;
	m_kdtree1 = NULL;
	annDeallocPt(query_pt);
	annDeallocPt(query_pt1);
	ist.close();
	return returnvalue;
}

#endif
bool PointClouds::IfSmoothed(){
	cerr<<is_smooth<<endl;
		return is_smooth;
}

float PointClouds::NearestNg(ANNpoint P){

	m_kdtree->annkSearch(
		P,
		1,
		m_nidx,
		m_dists,
		0.0);
	return sqrt(m_dists[0]);
}
XVecf PointClouds::ComputeTagentPlane(XVecf N){
	XVecf T1;
	if ( N.l2() == 0 ){
		cerr<<"normal not good"<<endl;
		return XVecf(0, 0, 0);
	}
	if ( N[0]*N[1] != 0 ){
		T1 = XVecf(-1*N[1], N[0], 0);
	}else if ( N[0]*N[2] != 0){
		T1 = XVecf( -1* N[2], 0, N[0] );
	} else if ( N[1]*N[2] != 0 ){
		T1 = XVecf(0, -1*N[2], N[1]);
	} else if ( N[0] !=0 ){
		T1 = XVecf(0,1,0);
	} else if ( N[1]!=0 ){
		T1 = XVecf(1, 0, 0);
	} else {
		T1 = XVecf(1, 0, 0);
	}
	T1.normalize();
	XVecf T2 = T1.cross(N);
#if 0
	srand( (unsigned)time( NULL ) );
	float angle = 2*M_PI*(float)rand()/RAND_MAX;
	T1 = cos(angle)*T1 + sin(angle)*T2;
	T1.normalize();
#endif
	if ( T1.dot(N) > 1e-6 ){
		cerr<<"normal not good"<<endl;
		return XVecf(0, 0, 0);
	}
	return T1;
}
// An algorithm from ... 
#define ELEM_SWAP(a,b) { register float t=(a);(a)=(b);(b)=t; }
float PointClouds::MedianOfArray(float * farray, int n){
	
    int low, high ;
    int median;
	int middle, ll, hh;
    low = 0 ; high = n-1 ; median = (low + high +1) / 2;
	while(true) {

        if (high <= low) /* One element only */
            return farray[median] ;

        if (high == low + 1) {  /* Two elements only */
            if (farray[low] > farray[high])
                ELEM_SWAP(farray[low], farray[high]) ;
            return farray[median] ;
        }

		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high +1) / 2;
		if (farray[middle] > farray[high])    ELEM_SWAP(farray[middle], farray[high]) ;
		if (farray[low] > farray[high])       ELEM_SWAP(farray[low], farray[high]) ;
		if (farray[middle] > farray[low])     ELEM_SWAP(farray[middle], farray[low]) ;

		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(farray[middle], farray[low+1] ) ;

	   /* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		//ll = low;
		hh = high;
		
		for (;;) {
			do ll++; while (farray[low] > farray[ll]) ;
			do hh--; while (farray[hh]  > farray[low]) ;

			if (hh < ll)
			break;

			ELEM_SWAP(farray[ll], farray[hh]) ;
		}

		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(farray[low], farray[hh]) ;

		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
    }

	//return farray[median];
}

float PointClouds::GetScale(){
	ANNpoint		m_bmin, m_bmax;
    m_bmin = annAllocPt(dim);
	m_bmax = annAllocPt(dim);
	m_kdtree->getBBox3d(m_bmin, m_bmax);
	XVecf max1, min1;
	max1 = XVecf(m_bmax[0], m_bmax[1], m_bmax[2]);
	min1 = XVecf(m_bmin[0], m_bmin[1], m_bmin[2]);
	//return sqrt( (m_bmax(0)-m_bmin(0))*(m_bmax(0)-m_bmin(0)) + (m_bmax(1)-m_bmin(1))*(m_bmax(1)-m_bmin(1)) + (m_bmax(2)-m_bmin(2))*(m_bmax(2)-m_bmin(2)) );
	max1 = max1 - min1;
	annDeallocPt(m_bmin);
	annDeallocPt(m_bmax);
	return max1.l2();

}

void PointClouds::SaveToObjFile(){
	ofstream ost("test.obj");
	for ( int i=0; i<NumOfPoints; i++){
		ost<<"v "<<PointsCloud[i].pos<<endl;
	}
	for ( int i=0; i<NumOfPoints; i++){
		ost<<"vn "<<PointsCloud[i].norm<<endl;
	}
	ost.close();
}