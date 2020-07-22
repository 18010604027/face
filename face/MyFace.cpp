#include"pch.h"
#include"MyFace.h"
#include"faceDlg.h"






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
	refresh = false;	//是否刷新判定
	m_bTracking = false;//鼠标悬停判定
	m_bOver = false;	//鼠标悬停于图标判定
	m_down = false;		//鼠标按下的判定
	air_down = 0;		//浮窗关闭判定
	air_view = 0;		//浮窗显示判定
	time_change = 0;	//计时器事件判定
	air_time = 0;		//计时器事件判定
	grid_old_f = 0;		//鼠标过去指向图标(用于主浮窗保存参数)
	grid_old = 0;		//拖拽保存参数使用
	grid_x = 1;			//根据窗口大小计算一排放多少图标
	grid_y = 1;			//根据窗口大小计算一列放多少图标
	grid_n = 1;			//根据窗口大小计算总共放多少图标
	grid_m = 0;			//根据窗口大小鼠标所在的图标的位数
	//鼠标左键按下瞬间的坐标(用于拖拽)
	Mouse_old.x = -1;
	Mouse_old.y = -1;
	////鼠标左键按下瞬间的位置(主浮窗）
	M_old_f.x = -1;
	M_old_f.y = -1;
	//鼠标拖拽改变量
	Mouse_cg.x = 0;
	Mouse_cg.y = 0;

	//初始化图层
	bmp_air = NULL;
	bmp_airfile = NULL;
	bmp_bk = NULL;
	bmp_bottom_file = NULL;
	bmp_bottom_re = NULL;
	bmp_drop = NULL;
}

