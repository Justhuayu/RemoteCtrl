#include "pch.h"
#include "MouseCtrl.h"
#include "ServerSocket.h"

//鼠标操作
int CMouseCtrl::MouseEvent() {
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
	CPacket packet(5, NULL, 0);
	CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
	return 0;
}
