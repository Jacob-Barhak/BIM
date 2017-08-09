// BIMView.cpp : implementation of the CBIMView class
#include "stdafx.h"
#include "BIM.h"

#include "BIMWorkspace.h"
#include "BIMView.h"
#include "BBox.h"
#include "MainFrm.h"
#include <string>
#include "ICPOptionsDlg.h"
#include "VisualizationOptionsDlg.h"
#include "ColorConversion.h"
#include "GLFont.h"
#include "bimview.h"
#include "xvec.h"
#include "AutoAlignOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBIMView

IMPLEMENT_DYNCREATE(CBIMView, CView)

// MFC message map
BEGIN_MESSAGE_MAP(CBIMView, CView)
	// Standard printing commands
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_PLUSX, OnPlusX)
	ON_COMMAND(ID_MINUSX, OnMinusX)
	ON_COMMAND(ID_PLUSY, OnPlusY)
	ON_COMMAND(ID_MINUSY, OnMinusY)
	ON_COMMAND(ID_PLUSZ, OnPlusZ)
	ON_COMMAND(ID_MINUSZ, OnMinusZ)
	ON_COMMAND(ID_PLUSROTATEX, OnPlusRotateX)
	ON_COMMAND(ID_MINUSROTATEX, OnMinusRotateX)
	ON_COMMAND(ID_PLUSROTATEY, OnPlusRotateY)
	ON_COMMAND(ID_MINUSROTATEY, OnMinusRotateY)
	ON_COMMAND(ID_PLUSROTATEZ, OnPlusRotateZ)
	ON_COMMAND(ID_MINUSROTATEZ, OnMinusRotateZ)
	ON_COMMAND(ID_PICKMODE, OnPickMode)
	ON_COMMAND(ID_ZOOMALL, OnZoomAll)
	ON_COMMAND(ID_MULTIPTSELECT, OnMultiPtSelect)
	ON_COMMAND(ID_3PTXFORM, On3ptXform)
	ON_COMMAND(ID_PLUS_STEPSIZE, OnPlusStepSize)	//Sowmya added this 4/2/05
	ON_COMMAND(ID_MINUS_STEPSIZE, OnMinusStepSize)	//Sowmya added this 4/2/05
	ON_WM_LBUTTONDOWN(OnLButtonDown)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_REGISTER_BYICP, OnRegisterByicp)
//	ON_COMMAND(ID_REGISTER_ICPOPTIONS143, OnRegisterIcpoptions143)
//	ON_COMMAND(ID_VIEW_VISUALIZATIONOPTIONS142, OnViewVisualizationoptions142)
ON_COMMAND(ID_REGISTER_ICPOPTIONS, OnRegisterIcpoptions)
ON_COMMAND(ID_VIEW_VISUALIZATIONOPTIONS, OnViewVisualizationoptions)
ON_WM_KEYUP()
ON_COMMAND(ID_REGISTER_SHOWCOLORBAR, OnRegisterShowcolorbar)
ON_COMMAND(ID_MEASURE_SCANPART, OnMeasureScanpart)
ON_COMMAND(ID_REGISTER_CURRENTDISTANCE, OnRegisterCurrentdistance)
ON_COMMAND(ID_FILE_SAVESCANS, OnFileSavescans)
ON_COMMAND(ID_FILE_EXPORTDISTANCES, OnFileExportdistances)
ON_COMMAND(ID_REGISTER_AUTO, OnRegisterAuto)
ON_COMMAND(ID_REGISTER_AUTO32807, OnAutoAlignOptions)
ON_COMMAND(ID_FILE_EXPORTSIGNATURE, OnFileExportsignature)
ON_COMMAND(ID_REGISTER_MULTI, OnRegisterMulti)
END_MESSAGE_MAP()


// CBIMView construction/destruction
/***********************************************
/*
	Default constructor
							modified by: Jun 
							date: 10/14/04
*/
/***********************************************/
CBIMView::CBIMView()
{
	// Initializing member variables
	stepsize = 1.0;  //Sowmya added this 4/2/05
	m_WindowWidth=0;
	m_WindowHeight=0;
	
	m_near = 1 / sinf( M_PI/180 * 25 ) ;
    m_far = 1 / sinf( M_PI/180 * 25 ) + 2 ;
    m_fov = 50;
    m_eps = 0.01f;
	bPan = false;
	redo = true;
	bPick = false;
	bTranslate = false;
	bTPan = false;
	iMode = NOEDIT_MODE;

	LButton = false;
	LButtonPressed = false;

	m_trans[0] = m_trans[1] = m_trans[2] = 0.0f;
	m_coldtrans[0] = m_coldtrans[1] = m_coldtrans[2] = 0.0f;
	m_ctrans[0] = m_ctrans[1] = 0.0f;
	m_ctrans[2] = -( 1+1/sinf(M_PI/180*25) );

	bmin = XVecf(1e15);
    bmax = XVecf(-1e15);
	//funcPtr = &CBIMView::Invalidate;
	drawColorbar = false;
	colorbarTex = 0;

	//Initialize 1D texture values
	for (int i=0; i < 256; i++)
	{
		double H = (double)(256-i);
		double R,G,B;
		ConvertHSVtoRGB(H, 1, 1, R, G, B);
		colorbarValues[i][0] = R;
		colorbarValues[i][1] = G;
		colorbarValues[i][2] = B;
		colorbarValues[i][3] = 1;
		//TRACE("Color: %d,%d,%d", colorbarValues[i][0], colorbarValues[i][1], colorbarValues[i][2]);
	}
	// Initialize our 1D Texture
	glEnable(GL_TEXTURE_1D);
	glGenTextures (1, &colorbarTex);	// Get A Free Texture ID
	glBindTexture (GL_TEXTURE_1D, colorbarTex);	// Bind This Texture. From Now On It Will Be 1D
	
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// Upload
	glTexImage1D (GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, colorbarValues);
	if (glIsTexture(colorbarTex)==GL_FALSE)
	{
		TRACE("Texture is fouled up\n");
	}
	glBindTexture(GL_TEXTURE_1D, 0);
	fontDispList = 1000;
}

CBIMView::~CBIMView()
{
	//MyMemCheck.DumpAllObjectsSince();
	TRACE("CBIMView Destrctor\n");
}

