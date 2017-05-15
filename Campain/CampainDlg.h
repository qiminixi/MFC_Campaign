// CampainDlg.h : header file
//

#pragma once


// CCampainDlg dialog
class CCampainDlg : public CDialog
{
public:
	BOOL ReadConfigFile();
	BOOL WriteConfigFile();
	BOOL ReadData();//�򿪼�¼��
	void InitStatus();//�ָ���ʼ״̬���Ա㿪ʼ�µ�һ��
public:
	int m_TimeOfSteps[3];//��һ�����ڵ�ʱ��
	int m_nCurStep;//��ǰ�����еĽ׶�
	int m_nTimeOfCur;//��ǰ�ļ�ʱ
	
	int m_nStatus;//��ǰ״̬��0Ϊδ��ʼ��1Ϊ�����У�2Ϊ��ͣ��3Ϊ����
	
	CString m_strCampaigner;//��ѡ��
	CString m_strCampaignPost;//��ѡְ��
	CString m_strCurPost;//����ְ��
	
	CString m_strDataFilePath;//�����ļ�·��
	CDatabase m_database;
	CRecordset m_recordset;
	int m_nNumOfCampaigners;
	int m_nNumOfPastedCampaigners;
	
	//jgpͼƬ��ʾ
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