CMyFace::~CMyFace()
{
	/*Gdiplus::GdiplusShutdown(m_uGdiplusToken);*/
	//删除图层
	delete bmp_bottom_re;
	delete bmp_bottom_file;
	delete bmp_airfile;
	delete bmp_bk;
	delete bmp_air;
	delete bmp_drop;
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
	if (nIDEvent == 0)
		time_change = 2;
	//else if (nIDEvent == 1)
	//	air_time = 2;
	KillTimer(nIDEvent);
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
	int x, y;//计算鼠标大概在哪排(y)哪列(x)格中(把左上间隔和图标看作一格)
	CRgn crgn;
	x = Mouse_x_y.x / (ICON + side_width) + 1;
	y = Mouse_x_y.y / (ICON + side_height + side_text) + 1;
	crgn.CreateRoundRectRgn(ICON + (x - 1) * (ICON + side_width), ICON + (y - 1) * (ICON + side_height + side_text), (x) * (ICON + side_width), (y) * (ICON + side_height + side_text) - side_text, ICON, ICON);
	{
		if (crgn.PtInRegion(point)&&x<=grid_x)//确定是否在格子中的图标上
		{
			grid_m = (y - 1) * grid_x + x;//鼠标指向的图标第n个
			if (grid_m > grid_n)
			{
				grid_m = 0;
			}
			else
			{
				if (time_change == 0)
				{
					SetTimer(0, 13, NULL);
					time_change = 1;
				}
				else if (time_change) {}
			}
		}
		else//未在图标上的情况
		{
			if (time_change == 1)
			{
				KillTimer(0);
				time_change = 0;
			}
			else if (time_change >= 2)
			{
				time_change = 0;
				DrawFile(grid_m, true);
			}
			grid_m = 0;
		}
		/*if (grid_m != grid_old)*/
		{
			if (time_change == 2)//防止=2改=0时反复加载计时器
			{
				time_change = 3;
				DrawFile(grid_m, true);
			}
		}
		if (m_down)
		{
			Mouse_cg.x = point.x - Mouse_old.x;
			Mouse_cg.y = point.y - Mouse_old.y;
			DrawDrop();
			if (!crgn.PtInRegion(point))//确定是否在格子中的图标上
			{
				grid_m = (y - 1) * grid_x + x;//鼠标指向的图标第n个之前(即把第n个及第n个之后的向后移动)
			}
		}
	}
}
void CMyFace::OnLButtonDown(UINT nFlags, CPoint point)
{
	//拖拽部分
	if (grid_m)
	{
		if (!grid_old)
			grid_old = grid_m;
		if (Mouse_old.x == -1)
			Mouse_old = point;
		m_down = true;
	}
	//关闭文件夹部分(因为是获取上次点击参数，所以要放在打开文件之前)
	{
		if (air_view == 1)//如果主浮窗开启
		{
			CRect main_rect;
			GetClientRect(&main_rect);//获取窗口大小
			CRect m_rect(0, M_old_f.y, main_rect.Width(), M_old_f.y + side_height * 2 + ICON + 2 * side_text);//主浮窗的位置和大小
			if (!m_rect.PtInRect(point))//如果按下时不在主浮窗内
			{
				air_down = 2;
			}
		}

	}
	//点击打开文件或文件夹部分
	if (air_view == 0)
	{
		int x, y;//计算鼠标大概在哪排(y)哪列(x)格中(把左上间隔和图标看作一格)
		CRgn crgn;
		x = point.x / (ICON + side_width) + 1;
		y = point.y / (ICON + side_height + side_text) + 1;
		int grid_m2 = x + (y - 1) * grid_x;//所在位置区域(区域由左、上ICON与图标构成)
		crgn.CreateRoundRectRgn(ICON + (x - 1) * (ICON + side_width), ICON + (y - 1) * (ICON + side_height + side_text), (x) * (ICON + side_width), (y) * (ICON + side_height + side_text) - side_text, ICON, ICON);
		{
			if (crgn.PtInRegion(point) && air_time == 0 && grid_m2 < grid_n)//确定是否在格子中的图标上
			{
				//SetTimer(1, 300, NULL);
				air_time = 1;
			}
		}
	}
}
void CMyFace::OnLButtonUp(UINT nFlags, CPoint point)
{
	//拖拽部分
	if (m_down)
	{
		m_down = false;
		DrawDrop();
	}
	grid_old = 0;
	Mouse_old.x = -1;
	Mouse_old.y = -1;
	//点击关闭文件夹部分
	{
		if (air_down == 2)//不允许打开浮窗
		{
			air_view = 0;
			air_down = 0;//参数还原
			grid_old_f = 0;
			M_old_f.x = -1;
			M_old_f.y = -1;
			Draw();
		}
	}
	//点击打开文件或文件夹部分
	{
		if (air_time == 1)
		{
			air_view = 1;
			KillTimer(1);
			if (M_old_f.y == -1)//主
			{
				//给出矩形浮窗左上点坐标
				M_old_f.y = (point.y / (ICON + side_height + side_text) + 1) * (ICON + side_height + side_text);
				M_old_f.x = 0;
				grid_old_f = grid_m;
			}
			air_time = 0;
			Draw();
		}
		if (air_time == 2)
		{
			air_time = 0;
		}
	}
}

LRESULT CMyFace::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = false;
	if (time_change)
	{
		KillTimer(0);
		time_change = 0;
	}
	DrawFile(grid_m, true);
	grid_m = 0;
	return 0;
}
/*鼠标悬浮的系列行为:图标上 */
LRESULT CMyFace::OnMouseHover(WPARAM wParam, LPARAM lParam)
{

	return 0;
}
//把外面的拖到软件内部的拖拽
void CMyFace::OnDropFiles(HDROP hDropInfo)
{
	DragQueryFile(hDropInfo, 0, szFilePath, sizeof(szFilePath)); //获取文件路径
	Draw();//拖拽后不会自动刷新，需要手动刷新
}

/*擦除背景之后重绘背景，原本函数用于刷新*/
BOOL CMyFace::OnEraseBkgnd(CDC* pDC)
{
	CDC* pdc = GetDC();
	if (!refresh)
	{
		Draw();
		refresh = true;
	}
	pdc->BitBlt(crect.left, crect.top, crect.Width(), crect.Height(), &dc, 0, 0, SRCCOPY);
	return true;
}

