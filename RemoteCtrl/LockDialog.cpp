// LockDialog.cpp: 实现文件
//

#include "pch.h"
#include "RemoteCtrl.h"
#include "afxdialogex.h"
#include "LockDialog.h"


// CLockDialog 对话框

IMPLEMENT_DYNAMIC(CLockDialog, CDialogEx)

CLockDialog::CLockDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INFO, pParent)
{

}

CLockDialog::~CLockDialog()
{
}

void CLockDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLockDialog, CDialogEx)
END_MESSAGE_MAP()


// CLockDialog 消息处理程序
