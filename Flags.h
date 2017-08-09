/************************************************

	This file is the header file for the
	Flags class

*************************************************/


#ifndef FLAGS_H
#define FLAGS_H


class Flags
{
	public:
	
	// Default constructor
	Flags();

	// Destructor
	~Flags();
	
	// member variables
	bool Registered;
	bool Active;
	bool AutoAlignAttempted;
	bool Selected;

};

#endif
