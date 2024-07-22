#pragma once

#include "pch.h"
#include "framework.h"


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

	bool dealRecv();//�������

	bool dealSend();//������
private:
	
	BOOL initSockEnv();//��ʼ��socket����

	CServerSocket& operator=(CServerSocket& ss) {}

	CServerSocket(const CServerSocket& ss) {
		m_sock = ss.m_sock;
		m_client_sock = ss.m_client_sock;
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
};

