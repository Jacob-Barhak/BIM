#include "stdafx.h"
#include "GLFont.h"
#include <stdarg.h>

using namespace std;

// Code is from http://nehe.gamedev.net and their OpenGL font example

// Creates the Window font bitmaps that we will use for displaying on the colorbar
void BuildFont(HDC hDC, GLuint& displayList)
{
	HFONT font;
	HFONT oldfont;

	displayList = glGenLists(256);
	if (glIsList(displayList) == GL_TRUE)
	{
		TRACE("Successfully made a disp list\n");
	}
	font = CreateFont(-10, 0,0,0, FW_BOLD, FALSE,FALSE,FALSE,
					  ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					  ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH,
					  "Courier New");
	oldfont = (HFONT)SelectObject(hDC, font);
	BOOL test = wglUseFontBitmaps(hDC, 0, 256, displayList);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

// Cleans up font memory
void KillFonts(GLuint fontDisplayList, int size)
{
	glDeleteLists(fontDisplayList, size);
}

// Uses the printf semantics to print a string with variables in it.
void glPrint(GLuint dispList, const char* fmt, ...)
{
	char text[256];
	va_list ap;

	if(fmt == NULL)
	{
		return;
	}
	
	va_start(ap, fmt);
		vsprintf(text, fmt, ap);
	va_end(ap);

	glPushAttrib(GL_LIST_BIT);
	glListBase(dispList);

	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}