// CampainDlg.h : header file
//

#pragma once


// CCampainDlg dialog
class CCampainDlg : public CDialog
{
public:
	BOOL ReadConfigFile();
	BOOL WriteConfigFile();
	BOOL ReadData();//打开记录集
	void InitStatus();//恢复初始状态，以便开始新的一轮
public:
	int m_TimeOfSteps[3];//第一个环节的时间
	int m_nCurStep;//当前所进行的阶段
	int m_nTimeOfCur;//当前的计时
	
	int m_nStatus;//当前状态：0为未开始，1为进行中，2为暂停，3为结束
	
	CString m_strCampaigner;//竞选者
	CString m_strCampaignPost;//竞选职务
	CString m_strCurPost;//现任职务
	
	CString m_strDataFilePath;//数据文件路径
	CDatabase m_database;
	CRecordset m_recordset;
	int m_nNumOfCampaigners;
	int m_nNumOfPastedCampaigners;
	
	//jgp图片显示
	HBITMAP m_hBitmap;
	CString m_strPicPathName;
	//-----------------------
// Construction
public:
	CCampainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CAMPAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedStartOrSuspend();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnBnClickedStop();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnClose();
public:
	afx_msg void OnBnClickedNext();
public:
	afx_msg void OnBnClickedPre();
public:
	afx_msg void OnBnClickedButtonSetting();
public:
	afx_msg void OnBnClickedNextStep();
};
