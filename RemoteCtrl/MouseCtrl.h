#pragma once
class CMouseCtrl
{
public:
	typedef struct _mouse_event{
		_mouse_event() {
			sAction = -1;
			sButton = -1;
			ptXY.x = 0;
			ptXY.y = 0;
		}
		WORD sAction;//����¼���������˫�������¡��ɿ����޲������ƶ���
		WORD sButton;//��������� �Ҽ� �м�
		POINT ptXY;//����
	}MOUSEEVENT,*PMOUSEEVENT;
public:
	int mouseEvent();//������
	int screenSend();//������Ļ��ͼ

};

