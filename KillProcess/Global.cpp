#include "StdAfx.h"
#include "Global.h"

/// 이 프로그램의 실행 파일 디렉터리 PATH.
TCHAR CGlobal::Program_PATH[] = {0,};
TCHAR CGlobal::PROGRAM_INI_FULLPATH[]= {0,0};   // 이 프로그램에서 사용할 INI파일에 대한 full path를 가지고 있는 변수.
int CGlobal::REALTIME_MON_PROC_COUNT = 0;

TCHAR CGlobal::srcUpdatePath[MAX_PATH] = {0,};			/// 업데이트할 소스 폴더+filename
TCHAR CGlobal::dstUpdatePath[MAX_PATH] = {0,};			/// 업데이틀할 목적 폴더

UINT CGlobal::INI_LOGFILE_SAVE_INTERVAL = 0;

void CreateDir(char* Path);
void GetCurrentDate(char *return_Date);
void LSITS_Write_ProgramLogFile(char *format, ...);

CGlobal::CGlobal(void)
{
}


CGlobal::~CGlobal(void)
{
}
/**
* @author 윤경섭
* @file Global.cpp
* @fn BOOL CGlobal::GetPath(TCHAR *src, TCHAR *dst)
* @param TCHAR *src
* @param TCHAR *dst
* @remark 소스 디렉토리에서 디렉토리만 뽑아 내는 함수.
* @return BOOL
* @date 20/03/05
*/
BOOL CGlobal::GetPath(TCHAR *src, TCHAR *dst)
{
	BOOL status = FALSE;

	TCHAR *p = strrchr(src, '\\');
	if(p != NULL)
	{
		int slen = p - src;
		memcpy(dst,src, slen);
		dst[slen] = 0;
		status = TRUE; 
	}

	return status;
}


/**
* @author 윤경섭
* @file MngType.cpp
* @fn void CreateDir(char* Path)
* @param char* Path
* @remark Path를 입력으로 주면 해당 디렉토리를 생성한다.
* @return void 없음
* @date 20/02/19
*/
void CreateDir(char* Path)

{

	char DirName[256];  //생성할 디렉초리 이름

	char* p = Path;     //인자로 받은 디렉토리

	char* q = DirName;  



	while(*p)

	{

		if (('\\' == *p) || ('/' == *p))   //루트디렉토리 혹은 Sub디렉토리

		{

			if (':' != *(p-1) && (!PathIsDirectory(DirName)))

			{

				CreateDirectory(DirName, NULL);

			}

		}

		*q++ = *p++;

		*q = '\0';

	}

	CreateDirectory(DirName, NULL);  

}

// 현재 날짜에 대한 YYYYMMDD 결과를 전달한다.
void GetCurrentDate(char *return_Date)
{
	if(return_Date == NULL)
		return;

	// 현재 날짜와 시간을 이용하여 String을 생성한다.
	time_t  now_time;
	struct tm *today;

	time(&now_time);
	today = localtime(&now_time);
	strftime(return_Date, 10, "%Y%m%d", today);
}

CCriticalSection CriticalSection_LogFile;