/*修改窗口后的一些操作*/
void CMyFace::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&crect);
	if (!start)
	{
		/*在修改窗口大小之后，同步修改缓存中画布大小*/
		refresh = false;
	}

	grid_x = (crect.right - crect.left) / (ICON + side_width);
	grid_y = (crect.bottom - crect.top) / (ICON + side_height + side_text);
	grid_n = grid_x * grid_y;
	/*CString a;
	a.Format(L"%d\n%d\n", crect.bottom, crect.top);
	OutputDebugString(a);*/
}
/*点击“文件夹”之后弹出的浮窗*/
void CMyFace::Air_View(int x, int y, int* file, int dep)
{
	//CRect rect;
	//GetClientRect(&rect);//获取窗口大小
	////画一个提示三角形
	//{
	//	CPen pen(PS_SOLID, 2, RGB(100, 100, 100));
	//	CPen* oldPen = dc.SelectObject(&pen);

	//	CPoint pt[8];

	//	//起点
	//	pt[0].x = 0;
	//	pt[0].y = 0;

	//	//第1条线终点
	//	pt[1].x = x + side_width / 2 - 6;
	//	pt[1].y = 0;

	//	//第2条线终点
	//	pt[2].x = x + side_width / 2;
	//	pt[2].y = 0 - 12 * 17 / 20;

	//	//第3条线终点
	//	pt[3].x = x + side_width / 2 + 6;
	//	pt[3].y = 0;

	//	//第4条线终点
	//	pt[4].x = rect.Width();
	//	pt[4].y = 0;

	//	//第5条线终点
	//	pt[5].x = rect.Width();
	//	pt[5].y = side_height + ICON + 2 * side_text;

	//	//第6条线终点
	//	pt[6].x = 0;
	//	pt[6].y = side_height + ICON + 2 * side_text;

	//	//回到起点
	//	pt[7].x = 0;
	//	pt[7].y = 0;
	//	dc_air.Polygon(pt, 7);//这是边框

	//	CBrush brush;
	//	brush.CreateSolidBrush(RGB(50, 70, 150)); //创建画刷
	//	dc_air.SelectObject(brush);

	//	dc_air.Polygon(pt, 7);//画边框

	//	brush.DeleteObject();
	//}
	///*画内部文件*/
	//{
	//	dc_airfile.BitBlt(0, 0, rect.Width(), rect.Height(), &dc_air, 0, 0, SRCCOPY);
	//	//加载图标
	//	int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
	//	for (int i = 1; i < grid_x; i++)
	//	{
	//		if (szFilePath != '\0')
	//		{
	//			DrawRignrim(ICON + (i - 1) * ((ICON + side_width)), 0 + ICON, side_width, side_height, RGB(150, 150, 150), &dc_airfile);
	//			if (i == grid_m && time_change >= 2)
	//			{
	//				DrawRign(ICON + (i - 1) * (ICON + side_width), 0 + ICON, side_width + 1, side_height + 1, RGB(100, 255, 255), 100, &dc_airfile);
	//			}
	//			else
	//				DrawRign(ICON + (i - 1) * (ICON + side_width), 0 + ICON, side_width + 1, side_height + 1, RGB(0, 255, 255), 60, &dc_airfile);
	//			DrawIco(ICON + (i - 1) * (ICON + side_width) + k, 0 + k + ICON, szFilePath, &dc_airfile);
	//			Drawtext(ICON + (i - 1) * (ICON + side_width), (ICON + side_width) + 0, szFilePath, &dc_airfile);
	//		}
	//	}
	//}
}
//画一个图标框体，填充透明(青)
void CMyFace::DrawRign(int x, int y, int width, int height, Color color, Bitmap* bmp)
{
	//创建平台
	Gdiplus::Graphics g(bmp);
	//采用覆盖模式
	g.SetCompositingMode(CompositingModeSourceCopy);
	//画矩形
	Gdiplus::SolidBrush brush(color);
	g.FillRectangle(&brush, Rect(x, y, width, height));
}

//画一个图标框边
void CMyFace::DrawRignrim(int x, int y, int width, int height, Color color, Bitmap* bmp)
{
	//创建平台
	Gdiplus::Graphics g(bmp);
	//采用覆盖模式
	g.SetCompositingMode(CompositingModeSourceCopy);
	//画透明矩
	Pen pen(color);
	g.DrawRectangle(&pen, Rect(x, y, width, height));
}