// Draws the BIM workspace thru opengl
void CBIMView::DrawScene()
{
	// Get BIMWorkspace
	BIMWorkspace* bimWorkspace = GetDocument();

	// Jun added this 10/14/04
	static GLdouble blue[] = {0.2, 0.2, 1.0, 1.0 } ; 
	
	/*
	
	Jun commented this out 10/14/04
	
	//Enable Lighting? Or no?
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	
	*/
	
	
	// Enable depth calculations
    glEnable(GL_DEPTH_TEST);

	// Clear the color and depth buffers
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	// Jun added this 10/14/04
	// Set the material color to follow the current color
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE) ;
	glEnable(GL_COLOR_MATERIAL) ;
	
	// Jun added this 10/14/04
	// set to modelview mode
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	// Jun added this 10/14/04
	glColor4dv(blue) ;
	
	// Jun added this 10/14/04
	glPushMatrix();
		// Jun added this 10/14/04
		// set viewing transform
		SetupViewingTransform();

		// Jun added this 10/14/04
		// setup lights
		//SetupLights();

		bimWorkspace->SetArcball(my_arcball);
				
		// Draw workspace
		bimWorkspace->Draw();	
		
		Draw3DAxes();

		// Code to draw Colorbar.
		if(drawColorbar)
		{
			HDC hdc = wglGetCurrentDC();
			BuildFont(hdc, fontDispList);
			
			//Code to draw color bar
			RECT window;										// Storage For Window Dimensions
			GetClientRect (&window);							// Get Window Dimensions
			TRACE("Client window: %d, %d, %d, %d\n", window.top, window.bottom, window.left, window.right);
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glPushMatrix();										// Store The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix
			glOrtho(0,window.right,0,window.bottom,-1,2);		// Set Up An Ortho Screen
			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glPushMatrix();
			glLoadIdentity();
		
			float barHeight = (window.bottom/2); // get number of pixels to make bar 25% of window height
			float barWidth = (window.right/25); // get number of pixels to make bar 5% the width of the window.
			float barSubdivisions = 1; // 1 pixel high.
			
			glDisable(GL_LIGHTING);
			for(int i = 0; i < barHeight; i++)
			{
				double dindex = (double)(256.0/barHeight)*(double)i;
				int index = (int)dindex;
				glPushMatrix();
				glTranslated(window.right - 2*barWidth, window.bottom-barHeight-barWidth+barSubdivisions*i,0.0f);
				glBegin(GL_QUADS);
					glColor4f(colorbarValues[index][0],colorbarValues[index][1],colorbarValues[index][2],colorbarValues[index][3]);
					glVertex3f(0, -barSubdivisions, 0.0f);
					glVertex3f(barWidth, -barSubdivisions, 0.0f);
					glVertex3f(barWidth, barSubdivisions, 0.0f);
					glVertex3f(0, barSubdivisions, 0.0f);
				glEnd();
				glPopMatrix();
			}

			BIMWorkspace* bimWorkspace = GetDocument();
			
			glPushMatrix();
			glColor3f(0,0,0);
			glRasterPos2f(window.right - 2*barWidth, window.bottom - barHeight - 1.25*barWidth);
			glPrint(fontDispList, "%lf", bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.DisplayPropertiesColorToolBarOptions.min);
			glRasterPos2f(window.right - 2*barWidth, window.bottom - barHeight - barWidth + barSubdivisions*barHeight + 1);
			glPrint(fontDispList, "%lf", bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.DisplayPropertiesColorToolBarOptions.max);
			glPopMatrix();

			KillFonts(fontDispList, 256);
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glPopMatrix();								// Restore The Old Projection Matrix
			glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
			glPopMatrix();
			glEnable(GL_LIGHTING);
		}
	

	// Jun added this 10/14/04	
	glPopMatrix();
	
	// Jun added this 10/14/04
	glFlush();
	
	GLenum error_code;
	error_code = glGetError();

	switch(error_code)
	{	
		case GL_NO_ERROR:
			// No error, do nothing
			break;
		case GL_INVALID_ENUM:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		case GL_INVALID_VALUE:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		case GL_INVALID_OPERATION:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		case GL_STACK_OVERFLOW:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		case GL_STACK_UNDERFLOW:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		case GL_OUT_OF_MEMORY:
			TRACE("Error in DrawScene(), error code = %d", error_code ) ;
			break;
		
	}
}

// Drawing the 3D axes
void CBIMView::Draw3DAxes()
{
	// Draw the Cylinders and cones
	//HDC hdc = wglGetCurrentDC();
	//BuildFont(hdc, fontDispList);
	glDisable(GL_LIGHTING);	
	//Set up Ortho projection
	RECT window;										// Storage For Window Dimensions
	GetClientRect (&window);							// Get Window Dimensions
	TRACE("Client window: %d, %d, %d, %d\n", window.top, window.bottom, window.left, window.right);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,window.right,0,window.bottom,-100,100);		// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();
	glLoadIdentity();

	// Get rotation matrix from the arcball settings.
	GLfloat M[4][4];
	my_arcball.Value( M );
	
	glPushMatrix();
	glTranslated(65, 65, -25);
	glPushMatrix();
	glMultMatrixf( ( float *)M );

	GLUquadricObj *cylinder, *cone, *disk;
	cylinder = gluNewQuadric();
    cone = gluNewQuadric();
	disk = gluNewQuadric();

	// draw z axis
	glPushMatrix();
		glColor3f(0, 1, 0);
		gluCylinder(cylinder, 5, 5, 45, 15, 15); 
		glPushMatrix();
			glColor3f(0,1,0);
			glTranslatef(0,0,45);
			gluCylinder(cone, 10, 0, 15, 15, 15);
			gluDisk(disk, 0, 10, 15, 15);
		glPopMatrix();
	glPopMatrix();

	// draw x axis
	glPushMatrix();
		glColor3f(1, 0, 0);
		glRotatef(90,0,1,0);
		gluCylinder(cylinder, 5, 5, 45, 15, 15); 
		glPushMatrix();
			glColor3f(1,0,0);
			glTranslatef(0,0,45);
			gluCylinder(cone, 10, 0, 15, 15, 15); 
			gluDisk(disk, 0, 10, 15, 15);
		glPopMatrix();
	glPopMatrix();
	
	// draw y axis
	glPushMatrix();
		glColor3f(0, 0, 1);
		glRotatef(-90,1,0,0);
		gluCylinder(cylinder, 5, 5, 45, 15, 15); 
		glPushMatrix();
			glColor3f(0,0,1);
			glTranslatef(0,0,45);
			gluCylinder(cone, 10, 0, 15, 15, 15); 
			gluDisk(disk, 0, 10, 15, 15);
		glPopMatrix();
	glPopMatrix();
	
    gluDeleteQuadric(cylinder);  
	gluDeleteQuadric(cone);  

	glPopMatrix();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();								// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glPopMatrix();
	glEnable(GL_LIGHTING);
	// Draw 3D text. ??
}

// MFC setup code
BOOL CBIMView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// OnDestroy MFC setup code
void CBIMView::OnDestroy() 
{
	CView::OnDestroy();
	
	// Clean up OpenGL handles and device contexts
	wglMakeCurrent(NULL,NULL); 
	if (m_hrc != NULL) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}		
}

