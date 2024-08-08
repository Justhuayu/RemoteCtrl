#pragma once

#include "pch.h"
#include "framework.h"
#include <string>
#include <vector>
#define PORT 9527 //服务端socket端口号
#define BUFFER_SIZE 4096 //recv buffer大小
#pragma pack(push,1)//按1字节对齐
class CPacket {
public:
	//默认构造
	CPacket() :sHead(0), nLength(0), sCmd(0), sSum(0) {}
	//拷贝 = 
	CPacket& operator=(CPacket& p)
	{
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
	//移动 = 
	/*CPacket& operator=(CPacket&& p) noexcept {
		if (this != &p) {
			 移动资源
			sHead = std::move(p.sHead);
			nLength = std::move(p.nLength);
			sCmd = std::move(p.sCmd);
			strData = std::move(p.strData);
			sSum = std::move(p.sSum);

			 清理源对象
			p.sHead = 0;
			p.nLength = 0;
			p.sCmd = 0;
			p.strData.clear();
			p.sSum = 0;
		}

		return *this;
	}*/
	//拷贝构造
	CPacket(CPacket& p)
	{
		(*this) = p;
	}
	//析构
	~CPacket() = default;

	//重载取地址符号

	//解包
	CPacket(const BYTE* pData, size_t& nSize);
	//打包
	CPacket(WORD sCmd, BYTE* pData, size_t nSize);


public:
	size_t size(); //求包长度(头 + 长度 + 命令 + 数据 + 校验和)
	const char* data();//返回数据区域的地址
	void showPacket();//打印包内容
public:
	WORD sHead;//包头 2字节 0xFEFF
	DWORD nLength;//长度 4字节 命令 + 数据 + 校验和
	WORD sCmd;//控制命令 2字节
	std::string strData;//包数据
	WORD sSum;//校验和 2字节，使用sum的方式
	std::string strOut;//解析包时输出的数据
};
#pragma pack(pop)
//单例模式
class CClientSocket
{
public:
	static CClientSocket* getInstance() {
		if (!m_instance) {
			m_instance = new CClientSocket();
		}
		return m_instance;
	}

	int  dealRecv();//处理接受
	int  dealSend(const char* pData, int nSize);//处理发送
	BOOL initSockEnv(int ipAddr, int nPort);//初始化socket环境
	CPacket& getCPacket() {//获取 CPakcet
		return m_packet;
	}
	void closeClient() {//关闭client socket
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
private:

	CClientSocket& operator=(CClientSocket& ss) {
		//防止自赋值
		if (this != &ss) {
			m_sock = ss.m_sock;
		}
		return *this;
	}

	CClientSocket(CClientSocket& ss) {
		(*this) = ss;
	}

	CClientSocket() {
		m_buffer.resize(BUFFER_SIZE*2);
		memset((char*)m_buffer.data(), 0, sizeof(m_buffer.data()));
	}
	~CClientSocket() {
		if (m_sock != INVALID_SOCKET) {
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
		WSACleanup();
	}

	//释放单例模式
	static void releaseInstance() {
		if (m_instance) {
			TRACE(_T("CClientSocket releaseInstance()"));

			CClientSocket* tmp = m_instance;
			m_instance = nullptr;
			delete tmp;
		}
	}
	//辅助单例模式释放
	class CServerHelp {
	public:
		CServerHelp() {
			CClientSocket::getInstance();
		}
		~CServerHelp() {
			CClientSocket::releaseInstance();
		}
	};

private:
	std::vector<char> m_buffer;
	static CClientSocket* m_instance;
	static CServerHelp m_help;
	SOCKET m_sock;
	CPacket m_packet;
};
