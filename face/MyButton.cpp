#include "pch.h"
#include "MyButton.h"
#include "faceDlg.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CMyButton, CButton)

CMyButton::CMyButton()
{
	m_bkColor = 0xFFFFFF;
	m_textColor = 0x000000;
	bOver = 150, bDown = 180, bDisable = 30;
	bedge_change = 0;
	type_height = 12;
}
CMyButton::~CMyButton()
{

}

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_ENABLE()
END_MESSAGE_MAP()

void CMyButton::SetBkColor(COLORREF color)
{
	m_bkColor = color;
}

void CMyButton::SetTextColor(COLORREF color)
{
	m_textColor = color;
}

void CMyButton::SetDiaphaneity(int Over, int Down, int Disable)
{
	bOver = Over, bDown = Down, bDisable = Disable;
}

BOOL CMyButton::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bRet = CButton::PreCreateWindow(cs);
	ButtonInit();
	return bRet;
}

void CMyButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
	ButtonInit();
}

void CMyButton::ButtonInit()
{
	m_bTracking = false;
	m_bOver = m_bDown = m_bDisable = false;
	m_bDisable = IsWindowEnabled() ? false : true;
	ModifyStyle(NULL, BS_OWNERDRAW);
}

void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DrawButton(lpDrawItemStruct->hDC);
}

void CMyButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		m_bOver = true;
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 13;//HOVER_DEFAULT
		m_bTracking = (bool)_TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDown = true;
	CButton::OnLButtonDown(nFlags, point);
}

void CMyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDown = false;
	CButton::OnLButtonUp(nFlags, point);
}

LRESULT CMyButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = false;
	m_bTracking = false;
	m_bDown = false;
	InvalidateRect(NULL, false);
	return 0;
}
LRESULT CMyButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = true;
	InvalidateRect(NULL);
	return 0;
}
void CMyButton::DrawButton()
{
	HDC hDC = ::GetDC(m_hWnd);
	DrawButton(hDC);
	::ReleaseDC(m_hWnd, hDC);
}
/*修改按钮圆角*/
void CMyButton::SetBTedgeCG(int edge)
{
	bedge_change = edge;
}
/*修改按钮文字大小*/
void CMyButton::SettypeCG(int type)
{
	type_height = type;
}
void CMyButton::DrawButton(HDC hDestDC)
{
	CRect rc;
	GetClientRect(rc);
	int nWindth = rc.Width();
	int nHeight = rc.Height();
	HDC hDC = CreateCompatibleDC(hDestDC);
	HDC hMaskDC = CreateCompatibleDC(hDestDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDestDC, nWindth, nHeight);
	HBITMAP hMaskBitmap = CreateCompatibleBitmap(hDestDC, nWindth, nHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	HBITMAP hOldMaskBitmap = (HBITMAP)SelectObject(hMaskDC, hMaskBitmap);

	/*设置字体*/
	CFont font;
	font.CreateFont(type_height,                                    //   字体的高度   
		0,                                          //   字体的宽度  
		0,                                          //  nEscapement 
		0,                                          //  nOrientation   
		FW_NORMAL,                                  //   nWeight   
		FALSE,                                      //   bItalic   
		FALSE,                                      //   bUnderline   
		0,                                                   //   cStrikeOut   
		ANSI_CHARSET,                             //   nCharSet   
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision   
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision   
		DEFAULT_QUALITY,                       //   nQuality   
		DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily     
		_T("宋体"));

	SetBkMode(hDC, TRANSPARENT);
	
	

	//把父窗口的背景图复制到按钮的DC上,实现视觉透明----------------
	CRect bgrect;
	Graphics g(hDC);
	CPoint point(0, 0);
	CfaceDlg* p = (CfaceDlg*)GetParent();
	p->GetClientRect(&bgrect);
	int bgwidth, bgheight;
	bgheight = p->m_img->GetHeight();
	bgwidth = p->m_img->GetWidth();
	MapWindowPoints(p, &point, 1);
	if (bgwidth < bgheight)
	{
		bgheight = bgheight * bgrect.Width() / bgwidth;
		bgwidth = bgrect.Width();
	}
	else
	{
		bgwidth = bgwidth * bgrect.Height() / bgheight;
		bgheight = bgrect.Height();
	}
	g.DrawImage(p->m_img, -point.x, -point.y, bgwidth, bgheight);
	//-------------------------------------------------------------
	int nAlpha = 100;//0--255
	int nOffset = 0;
	HBRUSH hbr = CreateSolidBrush(m_bkColor);
	FillRect(hMaskDC, &rc, hbr);
	DeleteObject(hbr);
	if (m_bDisable)
	{
		nAlpha = bDisable;
	}
	else if (m_bDown)
	{
		nAlpha = bDown;
		nOffset = 1;
	}
	else if (m_bOver)
	{
		nAlpha = bOver;
	}
	else
	{
		nAlpha = bDisable;
	}
	BLENDFUNCTION blend;
	memset(&blend, 0, sizeof(blend));
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = nAlpha; // 透明度 最大255
	HRGN hRgn = CreateRoundRectRgn(0, 0, nWindth, nHeight, bedge_change, bedge_change);
	SelectClipRgn(hDC, hRgn);
	AlphaBlend(hDC, 0, 0, nWindth, nHeight, hMaskDC, 0, 0, nWindth, nHeight, blend);
	

	
	

	
	CString strText;
	GetWindowText(strText);
	if (strText != _T(""))
	{
		rc.InflateRect(-2, -2);
		rc.OffsetRect(nOffset, nOffset);
		HFONT hOldFont = (HFONT)SelectObject(hDC, font);
		::SetTextColor(hDC, m_textColor);
		::DrawText(hDC, strText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_WORD_ELLIPSIS);
		::SelectObject(hDC, hOldFont);
	}
	SelectClipRgn(hDC, NULL);
	DeleteObject(hRgn);
	{
				/*if (sInfo.hIcon)
		{
			DestroyIcon(sInfo.hIcon);
			MessageBox(L"11");
		}*/
	}
	//复制到控件的DC上------------------------
	BitBlt(hDestDC, 0, 0, nWindth, nHeight, hDC, 0, 0, SRCCOPY);
	
	//删除资源,释放内存-----------------------
	SelectObject(hDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	SelectObject(hMaskDC, hOldMaskBitmap);
	DeleteObject(hMaskBitmap);
	DeleteDC(hMaskDC);
}
void CMyButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	m_bDisable = IsWindowEnabled() ? FALSE : TRUE;
	DrawButton();
}