//画一个图标样式(×48)
void CMyFace::DrawIco(int x, int y, CString szFilePath, Bitmap* bmp)
{
	//创建平台
	Gdiplus::Graphics g(bmp);
	//采用覆盖模式
	g.SetCompositingMode(CompositingModeSourceCopy);
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
			Gdiplus::Bitmap ico(hIcon);
			g.DrawImage(&ico, Rect(x, y, 48, 48));
		}
		/*销毁*/
		if (hIcon)
		{
			DestroyIcon(hIcon);
		}
	}


}
//画一个图标样式(×24)
void CMyFace::DrawIco2(int x, int y, CString szFilePath, Bitmap* bmp)
{
	//创建平台
	Gdiplus::Graphics g(bmp);
	//采用覆盖模式
	g.SetCompositingMode(CompositingModeSourceCopy);
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


		//增加放缩质量，抗边缘锯齿
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		//画入dc缓存区
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
			g.DrawImage(&ico, Rect(x, y, 24, 24));
		}
		/*销毁*/
		if (hIcon)
		{
			DestroyIcon(hIcon);
		}
	}


}
//画一个文件夹(软件内文件夹，与传统意义文件夹有所不同)
void CMyFace::Drawfolder(int x, int y, int* file, int dep, Bitmap* bmp)
{
	int j, k;
	j = side_height / 2;
	k = (j - 24) / 2;
	DrawIco2(x + k, y + k, szFilePath, bmp);
	DrawIco2(x + j + k, y + k, szFilePath, bmp);
	DrawIco2(x + k, y + j + k, szFilePath, bmp);
	DrawIco2(x + j + k, y + j + k, szFilePath, bmp);
}
//画一个图标脚底文字说明
void CMyFace::Drawtext(int x, int y, CString szFilePath, Bitmap* bmp)
{
	//创建平台
	Gdiplus::Graphics g(bmp);

	//清除原区域(擦除背景)
	g.SetCompositingMode(CompositingModeSourceCopy);
	Gdiplus::SolidBrush brush(Color(0, 0, 0, 0));
	g.FillRectangle(&brush, Rect(x, y, side_width, side_text));

	CString str = szFilePath;
	int len;
	len = str.ReverseFind('\\');
	str = str.Right(str.GetLength() - len - 1);

	//g.SetCompositingMode(CompositingModeSourceOver);
	//Gdiplus::StringFormat font_attribute;
	//font_attribute.SetAlignment(StringAlignment(CA_CENTER));
	//font_attribute.SetLineAlignment(StringAlignment(0));
	//Gdiplus::Font myFont(L"宋体", 8);                  //设置字体
	//Gdiplus::SolidBrush blackBrush(Color(255, 255, 255, 255));
	//g.DrawString(str, str.GetLength(), &myFont, RectF(x, y, side_width, side_text+3), &font_attribute, &blackBrush);
}
//鼠标移进点亮图标
void CMyFace::DrawFile(int num, bool bottom)
{
	if (num <= 0 || num > grid_n)
	{
		return;
	}
	if (bottom)
	{
		CDC* pDC = GetDC();

		int i = num;
		int x, y;
		x = ICON + ((i - 1) % grid_x) * ((ICON + side_width));
		y = ICON + ((i - 1) / grid_x) * (ICON + side_height + side_text);

		if (szFilePath != '\0')
		{
			if (time_change >= 2)
			{
				DrawRign(x, y, side_width + 1, side_height + 1, Color(100, 100, 255, 255), bmp_bottom_re);
			}
			else
				DrawRign(x, y, side_width + 1, side_height + 1, Color(60, 0, 255, 255), bmp_bottom_re);
		}
		//显示
		CDC* pdc = GetDC();
		Gdiplus::Graphics g(dc);
		g.DrawImage(bmp_bk, 0, 0);
		g.DrawImage(bmp_bottom_re, 0, 0);
		g.DrawImage(bmp_bottom_file, 0, 0);
		g.DrawImage(bmp_drop, 0, 0);
		pdc->BitBlt(crect.left, crect.top, crect.Width(), crect.Height(), &dc, 0, 0, SRCCOPY);
	}
}

