// ScreenWatch.cpp: 实现文件
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "ScreenWatch.h"
#include "RemoteClientDlg.h"


// CScreenWatch 对话框

IMPLEMENT_DYNAMIC(CScreenWatch, CDialog)

CScreenWatch::CScreenWatch(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_WATCH, pParent)
{

}

CScreenWatch::~CScreenWatch()
{
}

void CScreenWatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}


BEGIN_MESSAGE_MAP(CScreenWatch, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CScreenWatch 消息处理程序


void CScreenWatch::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0) {
		CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
		if (pParent->m_screenIsFull) {
			//显示数据
			CRect rect;
			m_picture.GetWindowRect(rect);
			//pParent->m_screenImage.BitBlt(m_picture.GetDC()->GetSafeHdc(),0,0,SRCCOPY);
			pParent->m_screenImage.StretchBlt(m_picture.GetDC()->GetSafeHdc(),
				0,0,rect.Width(),rect.Height(),SRCCOPY);
			m_picture.InvalidateRect(NULL);
			pParent->m_screenImage.Destroy();
			pParent->m_screenIsFull = FALSE;
		}
	}
	CDialog::OnTimer(nIDEvent);
}


BOOL CScreenWatch::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//定时器，用于显示监控图片
	SetTimer(0,40,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
