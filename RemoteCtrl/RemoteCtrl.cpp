// RemoteCtrl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include <direct.h>
#include "MachineCtrl.h"
#include "RemoteCtrl.h"
#include "ServerSocket.h"
#include "FileInfo.h"
#include "Protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment ( linker, "/subsystem:windows  /entry:WinMainCRTStartup")
//#pragma comment ( linker, "/subsystem:windows  /entry:mainCRTStartup")
//#pragma comment ( linker, "/subsystem: console /entry: mainCRTStartup")
//#pragma comment ( linker, "subsystem: console  /entry:WinMainCRTStartup")

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int executeCmd(const WORD sCmd) {
    int ret{};
    CFileInfo fInfo;
    CMachineCtrl mCtrl;
    switch (static_cast<CProtocol::event>(sCmd)) {
    case CProtocol::event::DISK_DRVIE_INFO:
    {
        //获取磁盘分区信息
        ret = fInfo.getDiskDriveInfo();
        break;
    }
    case CProtocol::event::DIR_INFO:
        //获取文件目录信息
        ret = fInfo.getDirectoryInfo();
        break;
    case CProtocol::event::RUN_FILE:
        //运行文件
        ret = fInfo.runFile();
        break;
    case CProtocol::event::DOWN_FILE:
        //下载文件
        ret = fInfo.downloadFile();
        break;
    case CProtocol::event::DELETE_FILE:
        ret = fInfo.deleteLocalFile();
        break;
    case CProtocol::event::MOUSE_CTRL:
        //鼠标操作
        ret = mCtrl.mouseEvent();
        break;
    case CProtocol::event::SCREEN_SEND:
        //屏幕监控
        ret = mCtrl.screenSend();
        break;
    case CProtocol::event::LOCK_MACHINE:
        //锁机
        ret = mCtrl.lockMachine();
        break;
    case CProtocol::event::UNLOCK_MACHINE:
        //解锁
        ret = mCtrl.unlockMachine();
        break;
    default:
        break;
    }
    return ret;
}
int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
            //套接字初始化
            CServerSocket* server = CServerSocket::getInstance();
            if (!server->initSockEnv()) {
                MessageBox(NULL, _T("网络初始化失败！"), _T("网络初始化失败！"), MB_OK | MB_ICONERROR);
                exit(0);
            }
            int count = 0;
            while (CServerSocket::getInstance() != NULL) {
                if (!server->acceptClient()) {
                    if (count >= 3) {
                        MessageBox(NULL,_T("多次连接用户失败！"),_T("连接用户失败！"),MB_OK|MB_ICONERROR);
                        exit(0);
                    }
                    MessageBox(NULL, _T("连接失败，自动重试！"), _T("连接用户失败！"), MB_OK | MB_ICONERROR);
                    count++;
                }
                int ret = server->dealRecv();
                if (ret < 0) {
                    MessageBox(NULL, _T("接受包失败！"), _T("处理包失败！"), MB_OK | MB_ICONERROR);
                    continue;
                }
                ret = executeCmd(server->getCPacket().sCmd);
                if (ret < 0) {
                    MessageBox(NULL, _T("执行命令失败！"), _T("处理包失败！"), MB_OK | MB_ICONERROR);
                    continue;
                }
                //短连接，连接一次，执行一次命令
                server->closeClient();
            }
        }

        
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