// OnCreate, MFC setup code
int CBIMView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	TRACE0("OnCreate\r\n");
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
    CClientDC dc(this);
	
	// Fill in the Pixel Format Descriptor
    PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);   
    pfd.nVersion = 1;                           // Version number
	pfd.dwFlags =  PFD_DOUBLEBUFFER |            // Use double buffer
	               PFD_SUPPORT_OPENGL |          // Use OpenGL
	               PFD_DRAW_TO_WINDOW;          // Pixel format is for a window.
	pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;                         // 24-bit color
	pfd.cDepthBits = 32;					   	 // 32-bit depth buffer
    pfd.iLayerType = PFD_MAIN_PLANE;            // Layer type

    int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	TRACE("Pixel Format %d\r\n", nPixelFormat) ;

    BOOL bResult = SetPixelFormat(dc.m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	
    // Create a rendering context.
    m_hrc = wglCreateContext(dc.m_hDC);
	if (!m_hrc)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return -1;
	}

	return 0;
}

// OnSize MFC setup code
void CBIMView::OnSize(UINT nType, int cx, int cy) 
{
	TRACE0("OnSize\r\n") ;
	CView::OnSize(nType, cx, cy);

	if ( (cx <= 0) || (cy <= 0) ) 
	{
		return ;
	}

	// Jun added this 10/14/04
	m_WindowWidth = cx;
	m_WindowHeight = cy;
	m_aspect = (GLdouble)cx / (GLdouble)cy;

    CClientDC dc(this) ;

	// Make the rendering context m_hrc current
    BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		return ;
	}

	// Jun added this 10/14/04
	SetupViewport();
	SetupViewingFrustum();
	SetupLights();
	// No rendering context will be current.
    wglMakeCurrent(NULL, NULL);	
}

// CBIMView drawing
void CBIMView::OnDraw(CDC* pDC)
{
	
	BIMWorkspace* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Make the HGLRC current
    BOOL bResult = wglMakeCurrent(pDC->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	// Jun added this 10/14/04
	SetupViewingFrustum();
	SetupLights();
	// Draw	
	DrawScene(); 

	//Swap Buffers
	SwapBuffers(pDC->m_hDC);

	wglMakeCurrent(NULL, NULL);
}

// PickObject
// Picks a mesh or point cloud and highlights that object in green.
// NOTE: For right now we only pick a point cloud.
//       Camera is also a hack at the moment.
// TODOD: Make this so we define the picking box....
void CBIMView::PickObject(CPoint point, CPoint point2, bool LButtonDown)
{
	//Picking code:
	// Enter OGL selection mode as per nehe or lighthouse sites.
	// Go into draw functions and name the objects.

	// Picking code
	TRACE("In PickObject\n");
	GLuint pickBuffer[65536]; // Fixed for now, may need to be some multiple of how big point clouds are.
	GLint viewport[4];
	int hits;

	// Make the HGLRC current
    BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	glSelectBuffer(65536, pickBuffer); 
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT, viewport);
	TRACE("Viewport: %d, %d, %d, %d\n", viewport[0], viewport[1], viewport[2], viewport[3]);
	// Set up pick matrix

	gluPickMatrix(point.x, viewport[3]-point.y, 5, 5, viewport);
	
	GLdouble gldAspect = (GLdouble)(viewport[2]-viewport[0])/(GLdouble) (viewport[3]-viewport[1]);
	// Set up arcball viewing
	gluPerspective( m_fov / __min( 1.f, gldAspect ), gldAspect, m_near, m_far*5 );
	glMatrixMode(GL_MODELVIEW);
	//Move camera to current arcball position
	SetupViewingTransform();
	glInitNames();

	// DO SELECTION RENDERING OF THE PT CLOUDS AND THE MESHES
	BIMWorkspace* pDoc = GetDocument();
	pDoc->Draw();
	pDoc->DrawMeshSelectionVerts();

	// Return the matrix modes to normal
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	// returning to normal rendering mode
	hits = glRenderMode(GL_RENDER);
	
	// Process the hits
	if(hits > 0)
	{
		TRACE("Had %d hits\n", hits);
		ProcessHits(hits, pickBuffer, LButtonDown);
	}
	else
	{
		// Get BIMWorkspace
		BIMWorkspace* bimWorkspace = GetDocument();
		bimWorkspace->SelectedPartMeasurement = false;
		bimWorkspace->SelectedPartMesh = false;
		bimWorkspace->BIMPartMesh.redo = true;
		OnDraw(GetDC());
	}
	
	return;
}

// ProcessHits
// Processes the returned hit buffer from the PickObject function
// and finds the closest object in the picking region and sets that as
// being selected.
void CBIMView::ProcessHits(int hits, GLuint* buffer, bool LButtonDown)
{
	GLuint minZ = 0xffffffff;
	GLuint numNames;
	GLuint name;
	GLuint pt_name;
	GLuint* new_buffer = buffer;

	// Get BIMWorkspace
	BIMWorkspace* bimWorkspace = GetDocument();

	// Assuming either 1 or 2 names per hit.
	for(int i = 0; i < hits; i++)
	{
	  numNames = *buffer;
	  buffer++;
	  if (numNames == 1)
	  {
		if((GLuint)*buffer < minZ)
		{
			minZ = *buffer;
			name = *(buffer+2);
			pt_name = name;
		}

		// if we're in pt pick mode, and we get the mesh as the first hit,
		// throw it out. Pt clouds always have 2 names in their hits.
		if(iMode == PT_PICK_MODE && ((int)name == -1))
		{
			// resetting minZ
			minZ = 0xffffffff;
		}
	  }
	  else if(numNames == 2)
	  {
		if ((GLuint)*buffer < minZ) 
		{
			minZ = *buffer;
			name = *(buffer+2);
			pt_name = *(buffer+3);
		}
	  }

	  // If in partition scan, highlight a point cloud point specified in buffer, ignoring mesh points.

	  buffer += numNames+2;
	}

	// Set which pt cloud or mesh to highlight
	TRACE("Picking pt cloud or mesh %d\n", (int)name);
	
	if(iMode == PICK_MODE)
	{
		if((int)name >= 0)
			bimWorkspace->SelectedPartMeasurement = true;
		else
		{
			bimWorkspace->SelectedPartMesh = true;
			bimWorkspace->BIMPartMesh.redo = true;
		}
		bimWorkspace->SelectedPart = (int)name;
	}
	else if(iMode == PT_PICK_MODE)
	{
		// put pt's we selected here.
		if((int)name >= 0 && (int)name < bimWorkspace->BIMPartMeasurements.BIMPartMeasurements.size())
		{
			if(bimWorkspace->pt_select == false)
			{
				float x,y,z;
				x = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].x;
				y = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].y;
				z = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].z;
				XVecf temp(x,y,z);
				float* mat = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementTransMatrix.Transpose();
				temp = mult(mat, temp);
				bimWorkspace->selectedMeasurementPts.push_back(temp);
				bimWorkspace->SelectedPart = (int)name;
				bimWorkspace->pt_select = true;
				delete[] mat;
				TRACE("In 3pt pick: Part %d, Pt %d\n", (int)name, (int)pt_name);
			}
			else if(bimWorkspace->SelectedPart == (int)name)
			{
				float x,y,z;
				x = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].x;
				y = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].y;
				z = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].z;
				XVecf temp(x,y,z);
				float* mat = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementTransMatrix.Transpose();
				temp = mult(mat, temp);
				if(bimWorkspace->selectedMeasurementPts.size() < 3)
				{
					bimWorkspace->selectedMeasurementPts.push_back(temp);
				}
				else
				{
					bimWorkspace->selectedMeasurementPts.pop_front();
					bimWorkspace->selectedMeasurementPts.push_back(temp);
				}	
				delete[] mat;
				TRACE("In 3pt pick: Part %d, Pt %d\n", (int)name, (int)pt_name);
			}
			else if(bimWorkspace->SelectedPart != (int)name)
			{
				float x,y,z;
				x = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].x;
				y = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].y;
				z = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementSampledPoints.BIMSampledPoints[(int)pt_name].z;
				XVecf temp(x,y,z);
				float* mat = bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[(int)name].PartMeasurementTransMatrix.Transpose();
				temp = mult(mat, temp);
				bimWorkspace->selectedMeasurementPts.clear();
				bimWorkspace->selectedMeasurementPts.push_back(temp);
				bimWorkspace->SelectedPart = (int)name;
				delete[] mat;
				TRACE("In 3pt pick: Part %d, Pt %d\n", (int)name, (int)pt_name);
			}
		}
		else
		{
			// Make sure the selected point is actually within the array bounds.
			if ((int)pt_name > 0 && (int)pt_name < (bimWorkspace->mesh_vertices.size()))
			{
				if (bimWorkspace->selectedMeshPts.size() < 3)
				{
					bimWorkspace->selectedMeshPts.push_back(bimWorkspace->mesh_vertices[(int)pt_name]);
				}
				else
				{
					bimWorkspace->selectedMeshPts.pop_front();
					bimWorkspace->selectedMeshPts.push_back(bimWorkspace->mesh_vertices[(int)pt_name]);
				}
				bimWorkspace->pt_select = true;
			}
		}
	}
	

	// Refresh the scene
	OnDraw(GetDC());
	
	return;
}

