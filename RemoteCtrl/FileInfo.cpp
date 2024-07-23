#include "pch.h"
#include "FileInfo.h"
#include "ServerSocket.h"
#include <direct.h>
#include <io.h>
//获取磁盘分区信息
void CFileInfo::getDiskDriveInfo() {
    std::string result = "";
    for (int i = 1; i <= 26; i++) {
        //_chdrive 1--A,2--B,...
        if (_chdrive(i) == 0) {
            if (!result.empty()) result += ',';
            result += 'A' + i - 1;
        }
    }
    //打包分区信息
    CPacket packet(1, (BYTE*)result.c_str(), result.size());
    packet.showPacket((BYTE*)packet.data(), packet.size());
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
}

//获取文件目录信息
int CFileInfo::getDirectoryInfo() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //没有执行文件查找cmd
        OutputDebugString(_T("当前命令，不是查找文件列表，命令解析失败！"));
        return -1;
    }
    if (_chdir(filePath.c_str()) != 0) {
        //切换路径失败
        file_info finfo;
        finfo.hasNext = false;
        finfo.isInvalid = true;
        finfo.isDirectory = true;
        memcpy(finfo.filename, filePath.c_str(), filePath.size());
        //发送文件
        //TODO:文件枚举
        CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 文件发送失败处理
        OutputDebugString(_T("当前路径没有访问权限，切换路径失败！"));
        return -2;
    }
    _finddata_t fdata;
    int hfind=0;
    if ((hfind = _findfirst("*", &fdata)) == -1) {
        //查找文件失败
        OutputDebugString(_T("没有找到文件！"));
        return -3;
    }
    do {
        file_info finfo;
        memcpy(finfo.filename, &fdata.name, fdata.size);
        finfo.isDirectory = (fdata.attrib & _A_SUBDIR) == 1;
        //发送文件
        //TODO:文件枚举
        CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 文件发送失败处理
    } while (_findnext(hfind, &fdata) != -1);
    //发送文件结束
    file_info finfo;
    finfo.isDirectory = false;
    finfo.hasNext = false;
    //发送文件
    //TODO:文件枚举
    CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
    //TODO: 文件发送失败处理

    return 1;
}