#include "pch.h"
#include "ClientSocket.h"
#include "pch.h"
#include "Protocol.h"
#include <set>


//��̬��������
CClientSocket* CClientSocket::m_instance = nullptr;
CClientSocket::CServerHelp CClientSocket::m_help;

std::string getErrorInfo(int wsaErrCode) {
	std::string str;
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		wsaErrCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	return str;
}

//��ʼ��socket����
BOOL CClientSocket::initSockEnv(int ipAddr,int nPort) {
	//�ر�client�������ظ���ʼ��
	if (m_sock != INVALID_SOCKET) closeClient();
	//��ʼ��socket����
	WSADATA data;
	if (0 != WSAStartup(MAKEWORD(1, 1), &data)) {
		MessageBox(NULL, _T("����socket����ʧ�ܣ�"), _T("�������"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sock == -1) return false;
	sockaddr_in sock_addr{};
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(ipAddr);//������ת������
	sock_addr.sin_port = htons(nPort);
	if (sock_addr.sin_addr.s_addr == INADDR_NONE) {
		AfxMessageBox(_T("IP��ַ�����ڣ�"));
		return false;
	}
	int ret = connect(m_sock, (const sockaddr*) & sock_addr,sizeof(sock_addr));
	if (ret == -1) {
		AfxMessageBox(_T("����ʧ�ܣ�"));
		TRACE(_T("����ʧ�ܣ� %d %s\r\n"),WSAGetLastError(), getErrorInfo(WSAGetLastError()));
		return false;
	}
	return true;
}

//�������
int CClientSocket::dealRecv() {
	if (!m_instance) return -1;
	//���建����
	char* buffer = m_buffer.data();
	//char* buffer = new char[BUFFER_SIZE];
	//memset(buffer, 0, BUFFER_SIZE);
	
	//��̬index�����ں�vector<char> m_bufferһ�����������
	static size_t index = 0;
	while (1) {
		//TODO: ճ������
		size_t ret = recv(m_sock, buffer + index, BUFFER_SIZE - index, 0);
		
		if (ret <= 0 && index <= 0) return -1;
		//���ܶ�� recv����˱����� +=
		index += ret;
		ret = index;
		m_packet = CPacket((BYTE*)buffer, ret);
		TRACE(_T("[INFO]RUN dealRecv() : %d\r\n"),ret);
		//����������ѭ���ظ���ȡx
		if (ret == 0) continue;
		//��ȡһ��������������Ϣɾ��
		//memmove �ƶ����ݣ��ڴ��غϽ�����������غϴ�ǰ�����ƶ����غ�ʱ�Ӻ���ǰ�ƶ���i=n;dest[i - 1] = src[i - 1];i--��
		memmove(buffer, buffer + ret, index - ret);
		//TRACE(_T("recv buffer remind : %s\r\n"), m_buffer.size());
		index -= ret;
		return m_packet.sCmd;
	}
	return -1;
}

//������
int CClientSocket::dealSend(const char* pData, int nSize) {
	if (!m_instance) return -1;
	TRACE(_T("client ready send!!\r\n"));
	return send(m_sock, pData, nSize, 0) > 0;
}

//-----------------------------------------------------------------------------------------------------------
//���ݻ������ͳ��ȣ���ʼ����
CPacket::CPacket(const BYTE* pData, size_t& nSize) {
	//���������buffer�ͳ��ȣ�������
	// ��ʼ��
	sHead = 0;
	nLength = 0;
	sCmd = 0;
	sSum = 0;
	//��������
	if (nSize < 2 + 4 + 2 + 2) {
		nSize = 0;
		return;
	}
	size_t i = 0;
	for (; i < nSize; i++) {
		if (0xFEFF == *(WORD*)(pData + i)) {
			//�ҵ���ͷ
			sHead = *(WORD*)(pData + i);
			i += 2;
			break;
		}
	}
	//��������
	if (i + 4 + 2 + 2 > nSize) {
		//���ض�ȡ0���ֽ�
		nSize = 0;
		return;
	}
	nLength = *(DWORD*)(pData + i); i += 4;
	//��������
	if (i + nLength > nSize) {
		nSize = 0;
		return;
	}
	sCmd = *(WORD*)(pData + i); i += 2;
	strData.resize(nLength - 2 - 2);
	//�����ݣ������
	if (nLength > 4) {
		memcpy((void*)strData.c_str(), pData + i, nLength - 2 - 2);
		i += nLength - 2 - 2;
	}
	//У���
	sSum = *(WORD*)(pData + i);
	WORD tmp_sum = 0;
	for (size_t j = 0; j < strData.size(); j++) {
		tmp_sum += BYTE(strData[j]) & 0xFF;
	}
	//������
	if (tmp_sum != sSum) {
		nSize = 0;
		return;
	}
	//nSize��ʾʵ�ʴ�����pData���ĸ�λ��
	nSize = i;
}

//���ݴ��
CPacket::CPacket(WORD sCmd, BYTE* pData, size_t nSize) {
	sHead = 0XFEFF;
	nLength = 2 + 2 + nSize;
	this->sCmd = sCmd;
	strData.resize(nSize);
	memcpy((void*)strData.c_str(), pData, nSize);
	sSum = 0;
	for (size_t i = 0; i < strData.size(); i++) {
		sSum += BYTE(strData[i]) & 0xFF;
	}
}

//�������(ͷ + ���� + ���� + ���� + У���)
size_t CPacket::size() {
	return 6 + nLength;
}

//��ȡ���������ݵ�ַ
const char* CPacket::data() {
	strOut.resize(this->size());
	BYTE* pData = (BYTE*)strOut.c_str();
	*(WORD*)pData = sHead; pData += 2;
	*(DWORD*)pData = nLength; pData += 4;
	*(WORD*)pData = sCmd; pData += 2;
	memcpy(pData, strData.c_str(), strData.size());
	pData += strData.size();
	*(WORD*)pData = sSum;
	return strOut.c_str();
}

//��ӡ������
void CPacket::showPacket() {
	std::string res = "";
	for (size_t i = 0; i < this->size(); i++) {
		char buf[8] = "";
		if (i > 0 && i % 16 == 0) res += "\n";
		snprintf(buf, sizeof(buf), "%02X ", strData[i] & 0xFF);
		res += buf;
	}
	res += "\n";
	OutputDebugStringA(res.c_str());
}
