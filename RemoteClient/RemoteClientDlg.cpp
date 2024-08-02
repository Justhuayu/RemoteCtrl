
// RemoteClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RemoteClient.h"
#include "RemoteClientDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "Protocol.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CRemoteClientDlg 对话框



CRemoteClientDlg::CRemoteClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECLIENT_DIALOG, pParent)
	, m_ipaddress_server(0)
	, m_port_server(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_SERVER, m_ipaddress_server);
	DDX_Text(pDX, IDC_PORT_SERVER, m_port_server);
	DDX_Control(pDX, IDC_TREE_DIR, m_tree_dir);
	DDX_Control(pDX, IDC_LIST_FILE, m_list_file);
}

BEGIN_MESSAGE_MAP(CRemoteClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CRemoteClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_FILEINFO, &CRemoteClientDlg::OnBnClickedButtonFileinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CRemoteClientDlg::OnNMDblclkTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CRemoteClientDlg::OnNMRClickListFile)
END_MESSAGE_MAP()


// CRemoteClientDlg 消息处理程序

BOOL CRemoteClientDlg::OnInitDialog()
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
	//初始化ip和port，显示到mfc上
	UpdateData();
	m_ipaddress_server = 0x7F000001;
	m_port_server = "9527";
	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRemoteClientDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CRemoteClientDlg::sendCommandPacket(int sCmd,BOOL isAutoClose,BYTE* pData, size_t nSize) {
	UpdateData();
	CClientSocket* pClient = CClientSocket::getInstance();
	if (!pClient->initSockEnv(m_ipaddress_server, _ttoi(m_port_server))) {
		AfxMessageBox(_T("连接失败！"));
		return -1;
	}
	CPacket pack(sCmd, pData, nSize);
	pClient->dealSend(pack.data(), pack.size());

	int ret = pClient->dealRecv();
	pack = pClient->getCPacket();
	//AfxMessageBox(pack.strData.c_str());
	TRACE(_T("ack data:%d %s\r\n"), pack.sCmd,pack.strData.c_str());
	if(isAutoClose) pClient->closeClient();
	return ret;
}


void CRemoteClientDlg::OnBnClickedButtonConnect()
{
	WORD sCmd = static_cast<WORD>(CProtocol::event::DISK_DRVIE_INFO);
	sendCommandPacket(sCmd);
}

void CRemoteClientDlg::OnBnClickedButtonFileinfo()
{
	//获取文件树
	//获取磁盘驱动CPacket
	WORD sCmd = static_cast<WORD>(CProtocol::event::DISK_DRVIE_INFO);
	sendCommandPacket(sCmd);
	CClientSocket* pClient = CClientSocket::getInstance();
	std::string strDrivers = pClient->getCPacket().strData;
	//遍历结果，添加到 m_tree_dir
	std::string dr;
	m_tree_dir.DeleteAllItems();
	for (int i = 0; i < strDrivers.size(); i++) {
		if (strDrivers[i] == ',') {
			dr += ':';
			HTREEITEM hTemp = m_tree_dir.InsertItem(dr.c_str(),TVI_ROOT,TVI_LAST);
			m_tree_dir.InsertItem("", hTemp, TVI_LAST);
			dr.clear();
			continue;
		}
		dr += strDrivers[i];
	}
	dr += ':';
	m_tree_dir.InsertItem(dr.c_str());
}

//获取树的路径
CString CRemoteClientDlg::getTreePath(HTREEITEM hItem) {
	CString strRes, strTmp;
	while(hItem != NULL){
		strTmp = m_tree_dir.GetItemText(hItem);
		if (strRes.IsEmpty()) strRes = strTmp;
		else strRes = strTmp + "\\" + strRes;
		hItem = m_tree_dir.GetParentItem(hItem);
	}
	return strRes;
}

void CRemoteClientDlg::deleteTreeChildrenItem(HTREEITEM hItem) {
	HTREEITEM hTemp = m_tree_dir.GetChildItem(hItem);
	while (hTemp != NULL) {
		m_tree_dir.DeleteItem(hTemp);
		hTemp = m_tree_dir.GetChildItem(hItem);
	}
}
void CRemoteClientDlg::loadFileInfo() {
	//获取鼠标在tree control的位置
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_tree_dir.ScreenToClient(&ptMouse);
	//获取选中item
	HTREEITEM hTreeSelected = m_tree_dir.HitTest(ptMouse, 0);
	if (hTreeSelected == NULL) return;
	deleteTreeChildrenItem(hTreeSelected);
	m_list_file.DeleteAllItems();
	//获取选中的路径
	//CString strCurPath = getTreePath(hTreeSelected);
	CString strCurPath = getTreePath(m_tree_dir.GetSelectedItem());
	TRACE(_T("getTreePath() : strCurPath = [%s]\r\n"), strCurPath);
	CClientSocket* pClient = CClientSocket::getInstance();
	WORD sCmd = static_cast<WORD>(CProtocol::event::DIR_INFO);
	int sRetCmd = sendCommandPacket(sCmd, false, reinterpret_cast<BYTE*>(strCurPath.GetBuffer()), strCurPath.GetLength());
	PFILEINFO pfInfo = (PFILEINFO)pClient->getCPacket().strData.c_str();
	while (pfInfo->hasNext) {
		//不处理 . 和 ..
		if (pfInfo->isDirectory) {
			if (CString(pfInfo->filename) == "." || CString(pfInfo->filename) == "..") {
				sRetCmd = pClient->dealRecv();
				if (sRetCmd < 0) break;
				TRACE(_T("ack data: %s\r\n"), pClient->getCPacket().strData.c_str());
				pfInfo = (PFILEINFO)pClient->getCPacket().strData.c_str();
				continue;
			}
			//添加item
			HTREEITEM hTemp = m_tree_dir.InsertItem(pfInfo->filename, hTreeSelected, TVI_LAST);
			m_tree_dir.InsertItem("", hTemp, TVI_LAST);
		}
		else {
			m_list_file.InsertItem(0, pfInfo->filename);
		}

		TRACE(_T("recv finfo filename = [%s]\r\n"), CString(pfInfo->filename));
		//继续接受
		sRetCmd = pClient->dealRecv();
		if (sRetCmd < 0) break;
		pfInfo = (PFILEINFO)pClient->getCPacket().strData.c_str();
	}
	pClient->closeClient();
}
void CRemoteClientDlg::OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	loadFileInfo();
}

void CRemoteClientDlg::OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	//获取当前选中item
	CPoint ptMouse,ptList;
	GetCursorPos(&ptList);
	ptMouse = ptList;
	m_list_file.ScreenToClient(&ptList);
	int listSelected = m_list_file.HitTest(ptList);
	if (listSelected < 0) return;
	//加载menu
	CMenu menu;
	menu.LoadMenu(IDR_MENU_LIST_RIGHT_FILE);
	//获取文件菜单，0号位置
	CMenu* pPupup = menu.GetSubMenu(0);
	if (pPupup) {
		//弹出菜单
		pPupup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this);
	}
}
