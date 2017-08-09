// BIMDoc.h : interface of the CBIMDoc class
//

#pragma once

class CBIMDoc : public CDocument
{
protected: // create from serialization only
	CBIMDoc();
	DECLARE_DYNCREATE(CBIMDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CBIMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


