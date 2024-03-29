
// KillProcessDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "KillProcess.h"
#include "KillProcessDlg.h"
#include "afxdialogex.h"
#include <psapi.h>
#include <TLHELP32.H>
#include "Global.h"
#include <excpt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PROC_NAME_WIDTH				(150)
#define DEFAULT_UPDATE_TIME					(5)
#define TIMER_NUM					(1)
#define DEFAULT_WAIT_TIME			(30)			//기본 대기 시간
#define DEFAULT_WAITUP_TIME			(30)			//기본 재기동 대기 시간		

#define	CHECK_INDEX					(0)
#define FILE_NAME_INDEX				(1)
#define WAIT_TIME_INDEX				(2)
#define WAKEUP_TIME_INDEX			(3)				//재 기동 할 시간
#define PATH_NAME_INDEX				(4)

#define WM_PROC_ALIVE			(WM_USER+200)	// 프로세스가 살아있음을 알림.

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
UINT DelDataThreadFunc(LPVOID lpParam);	
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKillProcessDlg 대화 상자


CKillProcessDlg::CKillProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKillProcessDlg::IDD, pParent)
	, m_ProcName(_T(""))
	, m_monFilePathName(_T(""))
	, m_waitTime(0)
	, m_wakeupTime(0)
	, m_srcUpdateAppPath(_T(""))
	, m_dstUpdateAppPath(_T(""))
	, m_RefreshProcessTime(0)
	,m_bHide(false)
	, m_InitTray(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_listIndex = 0;
	m_ProcID = 0;

	m_LogFileSaveInterval = 0;

	m_hDelEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	is_DEL_Run = FALSE;

	CTime now = CTime::GetCurrentTime();
	oldDay = now.GetDay();  //날짜 초기화

	CreateThead();

}

void CKillProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROC_LIST, m_ProcList);
	DDX_Text(pDX, IDC_PROC_NAME_EDIT,m_ProcName);
	DDX_Text(pDX,IDC_MON_PROC_NAME_EDIT,m_monFilePathName);
	DDX_Control(pDX, IDC_MON_PROCESS_LIST, m_monProcList);
	DDX_Text(pDX,IDC_MON_PROC_WAIT_EDIT,m_waitTime);
	DDX_Text(pDX,IDC_WAKEUP_TIME_EDIT, m_wakeupTime);
	DDX_Control(pDX, IDC_UPDATE_USE_YN, m_UpdateAppUseGroupBox);
	DDX_Text(pDX,IDC_SRC_UPDATE_PATH_EDIT,m_srcUpdateAppPath);
	DDX_Text(pDX,IDC_DST_UPDATE_PATH_EDIT,m_dstUpdateAppPath);
	DDX_Text(pDX,IDC_PROCESS_UPDATE_TIME_EDIT,m_RefreshProcessTime);
	DDX_Control(pDX, IDC_VERSION, m_Version);
	DDX_Text(pDX, IDC_LOGFILE_SAVEDAY_EDIT, m_LogFileSaveInterval);
}

