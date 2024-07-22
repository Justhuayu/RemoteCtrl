#pragma once

#include "pch.h"
#include "framework.h"


//单例模式
class CServerSocket
{
public:

	static CServerSocket* getInstance() {
		if (!m_instance) {
			m_instance = new CServerSocket;
		}
		return m_instance;
	}

	bool acceptClient();//接受新客户端

	bool dealRecv();//处理接受

	bool dealSend();//处理发送
private:
	
	BOOL initSockEnv();//初始化socket环境

	CServerSocket& operator=(CServerSocket& ss) {}

	CServerSocket(const CServerSocket& ss) {
		m_sock = ss.m_sock;
		m_client_sock = ss.m_client_sock;
	}

	CServerSocket() {
		if (!initSockEnv()) {
			MessageBox(NULL, _T("初始化socket失败，检查网络连接！"), _T("网络错误"), MB_OK | MB_ICONERROR);
			exit(0);
		}
		m_client_sock = INVALID_SOCKET;

	}
	~CServerSocket() {
		closesocket(m_sock);
		WSACleanup();
	}

	//释放单例模式
	static void releaseInstance() {
		if (m_instance) {
			CServerSocket* tmp = m_instance;
			m_instance = nullptr;
			delete tmp;
		}
	}
	//辅助单例模式释放
	class CServerHelp {
	public:
		CServerHelp() {
			CServerSocket::getInstance();
		}
		~CServerHelp() {
			CServerSocket::releaseInstance();
		}

	};

private:
	static CServerSocket* m_instance;
	static CServerHelp m_help;
	SOCKET m_sock;
	SOCKET m_client_sock;
};

