
// KillProcess.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "KillProcess.h"
#include "KillProcessDlg.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKillProcessApp

BEGIN_MESSAGE_MAP(CKillProcessApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CKillProcessApp ����

CKillProcessApp::CKillProcessApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CKillProcessApp ��ü�Դϴ�.

CKillProcessApp theApp;


// CKillProcessApp �ʱ�ȭ

BOOL CKillProcessApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	/// ������ ���μ����� ���� ���� �ʵ��� �ڵ� �߰� by ���漷 2020.04.29
	HANDLE hMutexOneInstance = ::CreateMutex( NULL, TRUE, _T("AFX_JWIS_KILLPROC_8DFF6919_58B7_11D6_8719_0008C79F2040")); 
	bool AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);

	if(hMutexOneInstance != NULL) 
		::ReleaseMutex(hMutexOneInstance); 

	if(AlreadyRunning) {
		//::MessageBox(NULL, _T("�̹� ���α׷��� �������Դϴ�."), _T("���������"), MB_OK);
		return FALSE;
	}


	if(GetModuleFileName(NULL, CGlobal::Program_PATH, 256) != 0) {                // �� ���α׷��� PATH�� ���Ѵ�.
		*(_tcsrchr(CGlobal::Program_PATH, _T('\\'))+1) = '\0';                           // FULL_PATH���� ���α׷� �̸��� ������ ���� PATH�� ���Ѵ�.
		_stprintf_s(CGlobal::PROGRAM_INI_FULLPATH,_countof(CGlobal::PROGRAM_INI_FULLPATH), "%s%s", CGlobal::Program_PATH, _T("KILLPROCESS.INI"));
	}
	else
	{
		::MessageBox(NULL, _T("���α׷���ġ�� ���Ҽ� �����ϴ�."), _T("���������"), MB_OK);
		return FALSE;
	}

	CKillProcessDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

