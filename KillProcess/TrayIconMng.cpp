// TrayIconMng.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "TrayIconMng.h"
#include "resource.h"
#include "Global.h"

// CTrayIconMng

CTrayIconMng::CTrayIconMng()
: m_bAdded(false)
, m_bHide(false)
{
}

CTrayIconMng::~CTrayIconMng()
{
}


// �˾��޴� ����
void CTrayIconMng::MakePopupMenu(HWND hWnd, int x, int y)
{
	try{
		//�˾� �޴��� �����ϰ� �޴� ����
		HMENU hMenu = CreatePopupMenu();
		if(m_bHide)		//���̾�αװ� ������ ���¶�� 
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("���̾�α� ���̱�"));
		else			//���̾�αװ� ������ ���¶��
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("���̾�α� ���߱�"));

		AppendMenu(hMenu, MF_STRING, WM_APP_EXIT, _T("����"));

		SetForegroundWindow(hWnd);//������ �˾��޴� ���� Ŭ���� �� �˾� �ݱ�
		//�˾� �޴� ����
		TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
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


// �˾��޴��� �̺�Ʈ �߻��� ó�� �Լ�
void CTrayIconMng::ProcTrayMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try{
		HMENU hMenu = NULL;
		POINT pos;

		if(lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pos);
			MakePopupMenu(hWnd, pos.x, pos.y); //�˾� �޴� ���� �� ���
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

// Ʈ���� ������ ����
BOOL CTrayIconMng::AddTrayIcon(HWND hWnd)
{
	try{
		//if(m_bAdded)		//�̹� Ʈ���� �������� �ִٸ� ����
		//	return FALSE;

		NOTIFYICONDATA nid;	//�������� �����Ͽ� ����
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		sprintf((char*)nid.szTip, (char*)_T("KillProcess"));
		nid.uID = 0;
		nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAIN_ICON));

		if(Shell_NotifyIcon(NIM_ADD, &nid)==0) //Ʈ���� ������ ǥ��
			return FALSE;
		m_bAdded = true;	
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

// Ʈ���� ������ ����
BOOL CTrayIconMng::DelTrayIcon(HWND hWnd)
{
	try{
		NOTIFYICONDATA nid;

		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uFlags = NULL;
		if(Shell_NotifyIcon(NIM_DELETE, &nid)==0) //Ʈ���� ������ ����
			return FALSE;
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
