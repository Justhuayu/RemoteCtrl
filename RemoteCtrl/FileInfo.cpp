#include "pch.h"
#include "FileInfo.h"
#include <direct.h>
#include "ServerSocket.h"
//��ȡ���̷�����Ϣ
void CFileInfo::getDiskDriveInfo() {
    std::string result = "";
    for (int i = 1; i <= 26; i++) {
        //_chdrive 1--A,2--B,...
        if (_chdrive(i) == 0) {
            if (!result.empty()) result += ',';
            result += 'A' + i - 1;
        }
    }
    //���������Ϣ
    CPacket packet(1, (BYTE*)result.c_str(), result.size());
    packet.showPacket((BYTE*)packet.data(), packet.size());
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
}