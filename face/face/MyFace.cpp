#include"pch.h"
#include"MyFace.h"
#include"faceDlg.h"

//特殊图片处理
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;




//用于获取系统图标
#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

IMPLEMENT_DYNAMIC(CMyFace, CWnd)//声明控件

CMyFace::CMyFace()
{
	start = true;		//全局绘图起始缓存判定
	m_bTracking = false;//鼠标悬停判定
	m_bOver = false;	//鼠标悬停于图标判定
	m_down = false;		//鼠标按下的判定
	time_change = 0;	//计时器事件判定
	grid_x = 1;			//根据窗口大小计算一排放多少图标
	grid_y = 1;			//根据窗口大小计算一列放多少图标
	grid_n = 1;			//根据窗口大小计算总共放多少图标
	grid_m = 0;			//根据窗口大小鼠标所在的图标的位数

	//初始化字体的系列参数
	font.CreateFont(12,					//  字体的高度   
		0,								//  字体的宽度  
		0,								//  nEscapement 
		0,								//  nOrientation   
		FW_NORMAL,						//  nWeight   
		FALSE,                          //  bItalic   
		FALSE,                          //  bUnderline   
		0,                              //  cStrikeOut   
		ANSI_CHARSET,                   //  nCharSet   
		OUT_DEFAULT_PRECIS,             //  nOutPrecision   
		CLIP_DEFAULT_PRECIS,			//  nClipPrecision   
		DEFAULT_QUALITY,				//  nQuality   
		DEFAULT_PITCH | FF_SWISS,		//  nPitchAndFamily     
		_T("宋体"));					//	字体类型
}

CMyFace::~CMyFace()
{
	/*Gdiplus::GdiplusShutdown(m_uGdiplusToken);*/
}

BEGIN_MESSAGE_MAP(CMyFace, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CMyFace::OnTimer(UINT_PTR nIDEvent)//计时器
{
	time_change = 2;
	KillTimer(0);
}

void CMyFace::OnMouseMove(UINT nFlags, CPoint point)
{
	Mouse_x_y = point;
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 13;//HOVER_DEFAULT// 若不设此参数，则无法触发mouseHover
		m_bTracking = (bool)_TrackMouseEvent(&tme);
	}
	int i;
	int x, y;//计算鼠标大概在哪排(y)哪列(x)格中(把左上间隔和图标看作一格)
	CRect rect;
	CRgn crgn;
	GetClientRect(&rect);
	x = Mouse_x_y.x / (ICON + side_width) + 1;
	y = Mouse_x_y.y / (ICON + side_height + side_text) + 1;
	crgn.CreateRoundRectRgn(ICON + (x - 1) * (ICON + side_width), ICON + (y - 1) * (ICON + side_height + side_text), (x) * (ICON + side_width), (y) * (ICON + side_height + side_text) - side_text, ICON, ICON);
	{
		if (crgn.PtInRegion(point))//确定是否在格子中的图标上
		{
			grid_m = (y - 1) * grid_x + x;//鼠标指向的图标第n个
			if (time_change == 0)
			{
				SetTimer(0, 37, NULL);
				time_change = 1;
			}
			else if (time_change) {}
		}
		else
		{
			if (time_change == 1)
			{
				KillTimer(0);
				time_change = 0;
			}
			else if (time_change >= 2)
			{
				time_change = 0;
				Draw();
			}
			grid_m = 0;//未在图标上的情况
		}
		/*if (grid_m != grid_old)*/
		{
			if (time_change == 2)
			{
				time_change = 3;
				Draw();
			}
		}
	}
}
void CMyFace::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (grid_m)
	{
		m_down = true;
	}
}
void CMyFace::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_down = false;
}

LRESULT CMyFace::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = false;
	grid_m = 0;
	if (time_change)
	{
		KillTimer(0);
		time_change = 0;
	}
	Draw();
	return 0;
}
/*鼠标悬浮的系列行为:图标上 */
LRESULT CMyFace::OnMouseHover(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

void CMyFace::OnDropFiles(HDROP hDropInfo)
{
	DragQueryFile(hDropInfo, 0, szFilePath, sizeof(szFilePath)); //获取文件路径
	Draw();//拖拽后不会自动刷新，需要手动刷新
}

/*擦除背景之后重绘背景，原本函数用于刷新*/
BOOL CMyFace::OnEraseBkgnd(CDC* pDC)
{
	Draw();
	return true;
}

/*修改窗口后的一些操作*/
void CMyFace::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&rect);
	if (!start)
	{
		/*在修改窗口大小之后，同步修改缓存中画布大小*/
		dc_bmp.Detach();
		dc_bmp.CreateCompatibleBitmap(GetDC(), rect.Width(), rect.Height());
		dc.SelectObject(&dc_bmp);
	}

	grid_x = (rect.right - rect.left) / (ICON + side_width) + 1;
	grid_y = (rect.bottom - rect.top) / (ICON + side_height + side_text);
	grid_n = grid_x * grid_y;
}

