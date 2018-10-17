
// SMB1FirstLevelsDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "SMB1Central.h"
;

// CSMB1FirstLevelsDlg dialog
class CSMB1FirstLevelsDlg : public CDialog
{
// Construction
public:
	CSMB1FirstLevelsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SMB1FIRSTLEVELS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void MainBtnClicked();
	CButton MainBtn;

private:
	SMB1Central * _smb1Central;
	
	static DWORD StartSecondMainThread(void * param);	
	void RunSecondMainThread();
public:
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMinus();
};