void LSITS_Write_LogFile(int param_KindOfLog, char *format, ...) {

  CriticalSection_LogFile.Lock();
    char TempPath[MAX_PATH];
  //----------------------------------------------------------------------------
  char TempDate[20];

  GetCurrentDate(TempDate);

  _stprintf_s(TempPath, _countof(TempPath), _T("%s%s\\%s"), CGlobal::Program_PATH, _T("log"),TempDate);
  if(!PathIsDirectory(TempPath))
  {
	  CreateDir(TempPath);
  }


  switch(param_KindOfLog) {
    case LOG_PROG:
      sprintf(TempPath, "%sLog\\%s\\log_%s.log", CGlobal::Program_PATH, TempDate,TempDate);
      break;
	case LOG_ERROR      :
		sprintf(TempPath, "%sLog\\%s\\error_%s.log", CGlobal::Program_PATH, TempDate,TempDate);
		break;
    
    default :
      CriticalSection_LogFile.Unlock();
      return;
  } // switch statement...

  if(format == NULL) {
    CriticalSection_LogFile.Unlock();
    return;
  }

  //----------------------------------------------------------------------------
	FILE *openLogFile;
	va_list var_args;

	time_t  now_time;
	struct tm *today;
	char szTimeStamp[50];

	time(&now_time);
	today = localtime(&now_time);
	strftime(szTimeStamp, 30, "%Y-%m-%d %H:%M:%S", today);

	// 1. 현재의 log파일을 open한다.(append mode로 open, 파일이 없으면 새로 생성)
  if( (openLogFile = fopen(TempPath, "ab")) == NULL ) {
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath(TempPath, drive, dir, fname, ext);

    // 디렉터리를 생성해준다.
    char TmpPath[MAX_PATH];
    sprintf(TmpPath, "%s%s", drive, dir);
    TmpPath[strlen(TmpPath)-1] = NULL;
    //CreateDirectory(TmpPath, NULL);
	 CreateDir(TempPath);

    if( (openLogFile = fopen(TempPath, "ab")) == NULL ) {
		CriticalSection_LogFile.Unlock();
		  return;
    }
  }

	// 2. 파일에 정보를 추가한다.
	va_start( var_args, format);     // Initialize variable arguments.

  fprintf( openLogFile, "[%s] ", szTimeStamp);
	vfprintf( openLogFile, format, var_args);

	va_end(var_args);              // Reset variable arguments.

	// 3. open한 파일을 close.
	fclose(openLogFile);

  CriticalSection_LogFile.Unlock();
}

void LSITS_WriteExceptionFile(TCHAR *filename, DWORD line, DWORD dwError)
{
	HLOCAL hlocal = NULL;	
	BOOL fOk = ::FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(0, SUBLANG_ENGLISH_US),
		(PTSTR)&hlocal,0,NULL);	
	if(!fOk)
	{
		HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"),NULL,DONT_RESOLVE_DLL_REFERENCES);
		if(hDll !=NULL){	
			::FormatMessage(
				FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM,
				hDll, dwError, MAKELANGID(0,SUBLANG_ENGLISH_US),
				(PTSTR)&hlocal,0,NULL);
			FreeLibrary(hDll);
		}
	}
	LSITS_Write_ErrorLogFile("File: %s Line: %d Error Code: %d\r\n",filename,line,dwError);
	if(hlocal != NULL)
	{
		LSITS_Write_ErrorLogFile((char*)LocalLock(hlocal),"\r\n");
		LocalFree(hlocal);  //메모리 해제

	}
	else
	{
		LSITS_Write_ErrorLogFile(_T("Error number not found\r\n"));
	}
}

CCriticalSection CriticalSection_ErrorLogFile;

void LSITS_Write_ErrorLogFile(char *format, ...) {

	CriticalSection_ErrorLogFile.Lock();


	//----------------------------------------------------------------------------
	char TempDate[20];

	GetCurrentDate(TempDate);

	char TempPath[MAX_PATH];
	TCHAR fileFullPath[MAX_PATH];

	_stprintf_s(TempPath, _countof(TempPath), _T("%s%s"), CGlobal::Program_PATH, _T("log"));
	if(!PathIsDirectory(TempPath))
	{
		CreateDir(TempPath);
	}

	sprintf(fileFullPath, "%s\\error_%s.log",TempPath,TempDate,TempDate);

	if(format == NULL) {
		CriticalSection_ErrorLogFile.Unlock();
		return;
	}

	//----------------------------------------------------------------------------
	FILE *openLogFile;
	va_list var_args;

	time_t  now_time;
	struct tm *today;
	char szTimeStamp[50];

	time(&now_time);
	today = localtime(&now_time);
	strftime(szTimeStamp, 30, "%Y-%m-%d %H:%M:%S", today);

	// 1. 현재의 log파일을 open한다.(append mode로 open, 파일이 없으면 새로 생성)
	if( (openLogFile = fopen(fileFullPath, "ab")) == NULL ) {
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(fileFullPath, drive, dir, fname, ext);

		// 디렉터리를 생성해준다.
		char TmpPath[MAX_PATH];
		sprintf(TmpPath, "%s%s", drive, dir);
		TmpPath[strlen(TmpPath)-1] = NULL;
		CreateDir(TmpPath);

		if( (openLogFile = fopen(fileFullPath, "ab")) == NULL ) {
			CriticalSection_ErrorLogFile.Unlock();
			return;
		}
	}

	// 2. 파일에 정보를 추가한다.
	va_start( var_args, format);     // Initialize variable arguments.

	fprintf( openLogFile, "[%s] ", szTimeStamp);
	vfprintf( openLogFile, format, var_args);

	va_end(var_args);              // Reset variable arguments.

	// 3. open한 파일을 close.
	fclose(openLogFile);

	CriticalSection_ErrorLogFile.Unlock();
}

