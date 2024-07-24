#include "pch.h"
#include "FileInfo.h"
#include "ServerSocket.h"
#include <direct.h>
#include <io.h>
#include "Protocol.h"
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
    WORD sCmd = static_cast<WORD>(CProtocol::event::DISK_DRVIE_INFO);

    //���������Ϣ
    CPacket packet(sCmd, (BYTE*)result.c_str(), result.size());
    //packet.showPacket();
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
}

//��ȡ�ļ�Ŀ¼��Ϣ //TODO:��ʾ����·��������
int CFileInfo::getDirectoryInfo() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //û��ִ���ļ�����cmd
        OutputDebugString(_T("��ǰ������ǲ����ļ��б��������ʧ�ܣ�"));
        return -1;
    }
    WORD sCmd = static_cast<WORD>(CProtocol::event::DIR_INFO);
    if (_chdir(filePath.c_str()) != 0) {
        //�л�·��ʧ��
        FILEINFO finfo;
        finfo.hasNext = false;
        finfo.isInvalid = true;
        finfo.isDirectory = true;
        memcpy(finfo.filename, filePath.c_str(), filePath.size());
        //�����ļ�
        CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
        //packet.showPacket();
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: �ļ�����ʧ�ܴ���
        OutputDebugString(_T("��ǰ·��û�з���Ȩ�ޣ��л�·��ʧ�ܣ�"));
        return -2;
    }
    _finddata_t fdata;
    int hfind=0;
    if ((hfind = _findfirst("*.*", &fdata)) == -1) {
        //�����ļ�ʧ��
        OutputDebugString(_T("û���ҵ��ļ���"));
        return -3;
    }
    do {
        FILEINFO finfo;
        memcpy(finfo.filename, fdata.name, strlen(fdata.name));
        finfo.isDirectory = (fdata.attrib & _A_SUBDIR) == 1;
        //�����ļ�
        CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
        //packet.showPacket();
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: �ļ�����ʧ�ܴ���
    } while (!_findnext(hfind, &fdata));
    //�����ļ�����
    FILEINFO finfo;
    finfo.isDirectory = false;
    finfo.hasNext = false;
    //�����ļ�
    CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
    //packet.showPacket();
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
    //TODO: �ļ�����ʧ�ܴ���

    //�رղ��Ҿ��
    _findclose(hfind);
    return 1;
}

//�����ļ�
int CFileInfo::runFile() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //û��ִ���ļ�����cmd
        OutputDebugString(_T("��ǰ������������ļ����������ʧ�ܣ�"));
        return -1;
    }
    //ִ���ļ�
    HINSTANCE result = ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    // ��鷵��ֵ
    if ((int)result <= 32) {
        OutputDebugString(_T("Error: Failed to open file"));
    }

    return 0;
}

//�����ļ�
int CFileInfo::downloadFile() {
    //��ȡ·��
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //û��ִ���ļ�����cmd
        OutputDebugString(_T("��ǰ������������ļ����������ʧ�ܣ�"));
        return -1;
    }
    //���ļ�
    FILE* file = NULL;
    errno_t err = fopen_s(&file,filePath.c_str(), "rb");
    long long dataLen = 0;
    WORD sCmd = static_cast<WORD>(CProtocol::event::DOWN_FILE);
    if (err != 0 || !file) {
        //��ȡʧ�ܣ��ظ������ݰ�
        CPacket packet(sCmd,(BYTE*)&dataLen,8);
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: ����ʧ��
        OutputDebugString(_T("���ļ�ʧ�ܣ�"));
        return -2;
    }

    fseek(file, 0, SEEK_END);
    dataLen = _ftelli64(file);
    fseek(file, 0, SEEK_SET);
 
    //��������ͷ
    CPacket head(sCmd, (BYTE*)dataLen, 8);
    CServerSocket::getInstance()->dealSend(head.data(), head.size());
    char buffer[1024] = "";
    size_t ret = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        ret = fread(buffer,1,sizeof(buffer),file);
        CPacket packet(sCmd, (BYTE*)buffer, ret);
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: ����ʧ��
    } while (ret >= 1024);

    //���ͽ�����־λ
    CPacket end(sCmd, NULL, 0);
    CServerSocket::getInstance()->dealSend(end.data(), end.size());

    fclose(file);
    return 0;
}
