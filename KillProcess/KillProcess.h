
// KillProcess.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CKillProcessApp:
// �� Ŭ������ ������ ���ؼ��� KillProcess.cpp�� �����Ͻʽÿ�.
//

class CKillProcessApp : public CWinApp
{
public:
	CKillProcessApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CKillProcessApp theApp;