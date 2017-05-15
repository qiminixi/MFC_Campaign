// CampainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Campain.h"
#include "CampainDlg.h"
#include "SettingDlg.h"
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#define WIDTH_CLIENT 800
#define HEIGHT_CLIENT 582

#define WIDTH_BUTTON 70
#define HEIGHT_BUTTON 30
#define XPOSITION_BUTTON 80
#define YPOSITION_BUTTON 500
#define GAP_BUTTON 20

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString GblGetDirectoryOfExecutableFile()//��ȡ��ִ���ļ�����·��
{
	CString str('\0',255);
	::GetCurrentDirectory(255,str.GetBuffer());
	::GetModuleFileName(NULL,str.GetBuffer(),255);
	int i=0,j=0;
	while(j!=-1)
	{
		i=j;
		j=str.Find('\\',j+1);
	}
	i++;//�������һ��'\'
	//for(;str.GetAt(i)!='\0';i++) str.SetAt(i,'\0');
	str.GetBufferSetLength(i);
	return str;
}
CString GetExcelDriver()
{   
	wchar_t szBuf[2001];
	WORD cbBufMax = 2000;    
	WORD cbBufOut;    
	wchar_t *pszBuf = szBuf;    
	CString sDriver;     
	// ��ȡ�Ѱ�װ����������(������odbcinst.h��)    
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))        
		return _T("");        // �����Ѱ�װ�������Ƿ���Excel...    
	do    
	{        
		if (wcsstr(pszBuf, _T("Excel")) != 0)        
		{            
			//���� !            
			sDriver = CString(pszBuf);            
			break;        
		}        
		pszBuf = wcschr(pszBuf, '\0') + 1;    
	}    
	while (pszBuf[1] != '\0');     
	return sDriver;
}
// CCampainDlg dialog




CCampainDlg::CCampainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCampainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TimeOfSteps[0]=5;//��һ�����ڵ�ʱ��
	m_TimeOfSteps[1]=6;//�ڶ������ڵ�ʱ��
	m_TimeOfSteps[2]=7;//���������ڵ�ʱ��
	m_nCurStep=0;
	m_nTimeOfCur=5;//��ǰ�ļ�ʱ
	
	m_nStatus=0;
	m_nNumOfCampaigners=0;
	m_nNumOfPastedCampaigners=0;
	
	m_hBitmap=NULL;
}

void CCampainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCampainDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCampainDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_START_OR_SUSPEND, &CCampainDlg::OnBnClickedStartOrSuspend)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_STOP, &CCampainDlg::OnBnClickedStop)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NEXT, &CCampainDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_PRE, &CCampainDlg::OnBnClickedPre)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CCampainDlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_NEXT_STEP, &CCampainDlg::OnBnClickedNextStep)
END_MESSAGE_MAP()


// CCampainDlg message handlers

