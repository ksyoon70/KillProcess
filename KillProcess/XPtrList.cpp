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
* @author 윤경섭
* @file XPtrList.cpp
* @fn CXPtrList::CXPtrList()
* @param void 없음
* @remark CXPtrList 생성자, 포인터 리스트로 멀티 쓰레드에서 사용 가능하도록 criticalsection이 추가 되었다.
* @return void 없음
* @date   20/02/20
*/
CXPtrList::CXPtrList()
{
	::InitializeCriticalSection(&m_cs);
}
/**
* @author 윤경섭
* @file XPtrList.cpp
* @fn CXPtrList::~CXPtrList()
* @param void 없음
* @remark CXPtrList 소멸자
* @return void 없음
* @date   20/02/20
*/
CXPtrList::~CXPtrList()
{
	::DeleteCriticalSection(&m_cs);
}
/**
* @author 윤경섭
* @file XPtrList.cpp
* @fn void CXPtrList::AddTail(void* pObject)
* @param void* pObject
* @remark CXPtrList에 item 추가
* @return void 없음
* @date   20/02/20
*/
void CXPtrList::AddTail(void* pObject)
{
	EnterCriticalSection(&m_cs);

	CPtrList::AddTail(pObject);

	LeaveCriticalSection(&m_cs);
}
/**
* @author 윤경섭
* @file XPtrList.cpp
* @fn void* CXPtrList::RemoveHead()
* @param void* pObject
* @remark CXPtrList에서 head 삭제
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
* @author 윤경섭
* @file XPtrList.cpp
* @fn void CXPtrList::RemoveAll()
* @param void 없음
* @remark CXPtrList에서 모든 리스트 삭제
* @return void 없음
* @date   20/02/20
*/
void CXPtrList::RemoveAll()
{
	EnterCriticalSection(&m_cs);

	CPtrList::RemoveAll();

	LeaveCriticalSection(&m_cs);
}
/**
* @author 윤경섭
* @file XPtrList.cpp
* @fn POSITION CXPtrList::Find(void *pObject)
* @param void* pObject
* @remark CXPtrList에서 item을 찾음
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
* @author 윤경섭
* @file XPtrList.cpp
* @fn void CXPtrList::RemoveAt(POSITION pos)
* @param POSITION pos
* @remark CXPtrList에서 pos에 해당하는 item 삭제
* @return void 없음
* @date   20/02/20
*/
void CXPtrList::RemoveAt(POSITION pos)
{
	EnterCriticalSection(&m_cs);

	CPtrList::RemoveAt(pos);

	LeaveCriticalSection(&m_cs);
}
/**
* @author 윤경섭
* @file XPtrList.cpp
* @fn POSITION CXPtrList::FindIndex( int nIndex )
* @param int nIndex
* @remark CXPtrList에서 index에 해당하는 item의 pos를 리턴
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
* @author 윤경섭
* @file XPtrList.cpp
* @fn DWORD CXPtrList::GetCount()
* @param void 없음
* @remark CXPtrList의 item 갯수를 리턴
* @return DWORD dwCount 아이템 갯수
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
* @author 윤경섭
* @file XPtrList.cpp
* @fn void CXPtrList::RemovePtr(void* pObject)
* @param void* pObject
* @remark CXPtrList에서 pObject 삭제
* @return void 없음
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
