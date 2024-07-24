#include "pch.h"
#include "MouseCtrl.h"
#include "ServerSocket.h"

//������
int CMouseCtrl::MouseEvent() {
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
	CPacket packet(5, NULL, 0);
	CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
	return 0;
}
