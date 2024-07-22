#pragma once

#include "pch.h"
#include "framework.h"

class CPacket {
public:
	CPacket() :sHead(0), nLength(0), sCmd(0), sSum(0) {}
	CPacket& operator=(CPacket& p) {
		//防止自赋值
		if (this != &p) {
			sHead = p.sHead;
			nLength = p.nLength;
			sCmd = p.sCmd;
			strData = p.strData;
			sSum = p.sSum;
		}
		return *this;
	}
	CPacket& operator=(CPacket&& p) noexcept {
		if (this != &p) {
			// 移动资源
			sHead = std::move(p.sHead);
			nLength = std::move(p.nLength);
			sCmd = std::move(p.sCmd);
			strData = std::move(p.strData);
			sSum = std::move(p.sSum);

			// 清理源对象
			p.sHead = 0;
			p.nLength = 0;
			p.sCmd = 0;
			p.strData.clear();
			p.sSum = 0;
		}

		return *this;
	}
	CPacket(CPacket& p) {
		(*this) = p;
	}

	CPacket(const BYTE* pData, size_t& nSize);
	~CPacket() = default;
public:
	WORD sHead;//包头 2字节 0xFEFF
	DWORD nLength;//长度 4字节 命令 + 数据 + 校验和
	WORD sCmd;//控制命令 2字节
	std::string strData;//包数据
	WORD sSum;//校验和 2字节，使用sum的方式
};

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

	int  dealRecv();//处理接受

	int  dealSend();//处理发送
private:
	
	BOOL initSockEnv();//初始化socket环境

	CServerSocket& operator=(CServerSocket& ss) {
		//防止自赋值
		if (this != &ss) {
			m_sock = ss.m_sock;
			m_client_sock = ss.m_client_sock;
		}
		return *this;
	}

	CServerSocket(CServerSocket& ss) {
		(*this) = ss;
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
	CPacket m_packet;
};

