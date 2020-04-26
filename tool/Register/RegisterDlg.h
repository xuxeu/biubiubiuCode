// RegisterDlg.h : header file
//

#if !defined(AFX_REGISTERDLG_H__1A4AE7D3_7A29_4F97_8915_BEE0E1B2687F__INCLUDED_)
#define AFX_REGISTERDLG_H__1A4AE7D3_7A29_4F97_8915_BEE0E1B2687F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
// Construction
public:
	CRegisterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTER_DIALOG };
	CEdit	m_ctrlInfo;
	CEdit	m_ctrlSerial4;
	CEdit	m_ctrlSerial3;
	CEdit	m_ctrlSerial1;
	CEdit	m_ctrlSerial2;
	CButton	m_ctrlOK;
	CString	m_strApply1;
	CString	m_strApply2;
	CString	m_strApply3;
	CString	m_strApply4;
	CString	m_strSerial1;
	CString	m_strSerial2;
	CString	m_strSerial3;
	CString	m_strSerial4;
	CString	m_strInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	afx_msg void OnChangeEditSerial1();
	afx_msg void OnChangeEditSerial2();
	afx_msg void OnChangeEditSerial3();
	afx_msg void OnChangeEditSerial4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	char m_applyCodeBuf[32];	//×¢²áÉêÇëÂëµÄ»º´æ¿Õ¼ä
	char m_applyCode[4][5];		//½«×¢²áÉêÇëÂë£¨16×Ö·û£©·Ö½âÎª4¶Î£¨Ã¿4¸ö×Ö·ûÎªÒ»¶Î£©
	FILE *m_fpApplyCode;	//×¢²áÉêÇëÂëµÄÎÄ¼þ¾ä±ú
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__1A4AE7D3_7A29_4F97_8915_BEE0E1B2687F__INCLUDED_)
