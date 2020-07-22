
// faceDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "face.h"
#include "faceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//用于装载和卸载
Gdiplus::GdiplusStartupInput m_GdiplusStarupInput;
ULONG_PTR m_uGdiplusToken;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CfaceDlg 对话框



CfaceDlg::CfaceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FACE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_S, face1);
	DDX_Control(pDX, IDC_BUTTON1, m_Exit);
}

BEGIN_MESSAGE_MAP(CfaceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCPAINT()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_NCLBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, &CfaceDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CfaceDlg 消息处理程序

BOOL CfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//控件大小初始化
	CRect pRect;
	GetClientRect(pRect);
	CRect rect;
	rect.left = (pRect.right - pRect.left) / 10;//调整控件大小
	rect.right = (pRect.right - pRect.left) * 47 / 50;
	rect.top = (pRect.bottom - pRect.top) / 10;
	rect.bottom = (pRect.bottom - pRect.top) * 47 / 50;
	face1.MoveWindow(rect);//设置控件大小
	m_Exit.SetBTedgeCG(0);
	m_Exit.SetTextColor(RGB(255, 255, 255));
	m_Exit.SettypeCG(20);

	//背景	//装载GDI+，用于缩放（效果较好）
	Gdiplus::GdiplusStartup(&m_uGdiplusToken, &m_GdiplusStarupInput, nullptr);
	CString Bmp;
	Bmp.Format(L"%s%s", PICTURE_FILE, L"111.bmp");
	m_img = Image::FromFile(Bmp);  //加载图片

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC m_dc(this);
		CRect bgrect;
		GetClientRect(&bgrect);
		Graphics g(m_dc);
		int bgwidth, bgheight;
		bgheight = m_img->GetHeight();
		bgwidth = m_img->GetWidth();
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
		g.DrawImage(m_img, 0, 0, bgwidth, bgheight);
		/*透明，制作各种各样的窗口*/
		//CBrush brush2;//想怎么填充 就创建怎么样的刷子
		//brush2.CreateSolidBrush(RGB( 0, 0, 0));//纯色
		//m_dc.SelectObject(brush2);
		//m_dc.Ellipse(1, 1, 40, 40);

		//SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		//SetLayeredWindowAttributes(RGB( 0, 0, 0), 100, LWA_COLORKEY);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfaceDlg::OnNcPaint()
{
	// TODO: 在此处添加消息处理程序代码
	CDialogEx::OnNcPaint();
}




void CfaceDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	rect.left = (pRect->right - pRect->left) / 10;//调整主控件大小
	rect.right = (pRect->right - pRect->left) * 47 / 50;
	rect.top = (pRect->bottom - pRect->top) / 10;
	rect.bottom = (pRect->bottom - pRect->top) * 47 / 50;

	//当窗口大小改变的时候，使客户区无效
	face1.MoveWindow(rect, false);//false使它不直接刷新

	rect.right = pRect->right - pRect->left-20;//调整关闭按钮的位置
	rect.top = 0;
	rect.left = rect.right-43;
	rect.bottom = rect.top+24;

	//当窗口大小改变的时候，使客户区无效
	m_Exit.MoveWindow(rect, false);//false使它不直接刷新
}




void CfaceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	Invalidate(TRUE);//为真时擦除背景
}

/*最小长宽*/
void CfaceDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = 120;   //x宽度  
	lpMMI->ptMinTrackSize.y = 300;   //y高度 
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

