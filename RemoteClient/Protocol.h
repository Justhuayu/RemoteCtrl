#pragma once
#include "pch.h"
#include "framework.h"

namespace CProtocol {
	enum class event :WORD {
		DISK_DRVIE_INFO = 0x01,
		DIR_INFO,
		RUN_FILE,
		DOWN_FILE,
		DELETE_FILE,
		MOUSE_CTRL,
		SCREEN_SEND,
		LOCK_MACHINE,
		UNLOCK_MACHINE
	};

};

typedef struct _file_info {
	_file_info() {
		isInvalid = false;
		hasNext = true;
		isDirectory = -1;
		memset(filename, 0, sizeof(filename));
	}
	BOOL isInvalid;//�ļ���Ч 0 ��Ч��1 ��Ч
	BOOL isDirectory;//�Ƿ�ΪĿ¼ 0 �ļ���1 Ŀ¼
	BOOL hasNext;//�Ƿ�����һ���ļ��������ļ��������
	char filename[256];//����ļ���
}FILEINFO, * PFILEINFO;

typedef struct _mouse_event {
	_mouse_event() {
		sAction = -1;
		sButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD sAction;//����¼���������˫�������¡��ɿ����޲������ƶ���
	WORD sButton;//��������� �Ҽ� �м�
	POINT ptXY;//����
}MOUSEEVENT, * PMOUSEEVENT;