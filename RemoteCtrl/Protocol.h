#pragma once
#include "pch.h"
#include "framework.h"

namespace CProtocol {
	enum class event:WORD {
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