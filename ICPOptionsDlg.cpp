// ICPOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BIM.h"
#include "ICPOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CICPOptionsDlg dialog

IMPLEMENT_DYNAMIC(CICPOptionsDlg, CDialog)
CICPOptionsDlg::CICPOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CICPOptionsDlg::IDD, pParent)
	, num_iterations(0)
	, epsilon(0)
	, normal_filter(0)
{
}

CICPOptionsDlg::~CICPOptionsDlg()
{
}

void CICPOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT__ICPOPTIONS_NUMITERATIONS, num_iterations);
	DDX_Text(pDX, IDC_EDIT_ICPOPTIONS_EPSILON, epsilon);
	DDX_Radio(pDX, IDC_ICPOPTIONS_Y2RING, second_ring);
	DDX_Text(pDX, IDC_ICPOPTIONS_NEARESTNEIGHBOR, nearest_neighbor);
	DDX_Text(pDX, IDC_ICPOPTIONS_OUTLIERFILT, outlier_filter);
	DDV_MinMaxDouble(pDX, outlier_filter, 0.0, 1.0e99);
	DDX_Text(pDX, IDC_ICPOPTIONS_NORMALFILT, normal_filter);
	DDV_MinMaxDouble(pDX, normal_filter, -2.0, 2.0);
}


BEGIN_MESSAGE_MAP(CICPOptionsDlg, CDialog)
END_MESSAGE_MAP()


// CICPOptionsDlg message handlers