// CBIMView printing

BOOL CBIMView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBIMView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBIMView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CBIMView diagnostics

#ifdef _DEBUG
void CBIMView::AssertValid() const
{
	CView::AssertValid();
}

void CBIMView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

BIMWorkspace* CBIMView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(BIMWorkspace)));
	return (BIMWorkspace*)m_pDocument;
}
#endif //_DEBUG


// CBIMView message handlers

// OnFileImport
// This message handler imports the selected file.
void CBIMView::OnFileImport()
{
	// string to hold filepath
	string filePath;
	// string to hold file extension
	string fileExt;

	// Get document object associated with this view
	BIMWorkspace* bimWorkspace = GetDocument();
	// Create a new file open dialog box
	CFileDialog* fileImport = new CFileDialog(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Point Cloud (*.snr)|*.snr|Point Cloud(*.pts)|*.pts|Mesh File (*.obj)|*.obj|Mesh Signature File (*.sig)|*.sig||\0",
		NULL,
		0);

	// Display FileDialog box
	fileImport->DoModal();
	// Get path and extension of file to be imported
	filePath = fileImport->GetPathName();
	fileExt = fileImport->GetFileExt();

	//MyMemCheck.Checkpoint();
	//MyMemCheck.DumpAllObjectsSince();

	// Adds point cloud to workspace
	if (fileExt == "snr" || fileExt == "pts")
	{
		bimWorkspace->AddNewPartMeasurements(filePath);
		
		CalculateBoundingBox(bimWorkspace);
	}
	else if (fileExt == "obj")
	{
		// Adds mesh to workspace
		
		// Jun added everything in this 'else' section 10/14/04
		CString filename = fileImport->GetPathName();
		bimWorkspace->AddNewPartMesh(filePath);
		CalculateBoundingBox(bimWorkspace);
		
		OnDraw(GetDC());	
	}
	else if (fileExt == "sig")
	{
		//load signature file
		CString filename = fileImport->GetPathName();
		bimWorkspace->AddNewSignature(filePath);
	}
	else
	{
		CDialog* newDialog = new CDialog(IDD_FILENOTSUPPORTED, NULL);
		newDialog->DoModal();
		delete newDialog;
	}

	delete fileImport;


	// Redraws the scene to show update
	OnDraw(GetDC());	
}

// Translates the point cloud along the X axis.
void CBIMView::OnPlusX()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated((0.25*stepsize), 0.0, 0.0);
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Translates the point cloud along the X axis.
void CBIMView::OnMinusX()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated((-0.25*stepsize), 0.0, 0.0);
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Translates the point cloud along the Y axis.
void CBIMView::OnPlusY()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	TRACE("Clicked PlusX\n");
	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(0.0, (0.25*stepsize), 0.0);
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
	
}

// Translates the point cloud along the Y axis.
void CBIMView::OnMinusY()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	TRACE("Clicked PlusX\n");
	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(0.0, (-0.25*stepsize), 0.0);
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Translates the point cloud along the Z axis.
void CBIMView::OnPlusZ()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	TRACE("Clicked PlusX\n");
	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(0.0, 0.0, (0.25*stepsize));
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
	
}

// Translates the point cloud along the Z axis.
void CBIMView::OnMinusZ()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	TRACE("Clicked PlusX\n");
	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(0.0, 0.0, (-0.25*stepsize));
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Rotates the point cloud about the X axis.
void CBIMView::OnPlusRotateX()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	TRACE("Clicked PlusX\n");
	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((0.50*stepsize), 1.0, 0.0, 0.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
	
}

