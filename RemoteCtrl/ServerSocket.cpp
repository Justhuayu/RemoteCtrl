#include "pch.h"
#include "ServerSocket.h"

#define PORT 9527 //服务端socket端口号
#define BUFFER_SIZE 4096 //recv buffer大小

//静态变量定义
CServerSocket* CServerSocket::m_instance = nullptr;
CServerSocket::CServerHelp CServerSocket::m_help;

//初始化socket连接
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

//创建新客户端连接
bool CServerSocket::acceptClient() {
	if (!m_instance) return false;
	if (m_sock == INVALID_SOCKET) return false;
	sockaddr_in client_sock;
	int client_length = sizeof(client_sock);
	m_client_sock = accept(m_sock, (sockaddr*)&client_sock, &client_length);
	if (m_client_sock == INVALID_SOCKET) return false;
	return true;
}

//处理接收
int CServerSocket::dealRecv() {
	if (!m_instance) return -1;
	if (m_client_sock == INVALID_SOCKET) return -1;
	//定义缓冲区
	char* buffer = new char[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	size_t index = 0;
	while (1) {
		size_t ret = recv(m_client_sock,buffer+index,BUFFER_SIZE - index,0);
		if (ret < 0) return -1;
		else if (ret == 0) return 0;//断开连接
		//可能多次 recv，因此必须是 +=
		index += ret;
		ret = index;
		m_packet = CPacket((BYTE*)buffer, ret);
		//包不完整，循环重复读取
		if (index == 0) continue;
		//读取一个包，缓冲区消息删除
		//memmove 移动数据，内存重合解决方法：不重合从前往后移动，重合时从后往前移动（i=n;dest[i - 1] = src[i - 1];i--）
		memmove(buffer, buffer + ret,BUFFER_SIZE - ret);
		index -= ret;
		return m_packet.sCmd;
	}
	return 0;
}

//处理发送
int CServerSocket::dealSend(const char* pData,int nSize) {
	if (!m_instance) return -1;
	if (m_client_sock == INVALID_SOCKET) return -1;

	return send(m_client_sock, pData, nSize, 0) > 0;
}

//获取文件路径
bool CServerSocket::getFilePath(std::string& strPath) {
	//TODO:文件处理枚举
	if (this->m_packet.sCmd != 2) return false;
	strPath = m_packet.strData;
	return true;
}


//-----------------------------------------------------------------------------------------------------------
//根据缓冲区和长度，初始化包
CPacket::CPacket(const BYTE* pData, size_t& nSize) {
	//根据输入的buffer和长度，构建包
	//包不完整
	if (nSize < 2 + 4 + 2 + 2) return;
	size_t i = 0;
	for (; i < nSize; i++) {
		if (0xFEFF == *(WORD*)(pData + i)) {
			//找到包头
			sHead = *(WORD*)(pData + i);
			i += 2;
			break;
		}
	}
	//包不完整
	if (i + 4 + 2 + 2 > nSize) {
		//返回读取0个字节
		nSize = 0;
		return;
	}
	nLength = *(DWORD*)(pData + i); i += 4;
	//包不完整
	if (i + nLength > nSize) {
		nSize = 0;
		return;
	}
	sCmd = *(WORD*)(pData + i); i += 2;
	strData.resize(nLength - 2 - 2);
	//读数据，如果有
	if (nLength > 4) {
		memcpy((void*)strData.c_str(), pData + i, nLength - 2 - 2);
		i += nLength - 2 - 2;
	}
	//校验和
	sSum = *(WORD*)(pData + i);
	WORD tmp_sum = 0;
	for (size_t j = 0; j < strData.size(); j++) {
		tmp_sum += BYTE(strData[j]) & 0xFF;
	}
	//包错误
	if (tmp_sum != sSum) {
		nSize = 0;
		return;
	}
	//nSize表示实际处理到了pData的哪个位置
	nSize = i;
}

//数据打包
CPacket::CPacket(WORD sCmd, BYTE* pData, size_t nSize) {
	sHead = 0XFEFF;
	nLength = 2 + 2 + nSize;
	this->sCmd = sCmd;
	strData.resize(nSize);
	memcpy((void*)strData.c_str(), pData, nSize);
	sSum = 0;
	for (int i = 0; i < strData.size(); i++) {
		sSum += BYTE(strData[i]) & 0xFF;
	}
}

//求包长度(头 + 长度 + 命令 + 数据 + 校验和)
size_t CPacket::size() {
	return 6 + nLength;
}

//获取连续的数据地址
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

//打印包内容
void CPacket::showPacket(BYTE* pData, size_t nSize) {
	std::string res = "";
	for (int i = 0; i < nSize; i++) {
		char buf[8] = "";
		if (i > 0 && i % 16 == 0) res += "\n";
		snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xFF);
		res += buf;
	}
	res += "\n";
	OutputDebugStringA(res.c_str());
}
