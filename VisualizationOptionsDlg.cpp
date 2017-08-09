// VisualizationOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BIM.h"
#include "VisualizationOptionsDlg.h"
#include ".\visualizationoptionsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CVisualizationOptionsDlg dialog

IMPLEMENT_DYNAMIC(CVisualizationOptionsDlg, CDialog)
CVisualizationOptionsDlg::CVisualizationOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualizationOptionsDlg::IDD, pParent)
	, point_size(0)
	, point_type(0)
{
}

CVisualizationOptionsDlg::~CVisualizationOptionsDlg()
{
}

void CVisualizationOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VIZOPTIONS_PTSIZE, point_size);
	DDX_Radio(pDX, IDC_RADIO_VIZOPTIONS_CROSS, point_type);
}


BEGIN_MESSAGE_MAP(CVisualizationOptionsDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_VIZOPTIONS_PTSIZE, OnEnChangeEditVizoptionsPtsize)
END_MESSAGE_MAP()


// CVisualizationOptionsDlg message handlers

void CVisualizationOptionsDlg::OnEnChangeEditVizoptionsPtsize()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