/*将主控件刷新改为透明*/
HBRUSH CfaceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	//判断控件为face1
	if (pWnd == &face1)
	{
		pDC->SetBkMode(TRANSPARENT); //设置face1背景透明
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return hbr;
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


LRESULT CfaceDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//当鼠标在客户区（控件区域不在内(这样也有好处就是在拖动图标时，不会导致窗口移动，当然也可另外设置）移动时通知窗口鼠标在标题栏
	{
		// 取得鼠标所在的窗口区域
		UINT nHitTest = CDialog::OnNcHitTest(point);

		//// 如果鼠标在窗口客户区，则返回标题条代号给Windows
		//// 使Windows按鼠标在标题条上类进行处理，即可单击移动窗口
		//return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
		CPoint ptCur;
		CRect rect;
		GetCursorPos(&ptCur);
		GetWindowRect(&rect);
		/*用于拖动窗口*/
		if (CRect(rect.left, rect.top+5, rect.right - 128, rect.top + 40).PtInRect(ptCur))
		{
			//MessageBox(L"你马勒戈壁");
			return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
		}
		/*用于拉伸窗口大小*//*注意先判断四个角，再判定四边，否则四角会被屏蔽*/
		if (CRect(rect.left, rect.top, rect.left + 5, rect.top + 5).PtInRect(ptCur))/*左上*/
		{
			return HTTOPLEFT;
		}
		else if (CRect(rect.right - 5, rect.top, rect.right, rect.top + 5).PtInRect(ptCur))/*右上*/
			return HTTOPRIGHT;
		else if (CRect(rect.left, rect.bottom - 5, rect.left + 5, rect.bottom).PtInRect(ptCur))/*左下*/
			return HTBOTTOMLEFT;
		else if (CRect(rect.right - 5, rect.bottom - 5, rect.right, rect.bottom).PtInRect(ptCur))/*右下*/
			return HTBOTTOMRIGHT;
		else if (CRect(rect.left, rect.top, rect.left + 5, rect.bottom).PtInRect(ptCur))
			return HTLEFT;
		else if (CRect(rect.right - 5, rect.top, rect.right, rect.bottom).PtInRect(ptCur))
			return HTRIGHT;
		else if (CRect(rect.left, rect.top, rect.right - 128, rect.top + 5).PtInRect(ptCur)) // 128 Min,Max,Close
			return HTTOP;
		else if (CRect(rect.left, rect.bottom - 5, rect.right, rect.bottom).PtInRect(ptCur))
			return HTBOTTOM;

		return CDialog::OnNcHitTest(point);
	}
	//return CDialogEx::OnNcHitTest(point);
}


BOOL CfaceDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CPoint ptCur;
	CRect rect;
	/*获取屏幕大小*/
	int m_nCxFullScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	int m_nCyFullScreen = GetSystemMetrics(SM_CYFULLSCREEN);
	/*获取鼠标权限和窗口权限*/
	GetCursorPos(&ptCur);
	GetWindowRect(&rect);

	/*窗口大小限制*/
	if (rect.Width() >= m_nCxFullScreen - 3 && rect.Height() >= m_nCyFullScreen - 3)
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	/*窗口大小改变&鼠标位置&光标改变*/
	if (CRect(rect.left, rect.top, rect.left + 5, rect.top + 5).PtInRect(ptCur) || CRect(rect.right - 5, rect.bottom - 5, rect.right, rect.bottom).PtInRect(ptCur))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));/*注意IDC_SIZENWSE中，最后的NW和SE是由北到东和由南到西，指明鼠标变化类型*/
		return 0;
	}
	else if (CRect(rect.left, rect.bottom - 5, rect.left + 5, rect.bottom).PtInRect(ptCur) || CRect(rect.right - 5, rect.top, rect.right, rect.top+5).PtInRect(ptCur))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		return 0;
	}
	else if (CRect(rect.left, rect.top, rect.right - 128, rect.top + 5).PtInRect(ptCur) || CRect(rect.left, rect.bottom - 5, rect.right, rect.bottom).PtInRect(ptCur))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		return 0;
	}
	else if (CRect(rect.left, rect.top, rect.left + 5, rect.bottom).PtInRect(ptCur) || CRect(rect.right - 5, rect.top, rect.right, rect.bottom).PtInRect(ptCur))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		return 0;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CfaceDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*鼠标按下后拖动改变窗口大小*/
	switch (nHitTest)
	{
	case HTTOP:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOM:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(point.x, point.y));
		return;
	case HTLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(point.x, point.y));
		return;
	case HTTOPLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTTOPRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOMLEFT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(point.x, point.y));
		return;
	case HTBOTTOMRIGHT:
		SendMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(point.x, point.y));
		return;
	default:
		CDialogEx::OnNcLButtonDown(nHitTest, point);
	}
}

//关闭键
void CfaceDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}
