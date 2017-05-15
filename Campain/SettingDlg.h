#pragma once


// CSettingDlg dialog

class CSettingDlg : public CDialog
{
public:
	CString m_strDataFilePath;
	CString m_strPicturePath;
private:
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonSelectFile();
public:
	int m_nTimeOfStep1;
public:
	int m_nTimeOfStep2;
public:
	int m_nTimeOfStep3;
public:
//	afx_msg void OnEnUpdateStep1();
public:
	afx_msg void OnBnClickedButtonSelectPicture();
};
