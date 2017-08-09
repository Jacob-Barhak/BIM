#pragma once


// CVisualizationOptionsDlg dialog

class CVisualizationOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CVisualizationOptionsDlg)

public:
	CVisualizationOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVisualizationOptionsDlg();

// Dialog Data
	enum { IDD = IDD_VISUALIZATIONOPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	double point_size;
	int point_type;
	afx_msg void OnEnChangeEditVizoptionsPtsize();
};
