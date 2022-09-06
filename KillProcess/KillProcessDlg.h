
// KillProcessDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "CheckListCtrl.h"
#include "CheckableGroupBox.h"
#include "TrayIconMng.h"
#include "afxwin.h"
#include "XPtrList.h"

/// 모니터링 프로세스를 하기 위한 구조체
typedef struct _SURVEIL_PROC_INFO
{
	TCHAR execName[MAX_PATH];
	TCHAR execPath[MAX_PATH];
	TCHAR execFullPath[MAX_PATH];
	BOOL useWatchdog;
	int curWaitSecond;		//현재 초
	int maxWaitSec;		//Process kill 최대 대기 시간
	int wakeupTime;		//재기동 대기 시간
	int curWakupSecond; //현재 재기동 초
	BOOL isActive;		//현재 살아 있는지 여부
	BOOL oldActive;
}SURVEIL_PROC_INFO,*PSURVEIL_PROC_INFO;
#pragma pack (push,1)

typedef struct _WATCHDOG_MSG
{
	TCHAR processName[MAX_PATH];
	int strlen;
}WATCHDOG_MSG;
#pragma pack (pop)

/// CKillProcessDlg 대화 상자
class CKillProcessDlg : public CDialogEx
{
// 생성입니다.
public:
	CKillProcessDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CKillProcessDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KILLPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CImageList	m_checkLCImgList;
	CString m_strProgramName;			//실행파일이름

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 프로세스 리스트
	CListCtrl m_ProcList;
	// 프로세스 이름
	CString m_ProcName;
	DWORD m_ProcID;
	afx_msg void OnBnClickedKillProcButton();
	afx_msg void OnBnClickedMonProcAddButton();
	void PrintProcessNameAndID(DWORD processID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 프로세스를 업데이트 한다
	void UpdateProcess(void);
	int m_listIndex;
	afx_msg void OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult);
	//프로세스를 죽인다.
	BOOL KillProcess(CString ProcessName);
	// pid로 프로세스 죽이기
	BOOL KillProcessPid(DWORD pid);
	// 프로세스 실행
	BOOL RunProcess(CString RunFilePath, int show = 1);
	afx_msg void OnBnClickedBrowseFileBtn();
	// 실행할 파일 이름
	CString m_monFilePathName;
	afx_msg void OnBnClickedMonProcRemoveButton();
	// 모니터 프로세스 리스트
	CCheckListCtrl m_monProcList;
	afx_msg void OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedMonProcModifyButton();
	// 와치독 대기시간
	unsigned int m_waitTime;

	PSURVEIL_PROC_INFO m_pProcInfo;
	afx_msg void OnDestroy();
	// INI 값을 읽는다
	void Read_INI(void);
	// INI 값을 쓴다
	void Write_INI(void);
	afx_msg void OnBnClickedReadIniButton();
	afx_msg void OnBnClickedWriteIniButton();
	CXPtrList m_surveilList;			//감시 리스트
	// 감시 리스트를 업데이트 한다
	void UpdateSurveilList(TCHAR *fullPathName, int param1 , int param2);
	// 프로세스를 깨울 때 기다리는 시간 (초)
	int m_wakeupTime;
	// 프로세스를 감시한다
	void SurveilProc(void);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	WATCHDOG_MSG m_wdMsg;
	COPYDATASTRUCT *m_pcds;
	// 업데이트 기능을 사용할지 여부
	CCheckableGroupBox m_UpdateAppUseGroupBox;
	// 업데이트 할 어플리케이션 이름 (경로포함)
	CString m_srcUpdateAppPath;
	// 복사할 Application Dir
	CString m_dstUpdateAppPath;
	afx_msg void OnBnClickedSrcBrowseFileBtn();
	afx_msg void OnBnClickedDestBrowseFileBtn();
	afx_msg LRESULT OnProcAliveFunc(WPARAM wParam, LPARAM lParam);
	// 프로세스 업데이트 주기
	UINT m_RefreshProcessTime;

	// 트레이 아이콘 관리용 객체
	CTrayIconMng m_myTray;
	// 트레이 아이콘 보이기/숨기기 여부
	bool m_bHide;
	long OnTrayIcon(WPARAM wParam, LPARAM lParam);
	void OnAppExit(void);
	void OnDialogShow(void);

private:
	// 초기 트레이 상태
	BOOL m_InitTray;
public:
	// 버전표시
	CStatic m_Version;
	afx_msg void OnNMCustomdrawMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
};
