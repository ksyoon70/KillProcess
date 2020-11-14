// XPtrList.h: interface for the CXPtrList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XPTRLIST_H__A6027E9C_E55B_40C4_8B3A_AD4A9E500023__INCLUDED_)
#define AFX_XPTRLIST_H__A6027E9C_E55B_40C4_8B3A_AD4A9E500023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXPtrList : public CPtrList  
{
private:
	CRITICAL_SECTION	m_cs;

public:
	void		RemovePtr(void* pObject);
	void		RemoveAt( POSITION pos);
	POSITION	Find(void* pObject);
	void		RemoveAll();
	void		AddTail(void* pObject);
	void*		RemoveHead();

	POSITION	FindIndex( int nIndex );

	DWORD		GetCount();

	CXPtrList();
	virtual ~CXPtrList();

};

#endif // !defined(AFX_XPTRLIST_H__A6027E9C_E55B_40C4_8B3A_AD4A9E500023__INCLUDED_)
