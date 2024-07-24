#include "pch.h"
#include "MouseCtrl.h"
#include "ServerSocket.h"
#include "Protocol.h"

#include <atlimage.h>
//鼠标操作
int CMouseCtrl::mouseEvent() {
	MOUSEEVENT mouse;
	if (!CServerSocket::getInstance()->getMouseEvent(mouse)) {
		//没有执行文件查找cmd
		OutputDebugString(_T("当前命令，不是鼠标控制，命令解析失败！"));
		return -1;
	}
	WORD sFlag = 0;//按钮 + 事件 组合flag
	//1. 鼠标事件
	switch (mouse.sAction) {
		case 1://单击
		{
			sFlag = 0x01;
			break;
		}
		case 2://双击
		{
			sFlag = 0x02;
			break;
		}
		case 4://按下
		{
			sFlag = 0x04;
			break;
		}
		case 8://松开
		{
			sFlag = 0x08;
			break;
		}
		default://移动
		{
			sFlag = 0x00;
			break;
		}
	}
	//2. 按钮
	switch (mouse.sButton) {
		case 1://左键
		{
			sFlag |= 0x10;
			break;
		}
		case 2://右键
		{
			sFlag |= 0x20;
			break;
		}
		case 4://中键
		{
			sFlag |= 0x40;
			break;
		}
		case 8://没有按键
		{
			sFlag |= 0x80;
			break;
		}
		default:
		{
			break;
		}
	}
	//3. sFlag 对应事件 高位按钮，低位事件
	switch (sFlag) {
		case 0x80://移动
		{
			mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
			break;
		}
		case 0x12: //左键双击
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x11: //左键单击
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x14: //左键按下
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x18: //左键松开
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x22: //右键双击
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x21: //右键单击
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x24: //右键按下
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x28: //右键松开
		{
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x42: //中键双击
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());

		}
		case 0x41: //中键单击
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x44: //中键按下
		{
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		}
		case 0x48: //中键松开
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

//发送屏幕截图
int CMouseCtrl::screenSend() {
	WORD sCmd = static_cast<WORD>(CProtocol::event::SCREEN_SEND);
	//HDC 通过各种 GDI（图形设备接口）函数传递，以执行绘图任务。
	//DC 描述了绘图环境并提供绘图操作的接口
	HDC hScreen = ::GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);
	int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeight = GetDeviceCaps(hScreen, VERTRES);
	CImage screen;
	//根据当前设备的宽高位图创建CImage
	screen.Create(nWidth, nHeight, nBitPerPixel);
	//位块传输函数，拷贝源DC hScreen 到 目标DC screen，实现屏幕像素复制
	BitBlt(screen.GetDC(), 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hScreen);
	//保存到本地
	//screen.Save(_T("C:/tt/screen.png"), Gdiplus::ImageFormatPNG);
	//将图片保存到内存，方便传输
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (!hMem) return -1;
	//内存流
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret == S_OK) {
		screen.Save(pStream, Gdiplus::ImageFormatPNG);
		//保存流到screen后，流的指针移动到末尾，手动移动到开头
		LARGE_INTEGER bg = { 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		//锁定全局内存 hMem
		PBYTE pData = (PBYTE)GlobalLock(hMem);
		SIZE_T nSize = GlobalSize(hMem);
		//根据全局内存，打包packet
		CPacket packet(sCmd, pData, nSize);
		CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
		GlobalUnlock(hMem);
	}
	
	screen.ReleaseDC();
	pStream->Release();
	GlobalFree(hMem);
	return 0;
}