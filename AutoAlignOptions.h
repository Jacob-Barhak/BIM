#pragma once


// CAutoAlignOptions dialog

class CAutoAlignOptions : public CDialog
{
	DECLARE_DYNAMIC(CAutoAlignOptions)

public:
	CAutoAlignOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAutoAlignOptions();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int SmoothingLevels;
	double SmoothStep;

	double AcceptAlignmentThreshold;
	double SmoothStartRadius;
	double FeatureRadius;
	BOOL UseFeatureRadiusForNeighborhood;
	BOOL IncludeBoundaryFeatures;
};
