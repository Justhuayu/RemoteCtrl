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
	BOOL isInvalid;//文件无效 0 有效，1 无效
	BOOL isDirectory;//是否为目录 0 文件，1 目录
	BOOL hasNext;//是否有下一个文件，用于文件传输结束
	char filename[256];//存放文件名
}FILEINFO, * PFILEINFO;

typedef struct _mouse_event {
	_mouse_event() {
		sAction = -1;
		sButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD sAction;//鼠标事件，单击、双击、按下、松开、无操作（移动）
	WORD sButton;//按键，左键 右键 中键
	POINT ptXY;//坐标
}MOUSEEVENT, * PMOUSEEVENT;