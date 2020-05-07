
// KillProcessDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "KillProcess.h"
#include "KillProcessDlg.h"
#include "afxdialogex.h"
#include <psapi.h>
#include <TLHELP32.H>
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PROC_NAME_WIDTH				(150)
#define DEFAULT_UPDATE_TIME					(5)
#define TIMER_NUM					(1)
#define DEFAULT_WAIT_TIME			(30)			//�⺻ ��� �ð�
#define DEFAULT_WAITUP_TIME			(30)			//�⺻ ��⵿ ��� �ð�		

#define	CHECK_INDEX					(0)
#define FILE_NAME_INDEX				(1)
#define WAIT_TIME_INDEX				(2)
#define WAKEUP_TIME_INDEX			(3)				//�� �⵿ �� �ð�
#define PATH_NAME_INDEX				(4)

#define WM_PROC_ALIVE			(WM_USER+200)	// ���μ����� ��������� �˸�.

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CKillProcessDlg ��ȭ ����




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
END_MESSAGE_MAP()


// CKillProcessDlg �޽��� ó����
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::OnInitDialog()
* @param void ����
* @remark ���̾�α� �ʱ�ȭ �Լ�
* @return void ����
* @date 20/03/03
*/
BOOL CKillProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAIN_ICON)); //icon ����
	this->SetIcon(hIcon, TRUE);
	this->SetIcon(hIcon, FALSE); //icon ����

	CRect rect;
	m_ProcList.GetClientRect(&rect);
	m_ProcList.InsertColumn(0,_T("Process Name"),LVCFMT_LEFT,PROC_NAME_WIDTH);
	m_ProcList.InsertColumn(1,_T("Process ID"),LVCFMT_LEFT,rect.Width() - PROC_NAME_WIDTH);

	//����� ���μ��� ����Ʈ �ʱ�ȭ

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
	m_monProcList.SetExtendedStyle( m_monProcList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	m_monProcList.Init();

	m_UpdateAppUseGroupBox.SetTitleStyle(BS_AUTOCHECKBOX, FALSE);
	m_UpdateAppUseGroupBox.SetGroupID(1);

	Read_INI();

	if(m_UpdateAppUseGroupBox.GetCheck() == TRUE)
	{
		BOOL ret;
		//������ ������Ʈ �Ѵ�.
		TCHAR szdstPath[MAX_PATH] = {0,};
		
		ret = CGlobal::GetPath(CGlobal::dstUpdatePath,szdstPath);
		if(PathFileExists(CGlobal::srcUpdatePath))
		{
			if(PathFileExists(szdstPath))
			{
				if ( CopyFile(CGlobal::srcUpdatePath , CGlobal::dstUpdatePath, false) == 0 ) {
					TCHAR szError[500];
					_stprintf_s(szError,_countof(szError),_T("copy %s ==> %s error\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_ProgramLogFile(szError);
				}
				else
				{
					TCHAR szLog[500];
					_stprintf_s(szLog,_countof(szLog),_T("copy %s ==> %s OK!\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_ProgramLogFile(szLog);
				}
			}
			else
			{
				CreateDir(szdstPath);
				if ( CopyFile(CGlobal::srcUpdatePath , CGlobal::dstUpdatePath, false) == 0 ) {
					TCHAR szError[500];
					_stprintf_s(szError,_countof(szError),_T("copy %s ==> %s error\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_ProgramLogFile(szError);
				}
				else
				{
					TCHAR szLog[500];
					_stprintf_s(szLog,_countof(szLog),_T("copy %s ==> %s OK!\r\n"),CGlobal::srcUpdatePath,CGlobal::dstUpdatePath);
					LSITS_Write_ProgramLogFile(szLog);
				}
			}
			
		}
	}

	UpdateProcess();

	m_myTray.m_bHide = m_bHide;
	m_myTray.AddTrayIcon(GetSafeHwnd());	//���۰� ���ÿ� Ʈ���� ������ ǥ��

	SetTimer(TIMER_NUM,m_RefreshProcessTime*1000,NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CKillProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_MINIMIZE)
	{
		OnDialogShow();
	}
	else
	{

		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CKillProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CKillProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedKillProcButton()
* @param void ����
* @remark ���μ����� ���δ�
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedKillProcButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if(UpdateData(TRUE))
	{
		KillProcess(m_ProcName);
		UpdateProcess();
	}
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedMonProcAddButton()
* @param void ����
* @remark ���� ���μ����� �߰��Ѵ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedMonProcAddButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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


		TCHAR path_buffer[MAX_PATH];        // ��ü ���
		TCHAR drive[MAX_PATH];               // ����̺� ��
		TCHAR dir[MAX_PATH];                      // ���丮 ���
		TCHAR fname[MAX_PATH];           // ���ϸ�
		TCHAR fname1[MAX_PATH];           // ���ϸ�
		TCHAR ext[MAX_PATH];                    // Ȯ���� ��

		int strLen = m_monFilePathName.GetLength();

		if(strLen > 0)
		{
			//������ ���μ����� �ִ��� Ȯ���Ѵ�.
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
			//���� ���μ����� ������ ����Ѵ�.
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
						/// ���μ����� ����Ѵ�.
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
				AfxMessageBox("�̹� ��ϵ� ���μ����� �ֽ��ϴ�",IDOK);
			}

		}
		else
		{
			AfxMessageBox("����� ���μ����� ���� �Ͻʽÿ�.",IDOK);
		}

		
	}
	
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::PrintProcessNameAndID( DWORD processID)
* @param DWORD processID
* @remark processID�� ���μ����� ����Ʈ�� �߰��Ѵ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::PrintProcessNameAndID( DWORD processID)
{
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

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnTimer(UINT_PTR nIDEvent)
* @param void ����
* @remark Ÿ�̸� �ڵ鷯. �Ź� ����Ʈ�� ������Ʈ �Ѵ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//���μ����� �����Ѵ�.
	KillTimer(nIDEvent);

	///�ʱ�ȭ �ÿ� ���� �ð��� ������ tray icon���� ����
	if(m_InitTray)
	{
		if(m_bHide == FALSE)
		{
			OnDialogShow();
		}
	
		m_InitTray = FALSE;
	}
	
	UpdateProcess();
	//���� ���μ����� �ִ��� Ȯ���Ѵ�.
	SurveilProc();
	SetTimer(nIDEvent,m_RefreshProcessTime*1000,NULL);
	CDialogEx::OnTimer(nIDEvent);
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::UpdateProcess(void)
* @param void ����
* @remark ���μ����� ������Ʈ �Ѵ�
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::UpdateProcess(void)
{

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

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
		if( aProcesses[i] != 0 )
		{
			PrintProcessNameAndID(aProcesses[i] );
		}
	}
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark ���μ��� ����Ʈ Ŭ������ �� �޽��� ó�� �Լ�
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_ProcList.GetFirstSelectedItemPosition();
	if(pos != NULL) {
		int nItem = 0;
		while(pos) {
			nItem = m_ProcList.GetNextSelectedItem(pos);

			char TmpBuf[30];
			m_ProcName = m_ProcList.GetItemText(nItem, 0);
			m_ProcID = _ttoi(m_ProcList.GetItemText(nItem, 1));
			UpdateData(FALSE);
			break;
		}
	}

	*pResult = 0;
}
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn BBOOL CKillProcessDlg::KillProcess(CString ProcessName)
* @param CString ProcessName ������ process name
* @remark process name���� ���μ��� ���̱�
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::KillProcess(CString ProcessName)
{
	BOOL b = FALSE;
	HANDLE   hSnapshot  = NULL;  
    PROCESSENTRY32 pe32 = {0};  

	// �ý��� ���μ����� ����(��, ���, ������)�� �о� �´�.
	// �����ϸ� �������� ���� �ڵ��� ��ȯ�ϰ�, �����ϸ� INVALID_HANDLE_VALUE�� ��ȯ�Ѵ�. 
    hSnapshot  = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );  

	// ��Ʈ�� ����ü ������
    pe32.dwSize = sizeof( PROCESSENTRY32 );

	// ��Ʈ������ ������ �ڷᱸ�� ��������
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
					hProcess : ���� �ڵ带 �ޱ� ���ϴ� ���μ����� �ڵ�
                    PROCESS_QUERY_INFORMATION �Ǵ� PROCESS_QUERY_LIMITED_INFORMATION
					������ �־�� ��.
                    
                    lpExitCode : ���μ��� ���� ���¸� ���� ������ ���� ������
                    
                    ���н� 0�� ��ȯ*/
                  
					GetExitCodeProcess( hProcess, &dwExitCode);
                    
                    /* BOOL WINAPI TerminateProcess
					(_In_ HANDLE hProcess, _In_ uExitCode)
                    
                    hProcess : ������ ���μ����� �ڵ��� ����
                    �ڵ��� ���� ������ �ݵ�� PROCESS_TERMINATE�� ������ ��.
                    
                    uExitCode : ���μ��� ���� �ڵ带 ����
                    ��������� ���� ���� �ΰ�� 0�� ������ �����ΰ�� -1�� �Է�*/
                    
					TerminateProcess( hProcess, dwExitCode);
					CloseHandle(hProcess);
					TCHAR szLog[MAX_PATH];
					_stprintf_s(szLog,_countof(szLog),_T("KillProcess %s\r\n"),ProcessName);
					LSITS_Write_ProgramLogFile(szLog);
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
	
	return b;
}
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::KillProcessPid(DWORD pid)
* @param DWORD pid ������ process pid
* @remark pid�� ���μ��� ���̱�
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::KillProcessPid(DWORD pid)
{
	BOOL b = FALSE;
	HANDLE   hSnapshot  = NULL;  
	PROCESSENTRY32 pe32 = {0};  
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

	return b;
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::RunProcess(CString RunFilePath, int show)
* @param CString RunFilePath ������ ���� ���
* @param int show ���� ���α׷� �������� ����
* @remark ���μ��� ����
* @return BOOL 
* @date 20/03/03
*/
BOOL CKillProcessDlg::RunProcess(CString RunFilePath, int show)
{
	// STARTUPINFO �����ϴ� ���μ����� �Ӽ��� ������ �� ����ϴ� ����ü
	STARTUPINFO si;

	// ���� ������ ���μ����� ������ ������ ����
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
	// �ڵ� �ݱ�
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	TCHAR szLog[MAX_PATH];
	_stprintf_s(szLog,_countof(szLog),_T("RunProcess %s\r\n"),RunFilePath);
	LSITS_Write_ProgramLogFile(szLog);

	return TRUE;
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedBrowseFileBtn()
* @param void ����
* @remark ������ ���μ����� ��ġ�� �����´�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedBrowseFileBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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


void CKillProcessDlg::OnBnClickedMonProcRemoveButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos,pos1;

	TCHAR fullPath[MAX_PATH];
	TCHAR path[MAX_PATH];
	TCHAR fileName[MAX_PATH];

	TCHAR szText[128] ={0,};
	LVITEM pitem;
	int nPos = 0;
	
	if(MessageBox("��ϵ� ���μ����� �����Ͻðڽ��ϱ�?", "ȯ�漳��", MB_YESNO) == IDYES)
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
				_tcscpy(fileName,pitem.pszText);  // ���� �̸� ���

				ZeroMemory(&pitem, sizeof(pitem));
				pitem.mask  = LVIF_TEXT;	
				pitem.pszText = szText ;
				pitem.cchTextMax = sizeof( szText );
				pitem.iItem = nItem;
				pitem.iSubItem = PATH_NAME_INDEX;
				m_monProcList.GetItem(&pitem);  // path ���
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


/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark ���� ����Ʈ�� Ŭ���ϸ� �ϸ� ó���ϴ� �Լ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark ���� Ŭ���� �ϸ� �������� �޽��� ����͸��� �ϴ��� Ȯ���Ѵ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedMonProcModifyButton()
* @param NMHDR *pNMHDR
* @param LRESULT *pResult
* @remark ���� ��ư�� ���� �� ���õ� ����Ʈ�� �Ϻ� ������ ���� �ȴ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedMonProcModifyButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
				AfxMessageBox("������ ����Ʈ�� ���� �Ͻʽÿ�.",IDOK);
			}
		}
	}

	

}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnDestroy()
* @param void ����
* @remark �����찡 ����� �� �޸𸮸� �����Ѵ�.
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos,pos1;
	for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
	{
		PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
		delete pInfo;
		pos1 = pos;
		m_surveilList.GetNext(pos);
		m_surveilList.RemoveAt(pos1);
	}
}
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::Read_INI(void)
* @param void ����
* @remark INI ���� �д´�
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::Read_INI(void)
{
	TCHAR buf[MAX_PATH];
	TCHAR TempBuf[MAX_PATH];

	TCHAR path_buffer[MAX_PATH];        // ��ü ���
	TCHAR drive[MAX_PATH];               // ����̺� ��
	TCHAR dir[MAX_PATH];                      // ���丮 ���
	TCHAR fname[MAX_PATH];           // ���ϸ�
	TCHAR ext[MAX_PATH];                    // Ȯ���� ��

	// m_surveilList ����
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
	//Ÿ�̸� ������Ʈ �ֱ�
	m_RefreshProcessTime = GetPrivateProfileInt(_T("CONFIG"),_T("REFRESH_PROC_TIME"),DEFAULT_UPDATE_TIME,CGlobal::PROGRAM_INI_FULLPATH);

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
			_tcscpy(pInfo->execPath,buf);  //���丮 ����
			_stprintf_s(buf,_countof(buf),_T("%s%s"),fname,ext);
			_tcscpy(pInfo->execName,buf);  //filename ����

			_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_USE_WATCHDOG"),i);
			GetPrivateProfileString(_T("CONFIG"), buf,  "N" , TempBuf, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);

			if(_tcscmp(TempBuf, "Y") == 0)
				pInfo->useWatchdog = TRUE;
			else
				pInfo->useWatchdog = FALSE;
			/// �ִ� ��� �ð�
			_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_MAX_WAIT_TIME"),i);
			pInfo->maxWaitSec =  GetPrivateProfileInt(_T("CONFIG"),buf,30,CGlobal::PROGRAM_INI_FULLPATH);

			/// �⵿ ��� �ð�
			_stprintf_s(buf,_countof(buf),_T("PROCESS_%d_WAKEUP_TIME"),i);
			pInfo->wakeupTime =  GetPrivateProfileInt(_T("CONFIG"),buf,30,CGlobal::PROGRAM_INI_FULLPATH);

			m_surveilList.AddTail(pInfo);
		}

	}

	//listcontrol ��� ������ �� ����

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

		//�߰�..
		if(pInfo != NULL)
		{
			for(int i = 0; i < nCount ;  i++)
			{
				/// ���μ����� ����Ѵ�.
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


	//������Ʈ ����
	GetPrivateProfileString(_T("CONFIG"), _T("UPDEATE_APP_USE"),  "N" , TempBuf, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);

	if(_tcscmp(TempBuf, "Y") == 0)
	{
		m_UpdateAppUseGroupBox.SetCheck(TRUE);
	}
	else
	{
		m_UpdateAppUseGroupBox.SetCheck(FALSE);
	}
	//������Ʈ �� ���丮 �б�
	GetPrivateProfileString(_T("CONFIG"), _T("SRC_UPDEATE_PATH"),  "" , CGlobal::srcUpdatePath, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);
	m_srcUpdateAppPath.Format(_T("%s"),CGlobal::srcUpdatePath);
	GetPrivateProfileString(_T("CONFIG"), _T("DST_UPDEATE_PATH"),  "" , CGlobal::dstUpdatePath, MAX_PATH, CGlobal::PROGRAM_INI_FULLPATH);
	m_dstUpdateAppPath.Format(_T("%s"),CGlobal::dstUpdatePath);

	UpdateData(FALSE);

}
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::Write_INI(void)
* @param void ����
* @remark INI ���� ����
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::Write_INI(void)
{
	TCHAR buf[MAX_PATH];
	CGlobal::REALTIME_MON_PROC_COUNT = m_monProcList.GetItemCount();

	CHeaderCtrl *pHeaderCtrl;

	pHeaderCtrl = m_monProcList.GetHeaderCtrl();
	int nCount = pHeaderCtrl->GetItemCount();

	if(MessageBox("���� ���μ��� �������� �����Ͻðڽ��ϱ�?", "ȯ�漳��", MB_YESNO) == IDYES)
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
					case FILE_NAME_INDEX: /// ��������..
						_tcscpy(pInfo->execName,pitem.pszText);
						break;
					case WAIT_TIME_INDEX: /// Ÿ�Ӿƿ�
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


		//������Ʈ ����
		WritePrivateProfileString(_T("CONFIG"), _T("UPDEATE_APP_USE"),(m_UpdateAppUseGroupBox.GetCheck()) ? _T("Y") : _T("N"), CGlobal::PROGRAM_INI_FULLPATH);

		//������Ʈ �� ���丮 ����
		WritePrivateProfileString(_T("CONFIG"), _T("SRC_UPDEATE_PATH"),CGlobal::srcUpdatePath, CGlobal::PROGRAM_INI_FULLPATH);
		WritePrivateProfileString(_T("CONFIG"), _T("DST_UPDEATE_PATH"),CGlobal::dstUpdatePath,CGlobal::PROGRAM_INI_FULLPATH);

		WritePrivateProfileString(NULL,NULL,NULL,CGlobal::PROGRAM_INI_FULLPATH);
	}

	
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedReadIniButton()
* @param void ����
* @remark INI ���� �д´�
* @return void ����
* @date 20/03/03
*/
void CKillProcessDlg::OnBnClickedReadIniButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	Read_INI();
}

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::OnBnClickedWriteIniButton()
* @param void ����
* @remark INI ���� ����
* @return void ����
* @date 20/03/04
*/
void CKillProcessDlg::OnBnClickedWriteIniButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	Write_INI();
}


