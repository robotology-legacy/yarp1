// LiveCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cameraTune.h"
#include "LiveCameraDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _BORDER 10

/////////////////////////////////////////////////////////////////////////////
// CLiveCameraDlg dialog


CLiveCameraDlg::CLiveCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLiveCameraDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLiveCameraDlg)
		// NOTE: the ClassWizard will add member initialization here
	pImage = NULL;
	m_drawdib = DrawDibOpen ();
	//}}AFX_DATA_INIT
	
	ACE_ASSERT (m_drawdib != NULL);
}


void CLiveCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLiveCameraDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLiveCameraDlg, CDialog)
	//{{AFX_MSG_MAP(CLiveCameraDlg)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveCameraDlg message handlers

void CLiveCameraDlg::UpdateState(YARPGenericImage *p_img)
{
	pImage = p_img;
	sizeX = pImage->GetWidth();
	sizeY = pImage->GetHeight();
	GetClientRect (&m_rect);
	InvalidateRect (m_rect, FALSE);
}

void CLiveCameraDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (( pImage != NULL) && (sizeX != 0) && (sizeY != 0) )
	{
		GetClientRect (&m_rect);
		double zoomX;
		double zoomY;

		zoomX= double(m_rect.Width() - 2 * _BORDER) / double(sizeX);
		zoomY = double(m_rect.Height() - 2 * _BORDER) / double(sizeY);
		unsigned char *dib = (unsigned char*)m_converter.ConvertAndFlipToDIB(*pImage);
		CopyToScreen(m_drawdib, dc.GetSafeHdc(), dib, _BORDER, _BORDER, zoomX, zoomY);
		
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CLiveCameraDlg::CopyToScreen(HDRAWDIB hDD, HDC hDC, unsigned char *img, int destX, int destY, double zoomX, double zoomY)
{
	ACE_ASSERT (img != NULL);				

	BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER *)img;
	
	int X = dibhdr->biWidth; 
	int Y = dibhdr->biHeight;
	int sizeX = int(X * zoomX);
	int sizeY = int(Y * zoomY);
	
	/// LATER: it should be a bit more generic.
	if (dibhdr->biBitCount == 8)
	{
		DrawDibDraw(
		  hDD, hDC,
		  destX, destY,
		  sizeX, sizeY,
		  (BITMAPINFOHEADER *)img,
		  img + sizeof(RGBQUAD) * 256 + sizeof(BITMAPINFOHEADER),
		  0, 0,                 
		  X, Y,
		  0);
	}
	else
	{
		DrawDibDraw(
		  hDD, hDC,
		  destX, destY,
		  sizeX, sizeY,
		  (BITMAPINFOHEADER *)img,
		  img + sizeof(BITMAPINFOHEADER),
		  0, 0,                 
		  X, Y,
		  0);
	}
}

void CLiveCameraDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	DrawDibClose(m_drawdib);

	CDialog::OnClose();
}

void CLiveCameraDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	YARPImageFile::Write("camera.ppm", *pImage,YARPImageFile::FORMAT_PPM);	
	CDialog::OnLButtonDblClk(nFlags, point);
}
