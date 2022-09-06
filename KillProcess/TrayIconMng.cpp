// TrayIconMng.cpp : 구현 파일입니다.
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


// 팝업메뉴 생성
void CTrayIconMng::MakePopupMenu(HWND hWnd, int x, int y)
{
	try{
		//팝업 메뉴를 생성하고 메뉴 구성
		HMENU hMenu = CreatePopupMenu();
		if(m_bHide)		//다이얼로그가 감춰진 상태라면 
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("다이얼로그 보이기"));
		else			//다이얼로그가 숨겨진 상태라면
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("다이얼로그 감추기"));

		AppendMenu(hMenu, MF_STRING, WM_APP_EXIT, _T("종료"));

		SetForegroundWindow(hWnd);//생성된 팝업메뉴 밖을 클릭할 때 팝업 닫기
		//팝업 메뉴 띄우기
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


// 팝업메뉴의 이벤트 발생시 처리 함수
void CTrayIconMng::ProcTrayMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try{
		HMENU hMenu = NULL;
		POINT pos;

		if(lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pos);
			MakePopupMenu(hWnd, pos.x, pos.y); //팝업 메뉴 생성 및 출력
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

// 트레이 아이콘 생성
BOOL CTrayIconMng::AddTrayIcon(HWND hWnd)
{
	try{
		//if(m_bAdded)		//이미 트레이 아이콘이 있다면 종료
		//	return FALSE;

		NOTIFYICONDATA nid;	//아이콘을 생성하여 설정
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		sprintf((char*)nid.szTip, (char*)_T("KillProcess"));
		nid.uID = 0;
		nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAIN_ICON));

		if(Shell_NotifyIcon(NIM_ADD, &nid)==0) //트레이 아이콘 표시
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

// 트레이 아이콘 제거
BOOL CTrayIconMng::DelTrayIcon(HWND hWnd)
{
	try{
		NOTIFYICONDATA nid;

		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uFlags = NULL;
		if(Shell_NotifyIcon(NIM_DELETE, &nid)==0) //트레이 아이콘 삭제
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
