// RemoteCtrl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include <direct.h>

#include "RemoteCtrl.h"
#include "ServerSocket.h"
#include "FileInfo.h"
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
            // TODO: 在此处为应用程序的行为编写代码。
            //套接字初始化
            CServerSocket* server = CServerSocket::getInstance();
            if (!server->acceptClient()) return false;
            while (CServerSocket::getInstance()) {
                server->dealRecv();
                //处理文件
                WORD sCmd = 1;
                CFileInfo fInfo;
                switch (sCmd) {
                case 1:
                {
                    //获取磁盘分区信息
                    fInfo.getDiskDriveInfo();
                    break;
                }
                default:
                    break;
                }
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
