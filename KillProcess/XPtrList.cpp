// XPtrList.cpp: implementation of the CXPtrList class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XPtrList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**
* @author ���漷
* @file XPtrList.cpp
* @fn CXPtrList::CXPtrList()
* @param void ����
* @remark CXPtrList ������, ������ ����Ʈ�� ��Ƽ �����忡�� ��� �����ϵ��� criticalsection�� �߰� �Ǿ���.
* @return void ����
* @date   20/02/20
*/
CXPtrList::CXPtrList()
{
	::InitializeCriticalSection(&m_cs);
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn CXPtrList::~CXPtrList()
* @param void ����
* @remark CXPtrList �Ҹ���
* @return void ����
* @date   20/02/20
*/
CXPtrList::~CXPtrList()
{
	::DeleteCriticalSection(&m_cs);
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn void CXPtrList::AddTail(void* pObject)
* @param void* pObject
* @remark CXPtrList�� item �߰�
* @return void ����
* @date   20/02/20
*/
void CXPtrList::AddTail(void* pObject)
{
	EnterCriticalSection(&m_cs);

	CPtrList::AddTail(pObject);

	LeaveCriticalSection(&m_cs);
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn void* CXPtrList::RemoveHead()
* @param void* pObject
* @remark CXPtrList���� head ����
* @return void* pObject
* @date   20/02/20
*/
void* CXPtrList::RemoveHead()
{
	void* pObject = NULL;

	EnterCriticalSection(&m_cs);

	pObject = CPtrList::RemoveHead();

	LeaveCriticalSection(&m_cs);

	return pObject;
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn void CXPtrList::RemoveAll()
* @param void ����
* @remark CXPtrList���� ��� ����Ʈ ����
* @return void ����
* @date   20/02/20
*/
void CXPtrList::RemoveAll()
{
	EnterCriticalSection(&m_cs);

	CPtrList::RemoveAll();

	LeaveCriticalSection(&m_cs);
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn POSITION CXPtrList::Find(void *pObject)
* @param void* pObject
* @remark CXPtrList���� item�� ã��
* @return POSITION pos
* @date   20/02/20
*/
POSITION CXPtrList::Find(void *pObject)
{
	POSITION pos = NULL;

	EnterCriticalSection(&m_cs);

	pos = CPtrList::Find(pObject);

	LeaveCriticalSection(&m_cs);

	return pos;
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn void CXPtrList::RemoveAt(POSITION pos)
* @param POSITION pos
* @remark CXPtrList���� pos�� �ش��ϴ� item ����
* @return void ����
* @date   20/02/20
*/
void CXPtrList::RemoveAt(POSITION pos)
{
	EnterCriticalSection(&m_cs);

	CPtrList::RemoveAt(pos);

	LeaveCriticalSection(&m_cs);
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn POSITION CXPtrList::FindIndex( int nIndex )
* @param int nIndex
* @remark CXPtrList���� index�� �ش��ϴ� item�� pos�� ����
* @return POSITION pos
* @date   20/02/20
*/
POSITION CXPtrList::FindIndex( int nIndex )
{
	POSITION pos = NULL;

	EnterCriticalSection(&m_cs);

	pos = CPtrList::FindIndex( nIndex );

	LeaveCriticalSection(&m_cs);

	return pos;
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn DWORD CXPtrList::GetCount()
* @param void ����
* @remark CXPtrList�� item ������ ����
* @return DWORD dwCount ������ ����
* @date   20/02/20
*/
DWORD CXPtrList::GetCount()
{
	DWORD dwCount = 0;

	EnterCriticalSection(&m_cs);

	dwCount = CPtrList::GetCount();

	LeaveCriticalSection(&m_cs);

	return dwCount;
}
/**
* @author ���漷
* @file XPtrList.cpp
* @fn void CXPtrList::RemovePtr(void* pObject)
* @param void* pObject
* @remark CXPtrList���� pObject ����
* @return void ����
* @date   20/02/20
*/
void CXPtrList::RemovePtr(void* pObject)
{
	POSITION pos = NULL;

	EnterCriticalSection(&m_cs);

	pos = CPtrList::Find( pObject );
	if( pos )
		CPtrList::RemoveAt( pos );

	LeaveCriticalSection(&m_cs);
}
