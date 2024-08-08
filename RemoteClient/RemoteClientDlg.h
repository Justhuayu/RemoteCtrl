
// RemoteClientDlg.h: 头文件
//

#pragma once
#include "StatusDownInfo.h"

#define WM_SEND_PACKET (WM_USER+1)
// CRemoteClientDlg 对话框
class CRemoteClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonConnect();
	DWORD m_ipaddress_server;
	CString m_port_server;
	afx_msg void OnBnClickedButtonFileinfo();
	CTreeCtrl m_tree_dir;
	CFont m_font;           // 字体成员变量
	CImage m_screenImage;
	BOOL m_screenIsFull;
	afx_msg void OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult);

private:
	int sendCommandPacket(int sCmd, BOOL isAutoClose = TRUE, BYTE* pData = NULL, size_t nSize = 0);
	CString getTreePath(HTREEITEM hItem);//获取树item的地址拼接结果
	void deleteTreeChildrenItem(HTREEITEM hItem);//删除树的所有子item
	void loadFileInfo();//加载文件树
	
public:
	CListCtrl m_list_file;
	CStatusDownInfo m_downInfoDlg;//下载中提示框
	static void threadEntryDownFile(void* arg);//多线程下载文件
	void threadDownFile();//下载文件
	static void threadEntryWatch(void* arg);//多线程监控
	void threadWatch();//监控桌面
	afx_msg void OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDownFile();
	afx_msg void OnRunFile();
	afx_msg void OnDeleteFile();
	afx_msg LRESULT WMSendPacket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonWatch();
};