void CMyFace::Draw()
{
	if (start)
	{
		Start();
		start = false;
	}
	Restart();
	DrawBk();
	DrawBottomRe();
	DrawBottomFile();


	/*if (air_view == 0)
	{
		dc.BitBlt(rect.GetLeft(), rect.GetTop(), rect.Width(), rect.Height(), &dc_bottom, 0, 0, SRCCOPY);
	}*/
	////浮窗显示
	//else if (air_view == 1)
	//{
	//	int y = (grid_old_f / grid_x + 1) * (ICON + side_height + side_text);
	//	int y2 = side_height + ICON + side_text;
	//	dc.BitBlt(crect.left, crect.top, crect.Width(), y, &dc_bottom, 0, 0, SRCCOPY);
	//	dc.BitBlt(crect.left, y + y2, crect.Width(), crect.Height() - (y + y2), &dc_bottom, 0, y, SRCCOPY);
	//	Air_View(ICON + (grid_old_f % grid_x - 1) * (ICON + side_width), ICON + (grid_old_f / grid_x) * (ICON + side_height + side_text), 0, 0);
	//	dc.BitBlt(crect.left, y, crect.Width(), crect.Height(), &dc_airfile, 0, 0, SRCCOPY);
	//}

	//拖拽图标动画
	Gdiplus::Graphics drop(bmp_drop);
	drop.Clear(Color(0, 0, 0, 0));//清除原图
	if (m_down)
	{
		int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
		DrawIco(ICON + ((grid_old - 1) % grid_x) * (ICON + side_width) + k + Mouse_cg.x, ICON + ((grid_old - 1) / grid_x) * (ICON + side_height + side_text) + k + Mouse_cg.y, szFilePath, bmp_drop);
	}

	//显示
	Gdiplus::Graphics g(dc);
	/*Gdiplus::Bitmap bmp(rect.Width, rect.Width);
	{
		CString str;
		str.Format(L"%d", bmp_bk.GetHeight());
		MessageBox(str);
		Gdiplus::Graphics g2(&bmp);
		SolidBrush Brush(Color(255, 255, 255, 255));
		g2.FillRectangle(&Brush, rect);
	}
	g.DrawImage(&bmp, 0, 0);*/
	g.DrawImage(bmp_bk, 0, 0);
	g.DrawImage(bmp_bottom_re, 0, 0);
	g.DrawImage(bmp_bottom_file, 0, 0);
	g.DrawImage(bmp_drop, 0, 0);
}

void CMyFace::DrawBottomRe()
{
	//加载图标
	int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
	for (int i = 0; i < grid_n; i++)
	{
		if (szFilePath != '\0')
		{

			if (i == grid_m - 1 && time_change >= 2)
			{
				DrawRign(ICON + (i % grid_x) * (ICON + side_width), ICON + (i / grid_x) * (ICON + side_height + side_text), side_width + 1, side_height + 1, Color(100, 100, 255, 255), bmp_bottom_re);
			}
			else
				DrawRign(ICON + (i % grid_x) * (ICON + side_width), ICON + (i / grid_x) * (ICON + side_height + side_text), side_width + 1, side_height + 1, Color(60, 0, 255, 255), bmp_bottom_re);
			DrawRignrim(ICON + (i % grid_x) * ((ICON + side_width)), ICON + (i / grid_x) * (ICON + side_height + side_text), side_width, side_height, Color(255, 150, 150, 150), bmp_bottom_re);
		}
	}
}

