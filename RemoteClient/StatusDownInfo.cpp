// StatusDownInfo.cpp: 实现文件
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "StatusDownInfo.h"


// CStatusDownInfo 对话框

IMPLEMENT_DYNAMIC(CStatusDownInfo, CDialogEx)

CStatusDownInfo::CStatusDownInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DOWNINFO, pParent)
{
	
}

CStatusDownInfo::~CStatusDownInfo()
{
}

void CStatusDownInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DOWNINFO, m_info);
}


BEGIN_MESSAGE_MAP(CStatusDownInfo, CDialogEx)
END_MESSAGE_MAP()


// CStatusDownInfo 消息处理程序