/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::UpdateSurveilList(TCHAR *fullPathName, int param)
* @param TCHAR *fullPathName
* @param int param
* @remark ������ ������ ������Ʈ�� �Ѵ�. ��ٸ��� �ð�.
* @return void ����
* @date 20/03/04
*/
void CKillProcessDlg::UpdateSurveilList(TCHAR *fullPathName, int param1 , int param2)
{
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

/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn void CKillProcessDlg::SurveilProc(void)
* @param void ����
* @remark ���μ����� �����Ѵ�
* @return void ����
* @date 20/03/04
*/
void CKillProcessDlg::SurveilProc(void)
{
	POSITION pos;
	TCHAR filename[MAX_PATH];
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
				}
			}
			//���� �� ���
			pInfo->oldActive = pInfo->isActive;
			if(find)
			{
				
				pInfo->isActive = TRUE;
				pInfo->curWakupSecond = 0;

				if(pInfo->useWatchdog == TRUE)
				{
					pInfo->curWaitSecond += m_RefreshProcessTime*1000;
					if(pInfo->maxWaitSec*1000 <= pInfo->curWaitSecond)
					{
						//���μ����� ���δ�.
						KillProcess(pInfo->execName);
						pInfo->curWaitSecond = 0;  //���̸�... watchdog ī��Ʈ �ʱ�ȭ
					}
				}
			}
			else
			{
				pInfo->isActive =FALSE;
				//���μ����� �����Ǿ�����... �� �⵿ ���θ� Ȯ���Ѵ�.
				if(pInfo->oldActive == TRUE)
				{
					pInfo->curWakupSecond = 0;
				}
				else
				{
					pInfo->curWakupSecond +=  m_RefreshProcessTime*1000;

					if(pInfo->wakeupTime*1000 <= pInfo->curWakupSecond)
					{
						//���μ����� �츰��.
						RunProcess(pInfo->execFullPath);
					}
				}
			}
		}
		m_surveilList.GetNext(pos);
	}
}
/**
* @author ���漷
* @file KillProcessDlg.cpp
* @fn BOOL CKillProcessDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
* @param CWnd* pWnd
* @param COPYDATASTRUCT* pCopyDataStruct
* @remark COPYDATASTRUCT ������ �޽����� ó���Ѵ�.
* @return void ����
* @date 20/03/04
*/
BOOL CKillProcessDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	m_pcds = (PCOPYDATASTRUCT)pCopyDataStruct;

	CopyMemory(&m_wdMsg,m_pcds->lpData,m_pcds->cbData);

	PostMessage(WM_PROC_ALIVE,(WPARAM)&m_wdMsg, NULL);

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CKillProcessDlg::OnBnClickedSrcBrowseFileBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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


