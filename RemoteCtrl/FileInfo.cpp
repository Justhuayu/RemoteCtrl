#include "pch.h"
#include "FileInfo.h"
#include "ServerSocket.h"
#include <direct.h>
#include <io.h>
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

//��ȡ�ļ�Ŀ¼��Ϣ
int CFileInfo::getDirectoryInfo() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //û��ִ���ļ�����cmd
        OutputDebugString(_T("��ǰ������ǲ����ļ��б��������ʧ�ܣ�"));
        return -1;
    }
    if (_chdir(filePath.c_str()) != 0) {
        //�л�·��ʧ��
        file_info finfo;
        finfo.hasNext = false;
        finfo.isInvalid = true;
        finfo.isDirectory = true;
        memcpy(finfo.filename, filePath.c_str(), filePath.size());
        //�����ļ�
        //TODO:�ļ�ö��
        CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: �ļ�����ʧ�ܴ���
        OutputDebugString(_T("��ǰ·��û�з���Ȩ�ޣ��л�·��ʧ�ܣ�"));
        return -2;
    }
    _finddata_t fdata;
    int hfind=0;
    if ((hfind = _findfirst("*", &fdata)) == -1) {
        //�����ļ�ʧ��
        OutputDebugString(_T("û���ҵ��ļ���"));
        return -3;
    }
    do {
        file_info finfo;
        memcpy(finfo.filename, &fdata.name, fdata.size);
        finfo.isDirectory = (fdata.attrib & _A_SUBDIR) == 1;
        //�����ļ�
        //TODO:�ļ�ö��
        CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        //TODO: �ļ�����ʧ�ܴ���
    } while (_findnext(hfind, &fdata) != -1);
    //�����ļ�����
    file_info finfo;
    finfo.isDirectory = false;
    finfo.hasNext = false;
    //�����ļ�
    //TODO:�ļ�ö��
    CPacket packet(2, (BYTE*)&finfo, sizeof(finfo));
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
    //TODO: �ļ�����ʧ�ܴ���

    return 1;
}