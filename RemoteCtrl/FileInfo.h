#pragma once
class CFileInfo
{
public:
	int getDiskDriveInfo();//获取磁盘分区信息
	int getDirectoryInfo();//获取文件目录信息
	int runFile();//运行文件
	int deleteLocalFile();//删除本地文件
	int downloadFile();//下载文件
public:
	typedef struct _file_info{
		_file_info() {
			isInvalid = false;
			hasNext = true;
			isDirectory = -1;
			memset(filename, 0, sizeof(filename));
		}
		BOOL isInvalid;//文件无效 0 有效，1 无效
		BOOL isDirectory;//是否为目录 0 文件，1 目录
		BOOL hasNext;//是否有下一个文件，用于文件传输结束
		char filename[256];//存放文件名
	}FILEINFO,*PFILEINFO;
};

