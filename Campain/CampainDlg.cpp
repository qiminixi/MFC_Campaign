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

CString GblGetDirectoryOfExecutableFile()//获取可执行文件所在路径
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
	i++;//保留最后一根'\'
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
	// 获取已安装驱动的名称(涵数在odbcinst.h里)    
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))        
		return _T("");        // 检索已安装的驱动是否有Excel...    
	do    
	{        
		if (wcsstr(pszBuf, _T("Excel")) != 0)        
		{            
			//发现 !            
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
	m_TimeOfSteps[0]=5;//第一个环节的时间
	m_TimeOfSteps[1]=6;//第二个环节的时间
	m_TimeOfSteps[2]=7;//第三个环节的时间
	m_nCurStep=0;
	m_nTimeOfCur=5;//当前的计时
	
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
		AfxMessageBox(_T("没有安装Excel驱动!"));
		return FALSE;
	}
	if(ReadConfigFile())
	{
		m_nTimeOfCur=m_TimeOfSteps[0];
		CFile file;
		if(file.Open(m_strDataFilePath,CFile::modeRead))//文件存在的话读取
		{
			file.Close();
			ReadData();
		}
		else if(m_strDataFilePath!=_T(""))
		{
			CString str;
			str.Format(_T("文件：%s 无法打开。"),m_strDataFilePath);
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
		
		int nOffsetX=(rectClient.Width()-WIDTH_CLIENT)/2;//x轴的偏移量
		int nOffsetY=(rectClient.Height()-HEIGHT_CLIENT)/2;//y轴的偏移量
		TRACE(_T("OnPaint: Offset(%d,%d)\n"),nOffsetX,nOffsetY);
		
		CDC memDC;
		memDC.CreateCompatibleDC(NULL);
		CBitmap memBmp;
		memBmp.CreateCompatibleBitmap(&dc,rectClient.Width(),rectClient.Height());
		CBitmap* pOldMemBitmap=memDC.SelectObject(&memBmp);
		memDC.SetBkMode(TRANSPARENT);
		
		//背景
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
		font.CreatePointFont(300,_T("楷体"));
		CFont* pOldFont=memDC.SelectObject(&font);
		memDC.TextOutW(nOffsetX+270,nOffsetY+20,_T("太原科技大学竞选"));
		//标题
		
		//------------------------------------------
		
		//竞选者信息
		font.DeleteObject();
		font.CreatePointFont(280,_T("黑体"));
		memDC.SelectObject(&font);
		int n=50;
		int m=-40;
		memDC.TextOutW(nOffsetX+50,nOffsetY+50+n,_T("竞选人："));
		memDC.TextOutW(nOffsetX+50,nOffsetY+180+n,_T("竞选职务："));
		memDC.TextOutW(nOffsetX+50,nOffsetY+310+n,_T("现任职务："));
		
		memDC.TextOutW(nOffsetX+110,nOffsetY+100+n,m_strCampaigner.GetBuffer());
		memDC.TextOutW(nOffsetX+110,nOffsetY+230+n,m_strCampaignPost.GetBuffer());
		memDC.TextOutW(nOffsetX+110,nOffsetY+360+n,m_strCurPost.GetBuffer());
		//-------------------------------------------------------------
		
		//倒计时数字
		CString str;
		if(m_nTimeOfCur<10)
			str.Format(_T("00%d"),m_nTimeOfCur);
		else if(m_nTimeOfCur<100)
			str.Format(_T("0%d"),m_nTimeOfCur);
		else
			str.Format(_T("%d"),m_nTimeOfCur);
		font.DeleteObject();
		font.CreatePointFont(1800,_T("黑体"));
		memDC.SelectObject(&font);
		memDC.TextOutW(nOffsetX+400,nOffsetY+150,str);
		//------------------------------------------------------------
		//进度显示
		CRect rect;
		rect.left=nOffsetX+620;
		rect.right=rect.left+110;
		rect.top=nOffsetY+490;
		rect.bottom=rect.top+23;
		memDC.SelectStockObject(NULL_BRUSH);
		memDC.Rectangle(rect);
		font.DeleteObject();
		font.CreatePointFont(120,_T("微软雅黑"));
		memDC.SelectObject(&font);
		str.Format(_T("总进度：%d/%d"),m_nNumOfPastedCampaigners,m_nNumOfCampaigners);
		memDC.TextOutW(rect.left,rect.top,str);
		
		rect.top+=24;
		rect.bottom+=24;
		memDC.Rectangle(rect);
		str.Format(_T("当前阶段：%d/3"),m_nCurStep+1);
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
	int nOffsetX=(cx-WIDTH_CLIENT)/2;//x轴的偏移量
	int nOffsetY=(cy-HEIGHT_CLIENT)/2;//y轴的偏移量
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
		//前后可用，停止不可用，呈现重新开始，开始下一环节可用
		GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("重新开始"));
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
		//MessageBox(_T("读取设置置文件失败！"));
		return FALSE;
	}
	//读取数据
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
		MessageBox(_T("写入设置文件失败！"));
		return FALSE;
	}
	//写入数据
	int n=m_strDataFilePath.GetLength()+1;
	file.Write(&n,4);//Excel文件路径大小
	file.Write(m_strDataFilePath,n*2);//Unicode字符，2个字节一个，还要把'\0'写进去
	
	file.Write(m_TimeOfSteps,4*3);
	
	n=m_strPicPathName.GetLength()+1;
	file.Write(&n,4);//背景图片路径大小
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
		//打开数据库
		m_database.Open(NULL, false, false, strConn);

		//打开记录集
		m_recordset.m_pDatabase=&m_database;
		CString strSql =_T("select * from [Sheet1$]"); //设置索引顺序     
		m_recordset.Open(CRecordset::snapshot,strSql, CRecordset::readOnly);

		//读取第一个数据
		if(m_recordset.IsEOF())//excel文件没有数据的话
			return FALSE;
		m_recordset.GetFieldValue(_T("姓名"),m_strCampaigner);
		m_recordset.GetFieldValue(_T("竞聘岗位"),m_strCampaignPost);
		m_recordset.GetFieldValue(_T("现任岗位"),m_strCurPost);
		//------------------------------------------------------
		//统计总人数
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
		// 数据库操作产生异常时...
		AfxMessageBox(_T("数据库错误: ")+e->m_strError);
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
	//停止并准备从头开始
	if(m_nStatus==0)
		return;
	KillTimer(1);
	m_nTimeOfCur=m_TimeOfSteps[m_nCurStep];//----------!!!!
	
	//前后可用，停止不可用，开始可用，下一环节可用
	GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
	GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("开始"));
	if(m_nCurStep<2)
			GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
	
	m_nStatus=0;
	
	Invalidate();
}
void CCampainDlg::OnBnClickedStartOrSuspend()
{
	if(m_nStatus==0)//初始状态，开始
	{
		//前后不可用，停止可用，暂停可用，下一环节不可用
		m_nStatus=1;
		m_nTimeOfCur=m_TimeOfSteps[m_nCurStep];
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("暂停"));
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		SetTimer(1,1000,NULL);
		Invalidate();
	}
	else if(m_nStatus==1)//正在进行中，进行暂停
	{
		//前后可用，停止可用(保持即可，之前也是可用状态)，继续可用，下一环节可用
		m_nStatus=2;
		GetDlgItem(IDC_PRE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("继续"));
		if(m_nCurStep<2)
			GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
		KillTimer(1);
	}
	else if(m_nStatus==2)//还在暂停中，继续
	{
		//前后不可用，停止可用，暂停可用，下一环节不可用
		m_nStatus=1;
		GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("暂停"));
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEXT_STEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		SetTimer(1,1000,NULL);
	}
}
void CCampainDlg::OnBnClickedNextStep()
{
	//如果已经是最后一个环节，不做处理
	//如果还在计时时状态下，不能进入下一个环节
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
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("开始"));
	
	Invalidate();
}
void CCampainDlg::OnBnClickedPre()
{
	// TODO: Add your control notification handler code here
	if(m_recordset.m_pDatabase==NULL||m_nStatus==1)//要求当前计时必须停止(暂停或者停止状态)
		return;
	m_recordset.MovePrev();
	if(m_recordset.IsBOF())
		m_recordset.MoveFirst();
	else m_nNumOfPastedCampaigners--;
	m_recordset.GetFieldValue(_T("姓名"),m_strCampaigner);
	m_recordset.GetFieldValue(_T("竞聘岗位"),m_strCampaignPost);
	m_recordset.GetFieldValue(_T("现任岗位"),m_strCurPost);
	InitStatus();
	Invalidate();
}
void CCampainDlg::OnBnClickedNext()
{
	// TODO: Add your control notification handler code here
	if(m_recordset.m_pDatabase==NULL||m_nStatus==1)//要求当前计时必须停止(暂停或者停止状态)
		return;
	m_recordset.MoveNext();
	if(m_recordset.IsEOF())
	{
		m_recordset.MoveLast();
	}
	else
		m_nNumOfPastedCampaigners++;
	m_recordset.GetFieldValue(_T("姓名"),m_strCampaigner);
	m_recordset.GetFieldValue(_T("竞聘岗位"),m_strCampaignPost);
	m_recordset.GetFieldValue(_T("现任岗位"),m_strCurPost);
	
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
	//初始状态：停止按钮不可用，暂停开始按钮显示开始并可用，下一环节按钮可用，前进后退按钮可用
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_OR_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_START_OR_SUSPEND)->SetWindowTextW(_T("开始"));
	GetDlgItem(IDC_NEXT_STEP)->EnableWindow(TRUE);
	
	m_nStatus=0;
	m_nCurStep=0;
	m_nTimeOfCur=m_TimeOfSteps[0];
}
