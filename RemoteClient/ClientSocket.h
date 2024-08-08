#pragma once

#include "pch.h"
#include "framework.h"
#include <string>
#include <vector>
#define PORT 9527 //�����socket�˿ں�
#define BUFFER_SIZE 4096 //recv buffer��С
#pragma pack(push,1)//��1�ֽڶ���
class CPacket {
public:
	//Ĭ�Ϲ���
	CPacket() :sHead(0), nLength(0), sCmd(0), sSum(0) {}
	//���� = 
	CPacket& operator=(CPacket& p)
	{
		//��ֹ�Ը�ֵ
		if (this != &p) {
			sHead = p.sHead;
			nLength = p.nLength;
			sCmd = p.sCmd;
			strData = p.strData;
			sSum = p.sSum;
		}
		return *this;
	}
	//�ƶ� = 
	/*CPacket& operator=(CPacket&& p) noexcept {
		if (this != &p) {
			 �ƶ���Դ
			sHead = std::move(p.sHead);
			nLength = std::move(p.nLength);
			sCmd = std::move(p.sCmd);
			strData = std::move(p.strData);
			sSum = std::move(p.sSum);

			 ����Դ����
			p.sHead = 0;
			p.nLength = 0;
			p.sCmd = 0;
			p.strData.clear();
			p.sSum = 0;
		}

		return *this;
	}*/
	//��������
	CPacket(CPacket& p)
	{
		(*this) = p;
	}
	//����
	~CPacket() = default;

	//����ȡ��ַ����

	//���
	CPacket(const BYTE* pData, size_t& nSize);
	//���
	CPacket(WORD sCmd, BYTE* pData, size_t nSize);


public:
	size_t size(); //�������(ͷ + ���� + ���� + ���� + У���)
	const char* data();//������������ĵ�ַ
	void showPacket();//��ӡ������
public:
	WORD sHead;//��ͷ 2�ֽ� 0xFEFF
	DWORD nLength;//���� 4�ֽ� ���� + ���� + У���
	WORD sCmd;//�������� 2�ֽ�
	std::string strData;//������
	WORD sSum;//У��� 2�ֽڣ�ʹ��sum�ķ�ʽ
	std::string strOut;//������ʱ���������
};
#pragma pack(pop)
//����ģʽ
class CClientSocket
{
public:
	static CClientSocket* getInstance() {
		if (!m_instance) {
			m_instance = new CClientSocket();
		}
		return m_instance;
	}

	int  dealRecv();//�������
	int  dealSend(const char* pData, int nSize);//������
	BOOL initSockEnv(int ipAddr, int nPort);//��ʼ��socket����
	CPacket& getCPacket() {//��ȡ CPakcet
		return m_packet;
	}
	void closeClient() {//�ر�client socket
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
private:

	CClientSocket& operator=(CClientSocket& ss) {
		//��ֹ�Ը�ֵ
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

	//�ͷŵ���ģʽ
	static void releaseInstance() {
		if (m_instance) {
			TRACE(_T("CClientSocket releaseInstance()"));

			CClientSocket* tmp = m_instance;
			m_instance = nullptr;
			delete tmp;
		}
	}
	//��������ģʽ�ͷ�
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
