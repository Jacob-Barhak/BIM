#include <string>

#pragma once
#define WM_XFORMDATA (WM_APP + 1)

// TransformDlg dialog

class TransformDlg : public CDialog
{
	DECLARE_DYNAMIC(TransformDlg)

public:
	TransformDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TransformDlg();

	double XAxisValue();
	double YAxisValue();
	double ZAxisValue();

	bool RotationSelected();
	bool TranslationSelected();

// Dialog Data
	enum { IDD = IDD_TRANSFORMDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedExit();
};