BOOL CCampainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	GetDlgItem(IDC_PRE)->MoveWindow(XPOSITION_BUTTON,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_STOP)->MoveWindow(XPOSITION_BUTTON+WIDTH_BUTTON+GAP_BUTTON,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_START_OR_SUSPEND)->MoveWindow(XPOSITION_BUTTON+WIDTH_BUTTON*2+GAP_BUTTON*2,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_NEXT)->MoveWindow(XPOSITION_BUTTON+WIDTH_BUTTON*3+GAP_BUTTON*3,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_NEXT_STEP)->MoveWindow(XPOSITION_BUTTON+WIDTH_BUTTON*4+GAP_BUTTON*4,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_BUTTON_SETTING)->MoveWindow(XPOSITION_BUTTON+WIDTH_BUTTON*5+GAP_BUTTON*5,YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->EnableWindow(TRUE);
	
	CString strDriverName= GetExcelDriver();
	if(strDriverName.IsEmpty())
	{
		AfxMessageBox(_T("û�а�װExcel����!"));
		return FALSE;
	}
	if(ReadConfigFile())
	{
		m_nTimeOfCur=m_TimeOfSteps[0];
		CFile file;
		if(file.Open(m_strDataFilePath,CFile::modeRead))//�ļ����ڵĻ���ȡ
		{
			file.Close();
			ReadData();
		}
		else if(m_strDataFilePath!=_T(""))
		{
			CString str;
			str.Format(_T("�ļ���%s �޷��򿪡�"),m_strDataFilePath);
			MessageBox(str);
		}
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCampainDlg::OnPaint()
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
		CPaintDC dc(this);
		CRect rectClient;
		GetClientRect(rectClient);
		
		int nOffsetX=(rectClient.Width()-WIDTH_CLIENT)/2;//x���ƫ����
		int nOffsetY=(rectClient.Height()-HEIGHT_CLIENT)/2;//y���ƫ����
		TRACE(_T("OnPaint: Offset(%d,%d)\n"),nOffsetX,nOffsetY);
		
		CDC memDC;
		memDC.CreateCompatibleDC(NULL);
		CBitmap memBmp;
		memBmp.CreateCompatibleBitmap(&dc,rectClient.Width(),rectClient.Height());
		CBitmap* pOldMemBitmap=memDC.SelectObject(&memBmp);
		memDC.SetBkMode(TRANSPARENT);
		
		//����
		static CBitmap bmp;
		CBrush brBg;
		if(!m_strPicPathName.IsEmpty())
		{
			
			if(m_hBitmap==NULL)
			{
				bmp.DeleteObject();
				m_hBitmap=(HBITMAP)::LoadImage(NULL,m_strPicPathName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
				bmp.Attach(m_hBitmap);
			}
			brBg.CreatePatternBrush(&bmp);
		}
		else
		{
			brBg.CreateSolidBrush(RGB(128,128,128));
		}
		
		memDC.FillRect(rectClient,&brBg);
		//--------------------------------------------
		CFont font;
		font.CreatePointFont(300,_T("����"));
		CFont* pOldFont=memDC.SelectObject(&font);
		memDC.TextOutW(nOffsetX+270,nOffsetY+20,_T("̫ԭ�Ƽ���ѧ��ѡ"));
		//����
		
		//------------------------------------------
		
		//��ѡ����Ϣ
		font.DeleteObject();
		font.CreatePointFont(280,_T("����"));
		memDC.SelectObject(&font);
		int n=50;
		int m=-40;
		memDC.TextOutW(nOffsetX+50,nOffsetY+50+n,_T("��ѡ�ˣ�"));
		memDC.TextOutW(nOffsetX+50,nOffsetY+180+n,_T("��ѡְ��"));
		memDC.TextOutW(nOffsetX+50,nOffsetY+310+n,_T("����ְ��"));
		
		memDC.TextOutW(nOffsetX+110,nOffsetY+100+n,m_strCampaigner.GetBuffer());
		memDC.TextOutW(nOffsetX+110,nOffsetY+230+n,m_strCampaignPost.GetBuffer());
		memDC.TextOutW(nOffsetX+110,nOffsetY+360+n,m_strCurPost.GetBuffer());
		//-------------------------------------------------------------
		
		//����ʱ����
		CString str;
		if(m_nTimeOfCur<10)
			str.Format(_T("00%d"),m_nTimeOfCur);
		else if(m_nTimeOfCur<100)
			str.Format(_T("0%d"),m_nTimeOfCur);
		else
			str.Format(_T("%d"),m_nTimeOfCur);
		font.DeleteObject();
		font.CreatePointFont(1800,_T("����"));
		memDC.SelectObject(&font);
		memDC.TextOutW(nOffsetX+400,nOffsetY+150,str);
		//------------------------------------------------------------
		//������ʾ
		CRect rect;
		rect.left=nOffsetX+620;
		rect.right=rect.left+110;
		rect.top=nOffsetY+490;
		rect.bottom=rect.top+23;
		memDC.SelectStockObject(NULL_BRUSH);
		memDC.Rectangle(rect);
		font.DeleteObject();
		font.CreatePointFont(120,_T("΢���ź�"));
		memDC.SelectObject(&font);
		str.Format(_T("�ܽ��ȣ�%d/%d"),m_nNumOfPastedCampaigners,m_nNumOfCampaigners);
		memDC.TextOutW(rect.left,rect.top,str);
		
		rect.top+=24;
		rect.bottom+=24;
		memDC.Rectangle(rect);
		str.Format(_T("��ǰ�׶Σ�%d/3"),m_nCurStep+1);
		memDC.TextOutW(rect.left,rect.top,str);
		//------------------------------------------------------------
		
		
		dc.BitBlt(0,0,rectClient.Width(),rectClient.Height(),&memDC,0,0,SRCCOPY);
		
		memDC.SelectObject(pOldMemBitmap);
		memDC.SelectObject(pOldFont);
		
	}
}
BOOL CCampainDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
void CCampainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	static int s=1;
	if(s)
	{
		s=0;
		return;
	}
	int nOffsetX=(cx-WIDTH_CLIENT)/2;//x���ƫ����
	int nOffsetY=(cy-HEIGHT_CLIENT)/2;//y���ƫ����
	GetDlgItem(IDC_PRE)->MoveWindow(nOffsetX+XPOSITION_BUTTON,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_STOP)->MoveWindow(nOffsetX+XPOSITION_BUTTON+WIDTH_BUTTON+GAP_BUTTON,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_START_OR_SUSPEND)->MoveWindow(nOffsetX+XPOSITION_BUTTON+WIDTH_BUTTON*2+GAP_BUTTON*2,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_NEXT)->MoveWindow(nOffsetX+XPOSITION_BUTTON+WIDTH_BUTTON*3+GAP_BUTTON*3,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_NEXT_STEP)->MoveWindow(nOffsetX+XPOSITION_BUTTON+WIDTH_BUTTON*4+GAP_BUTTON*4,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	GetDlgItem(IDC_BUTTON_SETTING)->MoveWindow(nOffsetX+XPOSITION_BUTTON+WIDTH_BUTTON*5+GAP_BUTTON*5,nOffsetY+YPOSITION_BUTTON,WIDTH_BUTTON,HEIGHT_BUTTON);
	Invalidate();
	// TODO: Add your message handler code here
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCampainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCampainDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

BOOL CCampainDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE)
			return FALSE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CCampainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	m_nTimeOfCur--;
	if(m_nTimeOfCur==0)
	{
		KillTimer(1);
		m_nStatus=0;
		//ǰ����ã�ֹͣ�����ã��������¿�ʼ����ʼ��һ���ڿ���
		GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("���¿�ʼ"));
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
		
		PlaySound(_T("note.wav"),NULL,SND_ASYNC|SND_FILENAME);
		
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CCampainDlg::ReadConfigFile()
{
	CFile file;
	CString strExecutableFilePath=GblGetDirectoryOfExecutableFile();
	//strExecutableFilePath.Remove('\0');
	strExecutableFilePath+=_T("config.data");
	if(!file.Open(strExecutableFilePath,CFile::modeRead))
	{
		//MessageBox(_T("��ȡ�������ļ�ʧ�ܣ�"));
		return FALSE;
	}
	//��ȡ����
	int n;
	file.Read(&n,4);
	m_strDataFilePath.GetBufferSetLength(n);
	file.Read(m_strDataFilePath.GetBuffer(),n*2);
	
	file.Read(m_TimeOfSteps,4*3);
	
	file.Read(&n,4);
	m_strPicPathName.GetBufferSetLength(n);
	file.Read(m_strPicPathName.GetBuffer(),n*2);
	//-------------------------------------------------
	file.Close();
	return TRUE;
}
BOOL CCampainDlg::WriteConfigFile()
{
	CFile file;
	CString strExecutableFilePath=GblGetDirectoryOfExecutableFile();
	wchar_t* buffer=strExecutableFilePath.GetBuffer();
	strExecutableFilePath.Remove('\0');
	strExecutableFilePath+=_T("config.data");
	if(!file.Open(strExecutableFilePath,CFile::modeWrite|CFile::modeCreate))
	{
		MessageBox(_T("д�������ļ�ʧ�ܣ�"));
		return FALSE;
	}
	//д������
	int n=m_strDataFilePath.GetLength()+1;
	file.Write(&n,4);//Excel�ļ�·����С
	file.Write(m_strDataFilePath,n*2);//Unicode�ַ���2���ֽ�һ������Ҫ��'\0'д��ȥ
	
	file.Write(m_TimeOfSteps,4*3);
	
	n=m_strPicPathName.GetLength()+1;
	file.Write(&n,4);//����ͼƬ·����С
	file.Write(m_strPicPathName.GetBuffer(),n*2);
	file.Flush();
	//-----------------------------------------------------------------
	file.Close();
	return TRUE;
}

BOOL CCampainDlg::ReadData()
{
	if(m_recordset.m_pDatabase!=NULL)
	{
		m_recordset.Close();
		m_database.Close();
	}
	
	CString strDriverName= GetExcelDriver();
	CString strConn;
	strConn.Format(_T("ODBC;DRIVER={%s};DSN=' ';DBQ=%s"),strDriverName,m_strDataFilePath);
	try
	{
		//�����ݿ�
		m_database.Open(NULL, false, false, strConn);

		//�򿪼�¼��
		m_recordset.m_pDatabase=&m_database;
		CString strSql =_T("select * from [Sheet1$]"); //��������˳��     
		m_recordset.Open(CRecordset::snapshot,strSql, CRecordset::readOnly);

		//��ȡ��һ������
		if(m_recordset.IsEOF())//excel�ļ�û�����ݵĻ�
			return FALSE;
		m_recordset.GetFieldValue(_T("����"),m_strCampaigner);
		m_recordset.GetFieldValue(_T("��Ƹ��λ"),m_strCampaignPost);
		m_recordset.GetFieldValue(_T("���θ�λ"),m_strCurPost);
		//------------------------------------------------------
		//ͳ��������
		m_nNumOfPastedCampaigners++;
		while(!m_recordset.IsEOF())
		{
			m_nNumOfCampaigners++;
			m_recordset.MoveNext();
		}
		m_recordset.MoveFirst();
		//---------------------------------------------------------
	}
	catch(CDBException* e)
	{
		// ���ݿ���������쳣ʱ...
		AfxMessageBox(_T("���ݿ����: ")+e->m_strError);
		return FALSE;
	}
	return TRUE;
}
void CCampainDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	WriteConfigFile();
	if(m_recordset.m_pDatabase!=NULL)
	{
		m_recordset.Close();
		m_database.Close();
	}
	CDialog::OnClose();
}
void CCampainDlg::OnBnClickedStop()
{
	//ֹͣ��׼����ͷ��ʼ
	if(m_nStatus==0)
		return;
	KillTimer(1);
	m_nTimeOfCur=m_TimeOfSteps[m_nCurStep];//----------!!!!
	
	//ǰ����ã�ֹͣ�����ã���ʼ���ã���һ���ڿ���
	GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
	GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("��ʼ"));
	if(m_nCurStep<2)
			GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
	
	m_nStatus=0;
	
	Invalidate();
}
void CCampainDlg::OnBnClickedStartOrSuspend()
{
	if(m_nStatus==0)//��ʼ״̬����ʼ
	{
		//ǰ�󲻿��ã�ֹͣ���ã���ͣ���ã���һ���ڲ�����
		m_nStatus=1;
		m_nTimeOfCur=m_TimeOfSteps[m_nCurStep];
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("��ͣ"));
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		SetTimer(1,1000,NULL);
		Invalidate();
	}
	else if(m_nStatus==1)//���ڽ����У�������ͣ
	{
		//ǰ����ã�ֹͣ����(���ּ��ɣ�֮ǰҲ�ǿ���״̬)���������ã���һ���ڿ���
		m_nStatus=2;
		GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("����"));
		if(m_nCurStep<2)
			GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
		KillTimer(1);
	}
	else if(m_nStatus==2)//������ͣ�У�����
	{
		//ǰ�󲻿��ã�ֹͣ���ã���ͣ���ã���һ���ڲ�����
		m_nStatus=1;
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("��ͣ"));
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		SetTimer(1,1000,NULL);
	}
}
void CCampainDlg::OnBnClickedNextStep()
{
	//����Ѿ������һ�����ڣ���������
	//������ڼ�ʱʱ״̬�£����ܽ�����һ������
	if(m_nCurStep>=2||m_nStatus==1)
		return;
	m_nCurStep++;
	if(m_nCurStep>=2)
	{
		m_nCurStep=2;
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(FALSE);
	}
	m_nTimeOfCur=m_TimeOfSteps[m_nCurStep];
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("��ʼ"));
	
	Invalidate();
}
void CCampainDlg::OnBnClickedPre()
{
	// TODO: Add your control notification handler code here
	if(m_recordset.m_pDatabase==NULL||m_nStatus==1)//Ҫ��ǰ��ʱ����ֹͣ(��ͣ����ֹͣ״̬)
		return;
	m_recordset.MovePrev();
	if(m_recordset.IsBOF())
		m_recordset.MoveFirst();
	else m_nNumOfPastedCampaigners--;
	m_recordset.GetFieldValue(_T("����"),m_strCampaigner);
	m_recordset.GetFieldValue(_T("��Ƹ��λ"),m_strCampaignPost);
	m_recordset.GetFieldValue(_T("���θ�λ"),m_strCurPost);
	InitStatus();
	Invalidate();
}
void CCampainDlg::OnBnClickedNext()
{
	// TODO: Add your control notification handler code here
	if(m_recordset.m_pDatabase==NULL||m_nStatus==1)//Ҫ��ǰ��ʱ����ֹͣ(��ͣ����ֹͣ״̬)
		return;
	m_recordset.MoveNext();
	if(m_recordset.IsEOF())
	{
		m_recordset.MoveLast();
	}
	else
		m_nNumOfPastedCampaigners++;
	m_recordset.GetFieldValue(_T("����"),m_strCampaigner);
	m_recordset.GetFieldValue(_T("��Ƹ��λ"),m_strCampaignPost);
	m_recordset.GetFieldValue(_T("���θ�λ"),m_strCurPost);
	
	InitStatus();
	Invalidate();
}

