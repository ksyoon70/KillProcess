#pragma once

#define LOG_PROG		  1       // 프로그램로그 저장
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
	static TCHAR Program_PATH[MAX_PATH];           // 이 프로그램의 실행 파일 디렉터리 PATH.
	static TCHAR PROGRAM_INI_FULLPATH[MAX_PATH];   // 이 프로그램에서 사용할 INI파일에 대한 full path를 가지고 있는 변수.
	static int REALTIME_MON_PROC_COUNT;				//실시간 모니터링 프로세스 카운트
	static TCHAR srcUpdatePath[MAX_PATH];			/// 업데이트할 소스 폴더+filename
	static TCHAR dstUpdatePath[MAX_PATH];			/// 업데이틀할 목적 폴더

	static UINT INI_LOGFILE_SAVE_INTERVAL;

	static BOOL GetPath(TCHAR *src, TCHAR *dst);
};

const static TCHAR *PROGRAM_VERSION =  _T("VER 1.01.20230111_1");

