#include "pch.h"
#include "FileInfo.h"
#include "ServerSocket.h"
#include <direct.h>
#include <io.h>
#include "Protocol.h"
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
    WORD sCmd = static_cast<WORD>(CProtocol::event::DISK_DRVIE_INFO);

    //打包分区信息
    CPacket packet(sCmd, (BYTE*)result.c_str(), result.size());
    //packet.showPacket();
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
}

//获取文件目录信息 //TODO:显示本地路径有问题
int CFileInfo::getDirectoryInfo() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //没有执行文件查找cmd
        OutputDebugString(_T("当前命令，不是查找文件列表，命令解析失败！"));
        return -1;
    }
    WORD sCmd = static_cast<WORD>(CProtocol::event::DIR_INFO);
    if (_chdir(filePath.c_str()) != 0) {
        //切换路径失败
        FILEINFO finfo;
        finfo.hasNext = false;
        finfo.isInvalid = true;
        finfo.isDirectory = true;
        memcpy(finfo.filename, filePath.c_str(), filePath.size());
        //发送文件
        CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
        //packet.showPacket();
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 文件发送失败处理
        OutputDebugString(_T("当前路径没有访问权限，切换路径失败！"));
        return -2;
    }
    _finddata_t fdata;
    int hfind=0;
    if ((hfind = _findfirst("*.*", &fdata)) == -1) {
        //查找文件失败
        OutputDebugString(_T("没有找到文件！"));
        return -3;
    }
    do {
        FILEINFO finfo;
        memcpy(finfo.filename, fdata.name, strlen(fdata.name));
        finfo.isDirectory = (fdata.attrib & _A_SUBDIR) == 1;
        //发送文件
        CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
        //packet.showPacket();
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 文件发送失败处理
    } while (!_findnext(hfind, &fdata));
    //发送文件结束
    FILEINFO finfo;
    finfo.isDirectory = false;
    finfo.hasNext = false;
    //发送文件
    CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
    //packet.showPacket();
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
    //TODO: 文件发送失败处理

    //关闭查找句柄
    _findclose(hfind);
    return 1;
}

//运行文件
int CFileInfo::runFile() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //没有执行文件查找cmd
        OutputDebugString(_T("当前命令，不是运行文件，命令解析失败！"));
        return -1;
    }
    //执行文件
    HINSTANCE result = ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    // 检查返回值
    if ((int)result <= 32) {
        OutputDebugString(_T("Error: Failed to open file"));
    }

    return 0;
}

//下载文件
int CFileInfo::downloadFile() {
    //获取路径
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //没有执行文件查找cmd
        OutputDebugString(_T("当前命令，不是运行文件，命令解析失败！"));
        return -1;
    }
    //打开文件
    FILE* file = NULL;
    errno_t err = fopen_s(&file,filePath.c_str(), "rb");
    long long dataLen = 0;
    WORD sCmd = static_cast<WORD>(CProtocol::event::DOWN_FILE);
    if (err != 0 || !file) {
        //读取失败，回复空数据包
        CPacket packet(sCmd,(BYTE*)&dataLen,8);
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 发送失败
        OutputDebugString(_T("打开文件失败！"));
        return -2;
    }

    fseek(file, 0, SEEK_END);
    dataLen = _ftelli64(file);
    fseek(file, 0, SEEK_SET);
 
    //发送数据头
    CPacket head(sCmd, (BYTE*)dataLen, 8);
    CServerSocket::getInstance()->dealSend(head.data(), head.size());
    char buffer[1024] = "";
    size_t ret = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        ret = fread(buffer,1,sizeof(buffer),file);
        CPacket packet(sCmd, (BYTE*)buffer, ret);
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: 发送失败
    } while (ret >= 1024);

    //发送结束标志位
    CPacket end(sCmd, NULL, 0);
    CServerSocket::getInstance()->dealSend(end.data(), end.size());

    fclose(file);
    return 0;
}