// Rotates the point cloud about the X axis.
void CBIMView::OnMinusRotateX()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((-0.50*stepsize), 1.0, 0.0, 0.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Rotates the point cloud about the Y axis.
void CBIMView::OnPlusRotateY()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		TRACE("%f, %f, %f\n", middle.x(), middle.y(), middle.z());
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((0.50*stepsize), 0.0, 1.0, 0.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
	
}

// Rotates the point cloud about the Y axis.
void CBIMView::OnMinusRotateY()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((-0.50*stepsize), 0.0, 1.0, 0.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// Rotates the point cloud about the Z axis.
void CBIMView::OnPlusRotateZ()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((0.50*stepsize), 0.0, 0.0, 1.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
	
}

// Rotates the point cloud about the Z axis.
void CBIMView::OnMinusRotateZ()
{
	BOOL bResult = wglMakeCurrent(GetDC()->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
	}

	int partNum;
	BIMWorkspace* bim = GetDocument();
	if(bim->SelectedPartMeasurement)
	{
		partNum = bim->SelectedPart;
		XVecf middle = bim->BIMPartMeasurements.BIMPartMeasurements[partNum].CalculateCenterOfMass();
		TRACE("Part %d selected\n", partNum);
		glPushMatrix();
		// Load old matrix
		glLoadMatrixf(bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		// Add new xform
		glTranslated(middle.x(), middle.y(), middle.z());
		glRotated((-0.50*stepsize), 0.0, 0.0, 1.0);
		glTranslated(-middle.x(), -middle.y(), -middle.z());
		// Get new matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementTransMatrix.TransformationMatrix);
		glPopMatrix();

		bim->BIMPartMeasurements.BIMPartMeasurements[partNum].PartMeasurementFlags.Registered = false;
	}
	OnDraw(GetDC());
}

// zooms out camera to see everything.  Not worrying about mesh because it never moves,
// only part measurements move and rotate and need recalculated bboxes.
void CBIMView::OnZoomAll()
{
	// Get document object associated with this view
	TRACE("In ZOOMALL\n");
	BIMWorkspace* bimWorkspace = GetDocument();

	CalculateBoundingBox(bimWorkspace);
	OnDraw(GetDC());
}

// Set pick mode to select a point cloud
void CBIMView::OnPickMode()
{
	if(iMode != PICK_MODE)
	{
		iMode = PICK_MODE;
		
		// Get BIMWorkspace
		BIMWorkspace* bimWorkspace = GetDocument();
		bimWorkspace->pt_select = false;
		bimWorkspace->selectedMeasurementPts.clear();
		bimWorkspace->selectedMeshPts.clear();
	}
	else
	{
		iMode = NOEDIT_MODE;
	}
}

// Jun added this part (start from here)
/***********************************************/
/*
	Function to set bounds
							modified by: Jun
							date: 10/15/04
*/
/***********************************************/
void CBIMView::SetBounds(float* _boxMin, float* _boxMax)
{
	//boxMin = _boxMin;
  memcpy(boxMin, _boxMin, 3*sizeof(float));
	//boxMax = _boxMax;
  memcpy(boxMax, _boxMax, 3*sizeof(float));
	boxCenter[0] = 0.5f*(boxMin[0]+boxMax[0]);
	boxCenter[1] = 0.5f*(boxMin[1]+boxMax[1]);
	boxCenter[2] = 0.5f*(boxMin[2]+boxMax[2]);
}




/***********************************************/
/*
	Function to set up viewing transform							
							modified by: Jun
							date: 10/15/04
*/
/***********************************************/
void CBIMView::SetupViewingTransform()
{
	glLoadIdentity();
  // back up the camera
  glTranslatef( m_ctrans[0], m_ctrans[1], m_ctrans[2] );
  //cerr << "ctrans: " << m_ctrans[0] << " " << m_ctrans[1] << " " << m_ctrans[2] << endl;

	GLfloat M[4][4];
	my_arcball.Update();
	my_arcball.Value( M );

#if 0
//AfxMessageBox("gl matrix");
//  cerr << "gl matrix" << endl;
//  cerr << M[0][0] << " " << M[0][1] << " " << M[0][2] << " " << M[0][3] << endl;
//  cerr << M[1][0] << " " << M[1][1] << " " << M[1][2] << " " << M[1][3] << endl;
//  cerr << M[2][0] << " " << M[2][1] << " " << M[2][2] << " " << M[2][3] << endl;
//  cerr << M[3][0] << " " << M[3][1] << " " << M[3][2] << " " << M[3][3] << endl;
#endif

  // apply model transform
  glMultMatrixf( ( float *)M );

  // move world to the origin
  glTranslatef( m_trans[0], m_trans[1], m_trans[2] );
  //cerr << "trans: " << m_trans[0] << " " << m_trans[1] << " " << m_trans[2] << endl;

}



/***********************************************/
/*
	
	Function to setup viewing frustum


							modified by: Jun
							date:10/15/04
*/
/***********************************************/
void CBIMView::SetupViewingFrustum()
{

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( m_fov / __min( 1.f, m_aspect ), m_aspect, m_near, m_far*5 );
	 //cerr << "perspective: " << m_fov << " " << m_aspect << " " << m_near << " " << m_far << endl;
	glMatrixMode( GL_MODELVIEW );

}


/***********************************************/
/*
	Function to set up viewport
							modified by: Jun
							date:10/15/04
*/
/***********************************************/
void CBIMView::SetupViewport()
{
	glViewport( 0, 0, m_WindowWidth, m_WindowHeight );
}


/***********************************************/
/*
	
	Function to set up lights
							modified by: Jun
							date: 10/15/04
*/
/***********************************************/
void CBIMView::SetupLights()
{

	GLfloat position[] = {0.0, 100.0, 200.0, 0.0};
	//GLfloat position[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diff[] = {0.5,0.5,0.5, 1.0};
	GLfloat amb[] = {0.2,0.2,0.2, 1.0};
	GLfloat lmamb[] = {0.2,0.2,0.2, 1.0};
	GLfloat spec[] = {0.5,0.5,0.5,1.0};
	GLint shin = {5};
	glClearColor (1.0, 1.0, 1.0, 1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali (GL_FRONT,GL_SHININESS, shin);
	glLightfv (GL_LIGHT0, GL_POSITION, position);
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, lmamb);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel (GL_SMOOTH);


}


// Performs the appropiate action when the left mouse button is depressed.
void CBIMView::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE("Detected Lbuttondown in CBIMView\n");
	TRACE("Mouse position %d, %d\n", point.x, point.y);
	// Select a point cloud, mesh, or individual point on the point cloud
	if (iMode != PICK_MODE && iMode != PT_PICK_MODE)
	{
		my_arcball.Mouse( ( m_aspect > 1 ? m_aspect : 1 ) * ( 2.f * point.x / m_WindowWidth - 1 ),
               -( m_aspect > 1 ? 1 : 1/m_aspect ) * ( 2.f * point.y / m_WindowHeight - 1 ) );
		my_arcball.BeginDrag();
		SetCapture();
	}
}

// Jun added this 10/15/04
// Releases the mouse, and ends all Arcball operations when the left mouse button is released.
void CBIMView::OnLButtonUp(UINT nFlags, CPoint point)
{
	my_arcball.EndDrag();
	ReleaseCapture();

	// End selection box
	if (iMode == PICK_MODE || iMode == PT_PICK_MODE)
	{
		PickObject(point, point, false);
		
		if (iMode == PICK_MODE){ // pick only one point cloud, return to viewing mode
			iMode = NOEDIT_MODE;
			TRACE("Return to no edit mode\n");
		}
	}

	Invalidate(TRUE);
}


// Jun added this 10/15/04
// Defines what to do when the Mouse is moved.
void CBIMView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMainFrame *pFrame;
	CString str;
	str.Format("Mouse at %d %d",point.x,point.y);
	pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.SetPaneText(0,str);


	if(nFlags & (MK_RBUTTON | MK_LBUTTON)) 
	{

		if(iMode==EDIT_MODE && bTPan) 
		{
			// Does nothing
		} 
		else if(iMode==PICK_MODE) 
		{
			bPick = 1;
		}
		// Move camera
		else 
		{
			my_arcball.Mouse( ( m_aspect > 1 ? m_aspect : 1 ) * ( 2.f * point.x / m_WindowWidth - 1 ),
               -( m_aspect > 1 ? 1 : 1/m_aspect ) * ( 2.f * point.y / m_WindowHeight - 1 ) );
			if( nFlags & MK_RBUTTON )
			{
				// translation case
				//CVecd t = m_ball.Trans();
				float* pt = my_arcball.Trans();
				if(bPan) 
				{
					m_ctrans[0] = 
						double(m_coldtrans[0] + .5 * pt[0] * -m_coldtrans[2]);
					m_ctrans[1] = 
						double(m_coldtrans[1] + .5 * pt[1] * -m_coldtrans[2]);
				} 
				else 
				{
					m_ctrans[2] = 
					double(m_coldtrans[2] + 2 * pt[1] * -m_coldtrans[2]);
				}
			}
		}		
		
		
		if(GetCapture() == this)
		{
			
			Invalidate(TRUE);
		}

	}

	//CView::OnMouseMove(nFlags, point);
}

// Jun added this 10/15/04
// Defines what to do when the Right Mouse button is depressed.
void CBIMView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(iMode==EDIT_MODE) 
	{
		bTPan = 1;
	}
	else if(iMode==PICK_MODE) 
	{
//		cerr << "redo = true 0" << endl;
		redo = true;
	}

	
	if(iMode!=PICK_MODE) 
	{
	  my_arcball.Mouse( ( m_aspect > 1 ? m_aspect : 1 ) * ( 2.f * point.x / m_WindowWidth - 1 ),
                 -( m_aspect > 1 ? 1 : 1/m_aspect ) * ( 2.f * point.y / m_WindowHeight - 1 ) );
	  my_arcball.BeginTrans();
	  //m_coldtrans = m_ctrans;
	  memcpy(m_coldtrans, m_ctrans, 3*sizeof(float));
	}
	SetCapture();
}