void CCampainDlg::OnBnClickedButtonSetting()
{
	CSettingDlg dlg;
	dlg.m_strDataFilePath=m_strDataFilePath;
	dlg.m_nTimeOfStep1=m_TimeOfSteps[0];
	dlg.m_nTimeOfStep2=m_TimeOfSteps[1];
	dlg.m_nTimeOfStep3=m_TimeOfSteps[2];
	dlg.m_strPicturePath=m_strPicPathName;
	
	if(dlg.DoModal()==IDOK)
	{
		KillTimer(1);
		if(m_strDataFilePath!=dlg.m_strDataFilePath)
		{
			m_strDataFilePath=dlg.m_strDataFilePath;
			ReadData();
		}
		if(m_strPicPathName!=dlg.m_strPicturePath)
		{
			m_strPicPathName=dlg.m_strPicturePath;
			::DeleteObject(m_hBitmap);
			m_hBitmap=NULL;
		}
		m_TimeOfSteps[0]=dlg.m_nTimeOfStep1;
		m_TimeOfSteps[1]=dlg.m_nTimeOfStep2;
		m_TimeOfSteps[2]=dlg.m_nTimeOfStep3;
		
		InitStatus();
		Invalidate(TRUE);
		//m_nTimeOfStep1=
	}
}
void CCampainDlg::InitStatus()
{
	//��ʼ״̬��ֹͣ��ť�����ã���ͣ��ʼ��ť��ʾ��ʼ�����ã���һ���ڰ�ť���ã�ǰ�����˰�ť����
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("��ʼ"));
	GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
	
	m_nStatus=0;
	m_nCurStep=0;
	m_nTimeOfCur=m_TimeOfSteps[0];
}
