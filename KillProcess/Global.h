#pragma once

#define LOG_PROG		  1       // ���α׷��α� ����
#define LOG_ERROR		  2

extern void CreateDir(char* Path);
extern void GetCurrentDate(char *return_Date);
extern void LSITS_Write_LogFile(int param_KindOfLog, char *format, ...);
extern void LSITS_Write_ErrorLogFile(char *format, ...);
extern void LSITS_WriteExceptionFile(TCHAR *filename, DWORD line, DWORD dwError);
extern BOOL DeleteDir(CString dir);

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

	static UINT INI_LOGFILE_SAVE_INTERVAL;

	static BOOL GetPath(TCHAR *src, TCHAR *dst);
};

const static TCHAR *PROGRAM_VERSION =  _T("VER 1.01.20230111_1");

