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
            WORD sCmd = 7;
            CFileInfo fInfo;
            CMachineCtrl mCtrl;
            switch (static_cast<CProtocol::event>(sCmd)) {
            case CProtocol::event::DISK_DRVIE_INFO:
            {
                //获取磁盘分区信息
                fInfo.getDiskDriveInfo();
                break;
            }
            case CProtocol::event::DIR_INFO:
                //获取文件目录信息
                fInfo.getDirectoryInfo();
                break;
            case CProtocol::event::RUN_FILE:
                //运行文件
                fInfo.runFile();
                break;
            case CProtocol::event::DOWN_FILE:
                //下载文件
                fInfo.downloadFile();
                break;
            case CProtocol::event::MOUSE_CTRL:
                //鼠标操作
                mCtrl.mouseEvent();
                break;
            case CProtocol::event::SCREEN_SEND:
                //屏幕监控
                mCtrl.screenSend();
                break;
            case CProtocol::event::LOCK_MACHINE:
                //锁机
                mCtrl.lockMachine();
                Sleep(50);
                mCtrl.lockMachine();
                break;
            case CProtocol::event::UNLOCK_MACHINE:
                //解锁
                mCtrl.unlockMachine();
                break;
            default:
                break;
            }
            Sleep(1000);
            mCtrl.unlockMachine();
            while (dlg.m_hWnd != NULL && dlg.m_hWnd != INVALID_HANDLE_VALUE) Sleep(50);
            TRACE(_T("m_hWnd %d\r\n",dlg.m_hWnd));
            // TODO: 在此处为应用程序的行为编写代码。
            //套接字初始化
            //CServerSocket* server = CServerSocket::getInstance();
            //if (!server->acceptClient()) return false;
            //while (CServerSocket::getInstance()) {
            //    //server->dealRecv();
            //    //处理文件
            //    WORD sCmd = 2;
            //    CFileInfo fInfo;
            //    switch (sCmd) {
            //    case 1:
            //    {
            //        //获取磁盘分区信息
            //        fInfo.getDiskDriveInfo();
            //        break;
            //    }
            //    case 2:
            //        //获取文件目录信息
            //        fInfo.getDirectoryInfo();
            //        break;
            //    default:
            //        break;
            //    }
            //}
            
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
