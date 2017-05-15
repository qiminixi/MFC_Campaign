// Campain.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCampainApp:
// See Campain.cpp for the implementation of this class
//

class CCampainApp : public CWinApp
{
public:
	CCampainApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCampainApp theApp;