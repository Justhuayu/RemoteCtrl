#include "pch.h"
#include "ServerSocket.h"

#define PORT 9527

CServerSocket* CServerSocket::m_instance = nullptr;
CServerSocket::CServerHelp CServerSocket::m_help;

BOOL CServerSocket::initSockEnv() {
	//初始化socket环境
	WSADATA data;
	if (0 != WSAStartup(MAKEWORD(1, 1), &data)) {
		MessageBox(NULL, _T("启动socket服务失败！"), _T("网络错误"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sock == -1) return false;
	sockaddr_in sock_addr{};
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	sock_addr.sin_port = htons(PORT);
	if (-1 == bind(m_sock, (const sockaddr*)&sock_addr, sizeof(sock_addr))) return false;
	if (-1 == listen(m_sock, 1)) return false;
	return true;
}

bool CServerSocket::acceptClient() {
	if (!m_instance) return false;
	if (m_sock == INVALID_SOCKET) return false;
	sockaddr_in client_sock;
	int client_length = sizeof(client_sock);
	m_client_sock = accept(m_sock, (sockaddr*)&client_sock, &client_length);
	if (m_client_sock == INVALID_SOCKET) return false;
	return true;
}

bool CServerSocket::dealRecv() {
	if (!m_instance) return false;
	if (m_client_sock == INVALID_SOCKET) return false;
	char buffer[1024] = { 0 };
	int ret = recv(m_client_sock, buffer, sizeof(buffer), 0);
	if (ret <= 0) {
		MessageBox(NULL, _T("接受数据错误"), _T("错误！"), MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

bool CServerSocket::dealSend() {
	if (!m_instance) return false;
	if (m_client_sock == INVALID_SOCKET) return false;
	return send(m_client_sock, "123", 3, 0) > 0;
}