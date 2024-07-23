#pragma once
class CFileInfo
{
public:
	void getDiskDriveInfo();
	int getDirectoryInfo();

public:
	typedef struct file_info{
		file_info() {
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

