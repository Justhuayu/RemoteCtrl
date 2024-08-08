
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
	ON_COMMAND(ID_DOWN_FILE, &CRemoteClientDlg::OnDownFile)
	ON_COMMAND(ID_RUN_FILE, &CRemoteClientDlg::OnRunFile)
	ON_COMMAND(ID_DELETE_FILE, &CRemoteClientDlg::OnDeleteFile)
	ON_MESSAGE(WM_SEND_PACKET,&CRemoteClientDlg::WMSendPacket)
	ON_BN_CLICKED(IDC_BUTTON_WATCH, &CRemoteClientDlg::OnBnClickedButtonWatch)
	ON_WM_TIMER()
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

	//设置字体
	// 获取系统默认字体
	CFont* pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));

	// 获取默认字体的 LOGFONT
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	// 设置字体为微软雅黑
	_tcscpy_s(lf.lfFaceName, _T("Microsoft YaHei"));

	// 创建新字体
	m_font.CreateFontIndirect(&lf);

	// 为控件设置新字体
	m_list_file.SetFont(&m_font);
	m_tree_dir.SetFont(&m_font);
	// TODO: 在此添加额外的初始化代码
	//初始化ip和port，显示到mfc上
	UpdateData();
	m_ipaddress_server = 0x7F000001;
	m_port_server = "9527";
	UpdateData(FALSE);
	//创建正在下载提示框
	m_downInfoDlg.Create(IDD_DLG_DOWNINFO, this);
	m_downInfoDlg.m_info.SetWindowText(_T("正在下载文件......"));
	m_downInfoDlg.ShowWindow(SW_HIDE);
	//初始化screen CImage的缓充没满
	m_screenIsFull = FALSE;
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
		strRes = strTmp + '\\' + strRes;
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
	WORD nCount = 0;

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
			HTREEITEM hTemp = m_tree_dir.InsertItem(CString(pfInfo->filename), hTreeSelected, TVI_LAST);
			m_tree_dir.InsertItem("", hTemp, TVI_LAST);
		}else {
			m_list_file.InsertItem(0, CString(pfInfo->filename));
		}

		TRACE(_T("recv finfo filename %d = [%s]\r\n"), nCount++,CString(pfInfo->filename));
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
	//右键菜单
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

void CRemoteClientDlg::threadEntryDownFile(void* arg) {
	CRemoteClientDlg* dlg = (CRemoteClientDlg*)arg;
	dlg->threadDownFile();
	_endthread();
}