void CKillProcessDlg::OnBnClickedDestBrowseFileBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if(m_strProgramName.IsEmpty())
	{
		AfxMessageBox("���� ������Ʈ ������ ���� �ϼ���.",IDOK);
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



//Ʈ���� �������� Ŭ������ ���� �޽��� �ڵ鷯
long CKillProcessDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	m_myTray.ProcTrayMsg(GetSafeHwnd(), wParam, lParam);
	return 0;
}

//Ʈ���� ������ �˾��޴��� ���� �޴� �޽��� �ڵ鷯
void CKillProcessDlg::OnAppExit(void)
{
	m_myTray.DelTrayIcon(GetSafeHwnd());
	CDialog::OnCancel();
}

//Ʈ���� ������ ���̱�/����� �޴� �޽��� �ڵ鷯
void CKillProcessDlg::OnDialogShow(void)
{
	if(!m_bHide) ShowWindow(false);	//���̴� ���¶�� �����
	else ShowWindow(true);			//������ ���¶�� ���̰�
	m_bHide = !m_bHide;
	m_myTray.m_bHide = m_bHide;
}

/**
*		@fn		LRESULT CKillProcessDlg::OnProcAliveFunc(WPARAM wParam, LPARAM lParam)
*		@brief	WM �� ������ ������ �Լ� (�ݹ��Լ�)
*		@param	wParam : ���ῡ ���� ���������� ������ �ִ�.
*		@param	lParam : ���� �̿��� �����͸� ������ ���� ��쿡 ���� �����Ѵ�.
*		@remark	WM_COPYDATA �޽��� ó�� �ݹ��Լ�
*/
LRESULT CKillProcessDlg::OnProcAliveFunc(WPARAM wParam, LPARAM lParam)
{
	WATCHDOG_MSG *pWdMsg = (WATCHDOG_MSG *)wParam;
	if(pWdMsg)
	{
		TCHAR procname[MAX_PATH] = { 0,};

		CopyMemory(procname,pWdMsg->processName,pWdMsg->strlen);

		POSITION pos;

		for(pos = m_surveilList.GetHeadPosition(); pos != NULL;)
		{
			PSURVEIL_PROC_INFO pInfo = (PSURVEIL_PROC_INFO)m_surveilList.GetAt(pos);
			if(pInfo)
			{
				//�ش� ���μ����� ã���� ������Ʈ�� ���ش�.
				if( _tcscmp(pInfo->execName, procname) == 0)
				{
					pInfo->curWaitSecond = 0;
					break;
				}
			}
			m_surveilList.GetNext(pos);
		}
	}
	

	return TRUE;
}