#pragma once

// CTrayIconMng 명령 대상입니다.

class CTrayIconMng : public CObject
{
public:
	CTrayIconMng();
	virtual ~CTrayIconMng();
	// 트레이 아이콘이 생성되었는지 여부
	bool m_bAdded;
	// 다이얼로그가 감춰진 상태인지 여부
	bool m_bHide;
	// 팝업메뉴 생성
	void MakePopupMenu(HWND hWnd, int x, int y);
	// 팝업메뉴의 이벤트 발생시 처리 함수
	void ProcTrayMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
	// 트레이 아이콘 생성
	BOOL AddTrayIcon(HWND hWnd);
	// 트레이 아이콘 제거
	BOOL DelTrayIcon(HWND hWnd);
};


