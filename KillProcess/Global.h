#pragma once

extern void CreateDir(char* Path);
extern void GetCurrentDate(char *return_Date);
extern void LSITS_Write_ProgramLogFile(char *format, ...);
extern void LSITS_Write_ErrorLogFile(char *format, ...);
extern void LSITS_WriteExceptionFile(TCHAR *filename, DWORD line, DWORD dwError);

class CGlobal
{
public:
	CGlobal(void);
	~CGlobal(void);
	static TCHAR Program_PATH[MAX_PATH];           // �� ���α׷��� ���� ���� ���͸� PATH.
	static TCHAR PROGRAM_INI_FULLPATH[MAX_PATH];   // �� ���α׷����� ����� INI���Ͽ� ���� full path�� ������ �ִ� ����.
	static int REALTIME_MON_PROC_COUNT;				//�ǽð� ����͸� ���μ��� ī��Ʈ
	static TCHAR srcUpdatePath[MAX_PATH];			/// ������Ʈ�� �ҽ� ����+filename
	static TCHAR dstUpdatePath[MAX_PATH];			/// ������Ʋ�� ���� ����

	static BOOL GetPath(TCHAR *src, TCHAR *dst);
};

const static TCHAR *PROGRAM_VERSION =  _T("VER 1.00.20201229_1");

