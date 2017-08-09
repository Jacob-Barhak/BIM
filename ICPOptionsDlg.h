#pragma once


// CICPOptionsDlg dialog

class CICPOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CICPOptionsDlg)

public:
	CICPOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CICPOptionsDlg();

// Dialog Data
	enum { IDD = IDD_ICPOPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	int num_iterations;
	double epsilon;
	int second_ring;
	int nearest_neighbor;
	double outlier_filter;
	double normal_filter;
};
