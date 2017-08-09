/************************************************

	This file contains the implementation for the
	TransMatrix class.
	In this class, we have the following functions:

	default constructor
	destructor

*************************************************/

#include "stdafx.h"
#include "TransMatrix.h"
#include "xvec.h"

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
// Default constructor
TransMatrix::TransMatrix()
{
	TransformationMatrix[0] = 1;
	TransformationMatrix[1] = 0;
	TransformationMatrix[2] = 0;
	TransformationMatrix[3] = 0;
	TransformationMatrix[4] = 0;
	TransformationMatrix[5] = 1;
	TransformationMatrix[6] = 0;
	TransformationMatrix[7] = 0;
	TransformationMatrix[8] = 0;
	TransformationMatrix[9] = 0;
	TransformationMatrix[10] = 1;
	TransformationMatrix[11] = 0;
	TransformationMatrix[12] = 0;
	TransformationMatrix[13] = 0;
	TransformationMatrix[14] = 0;
	TransformationMatrix[15] = 1;
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
// ~Destructor
TransMatrix::~TransMatrix()
{
}

/*
   Transpose the current matrix and return a new matrix.
   Igor's code wants matrices row wise, OGL is column wise.
*/
float* TransMatrix::Transpose()
{
	float* mat = new float[16];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat[4*i + j] = TransformationMatrix[i + 4*j];
		}
	}

	return mat;
}

/* 
	Multiply a new matrix into the old one from a non-OGL source
*/
void TransMatrix::multTransformMatrix(float* matrix)
{
	float* newMatrix;
	float* transpose = this->Transpose();

	newMatrix = mult4x4(matrix, transpose);

	// Transpose new matrix in OGL format
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TransformationMatrix[i + 4*j] = newMatrix[4*i + j];
		}
	}
	delete[] transpose;
	delete[] newMatrix;
	return;
}