// Jun added this 10/15/04
// Resets the Arcball controls.
void CBIMView::OnRButtonUp(UINT nFlags, CPoint point)
{
	bPan = false;
	bTPan = false;
	ReleaseCapture();
	Invalidate(TRUE);
}


// Jun added this 10/15/04
// Processes hot-keys and their associated actions
void CBIMView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
		
		case VK_CONTROL: // Set Arcball to pan camera
			bPan = true;
			break;
		case VK_DELETE: // Delete mesh or pointcloud
			BIMWorkspace* bimWorkspace = GetDocument();
			if(bimWorkspace->SelectedPartMesh)
			{
				bimWorkspace->DeletePartMesh();
				bimWorkspace->SelectedPartMesh = false;
				bimWorkspace->BIMPartMesh.redo = true;
				Invalidate(TRUE);
			}
			else if(bimWorkspace->SelectedPartMeasurement)
			{
				//AfxMessageBox("delete point cloud");
				bimWorkspace->DeletePartMeasurement(bimWorkspace->SelectedPart);
				bimWorkspace->SelectedPartMeasurement = false; 
				Invalidate(TRUE);
				
			}

			CalculateBoundingBox(bimWorkspace);

			OnDraw(GetDC());
			break;
	}
}

// Jun added this 10/15/04
// Needed to add this class to get rid of flicker
BOOL CBIMView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

// Geoff added this 10/27/04
// Saves the BIMWorkspace.
void CBIMView::OnFileSaveAs()
{
	// string to hold filepath
	string filePath;
	// string to hold file extension
	string fileExt;

	// Get document object associated with this view
	BIMWorkspace* bimWorkspace = GetDocument();
	// Create a new file save dialog box
	CFileDialog* fileSave = new CFileDialog(
		FALSE,
		"*.bim",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BIM Workspace (*.bim)|*.bim||\0",
		NULL,
		0);

	// Display FileDialog box
	fileSave->DoModal();
	// Get path and extension of file to be imported
	filePath = fileSave->GetPathName();
	fileExt = fileSave->GetFileExt();
	// Update our save path
	bimWorkspace->fileSavePath = filePath;

	// If we get a filename, then save.
	if (filePath.size() > 0)
	{
		bimWorkspace->WriteBIMWorkspace(filePath);
	}

	delete fileSave;
}

// Geoff added this 10/27/04
// Opens a bim workspace
void CBIMView::OnFileOpen()
{
	// string to hold filepath
	string filePath;
	// string to hold file extension
	string fileExt;

	// Get document object associated with this view
	BIMWorkspace* bimWorkspace = GetDocument();
	
	// Create a new file save dialog box
	CFileDialog* fileSave = new CFileDialog(
		TRUE,
		"*.bim",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BIM Workspace (*.bim)|*.bim||\0",
		NULL,
		0);

	// Display FileDialog box
	fileSave->DoModal();
	// Get path and extension of file to be imported
	filePath = fileSave->GetPathName();
	fileExt = fileSave->GetFileExt();

	// If we get a filename, then save.
	if (filePath.size() > 0)
	{
		bimWorkspace->ReadBIMWorkspace(filePath);
		bimWorkspace->fileSavePath = filePath;
	}
	OnZoomAll();
	OnDraw(GetDC());
	delete fileSave;
}

// Saves the current BIM workspace to the file last specified in SaveAs,
// if no file was specified, it works identical to SaveAs.
void CBIMView::OnFileSave()
{
	string filePath;

	BIMWorkspace* bimWorkspace = GetDocument();
	filePath = bimWorkspace->fileSavePath;

	if(!(filePath.size() > 0))
	{
		OnFileSaveAs();
	}
	else
	{
		bimWorkspace->WriteBIMWorkspace(filePath);
	}
	return;
}


// Jun added this 11/23
/* Function to register a point cloud using ICP algorithm */
void CBIMView::OnRegisterByicp()
{
	// Get document object associated with this view
	BIMWorkspace* bimWorkspace = GetDocument();
	
	bimWorkspace->RegisterByICP((CWnd*)this);
	//OnDraw(GetDC());
}


