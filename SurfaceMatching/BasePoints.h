#pragma once

#include "xvec.h"

class BasePoints
{
public:
	enum { INNERP = 0x00, NEARP = 0x01, BOUNDARY = 0x02 };

public:
	BasePoints(void);
	~BasePoints(void);
	BasePoints(const BasePoints& _bp);
	BasePoints& operator =(const BasePoints& _bp);
	bool SetLevel(int _l){
		if ( _l > 0)
            mLevels = _l;
		else
			return false;
		return true;
	}

// elements
public:
	XVecf pos;
	XVecf norm;
	unsigned int is_boundary;
	// for smoothing
	unsigned int *m_max;
	int mLevels;
	XVecf * pos1;
	XVecf * norm1;
	float * diff1;
};
