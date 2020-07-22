#pragma once

#include "pch.h"


class CMyButton :public CButton
{
	DECLARE_DYNAMIC(CMyButton)
public:
	CMyButton();
	virtual ~CMyButton();
	void SetBkColor(COLORREF color);
	void SetTextColor(COLORREF color);
	void SetDiaphaneity(int Over, int Down, int Disable);
	void SetBTedgeCG(int edge);
	void SettypeCG(int type);

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
private:
	bool m_bOver;
	bool m_bDown;
	bool m_bDisable;
	bool m_bTracking;
	COLORREF m_bkColor;
	COLORREF m_textColor;

	int bOver;
	int bDown;
	int bDisable;
	int bedge_change;
	int type_height;

	void ButtonInit();
	void DrawButton();
	void DrawButton(HDC hDestDC);
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
};