// AutoAlignOptions.cpp : implementation file
//

#include "stdafx.h"
#include "BIM.h"
#include "AutoAlignOptions.h"


// CAutoAlignOptions dialog

IMPLEMENT_DYNAMIC(CAutoAlignOptions, CDialog)
CAutoAlignOptions::CAutoAlignOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoAlignOptions::IDD, pParent)
	, SmoothingLevels(0)
	, SmoothStep(0)
	, AcceptAlignmentThreshold(0)
	, SmoothStartRadius(0)
	, FeatureRadius(0)
	, UseFeatureRadiusForNeighborhood(FALSE)
	, IncludeBoundaryFeatures(FALSE)
{
}

CAutoAlignOptions::~CAutoAlignOptions()
{
}

void CAutoAlignOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_SMOOTH_LEVEL, SmoothingLevels);
	DDX_Text(pDX, IDC_SMOOTH_STEP, SmoothStep);
	DDV_MinMaxDouble(pDX, SmoothStep, 1.0, 10.0);
	DDX_Text(pDX, IDC_EDIT_ACCEPT_ALIGNMENT_THRESHOLD, AcceptAlignmentThreshold);
	DDV_MinMaxDouble(pDX, AcceptAlignmentThreshold, 0.0, 1.0);
	DDX_Text(pDX, IDC_SMOOTH_START_RADIUS, SmoothStartRadius);
	DDV_MinMaxDouble(pDX, SmoothStartRadius, 0.0, 1.0);
	DDX_Text(pDX, IDC_FEATURE_RADIUS, FeatureRadius);
	DDV_MinMaxDouble(pDX, FeatureRadius, 0.0, 999.0);
	DDX_Check(pDX, IDC_USE_NEIGHBORHOOD, UseFeatureRadiusForNeighborhood);
	DDX_Check(pDX, IDC_INCLUDE_BOUNDARY_FEATURES, IncludeBoundaryFeatures);
}


BEGIN_MESSAGE_MAP(CAutoAlignOptions, CDialog)
END_MESSAGE_MAP()


// CAutoAlignOptions message handlers