BEGIN_MESSAGE_MAP(CKillProcessDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_KILL_PROC_BUTTON, &CKillProcessDlg::OnBnClickedKillProcButton)
	ON_BN_CLICKED(IDC_MON_PROC_ADD_BUTTON, &CKillProcessDlg::OnBnClickedMonProcAddButton)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROC_LIST, &CKillProcessDlg::OnLvnItemchangedProcList)
	ON_BN_CLICKED(IDC_BROWSE_FILE_BTN, &CKillProcessDlg::OnBnClickedBrowseFileBtn)
	ON_BN_CLICKED(IDC_MON_PROC_REMOVE_BUTTON, &CKillProcessDlg::OnBnClickedMonProcRemoveButton)
	ON_NOTIFY(NM_CLICK, IDC_MON_PROCESS_LIST, &CKillProcessDlg::OnNMClickMonProcessList)
	ON_NOTIFY(NM_DBLCLK, IDC_MON_PROCESS_LIST, &CKillProcessDlg::OnNMDblclkMonProcessList)
	ON_BN_CLICKED(IDC_MON_PROC_MODIFY_BUTTON, &CKillProcessDlg::OnBnClickedMonProcModifyButton)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_READ_INI_BUTTON, &CKillProcessDlg::OnBnClickedReadIniButton)
	ON_BN_CLICKED(IDC_WRITE_INI_BUTTON, &CKillProcessDlg::OnBnClickedWriteIniButton)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_SRC_BROWSE_FILE_BTN, &CKillProcessDlg::OnBnClickedSrcBrowseFileBtn)
	ON_BN_CLICKED(IDC_DEST_BROWSE_FILE_BTN, &CKillProcessDlg::OnBnClickedDestBrowseFileBtn)
	ON_MESSAGE(WM_TRAYICON, OnTrayIcon)
	ON_COMMAND(WM_APP_EXIT, OnAppExit)
	ON_COMMAND(WM_DIALOG_SHOW, OnDialogShow)
	ON_MESSAGE(WM_PROC_ALIVE, &CKillProcessDlg::OnProcAliveFunc)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MON_PROCESS_LIST, &CKillProcessDlg::OnNMCustomdrawMonProcessList)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CKillProcessDlg 메시지 처리기
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::OnInitDialog()
* @param void 없음
* @remark 다이얼로그 초기화 함수
* @return void 없음
* @date 20/03/03
*/
BOOL CKillProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	LSITS_Write_LogFile(LOG_PROG,_T("Program start \r\n"));

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAIN_ICON)); //icon 변경
	this->SetIcon(hIcon, TRUE);
	this->SetIcon(hIcon, FALSE); //icon 셋팅

	CRect rect;
	m_ProcList.GetClientRect(&rect);
	m_ProcList.InsertColumn(0,_T("Process Name"),LVCFMT_LEFT,PROC_NAME_WIDTH);
	m_ProcList.InsertColumn(1,_T("Process ID"),LVCFMT_LEFT,rect.Width() - PROC_NAME_WIDTH);

	//모니터 프로세스 리스트 초기화

	m_monProcList.GetClientRect(&rect);
	m_monProcList.InsertColumn(CHECK_INDEX, _T(""), LVCFMT_CENTER, 30);
	m_monProcList.InsertColumn(FILE_NAME_INDEX, _T("Program"), LVCFMT_LEFT, 100);
	m_monProcList.InsertColumn(WAIT_TIME_INDEX, _T("kill"), LVCFMT_LEFT, 50);
	m_monProcList.InsertColumn(WAKEUP_TIME_INDEX, _T("wakeup"), LVCFMT_LEFT, 50);
	m_monProcList.InsertColumn(PATH_NAME_INDEX, _T("Path"), LVCFMT_LEFT, rect.Width() - 230);


	VERIFY( m_checkLCImgList.Create(IDB_CHECKBOXES, 16, 4, RGB(255,0,255)) );
	int j = m_checkLCImgList.GetImageCount();
	m_monProcList.SetImageList(&m_checkLCImgList, LVSIL_SMALL);
	
	int listOrder[5];
	listOrder[0] = 1;
	listOrder[1] = 0;
	listOrder[2] = 2;
	listOrder[3] = 3;
	listOrder[4] = 4;

	m_monProcList.SetColumnOrderArray(5, listOrder);
	m_monProcList.SetExtendedStyle( m_monProcList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER);
	m_monProcList.Init();

	m_UpdateAppUseGroupBox.SetTitleStyle(BS_AUTOCHECKBOX, FALSE);
	m_UpdateAppUseGroupBox.SetGroupID(1);

	Read_INI();

	if(m_UpdateAppUseGroupBox.GetCheck() == TRUE)
	{
		BOOL ret;
		//파일을 업데이트 한다.
		TCHAR szdstPath[MAX_PATH] = {0,};
		
		ret = CGlobal::GetPath(CGlobal::dstUpdatePath,szdstPath);
		if(PathFileExists(CGlobal::srcUpdatePath))
		{
			if(PathFileExists(szdstPath))
			{
				if ( CopyFile(CGlobal::srcUpdatePath , CGlobal::dstUpdatePath, false) == 0 ) {
					TCHAR szError[500];
					_stprintf_s(szError,_countof(szError),_T("copy %s ==> %s error\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_LogFile(LOG_ERROR,szError);
				}
				else
				{
					TCHAR szLog[500];
					_stprintf_s(szLog,_countof(szLog),_T("copy %s ==> %s OK!\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_LogFile(LOG_PROG,szLog);
				}
			}
			else
			{
				CreateDir(szdstPath);
				if ( CopyFile(CGlobal::srcUpdatePath , CGlobal::dstUpdatePath, false) == 0 ) {
					TCHAR szError[500];
					_stprintf_s(szError,_countof(szError),_T("copy %s ==> %s error\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_LogFile(LOG_ERROR,szError);
				}
				else
				{
					TCHAR szLog[500];
					_stprintf_s(szLog,_countof(szLog),_T("copy %s ==> %s OK!\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_LogFile(LOG_PROG,szLog);
				}
			}
			
		}
	}

	UpdateProcess();

	m_myTray.m_bHide = m_bHide;
	//m_myTray.AddTrayIcon(GetSafeHwnd());	//시작과 동시에 트레이 아이콘 표시

	SetTimer(TIMER_NUM,m_RefreshProcessTime*1000,NULL);

	TCHAR buf[100];
	_stprintf_s(buf,_countof(buf),_T("%s"),PROGRAM_VERSION);
	m_Version.SetWindowText(buf);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CKillProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
#if 0
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	
	}
#endif
	//if (nID == SC_MINIMIZE || nID == SC_CLOSE)
	if (nID == SC_CLOSE) //아이콘을 클릭하면 Minimizse 되면서 tray icon으로 바뀌는 문제 수정
	{
		
		OnDialogShow();
	}
	else
	{

		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CKillProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CKillProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedKillProcButton()
* @param void 없음
* @remark 프로세스를 죽인다
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedKillProcButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	try{
		if(UpdateData(TRUE))
		{
			KillProcess(m_ProcName);
			UpdateProcess();
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedMonProcAddButton()
* @param void 없음
* @remark 감시 프로세스를 추가한다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedMonProcAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try{
		if(UpdateData(TRUE))
		{
			//RunProcess(m_monFilePathName);
			CHeaderCtrl *pHeaderCtrl;

			pHeaderCtrl = m_monProcList.GetHeaderCtrl();
			int nCount = pHeaderCtrl->GetItemCount();
			TCHAR szText[128] ={0,};
			LVITEM pitem;
			int nPos = 0;
			TCHAR buf[MAX_PATH];


			TCHAR path_buffer[MAX_PATH];        // 전체 경로
			TCHAR drive[MAX_PATH];               // 드라이브 명
			TCHAR dir[MAX_PATH];                      // 디렉토리 경로
			TCHAR fname[MAX_PATH];           // 파일명
			TCHAR fname1[MAX_PATH];           // 파일명
			TCHAR ext[MAX_PATH];                    // 확장자 명

			int strLen = m_monFilePathName.GetLength();

			if(strLen > 0)
			{
				//동일한 프로세스가 있는지 확인한다.
				BOOL bFindSameProc = FALSE;
				POSITION pos;
				for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
				{
					PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
					if(_tcscmp(m_monFilePathName, pInfo->execFullPath)  == 0)
					{
						bFindSameProc = TRUE;
						break;
					}
					else
					{
						m_surveilList.GetNext(pos);
					}
				}
				//동일 프로세스가 없으면 등록한다.
				if(bFindSameProc == FALSE)
				{
					_tcscpy(path_buffer,m_monFilePathName);

					_splitpath(path_buffer, drive, dir, fname, ext);
					_stprintf_s(fname1,_countof(fname1),_T("%s%s"),fname,ext);

					PSURVEIL_PROC_INFO pInfo = new SURVEIL_PROC_INFO();
					memset(pInfo,0x00,sizeof(SURVEIL_PROC_INFO));
					pInfo->isActive = TRUE;

					if(pInfo != NULL)
					{
						for(int i = 0; i < nCount ;  i++)
						{
							/// 프로세스를 등록한다.
							ZeroMemory(&pitem, sizeof(pitem));
							if(i == CHECK_INDEX)
							{
								pitem.mask  = LVIF_TEXT | LVIF_IMAGE;
								pitem.pszText = "";
								pitem.cchTextMax = lstrlen(pitem.pszText);
								pitem.iImage = 1;
								nPos = m_monProcList.InsertItem(&pitem);
								pInfo->useWatchdog = FALSE;
							}
							else
							{
								pitem.mask  = LVIF_TEXT;	
								pitem.cchTextMax = lstrlen(pitem.pszText);
								pitem.iItem = nPos;
								pitem.iSubItem = i;
								switch(i)
								{
								case FILE_NAME_INDEX: //exec
									pitem.pszText = fname1;
									_tcscpy(pInfo->execName,fname1);
									break;
								case WAIT_TIME_INDEX: // time
									_stprintf_s( buf , _countof(buf), _T("%d"),DEFAULT_WAIT_TIME);
									pitem.pszText = buf;
									pInfo->maxWaitSec = DEFAULT_WAIT_TIME;
									break;
								case WAKEUP_TIME_INDEX: // wakeup
									_stprintf_s( buf , _countof(buf), _T("%d"),DEFAULT_WAITUP_TIME);
									pitem.pszText = buf;
									pInfo->maxWaitSec = DEFAULT_WAITUP_TIME;
									break;
								case PATH_NAME_INDEX: //path
									_stprintf_s( buf , _countof(buf), _T("%s%s"),drive,dir);
									pitem.pszText = buf;
									_tcscpy(pInfo->execPath,pitem.pszText);
									_stprintf_s( pInfo->execFullPath , _countof(pInfo->execFullPath), _T("%s%s%s"),pInfo->execPath,pInfo->execName);
									break;
								default:
									break;
								}
								m_monProcList.SetItem(&pitem);
							}
						}

						m_surveilList.AddTail(pInfo);
					}
				}
				else
				{
					AfxMessageBox("이미 등록된 프로세스가 있습니다",IDOK);
				}

			}
			else
			{
				AfxMessageBox("등록할 프로세스를 선택 하십시오.",IDOK);
			}


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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::PrintProcessNameAndID( DWORD processID)
* @param DWORD processID
* @remark processID의 프로세스를 리스트에 추가한다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::PrintProcessNameAndID( DWORD processID)
{

	try{
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		// Get a handle to the process.

		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, processID );

		// Get the process name.

		if (NULL != hProcess )
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
				&cbNeeded) )
			{
				GetModuleBaseName( hProcess, hMod, szProcessName, 
					sizeof(szProcessName)/sizeof(TCHAR) );

				TCHAR strProcID[MAX_PATH];
				_stprintf_s(strProcID,_countof(strProcID),_T("%d"),processID);
				m_ProcList.InsertItem(m_listIndex ,szProcessName);
				m_ProcList.SetItemText(m_listIndex ,1,strProcID);
				m_listIndex++;
			}
		}


		CloseHandle( hProcess );
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnTimer(UINT_PTR nIDEvent)
* @param void 없음
* @remark 타이머 핸들러. 매번 리스트를 업데이트 한다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//프로세스를 갱신한다.
	KillTimer(nIDEvent);


	CTime NowTime = CTime::GetCurrentTime();
	if(NowTime.GetHour() == 0 && NowTime.GetDay() != oldDay) //날짜가 바뀌었다.
	{
		oldDay = NowTime.GetDay();
		SetEvent(m_hDelEvent);  // 파일 삭제 쓰레드를 부른다.
	}
	
	///초기화 시에 일정 시간이 지나면 tray icon으로 변경
	if(m_InitTray)
	{

		//처음에 작아지는 것 삭제
		/*/if(m_bHide == FALSE)
		{
			OnDialogShow();
		}*/
	
		m_InitTray = FALSE;
	}
	
	try
	{
		UpdateProcess();
		//죽은 프로세스가 있는지 확인한다.
		SurveilProc();
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
	SetTimer(nIDEvent,m_RefreshProcessTime*1000,NULL);
	CDialogEx::OnTimer(nIDEvent);
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::UpdateProcess(void)
* @param void 없음
* @remark 프로세스를 업데이트 한다
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::UpdateProcess(void)
{

	DWORD aProcesses[2048], cbNeeded, cProcesses;
	unsigned int i;
	UINT maxProcessLen  = sizeof(aProcesses)/sizeof(DWORD);

	try
	{
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		{
			return;
		}


		// Calculate how many process identifiers were returned.

		cProcesses = cbNeeded / sizeof(DWORD);

		if(cProcesses)
		{
			m_ProcList.DeleteAllItems();
			m_listIndex = 0;
		}

		// Print the name and process identifier for each process.

		for ( i = 0; i < cProcesses; i++ )
		{
			if(i < maxProcessLen)
			{
				if( aProcesses[i] != 0 )
				{
					PrintProcessNameAndID(aProcesses[i] );
				}
			}
			
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark 프로세스 리스트 클릭했을 때 메시지 처리 함수
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try
	{
		POSITION pos = m_ProcList.GetFirstSelectedItemPosition();
		if(pos != NULL) {
			int nItem = 0;
			while(pos) {
				nItem = m_ProcList.GetNextSelectedItem(pos);
				m_ProcName = m_ProcList.GetItemText(nItem, 0);
				m_ProcID = _ttoi(m_ProcList.GetItemText(nItem, 1));
				UpdateData(FALSE);
				break;
			}
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	

	*pResult = 0;
}
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn BBOOL CKillProcessDlg::KillProcess(CString ProcessName)
* @param CString ProcessName 종료할 process name
* @remark process name으로 프로세스 죽이기
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::KillProcess(CString ProcessName)
{
	BOOL b = FALSE;
	HANDLE   hSnapshot  = NULL;  
    PROCESSENTRY32 pe32 = {0}; 

	try
	{
		// 시스템 프로세서의 상태(힙, 모듈, 스레드)를 읽어 온다.
		// 성공하면 스냅샷에 대한 핸들을 반환하고, 실패하면 INVALID_HANDLE_VALUE를 반환한다. 
		hSnapshot  = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );  

		// 엔트리 구조체 사이즈
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		// 엔트리에서 구성된 자료구조 가져오기
		if( Process32First( hSnapshot , &pe32 ) )  
		{  
			do   
			{  
				// printf("%20s", pe32.szExeFile);  
				if(ProcessName.Compare(pe32.szExeFile) == 0)
				// if(strcmp(pe32.szExeFile, ProcessName) == 0)
				{
					b = TRUE;
					printf("Process Found = %20s !!!\n", pe32.szExeFile);  
					//AfxMessageBox("Found Process");
					HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE, pe32.th32ProcessID );
					if( hProcess )
					{
						DWORD dwExitCode;
                    
						/* 	BOOL WINAPI GetExitCodeProcess
							( _In_  HANDLE  hProcess, _Out_ LPDWORD lpExitCode )
						hProcess : 종료 코드를 받기 원하는 프로세스의 핸들
						PROCESS_QUERY_INFORMATION 또는 PROCESS_QUERY_LIMITED_INFORMATION
						권한이 있어야 함.
                    
						lpExitCode : 프로세스 종료 상태를 받을 변수에 대한 포인터
                    
						실패시 0을 반환*/
                  
						GetExitCodeProcess( hProcess, &dwExitCode);
                    
						/* BOOL WINAPI TerminateProcess
						(_In_ HANDLE hProcess, _In_ uExitCode)
                    
						hProcess : 종료할 프로세스의 핸들을 설정
						핸들의 접근 권한은 반드시 PROCESS_TERMINATE를 가져야 함.
                    
						uExitCode : 프로세스 종료 코드를 설정
						통상적으로 정상 종료 인경우 0을 비정상 종료인경우 -1을 입력*/
                    
						TerminateProcess( hProcess, dwExitCode);
						CloseHandle(hProcess);
						TCHAR szLog[MAX_PATH];
						_stprintf_s(szLog,_countof(szLog),_T("KillProcess %s\r\n"),ProcessName);
						LSITS_Write_LogFile(LOG_PROG,szLog);
						//return TRUE;
					}

				}
			} while ( Process32Next( hSnapshot , &pe32 ) );  
		}  
		else  
		{  
			printf("???\n");
		}  
  
		CloseHandle (hSnapshot ); 
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
	
	return b;
}
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::KillProcessPid(DWORD pid)
* @param DWORD pid 종료할 process pid
* @remark pid로 프로세스 죽이기
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::KillProcessPid(DWORD pid)
{
	BOOL b = FALSE;
	HANDLE   hSnapshot  = NULL;  
	PROCESSENTRY32 pe32 = {0}; 
	try
	{
		hSnapshot  = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );  
		pe32.dwSize = sizeof( PROCESSENTRY32 );  
		if( Process32First( hSnapshot , &pe32 ) )  
		{  
			do   
			{  
				//printf("%20s", pe32.szExeFile);  
				if(pe32.th32ProcessID == pid)
					//            if(strcmp(pe32.szExeFile, ProcessName) == 0)
				{
					b = TRUE;
					printf("Process Found = %20s !!!\n", pe32.szExeFile);  
					//AfxMessageBox("Found Process");
					HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE, pe32.th32ProcessID );
					if( hProcess )
					{
						DWORD dwExitCode;
						GetExitCodeProcess( hProcess, &dwExitCode);
						TerminateProcess( hProcess, dwExitCode);
						CloseHandle(hProcess);
						//return TRUE;
					}

				}
			} while ( Process32Next( hSnapshot , &pe32 ) );  
		}  
		else  
		{  
			printf("???\n");
		}  

		CloseHandle (hSnapshot ); 
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	

	return b;
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::RunProcess(CString RunFilePath, int show)
* @param CString RunFilePath 실행할 파일 경로
* @param int show 실행 프로그램 보여줄지 여부
* @remark 프로세스 실행
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::RunProcess(CString RunFilePath, int show)
{

	try
	{
		// STARTUPINFO 생성하는 프로세스의 속성을 지정할 때 사용하는 구조체
		STARTUPINFO si;

		// 새로 생선된 프로세스와 스레드 정보가 있음
		PROCESS_INFORMATION pi;

		si.cb				= sizeof(STARTUPINFO);
		si.lpReserved		= NULL;
		si.lpReserved2		= NULL;
		si.cbReserved2		= 0;
		si.lpDesktop		= NULL;
		si.lpTitle			= NULL;
		si.dwFlags			= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
		si.dwX				= 0;
		si.dwY				= 0;
		si.dwFillAttribute	= 0;
		si.wShowWindow		= show;

		CreateProcess(NULL,RunFilePath.GetBuffer(0),NULL,NULL,TRUE, NULL,NULL,NULL,&si,&pi);

		// WaitForSingleObject(pi.hProcess, INFINITE);
		// 핸들 닫기
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		TCHAR szLog[MAX_PATH];
		_stprintf_s(szLog,_countof(szLog),_T("RunProcess %s\r\n"),RunFilePath);
		LSITS_Write_LogFile(LOG_PROG,szLog);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	

	return TRUE;
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedBrowseFileBtn()
* @param void 없음
* @remark 실행할 프로세스의 위치를 가져온다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedBrowseFileBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try{
		CFileDialog browseFileDlg(TRUE,NULL,NULL,OFN_OVERWRITEPROMPT,"Exe Files (*.exe)|*.exe||");


		int iRet = browseFileDlg.DoModal();

		CString l_strFileName;

		l_strFileName = browseFileDlg.GetPathName();

		if(iRet == IDOK)
		{
			m_monFilePathName = l_strFileName;
			UpdateData(FALSE);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	

}


void CKillProcessDlg::OnBnClickedMonProcRemoveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos;

	TCHAR fullPath[MAX_PATH];
	TCHAR path[MAX_PATH];
	TCHAR fileName[MAX_PATH];

	TCHAR szText[128] ={0,};
	LVITEM pitem;
	int nPos = 0;

	try{
		if(MessageBox("등록된 프로세스를 삭제하시겠습니까?", "환경설정", MB_YESNO) == IDYES)
		{
			pos = m_monProcList.GetFirstSelectedItemPosition();
			if(pos != NULL) {
				int nItem = 0;

				while(pos) {
					nItem = m_monProcList.GetNextSelectedItem(pos);

					ZeroMemory(&pitem, sizeof(pitem));
					pitem.mask  = LVIF_TEXT;	
					pitem.pszText = szText ;
					pitem.cchTextMax = sizeof( szText );
					pitem.iItem = nItem;
					pitem.iSubItem = FILE_NAME_INDEX;
					m_monProcList.GetItem(&pitem);
					_tcscpy(fileName,pitem.pszText);  // 파일 이름 얻기

					ZeroMemory(&pitem, sizeof(pitem));
					pitem.mask  = LVIF_TEXT;	
					pitem.pszText = szText ;
					pitem.cchTextMax = sizeof( szText );
					pitem.iItem = nItem;
					pitem.iSubItem = PATH_NAME_INDEX;
					m_monProcList.GetItem(&pitem);  // path 얻기
					_tcscpy(path,pitem.pszText);

					_stprintf_s(fullPath , _countof(fullPath), _T("%s%s"),path,fileName);

					m_monProcList.DeleteItem(nItem);

				} // while statement...
			}

			for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
			{
				PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
				if(_tcscmp(fullPath, pInfo->execFullPath)  == 0)
				{
					m_surveilList.RemoveAt(pos);
					break;
				}
				else
				{
					m_surveilList.GetNext(pos);
				}
			}
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

}


/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark 감시 리스트를 클릭하면 하면 처리하는 함수.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try{
		int index = pNMItemActivate->iItem;

		if(index >= 0 && index < m_monProcList.GetItemCount())
		{
			TCHAR szText[128] ={0,};
			TCHAR buf[128];
			LVITEM pitem;
			POSITION pos = m_monProcList.GetFirstSelectedItemPosition();
			if(pos != NULL) {
				int nItem = 0;

				while(pos) {
					nItem = m_monProcList.GetNextSelectedItem(pos);
					ZeroMemory(&pitem, sizeof(pitem));
					pitem.mask  = LVIF_TEXT;
					pitem.iItem = nItem;
					pitem.iSubItem = WAIT_TIME_INDEX;
					pitem.pszText = szText ;
					pitem.cchTextMax = sizeof( szText );
					m_monProcList.GetItem(&pitem);

					_tcscpy(buf,pitem.pszText);
					m_waitTime = _ttoi(buf);

					ZeroMemory(&pitem, sizeof(pitem));
					pitem.mask  = LVIF_TEXT;
					pitem.iItem = nItem;
					pitem.iSubItem = WAKEUP_TIME_INDEX;
					pitem.pszText = szText ;
					pitem.cchTextMax = sizeof( szText );
					m_monProcList.GetItem(&pitem);

					_tcscpy(buf,pitem.pszText);
					m_wakeupTime = _ttoi(buf);

				}
			}

			UpdateData(FALSE);
		}

		*pResult = 0;
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark 더블 클릭을 하면 아이템이 메시지 모니터링을 하는지 확인한다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try{
		int index = pNMItemActivate->iItem;

		if(index >= 0 && index < m_monProcList.GetItemCount())
		{

			LVITEM pitem;
			char szText[128] ={0,};
			ZeroMemory(&pitem, sizeof(pitem));

			pitem.mask  = LVIF_TEXT | LVIF_IMAGE;
			pitem.iItem = index;
			pitem.iSubItem = CHECK_INDEX;
			pitem.pszText = szText ;
			pitem.cchTextMax = sizeof( szText );
			m_monProcList.GetItem(&pitem);

			if(pitem.iImage == 2)
				pitem.iImage = 1; // First image from image list
			else
				pitem.iImage = 2;

			m_monProcList.SetItem(&pitem);
		}

		*pResult = 0;
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedMonProcModifyButton()
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark 수정 버튼을 누를 때 선택된 리스트의 일부 값들이 변경 된다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedMonProcModifyButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	try{
		if(UpdateData(TRUE))
		{
			if(m_waitTime >= 0 && m_wakeupTime >=0 )
			{
				POSITION pos = m_monProcList.GetFirstSelectedItemPosition();
				TCHAR szText[MAX_PATH] ={0,};
				TCHAR fileName[MAX_PATH];
				TCHAR pathName[MAX_PATH];
				TCHAR fullPathName[MAX_PATH];

				LVITEM pitem;
				if (pos != NULL)
				{
					while (pos)
					{
						int nItem = m_monProcList.GetNextSelectedItem(pos);

						ZeroMemory(&pitem, sizeof(pitem));
						pitem.mask  = LVIF_TEXT;
						pitem.iItem = nItem;
						pitem.iSubItem = WAIT_TIME_INDEX;
						_stprintf_s(szText,_countof(szText),_T("%d"),m_waitTime);
						pitem.pszText = szText ;
						pitem.cchTextMax = sizeof( szText );
						m_monProcList.SetItem(&pitem);

						ZeroMemory(&pitem, sizeof(pitem));
						pitem.mask  = LVIF_TEXT;
						pitem.iItem = nItem;
						pitem.iSubItem = WAKEUP_TIME_INDEX;
						_stprintf_s(szText,_countof(szText),_T("%d"),m_wakeupTime);
						pitem.pszText = szText ;
						pitem.cchTextMax = sizeof( szText );
						m_monProcList.SetItem(&pitem);

						ZeroMemory(&pitem, sizeof(pitem));
						pitem.mask  = LVIF_TEXT;
						pitem.iItem = nItem;
						pitem.iSubItem = FILE_NAME_INDEX;
						pitem.pszText = szText;
						pitem.cchTextMax = sizeof( szText );
						m_monProcList.GetItem(&pitem);
						_tcscpy(fileName,pitem.pszText);


						ZeroMemory(&pitem, sizeof(pitem));
						pitem.mask  = LVIF_TEXT;
						pitem.iItem = nItem;
						pitem.iSubItem = PATH_NAME_INDEX;
						pitem.pszText = szText ;
						pitem.cchTextMax = sizeof( szText );
						m_monProcList.GetItem(&pitem);
						_tcscpy(pathName,pitem.pszText);

						_stprintf_s(fullPathName,_countof(fullPathName),_T("%s%s"),pathName,fileName);

						UpdateSurveilList(fullPathName , m_waitTime, m_wakeupTime);
					}
				}
				else
				{
					AfxMessageBox("변경할 리스트를 선택 하십시오.",IDOK);
				}
			}
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnDestroy()
* @param void 없음
* @remark 윈도우가 사라질 때 메모리를 삭제한다.
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	

	StopThread();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	try{
		POSITION pos,pos1;

		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
			delete pInfo;
			pos1 = pos;
			m_surveilList.GetNext(pos);
			m_surveilList.RemoveAt(pos1);
		}
		DWORD ncnt;
		ncnt = m_RemoveDirList.GetCount();
		if(ncnt)
		{
			m_RemoveDirList.RemoveAll();
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
	
}
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::Read_INI(void)
* @param void 없음
* @remark INI 값을 읽는다
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::Read_INI(void)
{
	TCHAR buf[MAX_PATH];
	TCHAR TempBuf[MAX_PATH];

	TCHAR path_buffer[MAX_PATH];        // 전체 경로
	TCHAR drive[MAX_PATH];               // 드라이브 명
	TCHAR dir[MAX_PATH];                      // 디렉토리 경로
	TCHAR fname[MAX_PATH];           // 파일명
	TCHAR ext[MAX_PATH];                    // 확장자 명

	try{
		// m_surveilList 삭제
		POSITION pos,pos1;
		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
			delete pInfo;
			pos1 = pos;
			m_surveilList.GetNext(pos);
			m_surveilList.RemoveAt(pos1);
		}

		CGlobal::REALTIME_MON_PROC_COUNT =  GetPrivateProfileInt(_T("CONFIG"),_T("REALTIME_MONITOR_PROC_COUNT"),0,CGlobal::PROGRAM_INI_FULLPATH);
		//타이머 업데이트 주기
		m_RefreshProcessTime = GetPrivateProfileInt(_T("CONFIG"),_T("REFRESH_PROC_TIME"),DEFAULT_UPDATE_TIME,CGlobal::PROGRAM_INI_FULLPATH);

		CGlobal::INI_LOGFILE_SAVE_INTERVAL = GetPrivateProfileInt(_T("DEBUG_LOG"), _T("LOG_COGNITION_SAVE_INTERVAL"), 10, CGlobal::PROGRAM_INI_FULLPATH);

		for(int i = 0; i < CGlobal::REALTIME_MON_PROC_COUNT;  i++)
		{



			PSURVEIL_PROC_INFO pInfo = new SURVEIL_PROC_INFO();
			memset(pInfo,0x00,sizeof(SURVEIL_PROC_INFO));
			pInfo->isActive = TRUE;

			if(pInfo != NULL)
			{
				_stprintf_s(buf,_countof(buf),_T("PROCESS_%d"),i);
				GetPrivateProfileString(_T("CONFIG"), buf,  "" , path_buffer, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);

				_splitpath(path_buffer, drive, dir, fname, ext);

				_tcscpy(pInfo->execFullPath,path_buffer);

				_stprintf_s(buf,_countof(buf),_T("%s%s"),drive,dir);
				_tcscpy(pInfo->execPath,buf);  //디렉토리 복사
				_stprintf_s(buf,_countof(buf),_T("%s%s"),fname,ext);
				_tcscpy(pInfo->execName,buf);  //filename 복사

				_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_USE_WATCHDOG"),i);
				GetPrivateProfileString(_T("CONFIG"), buf,  "N" , TempBuf, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);

				if(_tcscmp(TempBuf, "Y") == 0)
					pInfo->useWatchdog = TRUE;
				else
					pInfo->useWatchdog = FALSE;
				/// 최대 대기 시간
				_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_MAX_WAIT_TIME"),i);
				pInfo->maxWaitSec =  GetPrivateProfileInt(_T("CONFIG"),buf,30,CGlobal::PROGRAM_INI_FULLPATH);

				/// 기동 대기 시간
				_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_WAKEUP_TIME"),i);
				pInfo->wakeupTime =  GetPrivateProfileInt(_T("CONFIG"),buf,30,CGlobal::PROGRAM_INI_FULLPATH);

				m_surveilList.AddTail(pInfo);
			}

		}

		//listcontrol 모두 삭제후 재 설정

		m_monProcList.DeleteAllItems();

		TCHAR szText[128] ={0,};
		LVITEM pitem;

		CHeaderCtrl *pHeaderCtrl;

		pHeaderCtrl = m_monProcList.GetHeaderCtrl();
		int nCount = pHeaderCtrl->GetItemCount();
		int nPos = 0;

		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);

			//추가..
			if(pInfo != NULL)
			{
				for(int i = 0; i < nCount ;  i++)
				{
					/// 프로세스를 등록한다.
					ZeroMemory(&pitem, sizeof(pitem));
					if(i == CHECK_INDEX)
					{
						pitem.mask  = LVIF_TEXT | LVIF_IMAGE;
						pitem.pszText = "";
						pitem.cchTextMax = lstrlen(pitem.pszText);
						if(pInfo->useWatchdog)
						{
							pitem.iImage = 2;
						}
						else
						{
							pitem.iImage = 1;
						}
						nPos = m_monProcList.InsertItem(&pitem);
					}
					else
					{
						pitem.mask  = LVIF_TEXT;	
						pitem.cchTextMax = lstrlen(pitem.pszText);
						pitem.iItem = nPos;
						pitem.iSubItem = i;
						switch(i)
						{
						case FILE_NAME_INDEX: //exec
							pitem.pszText = pInfo->execName;
							break;
						case WAIT_TIME_INDEX: // time
							_stprintf_s( buf , _countof(buf), _T("%d"),pInfo->maxWaitSec);
							pitem.pszText = buf;
							break;
						case WAKEUP_TIME_INDEX: // wakeup
							_stprintf_s( buf , _countof(buf), _T("%d"),pInfo->wakeupTime);
							pitem.pszText = buf;
							break;
						case PATH_NAME_INDEX: //path
							pitem.pszText = pInfo->execPath;
							break;
						default:
							break;
						}
						m_monProcList.SetItem(&pitem);
					}
				}
			}

			m_surveilList.GetNext(pos);
		}


		//업데이트 설정
		GetPrivateProfileString(_T("CONFIG"), _T("UPDEATE_APP_USE"),  "N" , TempBuf, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);

		if(_tcscmp(TempBuf, "Y") == 0)
		{
			m_UpdateAppUseGroupBox.SetCheck(TRUE);
		}
		else
		{
			m_UpdateAppUseGroupBox.SetCheck(FALSE);
		}
		//업데이트 할 디렉토리 읽기
		GetPrivateProfileString(_T("CONFIG"), _T("SRC_UPDEATE_PATH"),  "" , CGlobal::srcUpdatePath, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);
		m_srcUpdateAppPath.Format(_T("%s"),CGlobal::srcUpdatePath);
		GetPrivateProfileString(_T("CONFIG"), _T("DST_UPDEATE_PATH"),  "" , CGlobal::dstUpdatePath, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);
		m_dstUpdateAppPath.Format(_T("%s"),CGlobal::dstUpdatePath);

		m_LogFileSaveInterval = CGlobal::INI_LOGFILE_SAVE_INTERVAL;

		UpdateData(FALSE);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
}
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::Write_INI(void)
* @param void 없음
* @remark INI 값을 쓴다
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::Write_INI(void)
{

	try{
		TCHAR buf[MAX_PATH];
		CGlobal::REALTIME_MON_PROC_COUNT = m_monProcList.GetItemCount();

		CHeaderCtrl *pHeaderCtrl;

		pHeaderCtrl = m_monProcList.GetHeaderCtrl();
		int nCount = pHeaderCtrl->GetItemCount();

		if(MessageBox("감시 프로세스 설정값을 변경하시겠습니까?", "환경설정", MB_YESNO) == IDYES)
		{
			UpdateData(TRUE);

			POSITION pos,pos1;
			for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
			{
				PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
				delete pInfo;
				pos1 = pos;
				m_surveilList.GetNext(pos);
				m_surveilList.RemoveAt(pos1);
			}
			_stprintf_s(buf,_countof(buf),_T("%d"),CGlobal::REALTIME_MON_PROC_COUNT);
			WritePrivateProfileString(_T("CONFIG"),_T("REALTIME_MONITOR_PROC_COUNT"),buf,CGlobal::PROGRAM_INI_FULLPATH);

			_stprintf_s(buf,_countof(buf),_T("%d"),m_RefreshProcessTime);
			WritePrivateProfileString(_T("CONFIG"),_T("REFRESH_PROC_TIME"),buf,CGlobal::PROGRAM_INI_FULLPATH);


			for(int i = 0; i < CGlobal::REALTIME_MON_PROC_COUNT;  i++)
			{

				LVITEM pitem;
				char szText[128] ={0,};


				PSURVEIL_PROC_INFO pInfo = new SURVEIL_PROC_INFO();
				memset(pInfo,0x00,sizeof(SURVEIL_PROC_INFO));
				pInfo->isActive = TRUE;

				if(pInfo != NULL)
				{
					for(int j = 0;  j <  nCount ; j++)
					{
						ZeroMemory(&pitem, sizeof(pitem));

						if(j == CHECK_INDEX)
						{
							pitem.mask  = LVIF_TEXT | LVIF_IMAGE;
						}
						else
						{
							pitem.mask  = LVIF_TEXT;
						}
						pitem.iItem = i;
						pitem.iSubItem = j;
						pitem.pszText = szText ;
						pitem.cchTextMax = sizeof( szText );
						m_monProcList.GetItem(&pitem);

						switch(j)
						{
						case CHECK_INDEX:
							_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_USE_WATCHDOG"),i);
							if(pitem.iImage == 2)
							{
								WritePrivateProfileString(_T("CONFIG"), buf,"Y", CGlobal::PROGRAM_INI_FULLPATH);
								pInfo->useWatchdog = TRUE;
							}
							else
							{
								pInfo->useWatchdog = FALSE;
								WritePrivateProfileString(_T("CONFIG"), buf,"N", CGlobal::PROGRAM_INI_FULLPATH);
							}
							break;
						case FILE_NAME_INDEX: /// 실행파일..
							_tcscpy(pInfo->execName,pitem.pszText);
							break;
						case WAIT_TIME_INDEX: /// 타임아웃
							_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_MAX_WAIT_TIME"),i);
							pInfo->maxWaitSec = _ttoi(pitem.pszText);
							WritePrivateProfileString(_T("CONFIG"),buf,pitem.pszText,CGlobal::PROGRAM_INI_FULLPATH);
							break;
						case WAKEUP_TIME_INDEX: /// wakeup time
							_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_WAKEUP_TIME"),i);
							pInfo->wakeupTime = _ttoi(pitem.pszText);
							WritePrivateProfileString(_T("CONFIG"),buf,pitem.pszText,CGlobal::PROGRAM_INI_FULLPATH);
							break;
						case PATH_NAME_INDEX: /// path
							_tcscpy(pInfo->execPath,pitem.pszText);
							_stprintf_s(pInfo->execFullPath,_countof(pInfo->execFullPath),_T("%s%s"),pInfo->execPath,pInfo->execName);
							_stprintf_s(buf,_countof(buf),_T("PROCESS_%d"),i);
							WritePrivateProfileString(_T("CONFIG"), buf,pInfo->execFullPath, CGlobal::PROGRAM_INI_FULLPATH);
							break;
						default:
							break;
						}
					}

					m_surveilList.AddTail(pInfo);
				}

			}


			//업데이트 설정
			WritePrivateProfileString(_T("CONFIG"), _T("UPDEATE_APP_USE"),(m_UpdateAppUseGroupBox.GetCheck()) ? _T("Y") : _T("N"), CGlobal::PROGRAM_INI_FULLPATH);

			//업데이트 할 디렉토리 쓰기
			WritePrivateProfileString(_T("CONFIG"), _T("SRC_UPDEATE_PATH"),CGlobal::srcUpdatePath, CGlobal::PROGRAM_INI_FULLPATH);
			WritePrivateProfileString(_T("CONFIG"), _T("DST_UPDEATE_PATH"),CGlobal::dstUpdatePath,CGlobal::PROGRAM_INI_FULLPATH);

			//로그 삭제 주기.
			_stprintf_s(buf,_countof(buf), _T("%d"), m_LogFileSaveInterval);
			WritePrivateProfileString(_T("DEBUG_LOG"), _T("LOG_COGNITION_SAVE_INTERVAL"),  buf, CGlobal::PROGRAM_INI_FULLPATH);

			WritePrivateProfileString(NULL,NULL,NULL,CGlobal::PROGRAM_INI_FULLPATH);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedReadIniButton()
* @param void 없음
* @remark INI 값을 읽는다
* @return void 없음
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedReadIniButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Read_INI();
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedWriteIniButton()
* @param void 없음
* @remark INI 값을 쓴다
* @return void 없음
* @date 20/03/04
*/
void CKillProcessDlg::OnBnClickedWriteIniButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Write_INI();
}


/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::UpdateSurveilList(TCHAR *fullPathName, int param)
* @param TCHAR *fullPathName
* @param int param
* @remark 감시할 정보의 업데이트를 한다. 기다리는 시간.
* @return void 없음
* @date 20/03/04
*/
void CKillProcessDlg::UpdateSurveilList(TCHAR *fullPathName, int param1 , int param2)
{
	try{
		POSITION pos;
		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
			if(_tcscmp(fullPathName, pInfo->execFullPath)  == 0)
			{
				pInfo->maxWaitSec = param1;
				pInfo->wakeupTime = param2;
				break;
			}
			else
			{
				m_surveilList.GetNext(pos);
			}
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::SurveilProc(void)
* @param void 없음
* @remark 프로세스를 감시한다
* @return void 없음
* @date 20/03/04
*/
void CKillProcessDlg::SurveilProc(void)
{
	POSITION pos;
	TCHAR filename[MAX_PATH];
	int fIndex = 0;
	try
	{
		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
			if(pInfo)
			{
				BOOL find = FALSE;
				
				for(int index = 0; index < m_ProcList.GetItemCount();index++)
				{
					m_ProcList.GetItemText(index,0,filename, _countof(filename));
					if( _tcscmp(pInfo->execName, filename) == 0)
					{
						find = TRUE;
						break;
					}
				}
				//이전 값 백업
				pInfo->oldActive = pInfo->isActive;
				if(find)
				{
					//프로세스가 살아 있으면...
					pInfo->isActive = TRUE;
					pInfo->curWakupSecond = 0;

					if(pInfo->useWatchdog == TRUE)
					{
						pInfo->curWaitSecond += m_RefreshProcessTime*1000;
						if(pInfo->maxWaitSec*1000 <= pInfo->curWaitSecond)
						{
							//프로세스를 죽인다.
							KillProcess(pInfo->execName);
							pInfo->curWaitSecond = 0;  //죽이면... watchdog 카운트 초기화
						}
						/*
						RECT rcItem;
						m_monProcList.GetItemRect(fIndex,&rcItem,LVIR_BOUNDS);
						m_monProcList.InvalidateRect(&rcItem,TRUE);*/
						//실제 모니터 리스트에서 해당 row를 찾아 업데이트 해주어야 한다.
						/*
						for(int iter = 0; iter < m_monProcList.GetItemCount();iter++)
						{
							m_monProcList.GetItemText(iter,1,filename, _countof(filename));
							if( _tcscmp(pInfo->execName, filename) == 0)
							{
								m_monProcList.SetItemText(iter,1,filename);
								break;
							}
						}*/

					}
				}
				else
				{
					pInfo->isActive =FALSE;
					//프로세스가 삭제되었으면... 재 기동 여부를 확인한다.
					if(pInfo->oldActive == TRUE)
					{
						//막 삭제 된 경우
						pInfo->curWakupSecond = 0;
					}
					else
					{
						pInfo->curWakupSecond +=  m_RefreshProcessTime*1000;

						if(pInfo->wakeupTime*1000 <= pInfo->curWakupSecond)
						{
							//프로세스를 살린다.
							RunProcess(pInfo->execFullPath);
						}
					}
					
				}

				if(pInfo->useWatchdog == TRUE)
				{
					for(int iter = 0; iter < m_monProcList.GetItemCount();iter++)
					{
						m_monProcList.GetItemText(iter,1,filename, _countof(filename));
						if( _tcscmp(pInfo->execName, filename) == 0)
						{
							m_monProcList.SetItemText(iter,1,filename);
							break;
						}
					}
				}
			}
			m_surveilList.GetNext(pos);
			fIndex++;
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

}
/**
* @author 윤경섭
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
* @param CWnd* pWnd
* @param COPYDATASTRUCT* pCopyDataStruct
* @remark COPYDATASTRUCT 구조의 메시지를 처리한다.
* @return void 없음
* @date 20/03/04
*/
BOOL CKillProcessDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	try
	{
		m_pcds = (PCOPYDATASTRUCT)pCopyDataStruct;

		if(m_pcds->cbData == sizeof(WATCHDOG_MSG))
		{
			PWATCHDOG_MSG pWdMsg = new WATCHDOG_MSG();
			if(pWdMsg)
			{
				CopyMemory(pWdMsg,m_pcds->lpData,m_pcds->cbData);

				PostMessage(WM_PROC_ALIVE,(WPARAM)pWdMsg, NULL);
			}
			else
			{
				TCHAR szLog[MAX_PATH];
				_stprintf_s(szLog,_countof(szLog),_T("WATCHDOG_MSG 메시지 메모리 할당 에러!\r\n"));
				LSITS_Write_LogFile(LOG_ERROR,szLog);
			}
			
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
	
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CKillProcessDlg::OnBnClickedSrcBrowseFileBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	try{
		CFileDialog browseFileDlg(TRUE,NULL,NULL,OFN_OVERWRITEPROMPT,"Exe Files (*.exe)|*.exe||");


		int iRet = browseFileDlg.DoModal();

		CString l_strFileName;

		l_strFileName = browseFileDlg.GetPathName();

		if(iRet == IDOK)
		{
			m_srcUpdateAppPath = l_strFileName;
			m_strProgramName = PathFindFileName(m_srcUpdateAppPath);
			_tcscpy(CGlobal::srcUpdatePath,(LPTSTR)(LPCTSTR)m_srcUpdateAppPath);
			UpdateData(FALSE);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	
}


void CKillProcessDlg::OnBnClickedDestBrowseFileBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	try{
		if(m_strProgramName.IsEmpty())
		{
			AfxMessageBox("먼저 업데이트 파일을 설정 하세요.",IDOK);
			return;
		}
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = this->GetSafeHwnd();
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		TCHAR szPath[MAX_PATH] = {0};
		m_dstUpdateAppPath = "";
		BOOL result = SHGetPathFromIDList(pidl, szPath);
		if(result)
		{
			_tcscpy((LPTSTR)(LPCTSTR)m_dstUpdateAppPath,szPath);
			m_dstUpdateAppPath.Format(_T("%s\\%s"),szPath,m_strProgramName);
			_tcscpy(CGlobal::dstUpdatePath,(LPTSTR)(LPCTSTR)m_dstUpdateAppPath);
			UpdateData(FALSE);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
}



//트레이 아이콘을 클릭했을 때의 메시지 핸들러
LRESULT CKillProcessDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	try{
		m_myTray.ProcTrayMsg(GetSafeHwnd(), wParam, lParam);
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
	return TRUE;
}

//트레이 아이콘 팝업메뉴의 종료 메뉴 메시지 핸들러
void CKillProcessDlg::OnAppExit(void)
{
	try{
		m_myTray.DelTrayIcon(GetSafeHwnd());
		LSITS_Write_LogFile(LOG_PROG,_T("OnAppExit Program closed \r\n"));
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
	CDialog::OnCancel();
}

//트레이 아이콘 보이기/숨기기 메뉴 메시지 핸들러
void CKillProcessDlg::OnDialogShow()
{
	try{
		if(m_myTray.m_bHide == FALSE)
		{
			m_myTray.AddTrayIcon(GetSafeHwnd());
			ShowWindow(false);	//보이는 상태라면 숨기고
			m_myTray.m_bHide = TRUE;
		}
		else if(m_myTray.m_bHide == TRUE)
		{
			ShowWindow(true);			//숨겨진 상태라면 보이게
			m_myTray.DelTrayIcon(GetSafeHwnd());
			m_myTray.m_bHide = FALSE;
		}
		else
		{
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	
}

/**
*		@fn		LRESULT CKillProcessDlg::OnProcAliveFunc(WPARAM wParam, LPARAM lParam)
*		@brief	WM 를 받으면 들어오는 함수 (콜백함수)
*		@param	wParam : 연결에 대한 상태정보를 가지고 있다.
*		@param	lParam : 연결 이외의 데이터를 가지고 있을 경우에 값이 존재한다.
*		@remark	WM_COPYDATA 메시지 처리 콜백함수
*/
LRESULT CKillProcessDlg::OnProcAliveFunc(WPARAM wParam, LPARAM lParam)
{
	try{
		WATCHDOG_MSG *pWdMsg = (WATCHDOG_MSG *)wParam;
		if(pWdMsg)
		{
			TCHAR procname[MAX_PATH] = { 0,};


			if(pWdMsg->strlen < sizeof(pWdMsg->processName))
			{
				CopyMemory(procname,pWdMsg->processName,pWdMsg->strlen);

				POSITION pos;

				for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
				{
					PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
					if(pInfo)
					{
						//해당 프로세스를 찾으면 업데이트를 해준다.
						if( _tcscmp(pInfo->execName, procname) == 0)
						{
							pInfo->curWaitSecond = 0;

							TCHAR szLog[MAX_PATH];
							_stprintf_s(szLog,_countof(szLog),_T("Refresh %s\r\n"),procname);
							LSITS_Write_LogFile(LOG_PROG,szLog);

							if(pInfo->useWatchdog == TRUE)
							{
								for(int iter = 0; iter < m_monProcList.GetItemCount();iter++)
								{
									m_monProcList.GetItemText(iter,1,procname, _countof(procname));
									if( _tcscmp(pInfo->execName, procname) == 0)
									{
										m_monProcList.SetItemText(iter,1,procname);
										break;
									}
								}
							}

							break;
						}
					}
					m_surveilList.GetNext(pos);
				}
			}


			delete pWdMsg;

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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	return TRUE;
}

void CKillProcessDlg::OnNMCustomdrawMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	try{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		if(pNMCD->dwDrawStage == CDDS_PREPAINT){
			*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW; 
			//*pResult = CDRF_NOTIFYPOSTERASE | CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW;
			return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
		}
		if(pNMCD->dwDrawStage == CDDS_ITEMPREPAINT){ 

			// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
			if(m_monProcList.GetItemData(pNMCD->dwItemSpec) == 0){//dwItemSpec 이 현재 그려지는 row index
				NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW*)(pNMHDR); 
 				pDraw->clrText = 0x0; 
				pDraw->clrTextBk = 0xffffff; 
				//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
				*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
				return;//여기서 중단해야 *pResult 값이 유지된다.
			}
			else{ // When all matrices are already made. 
				NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW*)(pNMHDR); 
 				pDraw->clrText = 0x0; 
				pDraw->clrTextBk = RGB(255,255,196);  
				*pResult = (LRESULT)CDRF_NEWFONT; 
				return; 
			}
		}
		else if(pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)){	
					// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로

					// 올 수 있었던 것이다.

			NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW*)(pNMHDR); 
			CString text= m_monProcList.GetItemText(pNMCD->dwItemSpec, pDraw->iSubItem); 

			POSITION pos;

			for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
			{
				PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
				if(pInfo)
				{
					//해당 프로세스를 찾으면 업데이트를 해준다.
					if( _tcscmp(pInfo->execName, text) == 0 && pDraw->iSubItem == 1)
					{
						if(pInfo->useWatchdog == TRUE &&  pInfo->isActive == TRUE && pInfo->curWaitSecond <= 1000)
						{
							pDraw->clrText = 0xff; //빨강(red) 색으로
							pDraw->clrTextBk = 0xffffff; 
						}
						else
						{
							pDraw->clrText = 0x0; 
							pDraw->clrTextBk = 0xffffff; 
						}
						break;
					}
					else
					{
						pDraw->clrText = 0x0; 
						pDraw->clrTextBk = 0xffffff; 
					}
				}
				m_surveilList.GetNext(pos);
			}
		
			*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
			return;
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}

	*pResult = 0;
}



void CKillProcessDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	try{
		m_myTray.DelTrayIcon(GetSafeHwnd());
		LSITS_Write_LogFile(LOG_PROG,_T("OnClose Program closed \r\n"));
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
	catch(...)
	{
		DWORD dwError = GetLastError();
		LSITS_WriteExceptionFile(__FILE__,__LINE__,dwError);
	}
	CDialogEx::OnClose();
}

CKillProcessDlg::~CKillProcessDlg()
{

}

BOOL CKillProcessDlg::CreateThead( void )
{
	is_DEL_Run = TRUE;

	DelDataThread = AfxBeginThread(DelDataThreadFunc,	      // thread function.
		(CKillProcessDlg *)this,		              // thread function parameter.
		THREAD_PRIORITY_IDLE,0,CREATE_SUSPENDED       // thread priority.											
		);

	// thread 생성에 대한 결과를 전달.
	if(DelDataThread == NULL) {
		LSITS_Write_LogFile(LOG_ERROR,_T("DelData Thread생성 실패.\r\n"));
	}
	else {
		LSITS_Write_LogFile(LOG_PROG,_T("DelData Thread생성 성공.\r\n"));
		::DuplicateHandle(GetCurrentProcess(), DelDataThread->m_hThread,
			GetCurrentProcess(), &m_hThreadD, 0 ,FALSE, DUPLICATE_SAME_ACCESS);
		DelDataThread->ResumeThread();
	}

	return TRUE;
}

int CKillProcessDlg::StopThread( void )
{
	MSG msg;

	if(DelDataThread)
		DelDataThread->m_bAutoDelete = TRUE;
	is_DEL_Run = FALSE;
	SetEvent(m_hDelEvent);

	while(TRUE)
	{
		DWORD dwExitCode;

		BOOL retVal = ::GetExitCodeThread(m_hThreadD,&dwExitCode);
		if(retVal == FALSE)
		{
			break;
		}
		if(dwExitCode == STILL_ACTIVE)
		{
			SetEvent(m_hDelEvent);
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);					
			}
			Sleep(100);
		}
		else
		{
			LSITS_Write_LogFile(LOG_PROG,_T("DelData Thread 종료.\r\n"));
			::CloseHandle(m_hThreadD);
			DelDataThread = NULL;
			break;
		}
	}
	return 0;
}

/**
* @author 윤경섭
* @fn  UINT DelDataThreadFunc(LPVOID lpParam)
* @param LPVOID lpParam
* @remark 데이터 삭제 쓰레드
* @return UINT
* @date 19/12/03
*/
UINT DelDataThreadFunc(LPVOID lpParam)
{
	POSITION pos,pos1;
	CKillProcessDlg *pKillProcessDlg =  (CKillProcessDlg *)lpParam;
	while(pKillProcessDlg->is_DEL_Run)
	{
		const DWORD dwRet = WaitForSingleObject(pKillProcessDlg->m_hDelEvent, INFINITE);

		if(pKillProcessDlg->is_DEL_Run == FALSE)
			break;

		///자정마다 데이터를 삭제한다.
		try
		{

			TCHAR TmpPath[MAX_PATH];
			TCHAR TargetPath[MAX_PATH];
			HANDLE hFind;
			WIN32_FIND_DATA fd;
			TCHAR TemDir[MAX_PATH];

			CTimeSpan ts;
			CTime NowTime;
			NowTime = CTime::GetCurrentTime();

			_stprintf_s(TemDir,_countof(TemDir),_T("디렉토리를 삭제 시작\r\n"));
			LSITS_Write_LogFile(LOG_PROG,TemDir);



			try
			{
				//먼저 로그 디렉토리를 지정한다.
				_stprintf_s(TmpPath, _countof(TmpPath), _T("%s%s\\*.*"), CGlobal::Program_PATH, _T("log"));

				if((hFind = ::FindFirstFile(TmpPath,&fd)) != INVALID_HANDLE_VALUE)
				{
					do{
						Sleep(100);

						if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
							CString strComp = (LPCTSTR) &fd.cFileName;
							if((strComp != _T(".")) && (strComp != _T(".."))){

								ULONG val;
								val = _ttol((TCHAR *)LPCTSTR(strComp));
								if(val)
								{
									char year[5]={0,};
									char mon[3]={0,};
									char day[3] ={0,};
									int nyear,nmon,nday;
									memcpy(year,(char *)LPCTSTR(strComp),4);
									nyear = _ttoi(year);
									memcpy(mon,(char *)LPCTSTR(strComp)+4,2);
									nmon = _ttoi(mon);

									memcpy(day,(char *)LPCTSTR(strComp)+6,2);
									nday = _ttoi(day);

									CTime ct(nyear,nmon,nday,0,0,0); 

									ts = NowTime - ct;
									if(ts.GetDays() > CGlobal::INI_LOGFILE_SAVE_INTERVAL)
									{
										//로그를 뿌리면 중간에 멈추는 문제가 있다. 그래서 모든 로그를 삭제한다.
										_stprintf_s(TemDir,_countof(TemDir),_T("디렉토리를 삭제 추가.-log\\%s\r\n"),strComp);
										LSITS_Write_LogFile(LOG_PROG,TemDir);

										_stprintf_s(TargetPath, _countof(TargetPath), _T("%s%s\\%s"), CGlobal::Program_PATH, _T("log"),strComp);
										TCHAR *pPath = new TCHAR[MAX_PATH];
										_tcscpy(pPath,TargetPath);
										pKillProcessDlg->m_RemoveDirList.AddTail(pPath);
									}
								}
							}
						}
					}while(::FindNextFile(hFind, &fd));
					::FindClose(hFind);	
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



			try
			{
				for(pos = pKillProcessDlg->m_RemoveDirList.GetHeadPosition(); pos != NULL;)
				{
					TCHAR *pTargetPath = (TCHAR *)pKillProcessDlg->m_RemoveDirList.GetAt(pos);
					if(PathIsDirectory(pTargetPath))
					{
						_stprintf_s(TemDir,_countof(TemDir),_T("디렉토리를 삭제.-log\\%s\r\n"),pTargetPath);
						LSITS_Write_LogFile(LOG_PROG,TemDir);
						_tcscat(pTargetPath,"\\*.*");
						DeleteDir(pTargetPath);
					}

					pos1 = pos;
					pKillProcessDlg->m_RemoveDirList.GetNext(pos);
					pKillProcessDlg->m_RemoveDirList.RemoveAt(pos1);
					delete [] pTargetPath;
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
			LSITS_Write_LogFile(LOG_PROG,_T("디렉토리 삭제완료\r\n"));
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
		}  //전체 try catch 완료

	}//while

	return 0;
}