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
		BOOL isInvalid;//�ļ���Ч 0 ��Ч��1 ��Ч
		BOOL isDirectory;//�Ƿ�ΪĿ¼ 0 �ļ���1 Ŀ¼
		BOOL hasNext;//�Ƿ�����һ���ļ��������ļ��������
		char filename[256];//����ļ���
	}FILEINFO,*PFILEINFO;
};