//画一个图标框体，填充透明(青)
void CMyFace::DrawRign(int x, int y, int width, int height, COLORREF color, int diaphaneity)
{
	//创建缓冲
	CDC dc2;
	dc2.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);//获取窗口大小
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	dc2.SelectObject(&bmp);

	//画矩形
	CRect rc;
	rc.SetRect(0, 0, width, height);
	int nOffset = 0;
	HBRUSH hbr = CreateSolidBrush(color);
	FillRect(dc2, &rc, hbr);
	DeleteObject(hbr);

	//透明
	BLENDFUNCTION blend;
	memset(&blend, 0, sizeof(blend));
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = diaphaneity; // 透明度 最大255
	CRgn cRgn;
	cRgn.CreateRoundRectRgn(x, y, width + x, height + y, ICON, ICON);
	dc.SelectClipRgn(&cRgn);
	dc.AlphaBlend(x, y, width, height, &dc2, 0, 0, width, height, blend);
	dc.SelectClipRgn(NULL);
	dc2.DeleteDC();
	cRgn.DeleteObject();

}

//画一个图标框边
void CMyFace::DrawRignrim(int x, int y, int width, int height, COLORREF color)
{
	CPen ppenRed;
	ppenRed.CreatePen(PS_SOLID, 1, color);
	CBrush fillbrush;
	fillbrush.CreateSolidBrush(NULL_BRUSH);
	dc.SelectObject(ppenRed);
	dc.SelectObject(fillbrush);
	dc.SelectStockObject(NULL_BRUSH);
	dc.RoundRect(x, y, x + width, y + height, ICON, ICON);
}

//画一个图标样式
void CMyFace::DrawIco(int x, int y, CString szFilePath)
{
	/*判断格式*/
	//获取格式
	CString str = szFilePath;
	int len;
	len = str.ReverseFind('.');
	str = str.Right(str.GetLength() - len - 1);

	//比较格式(获取图片图标)
	int i;
	for (i = 0; i < 3; i++)
	{
		if (str == format[i])
		{
			break;
		}
	}
	if (i != 3)
	{
		int height, width;
		Image* m_img;
		m_img = Image::FromFile(szFilePath);  //加载图片
		height = m_img->GetHeight();
		width = m_img->GetWidth();
		Graphics g(dc);
		//根据比例缩放
		if (height > width)
		{
			width = width * 48 / height;
			height = 48;
			x = x + (48 - width) / 2;
		}
		else if (height < width)
		{
			height = height * 48 / width;
			width = 48;
			y = y + (48 - height) / 2;
		}
		else
		{
			height = 48;
			width = 48;
		}
		//画阴影
		Pen pen(Color(140, 140, 132));
		//Pen pen2(Color(172, 168, 167));

		g.DrawLine(&pen, Point(x + width, y), Point(x + width, y + height));
		g.DrawLine(&pen, Point(x, y + height), Point(x + width, y + height));

		//g.DrawLine(&pen2, Point(x + width + 1, y), Point(x + width + 1, y + height));
		//g.DrawLine(&pen2, Point(x, y + height + 1), Point(x + width + 1, y + height + 1));

		g.DrawImage(m_img, x, y, width, height);

		return;
	}
	//PrivateExtractIcons只能获取内部图标（如exe,dll自带图标）
	//获取图标
	SHFILEINFOW sfi = { 0 };
	SHGetFileInfo(szFilePath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

	//获取系统图像列表
	HIMAGELIST* imageList;
	/* 48*48 */HRESULT hResult = SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);
	/* 256*256 *///HRESULT hResult = SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&imageList);
	if (hResult == S_OK)
	{
		//获取图标
		HICON hIcon = NULL;
		hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
		if (hResult == S_OK)
		{
			//画图标
			HDC dc2;
			dc2 = dc.GetSafeHdc();
			DrawIconEx(dc2, x, y, hIcon, 48, 48, 0, NULL, DI_NORMAL | DI_COMPAT);
			//DeleteDC(dc2);
		}
		/*销毁*/
		if (hIcon)
		{
			DestroyIcon(hIcon);
		}
	}


}

