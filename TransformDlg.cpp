// TransformDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BIM.h"
#include "TransformDlg.h"

// TransformDlg dialog

IMPLEMENT_DYNAMIC(TransformDlg, CDialog)
TransformDlg::TransformDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TransformDlg::IDD, pParent)
{
}

TransformDlg::~TransformDlg()
{
}

// Returns value in X-Axis edit box
double TransformDlg::XAxisValue()
{
}

// Returns value in Y-Axis edit box
double TransformDlg::YAxisValue()
{
}

// Returns value in Z-Axis edit box
double TransformDlg::ZAxisValue()
{
}

// Returns whether rotation radio button is pressed
bool TransformDlg::RotationSelected()
{
}

// Returns whether translation radio button is pressed
bool TransformDlg::TranslationSelected()
{
}

void TransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TransformDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedApply)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedExit)
END_MESSAGE_MAP()


// TransformDlg message handlers
// Send data back to the view to have it apply the transformation to the selected model
void TransformDlg::OnBnClickedApply()
{
	CWnd* parent = GetParent();
	ASSERT(parent);
	if(parent)
	{
		parent->SendMessage(WM_XFORMDATA, 0, 0);
	}
}

// Kills our dialog box
void TransformDlg::OnBnClickedExit()
{
	this->DestroyWindow();
}
