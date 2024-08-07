#pragma once
#include "afxdialogex.h"


// CStatusDownInfo 对话框

class CStatusDownInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CStatusDownInfo)

public:
	CStatusDownInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStatusDownInfo();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DOWNINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_info;
};
