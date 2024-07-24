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
		WORD sAction;//鼠标事件，单击、双击、按下、松开、无操作（移动）
		WORD sButton;//按键，左键 右键 中键
		POINT ptXY;//坐标
	}MOUSEEVENT,*PMOUSEEVENT;
public:
	int mouseEvent();//鼠标操作
	int screenSend();//发送屏幕截图

};

