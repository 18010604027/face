#pragma once
#include"pch.h"

//特殊图片处理
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

#define PICTURE_FILE L"Data\\textures\\"//储存内部需要的图片的文件夹
#define ICON 30	//设置圆角和图标间距的参数
#define side_height 70 //button的高
#define side_width 70 //button的宽
#define side_text 24 //button的文字预留	



class CMyFace :public CWnd
{
	DECLARE_DYNAMIC(CMyFace)//声明这是一个控件
protected:
	CRect crect;//控件矩形
	Rect rect;//控件矩形

	CDC dc; CBitmap dc_bmp;//缓存
	Bitmap* bmp_bottom_re;//底页图层矩形
	Bitmap* bmp_bottom_file;//底页图层文件信息
	Bitmap* bmp_airfile;//浮窗图层
	Bitmap* bmp_bk;//背景图层
	Bitmap* bmp_air;//浮窗背景图层
	Bitmap* bmp_drop;//拖拽图层
	
	bool start;//是否第一次绘画
	bool refresh;//是否刷新过背景
	int grid_x, grid_y, grid_n;	//一页存放图标的列数、排数、总个数
	//int grid_old;				
	int grid_m;					//鼠标指向第几个图标(从左向右，自上而下)
	int grid_old;				//鼠标过去指向图标(用于拖拽保存参数)
	int grid_old_f;				//鼠标过去指向图标(用于主浮窗保存参数)
	int time_change;			//用于图标变化，计时器状态 =0未计时，=1计时中，=2计时完毕;
	int air_time;				//用于浮窗创建，计时器状态 =0未计时，=1计时中，=2计时完毕;
	int air_view;		//浮窗显示参数0隐1主浮窗2副浮窗
	int air_down;		//浮窗关闭参数0全开1开主浮窗2全关

	bool m_bTracking;	//鼠标悬停
	bool m_bOver;		//鼠标悬停于图标上
	bool m_down;		//鼠标按下的判定参数
	CPoint Mouse_x_y;	//鼠标位置
	CPoint Mouse_old;	//鼠标左键按下瞬间的位置(图标拖拽)
	CPoint M_old_f;		//鼠标左键按下瞬间的浮窗位置(主浮窗
	CPoint Mouse_cg;	//鼠标拖拽改变量

	CString format[3] = { L"jpg",L"bmp",L"png" };
	wchar_t szFilePath[200] = { L"Data\\textures\\图标.png" };//拖拽文件地址
public:
	CMyFace();
	virtual ~CMyFace();

protected:
	void DrawRignrim(int x, int y, int width, int height, Color color, Bitmap* bmp);
	void DrawRign(int x, int y, int width, int height, Color color, Bitmap* bmp);
	void DrawIco(int x, int y, CString szFilePath, Bitmap* bmp);
	void DrawIco2(int x, int y, CString szFilePath, Bitmap* bmp);
	void Drawfolder(int x, int y, int* file,int dep, Bitmap* bmp);
	void Drawtext(int x, int y, CString szFilePath, Bitmap* bmp);
	
	void Air_View(int x,int y,int* file,int dep);

	void DrawFile(int num, bool bottom);//画文件（用于点亮或取消点亮）（序号，是否是在底层）

	void DrawBottomRe();//画底层矩形
	void DrawBottomFile();//画底层文件信息
	void DrawDrop();//画拖拽
	void DrawBk();//画背景
	void Draw();//结合

	void Start();
	void Restart();
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