// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Campain.h"
#include "SettingDlg.h"


// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
	, m_nTimeOfStep1(0)
	, m_nTimeOfStep2(0)
	, m_nTimeOfStep3(0)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_STEP1,m_nTimeOfStep1);
	DDX_Text(pDX,IDC_STEP2,m_nTimeOfStep2);
	DDX_Text(pDX,IDC_STEP3,m_nTimeOfStep3);
	DDV_MinMaxInt(pDX,m_nTimeOfStep1,0,999);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, &CSettingDlg::OnBnClickedButtonSelectFile)
//	ON_EN_UPDATE(IDC_STEP1, &CSettingDlg::OnEnUpdateStep1)
ON_BN_CLICKED(IDC_BUTTON_SELECT_PICTURE, &CSettingDlg::OnBnClickedButtonSelectPicture)
END_MESSAGE_MAP()


// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	GetDlgItem(IDC_EDIT_DATA_FILE_PATH)->SetWindowTextW(m_strDataFilePath);
	GetDlgItem(IDC_EDIT_PICTURE)->SetWindowTextW(m_strPicturePath);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnBnClickedButtonSelectFile()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Excel文件(*.xls)|*.xls||"));
	if(dlg.DoModal()==IDOK)
	{
		m_strDataFilePath=dlg.GetPathName();
		GetDlgItem(IDC_EDIT_DATA_FILE_PATH)->SetWindowTextW(m_strDataFilePath);
	}
}

void CSettingDlg::OnBnClickedButtonSelectPicture()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("bmp文件(*.bmp)|*.bmp||"));
	if(dlg.DoModal()==IDOK)
	{
		m_strPicturePath=dlg.GetPathName();
		GetDlgItem(IDC_EDIT_PICTURE)->SetWindowTextW(m_strPicturePath);
	}
}
