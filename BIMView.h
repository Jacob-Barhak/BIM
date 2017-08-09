// BIMView.h : interface of the CBIMView class
//

#include "Arcball.h"
#include "TransformDlg.h"
#include "xvec.h"

// mouse
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010

// keyboard
#define VK_CONTROL        0x11
#define VK_PRIOR          0x21
#define VK_NEXT           0x22

#pragma once


class CBIMView : public CView
{
	// Jun added this 10/14/04
	enum { NOEDIT_MODE, EDIT_MODE, PICK_MODE, PT_PICK_MODE, PARTMEASUREMENT, MESH};

protected: // create from serialization only
	CBIMView();
	DECLARE_DYNCREATE(CBIMView)

// Attributes
public:
	BIMWorkspace* GetDocument() const;
	HGLRC m_hrc ; 			//OpenGL Rendering Context
	CPalette* m_pPal ;		//Palette
	bool drawColorbar;
	GLuint colorbarTex;
	GLfloat colorbarValues[256][4];
	GLuint fontDispList;
	double stepsize ;         //Sowmya added this 4/2/05
	CPoint top_left;
	CPoint bottom_right;

	bool LButton;
	bool LButtonPressed;
	
#ifdef _DEBUG 
	CMemoryState MyMemCheck;
#endif

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void PickObject(CPoint point1, CPoint point2, bool LButtonDown);
	void ProcessHits(int hits, GLuint* buffer, bool LButtonDown);
	void DrawScene();
	void CalculateBoundingBox(BIMWorkspace* bimWorkspace);
	void Draw3DAxes();
	static void ROG_Callback(double x, double y, double z, void* bimview);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CBIMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileImport();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPlusX();
	afx_msg void OnMinusX();
	afx_msg void OnPlusY();
	afx_msg void OnMinusY();
	afx_msg void OnPlusZ();
	afx_msg void OnMinusZ();

	afx_msg void OnPlusRotateX();
	afx_msg void OnMinusRotateX();
	afx_msg void OnPlusRotateY();
	afx_msg void OnMinusRotateY();
	afx_msg void OnPlusRotateZ();
	afx_msg void OnMinusRotateZ();
	afx_msg void OnPickMode();
	afx_msg void OnZoomAll();

	
	
	// *******************************
	// Jun added this 10/14/04
	
	// Function to set up the lights
	void SetupLights(void);

	// Function to set up the viewport
	void SetupViewport(void);

	// Function to set up the viewing frustum
    void SetupViewingFrustum(void);
	
	// Function to set up the viewing transform
	void SetupViewingTransform(void);

	// Function to set the bounds
	void SetBounds(float* _boxMin, float* _boxMax);


	float boxCenter[3];
	float boxMax[3];
	float boxMin[3];
	
	ArcBall my_arcball;
	
	float m_far;
	float m_near;
	float m_fov;
	float m_aspect;
	float m_eps; // used for error driven rendering
	
	XVecf m_trans;  // object translation
	XVecf m_ctrans; // camera translation
	XVecf m_coldtrans; // old camera translation
	
	int iMode;
	
	bool bPick;
	bool bTPan;
	bool bTranslate;
	bool redo;
	bool bPan;
	
	int m_WindowWidth;
	int m_WindowHeight;

	XVecf bmin, bmax;
	// *******************************

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnRegisterByicp();

	afx_msg void OnRegisterIcpoptions();
	afx_msg void OnViewVisualizationoptions();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRegisterShowcolorbar();
	afx_msg void OnMultiPtSelect();
	afx_msg void On3ptXform();
	afx_msg void OnPlusStepSize();  //Sowmya added this 4/2/05
	afx_msg void OnMinusStepSize(); //Sowmya added this 4/2/05
	afx_msg void OnMeasureScanpart();
	afx_msg void OnRegisterCurrentdistance();
	afx_msg void OnFileSavescans();
	afx_msg void OnFileExportdistances();
	afx_msg void OnRegisterAuto();
	afx_msg void OnAutoAlignOptions();
	afx_msg void OnFileExportsignature();
	afx_msg void OnRegisterMulti();
};

#ifndef _DEBUG  // debug version in BIMView.cpp
inline BIMWorkspace* CBIMView::GetDocument() const
   { return (BIMWorkspace*)m_pDocument; }
#endif

