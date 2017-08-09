/*
	This defines the functions that we can use to display 2D fonts on screen.
*/

#ifndef GLFONT_H
#define GLFONT_H

// Builds the font
void BuildFont(HDC hDC, GLuint& displayList);
void KillFonts(GLuint fontDisplayList, int size);
void glPrint(GLuint dispList, const char* fmt, ...);

#endif