void CMyFace::DrawIco2(int x, int y, CString szFilePath)
{
	/*判断格式*/
	//获取格式
	CString str = szFilePath;
	int len;
	len = str.ReverseFind('.');
	str = str.Right(str.GetLength() - len - 1);

	//比较格式(获取图片图标)
	int i;
	for (i = 0; i < 3; i++)
	{
		if (str == format[i])
		{
			break;
		}
	}
	if (i != 3)
	{
		int height, width;
		Image* m_img;
		m_img = Image::FromFile(szFilePath);  //加载图片
		height = m_img->GetHeight();
		width = m_img->GetWidth();
		Graphics g(dc);
		//根据比例缩放
		if (height > width)
		{
			width = width * 24 / height;
			height = 24;
			x = x + (24 - width) / 2;
		}
		else if (height < width)
		{
			height = height * 24 / width;
			width = 24;
			y = y + (24 - height) / 2;
		}
		else
		{
			height = 24;
			width = 24;
		}
		//画阴影
		Pen pen(Color(140, 140, 132));
		//Pen pen2(Color(172, 168, 167));

		g.DrawLine(&pen, Point(x + width, y), Point(x + width, y + height));
		g.DrawLine(&pen, Point(x, y + height), Point(x + width, y + height));

		//g.DrawLine(&pen2, Point(x + width + 1, y), Point(x + width + 1, y + height));
		//g.DrawLine(&pen2, Point(x, y + height + 1), Point(x + width + 1, y + height + 1));

		g.DrawImage(m_img, x, y, width, height);

		return;
	}
	//PrivateExtractIcons只能获取内部图标（如exe,dll自带图标）
	//获取图标
	SHFILEINFOW sfi = { 0 };
	SHGetFileInfo(szFilePath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

	//获取系统图像列表
	HIMAGELIST* imageList;
	/* 48*48 */HRESULT hResult = SHGetImageList(SHIL_LARGE, IID_IImageList, (void**)&imageList);
	/* 256*256 *///HRESULT hResult = SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&imageList);
	if (hResult == S_OK)
	{
		//获取图标
		HICON hIcon = NULL;
		hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
		if (hResult == S_OK)
		{
			//画图标
			Gdiplus::Bitmap ico(hIcon);
			Graphics g(dc);
			g.DrawImage(&ico, Rect(x, y, 24, 24));
		}
		/*销毁*/
		if (hIcon)
		{
			DestroyIcon(hIcon);
		}
	}


}
//画一个图标脚底文字说明
void CMyFace::Drawtext(int x, int y, CString szFilePath)
{
	CRect rc;
	dc.SetTextColor(RGB(255, 255, 255));
	CString str = szFilePath;
	int len;
	len = str.ReverseFind('\\');
	str = str.Right(str.GetLength() - len - 1);
	rc.SetRect(x, y, x + side_width, y + side_text);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&font);
	dc.DrawText(str, -1, &rc, DT_WORDBREAK | DT_CENTER | DT_VCENTER | DT_WORD_ELLIPSIS);

}
void CMyFace::Draw()
{
	CDC* pdc = GetDC();
	if (start)
	{
		//初始化缓冲
		dc.CreateCompatibleDC(pdc);
		GetClientRect(&rect);
		dc_bmp.CreateCompatibleBitmap(GetDC(), rect.Width(), rect.Height());
		dc.SelectObject(&dc_bmp);
		start = false;
	}
	//刷新dc，用白色刷新
	CBrush brush(RGB(255, 255, 255));
	dc.FillRect(&rect, &brush);
	//复制背景
	CRect bgrect;
	Graphics g(dc);
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

	//加载图标
	int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
	for (int i = 1; i < grid_n; i++)
	{
		if (szFilePath != '\0')
		{
			DrawRignrim(ICON + (i % grid_x - 1) * ((ICON + side_width)), ICON + (i / grid_x) * (ICON + side_height + side_text), 48 + 2 * k, 48 + 2 * k, RGB(150, 150, 150));
			if (i == grid_m && time_change >= 2)
			{
				DrawRign(ICON + (i % grid_x - 1) * (ICON + side_width), ICON + (i / grid_x) * (ICON + side_height + side_text), 48 + 2 * k + 1, 48 + 2 * k + 1, RGB(100, 255, 255), 100);
			}
			else
				DrawRign(ICON + (i % grid_x - 1) * (ICON + side_width), ICON + (i / grid_x) * (ICON + side_height + side_text), 48 + 2 * k + 1, 48 + 2 * k + 1, RGB(0, 255, 255), 60);
			if (i == 2)
			{
				DrawIco2(ICON + (i % grid_x - 1) * (ICON + side_width) + k, ICON + (i / grid_x) * (ICON + side_height + side_text) + k, szFilePath);
			}
			else
			{
				DrawIco(ICON + (i % grid_x - 1) * (ICON + side_width) + k, ICON + (i / grid_x) * (ICON + side_height + side_text) + k, szFilePath);
			}
			Drawtext(ICON + (i % grid_x - 1) * (ICON + side_width), (ICON + side_width) + (i / grid_x) * (ICON + side_height + side_text), szFilePath);
		}
	}

	//显示
	pdc->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
}
