
// KillProcessDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "CheckListCtrl.h"
#include "CheckableGroupBox.h"
#include "TrayIconMng.h"
#include "afxwin.h"
#include "XPtrList.h"

/// ����͸� ���μ����� �ϱ� ���� ����ü
typedef struct _SURVEIL_PROC_INFO
{
	TCHAR execName[MAX_PATH];
	TCHAR execPath[MAX_PATH];
	TCHAR execFullPath[MAX_PATH];
	BOOL useWatchdog;
	int curWaitSecond;		//���� ��
	int maxWaitSec;		//Process kill �ִ� ��� �ð�
	int wakeupTime;		//��⵿ ��� �ð�
	int curWakupSecond; //���� ��⵿ ��
	BOOL isActive;		//���� ��� �ִ��� ����
	BOOL oldActive;
}SURVEIL_PROC_INFO,*PSURVEIL_PROC_INFO;
#pragma pack (push,1)

typedef struct _WATCHDOG_MSG
{
	TCHAR processName[MAX_PATH];
	int strlen;
}WATCHDOG_MSG;
#pragma pack (pop)

/// CKillProcessDlg ��ȭ ����
class CKillProcessDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CKillProcessDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	virtual ~CKillProcessDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KILLPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;
	CImageList	m_checkLCImgList;
	CString m_strProgramName;			//���������̸�

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// ���μ��� ����Ʈ
	CListCtrl m_ProcList;
	// ���μ��� �̸�
	CString m_ProcName;
	DWORD m_ProcID;
	afx_msg void OnBnClickedKillProcButton();
	afx_msg void OnBnClickedMonProcAddButton();
	void PrintProcessNameAndID(DWORD processID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// ���μ����� ������Ʈ �Ѵ�
	void UpdateProcess(void);
	int m_listIndex;
	afx_msg void OnLvnItemchangedProcList(NMHDR *pNMHDR, LRESULT *pResult);
	//���μ����� ���δ�.
	BOOL KillProcess(CString ProcessName);
	// pid�� ���μ��� ���̱�
	BOOL KillProcessPid(DWORD pid);
	// ���μ��� ����
	BOOL RunProcess(CString RunFilePath, int show = 1);
	afx_msg void OnBnClickedBrowseFileBtn();
	// ������ ���� �̸�
	CString m_monFilePathName;
	afx_msg void OnBnClickedMonProcRemoveButton();
	// ����� ���μ��� ����Ʈ
	CCheckListCtrl m_monProcList;
	afx_msg void OnNMClickMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedMonProcModifyButton();
	// ��ġ�� ���ð�
	unsigned int m_waitTime;

	PSURVEIL_PROC_INFO m_pProcInfo;
	afx_msg void OnDestroy();
	// INI ���� �д´�
	void Read_INI(void);
	// INI ���� ����
	void Write_INI(void);
	afx_msg void OnBnClickedReadIniButton();
	afx_msg void OnBnClickedWriteIniButton();
	CXPtrList m_surveilList;			//���� ����Ʈ
	// ���� ����Ʈ�� ������Ʈ �Ѵ�
	void UpdateSurveilList(TCHAR *fullPathName, int param1 , int param2);
	// ���μ����� ���� �� ��ٸ��� �ð� (��)
	int m_wakeupTime;
	// ���μ����� �����Ѵ�
	void SurveilProc(void);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	WATCHDOG_MSG m_wdMsg;
	COPYDATASTRUCT *m_pcds;
	// ������Ʈ ����� ������� ����
	CCheckableGroupBox m_UpdateAppUseGroupBox;
	// ������Ʈ �� ���ø����̼� �̸� (�������)
	CString m_srcUpdateAppPath;
	// ������ Application Dir
	CString m_dstUpdateAppPath;
	afx_msg void OnBnClickedSrcBrowseFileBtn();
	afx_msg void OnBnClickedDestBrowseFileBtn();
	afx_msg LRESULT OnProcAliveFunc(WPARAM wParam, LPARAM lParam);
	// ���μ��� ������Ʈ �ֱ�
	UINT m_RefreshProcessTime;

	// Ʈ���� ������ ������ ��ü
	CTrayIconMng m_myTray;
	// Ʈ���� ������ ���̱�/����� ����
	bool m_bHide;
	long OnTrayIcon(WPARAM wParam, LPARAM lParam);
	void OnAppExit(void);
	void OnDialogShow(void);

private:
	// �ʱ� Ʈ���� ����
	BOOL m_InitTray;
public:
	// ����ǥ��
	CStatic m_Version;
	afx_msg void OnNMCustomdrawMonProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
};