// Jun added this 01/15
// Function to invoke dialog box for user to 
// input ICP algorithm options
void CBIMView::OnRegisterIcpoptions()
{
	CClientDC dc(this);

	CICPOptionsDlg Dlg(this);
	
	BIMWorkspace* bimWorkspace = GetDocument();
	
	// get current values for num_iterations and epsilon
	Dlg.num_iterations = bimWorkspace->BIMOptions.ICP_num_iterations;
	Dlg.epsilon = bimWorkspace->BIMOptions.ICP_epsilon;
	Dlg.second_ring = bimWorkspace->BIMOptions.ICP_2ringfaces;
	Dlg.nearest_neighbor = bimWorkspace->BIMOptions.ICP_num_nearest_neighbor;
	Dlg.outlier_filter = bimWorkspace->BIMOptions.ICP_std_dev_filter;
	Dlg.normal_filter = bimWorkspace->BIMOptions.ICP_normal_agreement_filter;
	
	if(Dlg.DoModal() == IDOK)
	{
		// update values of num_iterations and epsilon
		bimWorkspace->BIMOptions.ICP_num_iterations = Dlg.num_iterations;
		bimWorkspace->BIMOptions.ICP_epsilon = Dlg.epsilon;
		
		bimWorkspace->BIMOptions.ICP_2ringfaces = Dlg.second_ring;
		bimWorkspace->BIMOptions.ICP_num_nearest_neighbor = Dlg.nearest_neighbor;
		bimWorkspace->BIMOptions.ICP_std_dev_filter = Dlg.outlier_filter;
		bimWorkspace->BIMOptions.ICP_normal_agreement_filter = Dlg.normal_filter;

	}

	
}

// Invokes auto-align options
void CBIMView::OnAutoAlignOptions()
{
	CClientDC dc(this);

	CAutoAlignOptions Dlg(this);

	BIMWorkspace* bimWorkspace = GetDocument();

	Dlg.SmoothingLevels = bimWorkspace->BIMOptions.SmoothingLevels;
	Dlg.SmoothStep = bimWorkspace->BIMOptions.SmoothStep;
	Dlg.SmoothStartRadius = bimWorkspace->BIMOptions.SmoothStartRadius;
	Dlg.IncludeBoundaryFeatures = bimWorkspace->BIMOptions.IncludeBoundaryFeatures;
	Dlg.FeatureRadius = bimWorkspace->BIMOptions.FeatureRadius;
	Dlg.UseFeatureRadiusForNeighborhood = bimWorkspace->BIMOptions.UseFeatureRadiusForNeighborhood;

	Dlg.AcceptAlignmentThreshold = bimWorkspace->BIMOptions.AcceptAlignmentThreshold;




	if(Dlg.DoModal() == IDOK)
	{
		bimWorkspace->BIMOptions.SmoothStep = Dlg.SmoothStep;
		bimWorkspace->BIMOptions.SmoothingLevels = Dlg.SmoothingLevels;

		bimWorkspace->BIMOptions.SmoothStartRadius = Dlg.SmoothStartRadius;
		bimWorkspace->BIMOptions.IncludeBoundaryFeatures = Dlg.IncludeBoundaryFeatures;
		bimWorkspace->BIMOptions.FeatureRadius = Dlg.FeatureRadius;
		bimWorkspace->BIMOptions.UseFeatureRadiusForNeighborhood = Dlg.UseFeatureRadiusForNeighborhood;

		bimWorkspace->BIMOptions.AcceptAlignmentThreshold = Dlg.AcceptAlignmentThreshold;
	}

	return;
}

// Jun added this 01/15
// Function to invoke dialog box for user
// to input visualization options
void CBIMView::OnViewVisualizationoptions()
{
	double point_size;
	int point_type;
	
	CClientDC dc(this);
	
	CVisualizationOptionsDlg Dlg(this);
	
	BIMWorkspace* bimWorkspace = GetDocument();
	// get current values of point_size and point_type
	
	Dlg.point_size = bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.PointSize;
	Dlg.point_type = bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.PointType;

	if(Dlg.DoModal() == IDOK)
	{
		// update values of point_size and point_type

		bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.PointSize = Dlg.point_size;
		bimWorkspace->BIMPartMeasurements.PartMeasurementsDisplayProperties.PointType = Dlg.point_type;
		bimWorkspace->BIMPartMeasurements.UpdateDisplayProperties();
	}
	
	Invalidate(TRUE);
}

// Turns Gourad shading on and off using the "s" key, and turns on or off edge display
// on the model using the "e" key.
void CBIMView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BIMWorkspace* bimWorkspace = GetDocument();
	switch(nChar)
	{
		case 'E': // Turn on edges of mesh over polygons
			bimWorkspace->BIMPartMesh.m_edgesOn = !bimWorkspace->BIMPartMesh.m_edgesOn;
			bimWorkspace->BIMPartMesh.m_edgesOnly = false;
			bimWorkspace->BIMPartMesh.redo = true;
			break;
		// Case to show edges only on the mesh.
		case 'O':
			bimWorkspace->BIMPartMesh.m_edgesOn = !bimWorkspace->BIMPartMesh.m_edgesOnly;
			bimWorkspace->BIMPartMesh.m_edgesOnly = !bimWorkspace->BIMPartMesh.m_edgesOnly;
			bimWorkspace->BIMPartMesh.redo = true;
			break;
		case 'S': // turn on/off gourad shading
			bimWorkspace->BIMPartMesh.m_smoothOn = !bimWorkspace->BIMPartMesh.m_smoothOn;
			bimWorkspace->BIMPartMesh.redo = true;
			break;
		case VK_ESCAPE: // Reset state of interface back to default
			iMode = NOEDIT_MODE;
			if (bimWorkspace->highLightedPts.size() > 0)
			{
				// clear highlighted points.
				for(int i = 0; i < bimWorkspace->highLightedPts.size(); i++)
				{
					bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[bimWorkspace->highLightedPts[i].x()].
					PartMeasurementSampledPoints.
					BIMSampledPoints[bimWorkspace->highLightedPts[i].y()].highlight = false;
				}
				bimWorkspace->highLightedPts.clear();
			}
			top_left.x = 0;
			top_left.y = 0;
			bottom_right = top_left; // Reset the selection box vertices.
			break;
	}
	//CView::OnKeyUp(nChar, nRepCnt, nFlags);
	Invalidate(TRUE);

}

// Displays the color bar on screen.
void CBIMView::OnRegisterShowcolorbar()
{
	drawColorbar = (!drawColorbar);
	OnDraw(GetDC());
	return;
}

// Puts the program into 3 point selection mode where the user can select 2 sets
// of 3 points, 1 set on the mesh, and another on 1 point cloud to be used for alignment.
void CBIMView::OnMultiPtSelect()
{
	if(iMode != PT_PICK_MODE)
	{
		iMode = PT_PICK_MODE;

		// Get BIMWorkspace
		BIMWorkspace* bimWorkspace = GetDocument();
		bimWorkspace->SelectedPartMeasurement = false;
		bimWorkspace->SelectedPartMesh = false;
		if (!bimWorkspace->BIMPartMeasurements.BIMPartMeasurements.empty())
		{
			bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[bimWorkspace->SelectedPart].PartMeasurementFlags.Selected = false;
		}
	}
	else
	{
		iMode = NOEDIT_MODE;
	}
}