void CMyFace::DrawBottomFile()
{
	//加载图标
	int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
	for (int i = 0; i < grid_n; i++)
	{
		if (szFilePath != '\0')
		{
			/*DrawIco(ICON + (i % grid_x ) * (ICON + side_width) + k, ICON + (i / grid_x) * (ICON + side_height + side_text) + k, szFilePath);*/
			Drawfolder(ICON + (i % grid_x) * (ICON + side_width), ICON + (i / grid_x) * (ICON + side_height + side_text), 0, 0, bmp_bottom_file);
			Drawtext(ICON + (i % grid_x) * (ICON + side_width), (ICON + side_height) + (i / grid_x) * (ICON + side_height + side_text), szFilePath, bmp_bottom_file);
		}
	}
}

void CMyFace::DrawBk()
{

	//刷新g_bk，用白色刷新
	Gdiplus::Graphics g(bmp_bk);
	SolidBrush Brush(Color(255, 255, 255, 255));
	g.FillRectangle(&Brush, rect);
	//复制背景
	CRect bgrect;
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
}

void CMyFace::DrawDrop()
{
	CDC* pdc = GetDC();
	Graphics g(dc);
	//dc.BitBlt(crect.left, crect.top, crect.Width(), crect.Height(), &dc_bottom, 0, 0, SRCCOPY);
	Gdiplus::Graphics drop(bmp_drop);
	drop.Clear(Color(0, 0, 0, 0));//清除原图

	if (m_down)
	{
		int k = (side_height - 48) / 2;//图标边框和图标图标图片间隔距离差
		DrawIco(ICON + ((grid_old-1) % grid_x ) * (ICON + side_width) + k + Mouse_cg.x, ICON + ((grid_old - 1) / grid_x) * (ICON + side_height + side_text) + k + Mouse_cg.y, szFilePath, bmp_drop);
	}

	//显示
	g.DrawImage(bmp_bk, 0, 0);
	g.DrawImage(bmp_bottom_re, 0, 0);
	g.DrawImage(bmp_bottom_file, 0, 0);
	g.DrawImage(bmp_drop, 0, 0);
	pdc->BitBlt(crect.left, crect.top, crect.Width(), crect.Height(), &dc, 0, 0, SRCCOPY);
}

void CMyFace::Start()
{
	CDC* pdc = GetDC();
	GetClientRect(&crect);
	rect.X = crect.top;
	rect.Y = crect.left;
	rect.Width = crect.Width();
	rect.Height = crect.Height();
	//初始化缓冲
	dc.CreateCompatibleDC(pdc);
	dc_bmp.CreateCompatibleBitmap(GetDC(), crect.Width(), crect.Height());
	dc.SelectObject(&dc_bmp);

	//初始化底页图层
	bmp_bottom_re = new Bitmap(rect.Width, rect.Height);
	bmp_bottom_file = new Bitmap(rect.Width, rect.Height);

	//初始化浮窗图层
	bmp_airfile = new Bitmap(rect.Width, rect.Height);;

	//初始化背景图层
	bmp_bk = new Bitmap(rect.Width, rect.Height);

	//初始化浮窗背景图层
	bmp_air = new Bitmap(rect.Width, rect.Height);

	//初始化拖拽图层
	bmp_drop = new Bitmap(rect.Width, rect.Height);
}

void CMyFace::Restart()
{
	GetClientRect(&crect);
	rect.X = crect.top;
	rect.Y = crect.left;
	rect.Width = crect.Width();
	rect.Height = crect.Height();

	dc_bmp.Detach();
	dc_bmp.CreateCompatibleBitmap(GetDC(), crect.Width(), crect.Height());
	dc.SelectObject(&dc_bmp);

	//重置底页图层
	delete bmp_bottom_re;
	delete bmp_bottom_file;
	bmp_bottom_re = new Bitmap(rect.Width, rect.Height);
	bmp_bottom_file = new Bitmap(rect.Width, rect.Height);

	//重置浮窗图层
	delete bmp_airfile;
	bmp_airfile = new Bitmap(rect.Width, rect.Height);

	//重置背景图层
	delete bmp_bk;
	bmp_bk = new Bitmap(rect.Width, rect.Height);

	//重置浮窗背景图层
	delete bmp_air;
	bmp_air = new Bitmap(rect.Width, rect.Height);

	//重置拖拽图层
	delete bmp_drop;
	bmp_drop = new Bitmap(rect.Width, rect.Height);
}