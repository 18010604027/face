#pragma once
#include"pch.h"

//����ͼƬ����
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

#define PICTURE_FILE L"Data\\textures\\"//�����ڲ���Ҫ��ͼƬ���ļ���
#define ICON 30	//����Բ�Ǻ�ͼ����Ĳ���
#define side_height 70 //button�ĸ�
#define side_width 70 //button�Ŀ�
#define side_text 24 //button������Ԥ��	



class CMyFace :public CWnd
{
	DECLARE_DYNAMIC(CMyFace)//��������һ���ؼ�
protected:
	CRect crect;//�ؼ�����
	Rect rect;//�ؼ�����

	CDC dc; CBitmap dc_bmp;//����
	Bitmap* bmp_bottom_re;//��ҳͼ�����
	Bitmap* bmp_bottom_file;//��ҳͼ���ļ���Ϣ
	Bitmap* bmp_airfile;//����ͼ��
	Bitmap* bmp_bk;//����ͼ��
	Bitmap* bmp_air;//��������ͼ��
	Bitmap* bmp_drop;//��קͼ��
	
	bool start;//�Ƿ��һ�λ滭
	bool refresh;//�Ƿ�ˢ�¹�����
	int grid_x, grid_y, grid_n;	//һҳ���ͼ����������������ܸ���
	//int grid_old;				
	int grid_m;					//���ָ��ڼ���ͼ��(�������ң����϶���)
	int grid_old;				//����ȥָ��ͼ��(������ק�������)
	int grid_old_f;				//����ȥָ��ͼ��(�����������������)
	int time_change;			//����ͼ��仯����ʱ��״̬ =0δ��ʱ��=1��ʱ�У�=2��ʱ���;
	int air_time;				//���ڸ�����������ʱ��״̬ =0δ��ʱ��=1��ʱ�У�=2��ʱ���;
	int air_view;		//������ʾ����0��1������2������
	int air_down;		//�����رղ���0ȫ��1��������2ȫ��

	bool m_bTracking;	//�����ͣ
	bool m_bOver;		//�����ͣ��ͼ����
	bool m_down;		//��갴�µ��ж�����
	CPoint Mouse_x_y;	//���λ��
	CPoint Mouse_old;	//����������˲���λ��(ͼ����ק)
	CPoint M_old_f;		//����������˲��ĸ���λ��(������
	CPoint Mouse_cg;	//�����ק�ı���

	CString format[3] = { L"jpg",L"bmp",L"png" };
	wchar_t szFilePath[200] = { L"Data\\textures\\ͼ��.png" };//��ק�ļ���ַ
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

	void DrawFile(int num, bool bottom);//���ļ������ڵ�����ȡ������������ţ��Ƿ����ڵײ㣩

	void DrawBottomRe();//���ײ����
	void DrawBottomFile();//���ײ��ļ���Ϣ
	void DrawDrop();//����ק
	void DrawBk();//������
	void Draw();//���

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

	DECLARE_MESSAGE_MAP()//��Ϣ�ؼ�
};