void CRemoteClientDlg::threadDownFile() {
	//下载文件
	//1. 获取文件路径
	int selectedList = m_list_file.GetSelectionMark();
	CString filePath = m_list_file.GetItemText(selectedList, 0);
	HTREEITEM hTmp = m_tree_dir.GetSelectedItem();
	filePath = getTreePath(hTmp) + filePath;
	TRACE(_T("filepath: %s\r\n"), filePath);
	//2. 设置保存路径
	CFileDialog fDlg(FALSE, "*",
		m_list_file.GetItemText(selectedList, 0),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
	if (fDlg.DoModal() != IDOK) {
		TRACE(_T("打开文件对话框失败！"));
		m_downInfoDlg.ShowWindow(SW_HIDE);
		EndWaitCursor();
		return;
	}
	//3. 创建文件
	FILE* file;
	errno_t err = fopen_s(&file, fDlg.GetPathName(), "wb+");
	if (err != 0 || !file) {
		TRACE(_T("新建文件失败！！"));
		m_downInfoDlg.ShowWindow(SW_HIDE);
		EndWaitCursor();
		return;
	}
	//4. 接收文件数据
	WORD sCmd = static_cast<WORD>(CProtocol::event::DOWN_FILE);
	CClientSocket* pClient = CClientSocket::getInstance();
	//int ret = sendCommandPacket(sCmd, false, (BYTE*)(LPCSTR)filePath, filePath.GetLength());
	int ret = SendMessage(WM_SEND_PACKET, sCmd << 1 | 0, (LPARAM)(LPCSTR)filePath);
	if (ret < 0) {
		TRACE(_T("接收文件头失败！！"));
		m_downInfoDlg.ShowWindow(SW_HIDE);
		EndWaitCursor();
		return;
	}
	long long nHead = *(long long*)pClient->getCPacket().strData.c_str();
	long long nCount = 0;
	while (nCount < nHead) {
		//接收数据，消耗大量时间
		ret = pClient->dealRecv();
		if (ret < 0) {
			TRACE(_T("传输数据失败!!"));
			//m_downInfoDlg.ShowWindow(SW_HIDE);
			//EndWaitCursor();
			break;
		}
		//TODO:大文件接收
		fwrite(pClient->getCPacket().strData.c_str(), 1, pClient->getCPacket().strData.size(), file);
		nCount += pClient->getCPacket().strData.size();
		//TRACE(_T("####recv file size: %d\r\n"), nCount);
	}
	if (nCount != nHead) {
		TRACE(_T("文件接收不完整，期望大小：%d，实际大小：%d\n"), nHead, nCount);
	}
	fclose(file);
	pClient->closeClient();
	m_downInfoDlg.ShowWindow(SW_HIDE);
	MessageBox(_T("文件下载完毕!"), _T("提示"), MB_OK);
}
void CRemoteClientDlg::OnDownFile()
{
	_beginthread(threadEntryDownFile, 0, this);
	//等待50ms，让子线程的update获取数据能够准确的获取，防止多线程冲突(锁)
	Sleep(50);
	m_downInfoDlg.ShowWindow(SW_SHOW);
	BeginWaitCursor();
	m_downInfoDlg.SetActiveWindow();
	m_downInfoDlg.CenterWindow(this);
}
LRESULT CRemoteClientDlg::WMSendPacket(WPARAM wParam, LPARAM lParam) 
{
	WORD sCmd = wParam >> 1;
	int ret;
	switch (static_cast<CProtocol::event>(sCmd)) {
		case CProtocol::event::DOWN_FILE:
		{
			CString filePath = (LPCSTR)lParam;
			ret = sendCommandPacket(sCmd, wParam & 1, (BYTE*)(LPCSTR)filePath, filePath.GetLength());
			break;
		}
		case CProtocol::event::SCREEN_SEND:
		{
			ret = sendCommandPacket(sCmd, wParam & 1,NULL,0);
			break;
		}
		default:
		{
			ret = -1;
			break;
		}
	}
	return ret;
}

void CRemoteClientDlg::OnRunFile()
{
	// 运行文件
	int selectedIndex = m_list_file.GetSelectionMark();
	CString strFilePath = m_list_file.GetItemText(selectedIndex, 0);
	strFilePath = getTreePath(m_tree_dir.GetSelectedItem()) + strFilePath;
	TRACE(_T("run file : %s\r\n"),strFilePath);
	WORD sCmd = static_cast<WORD>(CProtocol::event::RUN_FILE);
	int ret = sendCommandPacket(sCmd, true, (BYTE*)(LPCSTR)strFilePath, strFilePath.GetLength());
	if (ret < 0) {
		TRACE(_T("[ERROR]run file : %s\r\n"), strFilePath);
	}
	return;
}


void CRemoteClientDlg::OnDeleteFile()
{
	//删除文件
	//1.获取文件路径
	int selectedIndex = m_list_file.GetSelectionMark();
	CString strFilePath = m_list_file.GetItemText(selectedIndex,0);
	strFilePath = getTreePath(m_tree_dir.GetSelectedItem()) + strFilePath;
	TRACE(_T("delete file : %s\r\n"), strFilePath);
	//2.发送请求删除文件
	WORD sCmd = static_cast<WORD>(CProtocol::event::DELETE_FILE);
	int ret = sendCommandPacket(sCmd, true, (BYTE*)(LPCSTR)strFilePath, strFilePath.GetLength());
	if (ret < 0) {
		TRACE(_T("[ERROR]delete file : %s\r\n"), strFilePath);
	}
	//3.刷新文件
	//TODO:删除失败情况处理
	m_list_file.DeleteItem(selectedIndex);
	return;
}

//多线程监控
void CRemoteClientDlg::threadEntryWatch(void* arg) {
	CRemoteClientDlg* pDlg = (CRemoteClientDlg*)arg;
	pDlg->threadWatch();
	_endthread();
}
//监控桌面
void CRemoteClientDlg::threadWatch() 
{
	CClientSocket* pClient = NULL;
	do {
		pClient = CClientSocket::getInstance();
	} while (!pClient);
	WORD sCmd = static_cast<WORD>(CProtocol::event::SCREEN_SEND);

	//使用流来缓存图片
	IStream* pStream = NULL;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == NULL) {
		TRACE(_T("[ERROR]GlobalAlloc() failed，内存不足!"));
		GlobalFree(hMem);
		return;
	}
	HRESULT hRet = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (hRet != S_OK) {
		TRACE(_T("[ERROR]CreateStreamOnHGlobal() failed!"));
		return;
	}
	while (hRet == S_OK) {
		//if (GetTickCount64() - tick < 50) {
		//	Sleep(static_cast<DWORD>(50 - (GetTickCount64() - tick)));
		//}
		//tick = GetTickCount64(); // 更新 tick
		if (m_screenIsFull) {
			//缓冲区图片没有处理
			Sleep(1);
			continue;
		}
		//接收桌面图片
		int recvRet = SendMessage(WM_SEND_PACKET, sCmd << 1 | 0, NULL);
		if(recvRet < 0) {
			TRACE(_T("[ERROR]recv screen request failed!"));
			Sleep(1);
			continue;
		}
		//接收头
		SIZE_T nSize = *(SIZE_T*)pClient->getCPacket().strData.c_str();
		SIZE_T nCount = 0;
		ULONG streamSize = 0;//实际往流里写的长度

		// 清空并重置内存和流
		ULARGE_INTEGER zeroSize;
		zeroSize.QuadPart = 0;
		pStream->SetSize(zeroSize); // 清空流内容

		LARGE_INTEGER zeroPos;
		zeroPos.QuadPart = 0;
		pStream->Seek(zeroPos, STREAM_SEEK_SET, NULL); // 重置流指针

		LARGE_INTEGER zero = {};
		pStream->Seek(zero, STREAM_SEEK_SET, NULL); // 重置流指针


		while (nCount < nSize) {
			recvRet = pClient->dealRecv();
			if (recvRet < 0) {
				TRACE(_T("[ERROR]recv screen data failed!"));
				continue;
			}
			pStream->Write(pClient->getCPacket().strData.c_str(), pClient->getCPacket().strData.size(), &streamSize);
			nCount += streamSize;
		}
		LARGE_INTEGER bg = { 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		m_screenImage.Load(pStream);
		m_screenIsFull = true;
		
		//m_screenImage.Save(_T("C:/tt/screen1.png"), Gdiplus::ImageFormatPNG);

		
	}
	// 释放流对象
	pStream->Release();
	GlobalFree(hMem);
}
void CRemoteClientDlg::OnBnClickedButtonWatch()
{
	//远程监控
	//显示屏幕监控
	CScreenWatch dlg(this);
	//子线程获取屏幕
	_beginthread(CRemoteClientDlg::threadEntryWatch,0,this);

	dlg.DoModal();
}


void CRemoteClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}
