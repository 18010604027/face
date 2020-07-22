#pragma once
#include"pch.h"
#define PICTURE_FILE L"Data\\textures\\"//�����ڲ���Ҫ��ͼƬ���ļ���
#define ICON 30	//����Բ�Ǻ�ͼ����Ĳ���
#define side_height 60 //button�ĸ�
#define side_width 60 //button�Ŀ�
#define side_text 24 //button������Ԥ��	

class CMyFace :public CWnd
{
	DECLARE_DYNAMIC(CMyFace)//��������һ���ؼ�
protected:
	CRect rect;//�ؼ�����
	CDC dc; CBitmap dc_bmp;//����
	CFont font;//�ļ�������
	bool start;//�Ƿ��һ�λ滭
	int grid_x, grid_y, grid_n;	//һҳ���ͼ����������������ܸ���
	int grid_m;					//���ָ��ڼ���ͼ��(�������ң����϶���)
	int time_change;			//��ʱ��״̬ =0δ��ʱ��=1��ʱ�У�=2��ʱ���;

	bool m_bTracking;	//�����ͣ
	bool m_bOver;		//�����ͣ��ͼ����
	bool m_down;		//��갴�µ��ж�����
	CPoint Mouse_x_y;	//���λ��

	CString format[3] = { L"jpg",L"bmp",L"png" };
	wchar_t szFilePath[200] = { L"C:\\Users\\18010\\Desktop\\�½��ı��ĵ� (3).txt" };//��ק�ļ���ַ
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

	DECLARE_MESSAGE_MAP()//��Ϣ�ؼ�
};