// Takes the 2 pairs of 3 points and aligns them.
void CBIMView::On3ptXform()
{	
	BIMWorkspace* bimWorkspace = GetDocument();
	bimWorkspace->PointTransformation();

	// Clean up deques.
	bimWorkspace->selectedMeasurementPts.clear();
	bimWorkspace->selectedMeshPts.clear();

	OnDraw(GetDC());
}

// Added by Sowmya 4/05

// Increases stepsize by one.
void CBIMView::OnPlusStepSize()
{
	stepsize = stepsize * 2.0;
}

// Decreases stepsize by one.
void CBIMView::OnMinusStepSize()
{
	stepsize = stepsize / 2.0;
}

// Measures the part with the ROG probe.
void CBIMView::OnMeasureScanpart()
{
	//Creating the function pointer to the Callback function.
	void (*callback)(double, double, double, void*) = ROG_Callback;
	
	BIMWorkspace* bimWorkspace = GetDocument();
	// Create new pt cloud
	PartMeasurement p;
	bimWorkspace->BIMPartMeasurements.BIMPartMeasurements.push_back(p);

	// Create a save dialog box to also save this .pts or .snr cloud to a file too while measuring.
	//ROG(callback, (void*)this);
}

// Function used when we are running the machine.
void CBIMView::ROG_Callback(double x, double y, double z, void* bimview)
{
	// Update the new partmeasurment that is being added.
	SampledPoint p;
	p.x = x;
	p.y = y;
	p.z = z;
	p.SNR = 0;

	CBIMView* ptr = (CBIMView*)bimview;

	BIMWorkspace* bim = ptr->GetDocument();
	int end = bim->BIMPartMeasurements.BIMPartMeasurements.size()-1;
	bim->BIMPartMeasurements.BIMPartMeasurements[end].PartMeasurementSampledPoints.BIMSampledPoints.push_back(p);
	
	ptr->CalculateBoundingBox(bim);

	// Redraw the scene.
	ptr->OnDraw(ptr->GetDC());
}

// Function that calculates the overall bounding box for the workspace.
void CBIMView::CalculateBoundingBox(BIMWorkspace* bimWorkspace)
{
	BBox temp;
	XVecf temp_bmin,temp_bmax;
	bmin = XVecf(1e15);
    bmax = XVecf(-1e15);

	// Get part Measurement bounds.
	bimWorkspace->BIMPartMeasurements.CalculatePartMeasurementsBounds();
	temp_bmin = bimWorkspace->BIMPartMeasurements.GetMinBound();
	temp_bmax = bimWorkspace->BIMPartMeasurements.GetMaxBound();
	
	// Get mesh bounds
	temp = bimWorkspace->BIMPartMesh.PartMeshDisplayProperties.DisplayPropertiesBBox;
	
	bimWorkspace->BIMPartMeasurements.CalculatePartMeasurementsBounds();
	
	// Set bounds
	bmin = bmin.Min(temp_bmin);
	bmax = bmax.Max(temp_bmax);
	bmin = bmin.Min(temp.bmin);
	bmax = bmax.Max(temp.bmax);

	SetBounds(bmin,bmax);

	// setting view parameters
	m_trans[0] = -boxCenter[0];
	m_trans[1] = -boxCenter[1];
	m_trans[2] = -boxCenter[2];

	//radius of the world
	//double radius = .7f * vmax.dist( vmin );
	double radius = .7f * 
		sqrt( (boxMax[0]-boxMin[0])*(boxMax[0]-boxMin[0]) + 
			(boxMax[1]-boxMin[1])*(boxMax[1]-boxMin[1]) + 
			(boxMax[2]-boxMin[2])*(boxMax[2]-boxMin[2]) );

	// setup correct near and far
	// contain the world between near and far
	m_near = .2f * radius / sinf( m_fov * M_PI / 180 / 2 );// / ::min( 1.f, m_aspect );
	m_far = m_near + 2 * radius;
	m_ctrans[2] = -( m_near + radius );
	m_near *= .1f;
	m_far *= 10;

	return;
}

// Measures current distance from selected point cloud to mesh, no registration performed
void CBIMView::OnRegisterCurrentdistance()
{
	BIMWorkspace* bimWorkspace = GetDocument();
	bimWorkspace->MeasureCurrentDistances((CWnd*)this);
	
	OnDraw(GetDC());
	return;
}

// Saves any unamed pt clouds
void CBIMView::OnFileSavescans()
{
	BIMWorkspace* bimWorkspace = GetDocument();

	bimWorkspace->SaveScans((CWnd*)this);
	
	return;
}


// Exports all registered scans
void CBIMView::OnFileExportdistances()
{
	BIMWorkspace* bimWorkspace = GetDocument();

	for (int i = 0; i < bimWorkspace->BIMPartMeasurements.BIMPartMeasurements.size(); i++)
	{
		if (bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[i].PartMeasurementFlags.Registered)
		{
			bimWorkspace->BIMPartMeasurements.BIMPartMeasurements[i].ExportTransformedDataPts(i);
		}
	}
	return;
}

// Call Xinju's auto-align code.
void CBIMView::OnRegisterAuto()
{
	// Add registration type code to call 
	BIMWorkspace* bimWorkspace = GetDocument();

	bimWorkspace->AutoAlign((CWnd*)this);

	return;
}



void CBIMView::OnFileExportsignature()
{
	// TODO: Add your command handler code here
	

	// string to hold filepath
	string filePath;
	// string to hold file extension
	string fileExt;

	// Get document object associated with this view
	BIMWorkspace* bimWorkspace = GetDocument();
	
	// Create a new file save dialog box
	CFileDialog* fileSave = new CFileDialog(
		FALSE,
		"*.sig",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BIM model signature file (*.sig)|*.sig||\0",
		NULL,
		0);

	// Display FileDialog box
	fileSave->DoModal();
	// Get path and extension of file to be exported
	filePath = fileSave->GetPathName();
	fileExt = fileSave->GetFileExt();
	delete fileSave;

	// If we get a filename, then save.
	if (filePath.size() > 0)
	{

		bimWorkspace->BIMPartMesh.CalculateSignature(bimWorkspace->BIMOptions);
		bimWorkspace->BIMPartMesh.WriteSignature(filePath);
		AfxMessageBox("Signature saved");
		
	}

	// refresh screen
	OnDraw(GetDC());
	
}

void CBIMView::OnRegisterMulti()
{
	// TODO: Add your command handler code here
		// Add registration type code to call 
	BIMWorkspace* bimWorkspace = GetDocument();

	bimWorkspace->MultiAutoAlignAndRefine((CWnd*)this);

	return;

}
