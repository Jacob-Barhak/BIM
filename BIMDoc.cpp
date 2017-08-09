// BIMDoc.cpp : implementation of the CBIMDoc class
//

#include "stdafx.h"
#include "BIM.h"

#include "BIMDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBIMDoc

IMPLEMENT_DYNCREATE(CBIMDoc, CDocument)

BEGIN_MESSAGE_MAP(CBIMDoc, CDocument)
END_MESSAGE_MAP()


// CBIMDoc construction/destruction

CBIMDoc::CBIMDoc()
{
	// TODO: add one-time construction code here

}

CBIMDoc::~CBIMDoc()
{
}

BOOL CBIMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CBIMDoc serialization

void CBIMDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CBIMDoc diagnostics

#ifdef _DEBUG
void CBIMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBIMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBIMDoc commands
