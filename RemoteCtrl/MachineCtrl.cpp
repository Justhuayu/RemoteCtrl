#include "pch.h"
#include "MachineCtrl.h"
#include "ServerSocket.h"
#include "Protocol.h"
#include "resource.h"
#include <atlimage.h>
//����ȫ�ֱ���
CLockDialog dlg;
unsigned dlg_threadid = 0;
//������
int CMachineCtrl::mouseEvent() {
	MOUSEEVENT mouse;
	if (!CServerSocket::getInstance()->getMouseEvent(mouse)) {
		//û��ִ���ļ�����cmd
		OutputDebugString(_T("��ǰ������������ƣ��������ʧ�ܣ�"));
		return -1;
	}
	WORD sFlag = 0;//��ť + �¼� ���flag
	//1. ����¼�
	switch (mouse.sAction) {
		case 1://����
		{
			sFlag = 0x01;
			break;
		}
		case 2://˫��
		{
			sFlag = 0x02;
			break;
		}
		case 4://����
		{
			sFlag = 0x04;
			break;
		}
		case 8://�ɿ�
		{
			sFlag = 0x08;
			break;
		}
		default://�ƶ�
		{
			sFlag = 0x00;
			break;
		}
	}
	//2. ��ť
	switch (mouse.sButton) {
		case 1://���
		{
			sFlag |= 0x10;
			break;
		}
		case 2://�Ҽ�
		{
			sFlag |= 0x20;
			break;
		}
		case 4://�м�
		{
			sFlag |= 0x40;
			break;
		}
		case 8://û�а���
		{
			sFlag |= 0x80;
			break;
		}
		default:
		{
			break;
		}
	}
	//3. sFlag ��Ӧ�¼� ��λ��ť����λ�¼�
	switch (sFlag) {
		case 0x80://�ƶ�
		{
			mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
			break;
		}
		case 0x12: //���˫��
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x11: //�������
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x14: //�������
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x18: //����ɿ�
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x22: //�Ҽ�˫��
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x21: //�Ҽ�����
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x24: //�Ҽ�����
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x28: //�Ҽ��ɿ�
		{
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x42: //�м�˫��
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x41: //�м�����
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x44: //�м�����
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x48: //�м��ɿ�
		{
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		default:
			break;
	}
	WORD sCmd = static_cast<WORD>(CProtocol::event::MOUSE_CTRL);

	CPacket packet(sCmd, NULL, 0);
	CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
	return 0;
}

//������Ļ��ͼ
int CMachineCtrl::screenSend() {
	WORD sCmd = static_cast<WORD>(CProtocol::event::SCREEN_SEND);
	//HDC ͨ������ GDI��ͼ���豸�ӿڣ��������ݣ���ִ�л�ͼ����
	//DC �����˻�ͼ�������ṩ��ͼ�����Ľӿ�
	HDC hScreen = ::GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);
	int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeight = GetDeviceCaps(hScreen, VERTRES);
	CImage screen;
	//���ݵ�ǰ�豸�Ŀ��λͼ����CImage
	screen.Create(nWidth, nHeight, nBitPerPixel);
	//λ�鴫�亯��������ԴDC hScreen �� Ŀ��DC screen��ʵ����Ļ���ظ���
	BitBlt(screen.GetDC(), 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hScreen);
	//���浽����
	//screen.Save(_T("C:/tt/screen.png"), Gdiplus::ImageFormatPNG);
	//��ͼƬ���浽�ڴ棬���㴫��
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (!hMem) return -1;
	//�ڴ���
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret == S_OK) {
		screen.Save(pStream, Gdiplus::ImageFormatPNG);
		//��������screen������ָ���ƶ���ĩβ���ֶ��ƶ�����ͷ
		LARGE_INTEGER bg = { 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		//����ȫ���ڴ� hMem
		PBYTE pData = (PBYTE)GlobalLock(hMem);
		SIZE_T nSize = GlobalSize(hMem);
		//����ȫ���ڴ棬���packet
		CPacket packet(sCmd, pData, nSize);
		CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
		GlobalUnlock(hMem);
	}
	
	screen.ReleaseDC();
	pStream->Release();
	GlobalFree(hMem);
	return 0;
}

//�������߳�
unsigned __stdcall threadLockDlg(void *arg) {
	//��ģ̬�������ڣ�DoModalģ̬����
	dlg.Create(IDD_DIALOG_INFO, NULL);
	//��ʾ����
	dlg.ShowWindow(SW_SHOW);

	//��ס��Ļ
	CRect rect;
	rect.left = -10;
	rect.top = -10;
	rect.right = GetSystemMetrics(SM_CXFULLSCREEN) * 1.1;
	rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN) * 1.1;
	dlg.MoveWindow(rect);
	//�����ö�
	dlg.SetWindowPos(&dlg.wndTopMost, -10, -10, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	//����������
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_HIDE);

	//�������
	ShowCursor(false);
	rect.left = 10;
	rect.right = 1;
	rect.top = 10;
	rect.bottom = 1;
	ClipCursor(rect);

	//MFC ��Ϣѭ��
	MSG msg{};
	//ѭ����ȡ��Ϣ
	while (GetMessageA(&msg, NULL, 0, 0)) {
		//������Ϣ
		TranslateMessage(&msg);
		//������Ϣ
		DispatchMessage(&msg);
		//������Ϣ(msg.wParam �� msg.lParam Я��������Ϣ)
		if (msg.message == WM_KEYDOWN) {
			if (msg.wParam == 0x1B) {
				TRACE(_T("msg: %08x, wParam: %08x, lParam: %08x\r\n"), msg.message, msg.wParam, msg.lParam);
				//esc �˳�
				break;
			}
		}
	}
	//��ʾϵͳ������
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_SHOW);
	ShowCursor(TRUE);
	dlg.DestroyWindow();
	_endthreadex(0);
	return 0;
}
//����
int CMachineCtrl::lockMachine() {
	if (dlg.m_hWnd == NULL || dlg.m_hWnd == INVALID_HANDLE_VALUE) {
		//��һ��ִ������
		_beginthreadex(NULL,0,threadLockDlg, NULL,0, &dlg_threadid);
	}
	WORD sCmd = static_cast<WORD>(CProtocol::event::LOCK_MACHINE);
	CPacket packet(sCmd, NULL, 0);
	CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
	return 0;
}

//��������
int CMachineCtrl::unlockMachine() {

	//dlg.SendMessage(WM_KEYDOWN, VK_ESCAPE, lParam);
	//���̷߳��Ͱ�����Ϣ��SednMessageֻ�ᵥ�߳��ڷ�����Ϣ�����߳̽��ղ���
	PostThreadMessage(dlg_threadid, WM_KEYDOWN, VK_ESCAPE, 0x00010001);

	WORD sCmd = static_cast<WORD>(CProtocol::event::UNLOCK_MACHINE);
	CPacket packet(sCmd, NULL, 0);
	CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
	return 0;
}