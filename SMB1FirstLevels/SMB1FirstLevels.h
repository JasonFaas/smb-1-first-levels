
// SMB1FirstLevels.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSMB1FirstLevelsApp:
// See SMB1FirstLevels.cpp for the implementation of this class
//

class CSMB1FirstLevelsApp : public CWinAppEx
{
public:
	CSMB1FirstLevelsApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSMB1FirstLevelsApp theApp;