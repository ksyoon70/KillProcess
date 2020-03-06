#if !defined(AFX_CHECKABLEGROUPBOX_H__BBBAE5EC_53CC_47CE_A858_4E7E7C24DDF1__INCLUDED_)
#define AFX_CHECKABLEGROUPBOX_H__BBBAE5EC_53CC_47CE_A858_4E7E7C24DDF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckableGroupBox.h : header file
//
//#include "..\..\Button\CButtonST\BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CCheckableGroupBox window

class CCheckableGroupBox : public CButton
{
	DECLARE_DYNAMIC(CCheckableGroupBox)

// Construction
public:
	CCheckableGroupBox();

// Attributes
public:
	void SetTitleStyle(UINT style = BS_AUTOCHECKBOX, BOOL transparent = FALSE);
	int GetCheck() const;
	void SetCheck(int nCheck);

	void SetGroupID(int nGroup);
	int	 GetGroupID() const;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckableGroupBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckableGroupBox();
	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckableGroupBox)
	//}}AFX_MSG
	afx_msg void OnClicked() ;
	DECLARE_MESSAGE_MAP()

	CButton	 m_TitleBox;//Could be check box or radio box
	//CButtonST	 m_TitleBox;//Could be check box or radio box
	UINT	 m_nGroupID;//Radio button holds same group id.
	void CheckGroupboxControls();
	CBitmap m_Bmp;
	BOOL m_Transparent;
	
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKABLEGROUPBOX_H__BBBAE5EC_53CC_47CE_A858_4E7E7C24DDF1__INCLUDED_)
