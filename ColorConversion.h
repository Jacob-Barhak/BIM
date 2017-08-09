// Function to convert HSV to RGB.
// Supply H, S and V values and also pass by references 
// R, G, B to obtain their values
// Algorithm was taken from 
// Section 9.1 of http://www.biology.duke.edu/johnsenlab/pdfs/tech/colorconversion.pdf
// Jun added this 01/21/05 

#include <math.h>

inline void ConvertHSVtoRGB(double H, double S, double V, double &R, double &G, double &B)
{

	// H must be between 0 and 360
	if(H < 0 || H > 360)
		return;

	double Hex, primary_color, secondary_color, a, b, c;
	// perform calculations

	Hex = H/60;
	primary_color = floor(Hex);
	secondary_color = Hex - primary_color;
	a = (1 - S) * V;
	b = (1 - (S * secondary_color)) * V;
	c = (1 - (S * (1 - secondary_color))) * V;

	
	// Now find RGB values
	switch((int)primary_color)
	{
		case 0:
			R = V;
			G = c;
			B = a;
			break;
		case 1:
			R = b;
			G = V;
			B = a;
			break;
		case 2:
			R = a;
			G = V;
			B = c;
			break;
		case 3: 
			R = a;
			G = b;
			B = V;
			break;
		case 4:
			R = c;
			G = a;
			B = V;
			break;
		case 5:
			R = V;
			G = a;
			B = b;
			break;
		
	}

	return;
}


// Function that return a value(hue) based on the min and max values given
// Jun added this 01/21/05 
inline double GetHueFromDistance(double distance, double min, double max)
{
	return (256 - ((distance - min)/(max - min)) * 256);

}