#pragma once
#include "afxdialogex.h"
// CScreenWatch 对话框

class CScreenWatch : public CDialog
{
	DECLARE_DYNAMIC(CScreenWatch)

public:
	CScreenWatch(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CScreenWatch();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_WATCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	CStatic m_picture;
};
