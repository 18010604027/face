#pragma once
#include"pch.h"
#define PICTURE_FILE L"Data\\textures\\"//储存内部需要的图片的文件夹
#define ICON 30	//设置圆角和图标间距的参数
#define side_height 60 //button的高
#define side_width 60 //button的宽
#define side_text 24 //button的文字预留	

class CMyFace :public CWnd
{
	DECLARE_DYNAMIC(CMyFace)//声明这是一个控件
protected:
	CRect rect;//控件矩形
	CDC dc; CBitmap dc_bmp;//缓存
	CFont font;//文件名字体
	bool start;//是否第一次绘画
	int grid_x, grid_y, grid_n;	//一页存放图标的列数、排数、总个数
	int grid_m;					//鼠标指向第几个图标(从左向右，自上而下)
	int time_change;			//计时器状态 =0未计时，=1计时中，=2计时完毕;

	bool m_bTracking;	//鼠标悬停
	bool m_bOver;		//鼠标悬停于图标上
	bool m_down;		//鼠标按下的判定参数
	CPoint Mouse_x_y;	//鼠标位置

	CString format[3] = { L"jpg",L"bmp",L"png" };
	wchar_t szFilePath[200] = { L"C:\\Users\\18010\\Desktop\\新建文本文档 (3).txt" };//拖拽文件地址
public:
	CMyFace();
	virtual ~CMyFace();

protected:
	void DrawRignrim(int x, int y, int width, int height, COLORREF color);
	void DrawRign(int x, int y, int width, int height, COLORREF color, int diaphaneity);
	void DrawIco2(int x, int y, CString szFilePath);
	void DrawIco(int x, int y, CString szFilePath);
	void Drawtext(int x, int y, CString szFilePath);
	void Draw();

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()//消息必加
};