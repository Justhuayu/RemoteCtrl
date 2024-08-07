#pragma once
class CFileInfo
{
public:
	int getDiskDriveInfo();//��ȡ���̷�����Ϣ
	int getDirectoryInfo();//��ȡ�ļ�Ŀ¼��Ϣ
	int runFile();//�����ļ�
	int deleteLocalFile();//ɾ�������ļ�
	int downloadFile();//�����ļ�
public:
	typedef struct _file_info{
		_file_info() {
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

