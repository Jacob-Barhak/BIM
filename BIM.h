// BIM.h : main header file for the BIM application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CBIMApp:
// See BIM.cpp for the implementation of this class
//

class CBIMApp : public CWinApp
{
public:
	CBIMApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBIMApp theApp;