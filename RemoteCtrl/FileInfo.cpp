#include "pch.h"
#include "FileInfo.h"
#include "ServerSocket.h"
#include <direct.h>
#include <io.h>
#include "Protocol.h"
//��ȡ���̷�����Ϣ
int CFileInfo::getDiskDriveInfo() {
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
    return 0;
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
    if ((hfind = _findfirst("*", &fdata)) == -1) {
        //�����ļ�ʧ��
        OutputDebugString(_T("û���ҵ��ļ���"));
        FILEINFO finfo;
        finfo.hasNext = false;
        //�����ļ�
        CPacket packet(sCmd, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        return -3;
    }
    do {
        FILEINFO finfo;
        memcpy(finfo.filename, fdata.name, strlen(fdata.name));
        finfo.isDirectory = (fdata.attrib & _A_SUBDIR) !=0;
        //�����ļ�
        TRACE(_T("send finfo filename = [%s]\r\n"), CString(finfo.filename));
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
    return 0;
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
    //TODO:��������ʧ�ܺ����гɹ�
    WORD sCmd = static_cast<WORD>(CProtocol::event::RUN_FILE);
    CPacket packet(sCmd, NULL, 0);
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
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
    CPacket head(sCmd, (BYTE*)&dataLen, 8);
    CServerSocket::getInstance()->dealSend(head.data(), head.size());
    char buffer[BUFFER_SIZE] = "";
    size_t ret = 0;
    int count = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        ret = fread(buffer,1,sizeof(buffer),file);
        if (ret < 0) {
            // �����ȡ����
            OutputDebugString(_T("��ȡ�ļ�ʱ����"));
            fclose(file);
            return -3;
        }
        if (ret == 0) {
            // �ļ��Ѷ���
            break;
        }
        CPacket packet(sCmd, (BYTE*)buffer, ret);
        int sendRet = CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
        if (sendRet < 0) {
            TRACE(_T("���ݷ���ʧ�ܣ�\n"));
            break;
        }
        count += ret;
        //TODO: ����ʧ��
        TRACE(_T("send file size �� %d ret: %d\r\n"), count,ret);
    } while (ret > 0);
    //���ͽ�����־λ
    CPacket end(sCmd, NULL, 0);
    CServerSocket::getInstance()->dealSend(end.data(), end.size());
    fclose(file);
    TRACE(_T("send file size �� %d\r\n"), count);
    return 0;
}

//ɾ�������ļ�
int CFileInfo::deleteLocalFile() {
    std::string filePath = "";
    if (!CServerSocket::getInstance()->getFilePath(filePath)) {
        //û��ִ���ļ�����cmd
        OutputDebugString(_T("��ǰ�������ɾ���ļ����������ʧ�ܣ�"));
        return -1;
    }
    if (!DeleteFile(filePath.c_str())) {
        OutputDebugString(_T("ɾ���ļ�ʧ�ܣ�"));
        return -2;
    }
    //TODO:����ɾ��ʧ�ܺ�ɾ���ɹ�
    WORD sCmd = static_cast<WORD>(CProtocol::event::DELETE_FILE);
    CPacket packet(sCmd, NULL,0);
    CServerSocket::getInstance()->dealSend(packet.data(), packet.size());
    return 0;
}
