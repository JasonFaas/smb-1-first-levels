
// SMB1FirstLevelsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMB1FirstLevels.h"
#include "SMB1FirstLevelsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSMB1FirstLevelsDlg dialog




CSMB1FirstLevelsDlg::CSMB1FirstLevelsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSMB1FirstLevelsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSMB1FirstLevelsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, MainBtn);
}

BEGIN_MESSAGE_MAP(CSMB1FirstLevelsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSMB1FirstLevelsDlg::MainBtnClicked)
	ON_BN_CLICKED(IDC_BUTTON2, &CSMB1FirstLevelsDlg::OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON3, &CSMB1FirstLevelsDlg::OnBnClickedButtonMinus)
END_MESSAGE_MAP()


// CSMB1FirstLevelsDlg message handlers

BOOL CSMB1FirstLevelsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSMB1FirstLevelsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSMB1FirstLevelsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSMB1FirstLevelsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSMB1FirstLevelsDlg::MainBtnClicked()
{
	CString btnText;
	MainBtn.GetWindowTextW(btnText);

	if(btnText == StrToCStr("Click Me First"))
	{
		_smb1Central = NULL;
		
		TRACE("Running first video display\n");

		MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("Processing"));

		_smb1Central = new SMB1Central(2,"Test Display");
		CvRect vidProp = _smb1Central->NoThreadVideo();

		delete _smb1Central;
		_smb1Central = NULL;

		MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("SMB1 Start"));
	}
	
	else if(btnText == StrToCStr("SMB1 Start"))
	{
		TRACE("SMB1 Starting\n");
		MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("Processing"));

		DWORD thr_id1;
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartSecondMainThread,this,0,&thr_id1);
		
		MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("SMB1 Stop"));
	}
	else if(btnText == StrToCStr("SMB1 Stop"))
	{
		
		TRACE("SMB1 Stopping\n");
		MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("Processing"));

		if(_smb1Central!=NULL)
			_smb1Central->StopSMB1();
	}
	
	
}

DWORD
CSMB1FirstLevelsDlg::StartSecondMainThread(void * param){
	((CSMB1FirstLevelsDlg*)param)->RunSecondMainThread();
	return 0;
}

void
CSMB1FirstLevelsDlg::RunSecondMainThread()
{
	//TODO: Record with time
	_smb1Central = new SMB1Central(2, "First Run");
	_smb1Central->StartSMB1();

	delete _smb1Central;
	_smb1Central = NULL;

	MainBtn.SetWindowTextW((LPCTSTR)StrToCStr("SMB1 Start"));
}



void CSMB1FirstLevelsDlg::OnBnClickedButtonPlus()
{
	if(_smb1Central!=NULL)
		_smb1Central->ModifyThresh(true);
}

void CSMB1FirstLevelsDlg::OnBnClickedButtonMinus()
{
	if(_smb1Central!=NULL)
		_smb1Central->ModifyThresh(false);
}
