#pragma once

#include "pch.h"
#include "framework.h"

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
	CPacket& operator=(CPacket&& p) noexcept {
		if (this != &p) {
			// �ƶ���Դ
			sHead = std::move(p.sHead);
			nLength = std::move(p.nLength);
			sCmd = std::move(p.sCmd);
			strData = std::move(p.strData);
			sSum = std::move(p.sSum);

			// ����Դ����
			p.sHead = 0;
			p.nLength = 0;
			p.sCmd = 0;
			p.strData.clear();
			p.sSum = 0;
		}

		return *this;
	}
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
	void showPacket(BYTE* pData, size_t nSize);//��ӡ������
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
class CServerSocket
{
public:
	static CServerSocket* getInstance() {
		if (!m_instance) {
			m_instance = new CServerSocket;
		}
		return m_instance;
	}

	bool acceptClient();//�����¿ͻ���
	int  dealRecv();//�������
	int  dealSend(const char* pData, int nSize);//������
	bool getFilePath(std::string& strPath);//��ȡ�ļ�·��

private:
	
	BOOL initSockEnv();//��ʼ��socket����

	CServerSocket& operator=(CServerSocket& ss) {
		//��ֹ�Ը�ֵ
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
			MessageBox(NULL, _T("��ʼ��socketʧ�ܣ�����������ӣ�"), _T("�������"), MB_OK | MB_ICONERROR);
			exit(0);
		}
		m_client_sock = INVALID_SOCKET;

	}
	~CServerSocket() {
		closesocket(m_sock);
		WSACleanup();
	}

	//�ͷŵ���ģʽ
	static void releaseInstance() {
		if (m_instance) {
			CServerSocket* tmp = m_instance;
			m_instance = nullptr;
			delete tmp;
		}
	}
	//��������ģʽ�ͷ�
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