/**
* @author 윤경섭
* @file MngType.cpp
* @fn BOOL DeleteDir(CString dir)
* @param CString dir
* @remark Path를 입력으로 주면 해당 디렉토리를 삭제 한다.
* @details 디렉토리내에 존재하는 하위 폴더 및 모든 파일 삭제 
함수 사용예: DeleteDir(_T("C:\\dir_name\\*.*"));
* @return BOOL 성공 여부
* @date 20/02/19
*/
BOOL DeleteDir(CString dir)
{
	if(dir == _T(""))
	{
		return FALSE;
	}

	BOOL bRval = FALSE;
	int nRval = 0;
	CString szNextDirPath = _T("");
	CString szRoot = _T("");
	CFileFind find;
	TCHAR strRoot[MAX_PATH];

	try
	{
		// Directory가 존재 하는지 확인 검사
		bRval = find.FindFile(dir);
		Sleep(100);
		if(bRval == FALSE)
		{
			return bRval;
		}

		while(bRval)
		{
			Sleep(100);
			bRval = find.FindNextFile();
			// . or .. 인 경우 무시한다.
			if(find.IsDots() == TRUE)
			{
				continue;
			}

			// Directory 일 경우
			if(find.IsDirectory())
			{
				szNextDirPath.Format(_T("%s\\*.*"), find.GetFilePath());

				// Recursion function 호출
				Sleep(100);
				//_stprintf_s(TemDir,_countof(TemDir),_T("디렉토리를 삭제합니다->\\%s\r\n"),szNextDirPath);
				//ProgramLog(TemDir,LOG_PROG);
				DeleteDir(szNextDirPath);
			}

			// file일 경우
			else
			{
				//파일 삭제
				Sleep(100);
				//_stprintf_s(TemDir,_countof(TemDir),_T("file->\\%s\r\n"),find.GetFilePath());
				//ProgramLog(TemDir,LOG_PROG);
				::DeleteFile(find.GetFilePath());

			}
		}

		szRoot = find.GetRoot();
		find.Close();

		Sleep(100);
		_stprintf_s(strRoot,_countof(strRoot),_T("%s"),szRoot);
		int len = _tcslen(strRoot);
		if(strRoot[len -1] == '\\')
		{
			strRoot[len - 1] = 0x00;
		}

		//_stprintf_s(TemDir,_countof(TemDir),_T("root->\\%s\r\n"),strRoot);
		//ProgramLog(TemDir,LOG_PROG);


		if(PathIsDirectory(strRoot))
		{
			bRval = RemoveDirectory(strRoot);
		}

	}
	catch (CMemoryException* e)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
		TCHAR szException[1024] = {0, };
		e->GetErrorMessage(szException,sizeof(szException));
		LSITS_Write_ErrorLogFile(szException);
	}
	catch (CException* e)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
		TCHAR szException[1024] = {0, };
		e->GetErrorMessage(szException,sizeof(szException));
		LSITS_Write_ErrorLogFile(szException);
	}
	catch (...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	//_stprintf_s(TemDir,_countof(TemDir),_T("out->\\%s\r\n"),strRoot);
	//ProgramLog(TemDir,LOG_PROG);

	return bRval;
}