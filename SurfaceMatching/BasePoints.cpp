#include ".\basepoints.h"



BasePoints::BasePoints(void)
{
	pos = XVecf(0, 0, 0);
	norm = XVecf(0, 0, 0);
	is_boundary = INNERP;
	mLevels = 0;
	// for smoothing
	m_max = NULL;
	pos1 = NULL;
	norm1 = NULL;
	diff1 = NULL;
	is_boundary = BasePoints::INNERP;

}

BasePoints::BasePoints(const BasePoints &_bp){
	pos = _bp.pos;
	norm = _bp.norm;
	is_boundary = _bp.is_boundary;
	mLevels = _bp.mLevels;
	if ( mLevels>0 ){
		m_max = new unsigned int[mLevels];
		pos1 = new XVecf[mLevels];
		norm1 = new XVecf[mLevels];
		diff1 = new float[mLevels];
		for ( int l=0; l<mLevels; l++){
			m_max[l] = _bp.m_max[l];
			diff1[l] = _bp.diff1[l];
			pos1[l] = _bp.pos1[l];
			norm1[l] = _bp.norm1[l];
		}
	}else{
		pos1 = NULL;
		norm1 = NULL;
		m_max = NULL;
		diff1 = NULL;
	}
}

BasePoints & BasePoints::operator =(const BasePoints& _bp){
	pos = _bp.pos;
	norm = _bp.norm;
	is_boundary = _bp.is_boundary;
	mLevels = _bp.mLevels;
	if ( mLevels>0 ){
		m_max = new unsigned int[mLevels];
		pos1 = new XVecf[mLevels];
		norm1 = new XVecf[mLevels];
		diff1 = new float[mLevels];
		for ( int l=0; l<mLevels; l++){
			m_max[l] = _bp.m_max[l];
			diff1[l] = _bp.diff1[l];
			pos1[l] = _bp.pos1[l];
			norm1[l] = _bp.norm1[l];
		}
	}else{
		pos1 = NULL;
		norm1 = NULL;
		m_max = NULL;
		diff1 = NULL;
	}
	return *this;
}

BasePoints::~BasePoints(void)
{
	// for smoothing
	if ( m_max ){
		delete [] m_max;
		m_max = NULL;
	}
	if ( pos1 ){
		delete [] pos1;
		delete [] norm1;
		delete [] diff1;
		pos1 = norm1 = NULL;
		diff1 = NULL;
	}
	
}
