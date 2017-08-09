/************************************************

	This file is the header file for the
	TransMatrix class

*************************************************/


#ifndef TRANSMATRIX_H
#define TRANSMATRIX_H



// Edited to work better with OpenGL. Hopefully arcball works well with this way too.

class TransMatrix
{
	
	
	public:
	
	// Default constructor
	TransMatrix();

	// ~Destructor
	~TransMatrix();

	float* Transpose();
	void multTransformMatrix(float* matrix);

	// member variables
	float TransformationMatrix[16];
};

#endif
