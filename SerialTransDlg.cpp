/*******************************************************
Copyright 2008-2011 Hikvision Digital Technology Co., Ltd. 
文  件：	SerialTransDlg.cpp
开发单位：	杭州海康威视
编  写：	shizhiping
日  期：	2009.5
描  述：	透明通道
修  改：	
********************************************************/

#include "stdafx.h"
#include "SerialTrans.h"
#include <process.h>
#include <string>
#include <cassert>
#include "Comutil.h"
#include "comdef.h"
//#include "PlayM4.h"
//#include "HCNetSDK.h"
#include "SerialTransDlg.h"
#include "Parameter.h"
#include <opencv2/core/core.hpp>
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/legacy/legacy.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp> 
#include <afxsock.h>
#include "GlobleCtrl.h"
#include "HKPlayWnd.h"

using std::string;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(lib,"ws2_32.lib")
using namespace cv;
///////////////////////////////////////////////////////////////////////
CWnd* pVideoWnd;
CSocket m_socket;
LONG llRealHandle;//播放返回值
//int iPicNum=0;//Set channel NO.1
int EA=0;//水平角度
int AZ=0;//俯仰角度
int WaitTime=15;//等待时间
int EAmove=30,AZmove=10;//单步转动角度
int gFocus,gZOOM;//实时角度和变倍数
double gAPos,gEPos;//实时云台方位
LONG m_lSerialHandle;
char filename[100];//文件路径
HWND hPlayWnd=NULL;
CString HWIP,KJIP;
CString ErrorNum;
CString gFilename=' ';
CString m_strFilePath;
bool gIsReady=false;// true时开始追踪
bool gPM=false;
byte recvdata[1024],data[7];
long recvlenth;
CSerialTransDlg* m_pDemoDlg;
CParameter gParam;
BYTE gSpeed=0x0D;
GlobleCtrl *pGlobleCtrl;
Mat a;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialTransDlg dialog

CSerialTransDlg::CSerialTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialTransDlg::IDD, pParent)
	, m_ha(0)
	, m_va(0)
	, m_dbldisplayh(0)
	, m_dbdisplayv(0)
	, m_Zoom(0)
	, m_Focus(0)
{
	//{{AFX_DATA_INIT(CSerialTransDlg)
	gParam.GetParam();
	m_csDataSend = _T("");
	m_iSendInterval = 0;
	m_bSendContinual = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_nDevPort = gParam.LocalPort;
	m_csUser = _T("admin");
	m_csPWD = gParam.C1.Password;
	m_bIsLogin = FALSE;
    m_iCurSerialType = 1;
	m_iSerialChan = 1;
	m_lSerialHandle = -1;
	m_DataLen = 0;

	memset(m_DataBuf,0,sizeof(m_DataBuf));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialTransDlg)
	DDX_Text(pDX, IDC_EDIT_IPCH, iPChannel);
	DDX_Control(pDX, IDC_COMBO_INTERFACE, m_comboInterface);
	DDX_Control(pDX, IDC_COMBO_CHAN, m_comboChan);
	DDX_Control(pDX, IDC_COMBO_SERIAL_TYPE, m_comboSerialType);
	DDX_Control(pDX, IDC_IPADDRESS_DEV, m_ctrlDevIp);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nDevPort);
	DDX_Text(pDX, IDC_EDIT_USER, m_csUser);
	DDX_Text(pDX, IDC_EDIT_PWD, m_csPWD);
	DDX_Text(pDX, IDC_EDIT_DATA_SEND, m_csDataSend);
	DDV_MaxChars(pDX, m_csDataSend[0], 1000);
	DDX_Text(pDX, IDC_EDIT_ha, m_ha);
	DDX_Text(pDX, IDC_EDIT_va, m_va);
	DDX_Text(pDX, IDC_EDIT_hangle, m_dbldisplayh);
	DDX_Text(pDX, IDC_EDIT_vangle, m_dbdisplayv);
	DDX_Text(pDX, IDC_EDIT_setzoom, m_Zoom);
	DDX_Control(pDX, IDC_IPADDRESS_DEV2, m_ctrlDevIP);
	DDX_Control(pDX, IDC_COMBO_PIC_TYPE, m_coPicType);
	DDX_Control(pDX, IDC_COMBO_JPG_SIZE, m_coJpgSize);
	DDX_Control(pDX, IDC_COMBO_JPG_QUALITY, m_coJpgQuality);
	DDX_Text(pDX, IDC_EDIT_SETFOCUS, m_Focus);
}

BEGIN_MESSAGE_MAP(CSerialTransDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialTransDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, OnButtonLogin)
	ON_CBN_SELCHANGE(IDC_COMBO_SERIAL_TYPE, OnSelchangeComboSerialType)
	ON_BN_CLICKED(IDC_BUTTON_SERIALSTART, OnButtonSerialstart)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_CBN_SELCHANGE(IDC_COMBO_CHAN, OnSelchangeComboChan)
	ON_BN_CLICKED(IDC_BUTTON_SERIALSTOP, OnButtonSerialstop)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SETTED, &CSerialTransDlg::OnBnClickedButtonSetted)
	ON_BN_CLICKED(IDC_BUTTON_QH, &CSerialTransDlg::OnBnClickedButtonQh)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CSerialTransDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_set, &CSerialTransDlg::OnBnClickedButtonset)
	ON_BN_CLICKED(IDC_BUTTON_CapImg, &CSerialTransDlg::OnBnClickedButtonCapimg)
	ON_BN_CLICKED(IDC_BUTTON_setzoom, &CSerialTransDlg::OnBnClickedButtonsetzoom)
	ON_CBN_SELCHANGE(IDC_COMBO_PIC_TYPE, &CSerialTransDlg::OnCbnSelchangeComboPicType)
	ON_BN_CLICKED(IDC_BUTTON_inscap, &CSerialTransDlg::OnBnClickedButtoninscap)
	ON_BN_CLICKED(IDC_BUTTON_FOCUS, &CSerialTransDlg::OnBnClickedButtonFocus)
	ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_starttrace, &CSerialTransDlg::OnBnClickedButtonstarttrace)
    ON_BN_CLICKED(IDC_BUTTON_stoptrace, &CSerialTransDlg::OnBnClickedButtonstoptrace)
    ON_BN_CLICKED(IDC_BUTTON_openlaser, &CSerialTransDlg::OnBnClickedButtonopenlaser)
    ON_BN_CLICKED(IDC_BUTTON_closelaser, &CSerialTransDlg::OnBnClickedButtoncloselaser)
    ON_BN_CLICKED(IDC_BUTTON_addlaserarea, &CSerialTransDlg::OnBnClickedButtonaddlaserarea)
    ON_BN_CLICKED(IDC_BUTTON_minuslaserarea, &CSerialTransDlg::OnBnClickedButtonminuslaserarea)
    ON_BN_CLICKED(IDC_BUTTON_distance, &CSerialTransDlg::OnBnClickedButtondistance)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialTransDlg message handlers

BOOL CSerialTransDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	gParam.GetParam();
	pGlobleCtrl=new GlobleCtrl;
	m_socket.Create(gParam.LocalPort,SOCK_DGRAM);//udp发送
	m_isFullScreen=false;
	bInsCap=false;//连续抓拍标识
	//m_picplay.GetWindowRect(&m_Old);
	ScreenToClient(&m_Old);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// Dev ip
	DWORD dwIP1,dwIP2;
	dwIP1=inet_addr(gParam.C1.IP1);
	dwIP2=inet_addr(gParam.C1.IP2);
	unsigned char *pIP1=(unsigned char*)&dwIP1;
	unsigned char *pIP2=(unsigned char*)&dwIP2;
	m_ctrlDevIp.SetAddress(*pIP1,*(pIP1+1),*(pIP1+2),*(pIP1+3));//红外ip
	m_ctrlDevIP.SetAddress(*pIP2,*(pIP2+1),*(pIP2+2),*(pIP2+3));//可见光ip
    
	//串口类型COMBO
	m_comboSerialType.SetCurSel(1);
	OnSelchangeComboSerialType();
	//编码，默认为ASCII
	((CButton*)GetDlgItem(IDC_RADIO_HEX))->SetCheck(1);
	//发送接口
	m_comboInterface.SetCurSel(0);
	//获取文件路径
	GetModuleFileName(NULL, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	m_strFilePath.ReleaseBuffer();
	int nPos = m_strFilePath.ReverseFind('\\');
	m_strFilePath =m_strFilePath.Left(nPos);
	
    llRealHandle=-1;
	m_lSerialHandle=-1;
	GetDlgItem(IDC_BUTTON_set)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_QH)->EnableWindow(FALSE);
	lUserID=-1;
	iPChannel=1;
    recvlenth=0;

	m_coPicType.SetCurSel(0);
	m_coJpgSize.SetCurSel(0);
	m_coJpgQuality.SetCurSel(0);

	//发送关闭按钮状态
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_SERIALSTOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERIALSTART)->EnableWindow(FALSE);
    pUDPRemoteLisenThread=::AfxBeginThread(UDPRemoteListenThread,this);	//开始远程UDP监听线程
	IMAGE_PORT=9000;
	((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->SetCheck(true);
	if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
		GetDlgItem(IDC_BUTTON_setzoom)->SetWindowText("红外相机变倍");
	SetDlgItemInt(IDC_EDIT_imageport, gParam.PeerPort);
	SetDlgItemInt(IDC_EDIT_IPCH, 1);
	SetDlgItemText(IDC_EDIT_PWD,gParam.C1.Password);

	SetTimer(1,10,0);
	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialTransDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSerialTransDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		//SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSerialTransDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSerialTransDlg::OnButtonLogin() 
{
	if(!m_bIsLogin)    //login
	{
		if(!DoLogin())
			return;
		GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowText("Logout");
		GetDlgItem(IDC_BUTTON_SERIALSTART)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		m_bIsLogin = TRUE;
		//保存参数
		unsigned char *pIP1,*pIP2;
		DWORD dwIP1,dwIP2;
		m_ctrlDevIp.GetAddress(dwIP1);
	    m_ctrlDevIP.GetAddress(dwIP2);
        pIP1=(unsigned char*)&dwIP1;
		pIP2=(unsigned char*)&dwIP2;
		gParam.C1.IP2.Format("%u.%u.%u.%u",*(pIP2+3),*(pIP2+2),*(pIP2+1),*pIP2);
		gParam.C1.IP1.Format("%u.%u.%u.%u",*(pIP1+3),*(pIP1+2),*(pIP1+1),*pIP1);
		gParam.PeerPort=GetDlgItemInt(IDC_EDIT_imageport);
		gParam.LocalPort=GetDlgItemInt(IDC_EDIT_PORT);
		gParam.SaveParam();
	}
	else      //logout
	{
		if(m_lSerialHandle >=0 )
		{
			MessageBox("先关闭透明通道");
			return;
		}
        pGlobleCtrl->DoLogout();
		GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowText("Login");
		GetDlgItem(IDC_BUTTON_SERIALSTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		m_bIsLogin = FALSE;
	}
}

BOOL CSerialTransDlg::DoLogin()
{
	UpdateData(TRUE);
	CString DeviceIp,DeviceIp1;
	BYTE nField0,nField1,nField2,nField3; 
	m_ctrlDevIp.GetAddress(nField0,nField1,nField2,nField3);
	DeviceIp.Format("%d.%d.%d.%d",nField0,nField1,nField2,nField3);
	m_ctrlDevIP.GetAddress(nField0,nField1,nField2,nField3);
	DeviceIp1.Format("%d.%d.%d.%d",nField0,nField1,nField2,nField3);
	HWIP=DeviceIp;
	KJIP=DeviceIp1;
	pGlobleCtrl->UserName=m_csUser.GetBuffer(m_csUser.GetLength());
	pGlobleCtrl->PassWord=m_csPWD.GetBuffer(m_csPWD.GetLength());
	if(!pGlobleCtrl->DoLogin())
		return false;
	UpdateData(FALSE);
	return TRUE;
}

void CSerialTransDlg::OnSelchangeComboSerialType() 
{
	CString csTmp;
	int i=0;
	m_iCurSerialType = m_comboSerialType.GetCurSel() + 1;
	m_comboChan.ResetContent();
	switch(m_iCurSerialType)
	{
	case 1:   //232
        csTmp = "Serial 1";
        m_comboChan.AddString(csTmp);
		m_comboChan.EnableWindow(FALSE);
		break;
	case 2:   //485
		for(i=0; i<m_struDeviceInfo.iDeviceChanNum; i++)
		{
             csTmp.Format("Channel %d",i+1);
			 m_comboChan.AddString(csTmp);
		}
		m_comboChan.EnableWindow(TRUE);
		break;
	default:
		break;
	}
	m_iSerialChan = 1;
	m_comboChan.SetCurSel(m_iSerialChan - 1);
}

void CSerialTransDlg::OnButtonSerialstart() 
{
	pGlobleCtrl->m_iCurSerialType=m_iCurSerialType;
	if(!pGlobleCtrl->SerialTransStart())
	{
		CString err;
		err.Format("建立透明通道失败,错误代码：%d",NET_DVR_GetLastError());
		MessageBox(err);
		return;
	}
	/*SetTimer(2,1000,0);
	SetTimer(3,1000,0);
	SetTimer(4,1000,0);
	SetTimer(5,1000,0);*/
	//SetTimer(7,500,0);
    //MessageBox("建立透明通道成功");
	GetDlgItem(IDC_BUTTON_SERIALSTART)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERIALSTOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_set)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_QH)->EnableWindow(TRUE);
}

void CSerialTransDlg::OnButtonSend() 
{
	GetDlgItemText(IDC_EDIT_DATA_SEND,m_csDataSend);
	m_DataLen = m_csDataSend.GetLength();
	if(m_DataLen == 0)
	{
	    MessageBox("不能发送空数据");
	    return;
	}
	SendData(m_csDataSend);
}

void CSerialTransDlg::OnSelchangeComboChan() 
{
    m_iSerialChan = m_comboChan.GetCurSel()+1;	
}

void CSerialTransDlg::OnButtonSerialstop() 
{
	if(!pGlobleCtrl->SerialTransStop())
		return;
	KillTimer(7);
    GetDlgItem(IDC_BUTTON_SERIALSTART)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERIALSTOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_set)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_QH)->EnableWindow(FALSE);
	m_lSerialHandle = -1;
}
/****************************************************
函数名:    	UDPRemoteListenThread
函数描述:	接收udp消息
输入参数:   
输出参数:   			
返回值:		
****************************************************/
UINT CSerialTransDlg::UDPRemoteListenThread(LPVOID lparam)	
{	
	unsigned char UDPBuf[2048];
	double dAZ,dEL;
	//char sFileName[256];
	int iDataLen;
	char MsgList[10][128];
	int  i,RegCount,MsgCount;
	//int nRow,nCol;
	char cTag;
	CString sParamFile;
	cTag=';';

	CSerialTransDlg *pDlg=(CSerialTransDlg *)lparam;
	SOCKADDR_IN	enet_receive_addr;
	SOCKADDR_IN	enet_cur_addr;
	SOCKET enet_receive_socket;
	int    len = sizeof(enet_cur_addr);
	enet_receive_addr.sin_family       = AF_INET;
    enet_receive_addr.sin_port         = htons((short)pDlg->IMAGE_PORT);
	enet_receive_addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	
	if ((enet_receive_socket=socket (AF_INET, SOCK_DGRAM,0)) == INVALID_SOCKET ) 
    { 
        pDlg->MessageBox(_T("cannot create enet_receive_socket\n"));  
		return -1;
	}

	if (bind (enet_receive_socket, (struct sockaddr *)&enet_receive_addr, sizeof(enet_receive_addr)) == SOCKET_ERROR)
	{
		pDlg->MessageBox (_T("Bind enet_receive_socket failed\n"));
		return -1;
	}
	while(1)
	{
		CString str,strAStatus,strHStatus,msg;
		if((iDataLen = recvfrom(enet_receive_socket, (char *)UDPBuf, 2048, 0,(struct sockaddr *)&enet_cur_addr,&len)) != ERROR)
		{
			if(iDataLen>1280) iDataLen=1280;
			UDPBuf[iDataLen]=0x0;
			msg.Format(_T("Received:%s"),UDPBuf);

			MsgCount=0;
			RegCount=0;
			memset(MsgList,0,1280);
			for(i=0;i<iDataLen;i++)
			{
				if(UDPBuf[i]!=cTag)
				{
					MsgList[MsgCount][RegCount]=UDPBuf[i];
					RegCount++;
				}
				else
				{
					if(MsgCount<10) MsgCount++;
					RegCount=0;
				}
			}
			if(MsgCount>=3)
			{
				if(MsgList[0][0]=='A'&&MsgList[0][1]=='Z')
				{
				dAZ=atof(MsgList[1]);
				}
				if(MsgList[2][0]=='E'&&MsgList[2][1]=='L')
				{
				dEL=atof(MsgList[3]);
				//dEL=-1*dEL;
				}
			}
		}
		if(dAZ>=0&&dAZ<=360)
        {
            if(dAZ>270)
			    pDlg->SetHAngle(dAZ+90-360);
            else
                pDlg->SetHAngle(dAZ+90);
        }
		Sleep(10);
		if(dEL>=-90&&dEL<=90)
			pDlg->SetVAngle(dEL);
		Sleep(10);
	}
	return 0;
}

void CSerialTransDlg::SetHAngle(double HAngle)
{
	int iH;
	BYTE h1,h2,checksum;
	CString hset;
	iH=(int)(HAngle*100);
	h1=(iH>>8)&0xFF;
	h2=iH&0xFF;
	checksum=0x01+0x00+0x4B+h1+h2;
	hset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x4B,h1,h2,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, hset);
    SetDlgItemText(IDC_EDIT_DATA_SEND, hset);
	m_csDataSend=hset;
	SendData(hset);
}
void CSerialTransDlg::SetVAngle(double VAngle)
{
	int iV;
	BYTE v1,v2,checksum;
	CString vset;
	if(VAngle>0)
		iV=36000-VAngle*100;
	else
	    iV=(int)(-1*VAngle*100);
	v1=(iV>>8)&0xFF;
	v2=iV&0xFF;	
	checksum=0x01+0x00+0x4D+v1+v2;
	vset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x4D,v1,v2,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, vset);	
	m_csDataSend=vset;
	pGlobleCtrl->SendData(vset);
}
BOOL CSerialTransDlg::PreTranslateMessage(MSG *pMsg)
{
	BYTE checksum;
    CString msg;
	NET_DVR_CLIENTINFO ClientInfo;
	//LONG lPlayHandle = g_pMainDlg->GetPlayHandle();
	if(pMsg->message==WM_LBUTTONDOWN)
	{
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_LEFT)->m_hWnd)
		{
			checksum=0x01+0x00+0x04+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x04,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_RIGHT)->m_hWnd)
		{
			checksum=0x01+0x00+0x02+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x02,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_UP)->m_hWnd)
		{
			checksum=0x01+0x00+0x08+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x08,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_DOWN)->m_hWnd)
		{
			checksum=0x01+0x00+0x10+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x10,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(m_csDataSend);
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_LU)->m_hWnd)
		{
			checksum=0x01+0x00+0x04+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x04,gSpeed,gSpeed,checksum);			
			m_csDataSend=msg;
			checksum=0x01+0x00+0x08+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x08,gSpeed,gSpeed,checksum);
			m_csDataSend+=" ";
			m_csDataSend+=msg;
			SetDlgItemText(IDC_EDIT_DATA_SEND,m_csDataSend);
			SendData(m_csDataSend);
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_LD)->m_hWnd)
		{
			checksum=0x01+0x00+0x04+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x04,gSpeed,gSpeed,checksum);
			m_csDataSend=msg;
			checksum=0x01+0x00+0x10+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x10,gSpeed,gSpeed,checksum);
			m_csDataSend+=" ";
			m_csDataSend+=msg;
			SendData(m_csDataSend);
			SetDlgItemText(IDC_EDIT_DATA_SEND,m_csDataSend);	
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_RU)->m_hWnd)
		{
			checksum=0x01+0x00+0x02+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x02,gSpeed,gSpeed,checksum);
			m_csDataSend=msg;
			checksum=0x01+0x00+0x08+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x08,gSpeed,gSpeed,checksum);
			m_csDataSend+=" ";
			m_csDataSend+=msg;
			SendData(m_csDataSend);
			SetDlgItemText(IDC_EDIT_DATA_SEND,m_csDataSend);	
		}
		if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_RD)->m_hWnd)
		{
			checksum=0x01+0x00+0x02+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x02,gSpeed,gSpeed,checksum);
			m_csDataSend=msg;
			checksum=0x01+0x00+0x10+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x10,gSpeed,gSpeed,checksum);
			m_csDataSend+=" ";
			m_csDataSend+=msg;
			SendData(m_csDataSend);
			SetDlgItemText(IDC_EDIT_DATA_SEND,m_csDataSend);	
		}
        if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_focusup)->m_hWnd)
		{
			checksum=0x01+0x01+0x00+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x01,0x00,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);	
		}	
        if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_focusdown)->m_hWnd)
		{
			checksum=0x01+0x00+0x80+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x80,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);	
		}
        if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_addzoom)->m_hWnd)
		{
			checksum=0x01+0x00+0x40+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x40,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);	
		}
        if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_lesszoom)->m_hWnd)
		{
			checksum=0x01+0x00+0x20+gSpeed+gSpeed;
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x20,gSpeed,gSpeed,checksum);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);	
		}
	}
	if(pMsg->message==WM_LBUTTONUP)
	{
	   if(pMsg->hwnd==GetDlgItem(IDC_BUTTON_RD)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_RU)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_LD)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_LU)->m_hWnd||
		   pMsg->hwnd==GetDlgItem(IDC_BUTTON_DOWN)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_UP)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_RIGHT)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_LEFT)->m_hWnd||
           pMsg->hwnd==GetDlgItem(IDC_BUTTON_focusup)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_focusdown)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_addzoom)->m_hWnd||pMsg->hwnd==GetDlgItem(IDC_BUTTON_lesszoom)->m_hWnd)
	   {
			msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x00,0x00,0x00,0x01);
			SetDlgItemText(IDC_EDIT_DATA_SEND,msg);	
			m_csDataSend=msg;
			SendData(msg);
	   }
	}
	return false;
//	//return CDialog::PreTranslateMessage(pMsg);
}
void CSerialTransDlg::OnBnClickedButtonSetted()
{
	// TODO: 在此添加控件通知处理程序代码
	IMAGE_PORT=GetDlgItemInt(IDC_EDIT_imageport);
	pUDPRemoteLisenThread=::AfxBeginThread(UDPRemoteListenThread,this);	//开始远程UDP监听线程
}
void CSerialTransDlg::OnBnClickedButtonQh()
{
	// TODO: 在此添加控件通知处理程序代码
	gPM=true;
    AZ=-1*AZmove;
	SetHAngle(EA);
	Sleep(10);
	SetVAngle(AZ);
}

void CSerialTransDlg::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: Add extra validation here
   	CRect rect(0,0,900,900);      
	pVideoWnd=new CHKPlayWnd;
	pVideoWnd->CreateEx(NULL,AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,NULL,(HBRUSH)::GetStockObject(BLACK_BRUSH),NULL),\
	_T("video"),WS_POPUP|WS_VISIBLE|WS_OVERLAPPEDWINDOW,rect,this,0);
	::SetWindowPos(pVideoWnd->m_hWnd,HWND_NOTOPMOST ,rect.left,rect.top,rect.right,rect.bottom,SWP_SHOWWINDOW|SWP_DRAWFRAME);
	::ShowWindow(pVideoWnd->m_hWnd,true); 
	hPlayWnd = pVideoWnd->GetSafeHwnd();//播放控件句柄
	UpdateData(TRUE);
	if (pGlobleCtrl->llRealHandle<0)
	{
		StartPlay();
		UpdateData(TRUE);
		if (pGlobleCtrl->llRealHandle<0)
        {
			ErrorNum.Format("NET_DVR_RealPlay_V30 failed! Error number: %d\n",NET_DVR_GetLastError());
			AfxMessageBox(ErrorNum);
			return;
        }
	    if(m_isFullScreen == false)
		{
			m_isFullScreen = true; 
		}    
		GetDlgItem(IDC_BUTTON_OK)->SetWindowText("停止播放");
	}
	else
	{	
		::ShowWindow(pVideoWnd->m_hWnd,false); 
		pGlobleCtrl->StopPlay();

		GetDlgItem(IDC_BUTTON_OK)->SetWindowText("开始播放");
	}
	gParam.SaveParam();
	//SetTimer(8,50,0);
	UpdateData(FALSE);
}

void CSerialTransDlg::StopPlay()
{
	//pGlobleCtrl->StopPlay();
}
void CSerialTransDlg::StartPlay()
{
	UpdateData(TRUE);
    pGlobleCtrl->iPChannel=iPChannel;
	//预览取流 
	if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
	{
        gParam.ROIx=128;
        gParam.ROIy=128;
		pGlobleCtrl->StartPlay(pGlobleCtrl->HWUserID);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_visiblelightcam))->GetCheck())
	{
		pGlobleCtrl->StartPlay(pGlobleCtrl->KJUserID);
	}
	if(m_isFullScreen == false)
	{
		m_isFullScreen = true; 
	}    
}

void CSerialTransDlg::OnBnClickedButtonset()
{
	// TODO: 在此添加控件通知处理程序代
	UpdateData(true);
	SetHAngle(m_ha);
	Sleep(100);
	SetVAngle(m_va);
}
void CSerialTransDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
   CString msg;
   long msglenth;
   byte addr;
	switch (nIDEvent)
	{
	case 1:
        if(gAPos>270)
	        msg.Format(_T("%8.4f"), gAPos-270);
        else
            msg.Format(_T("%8.4f"), gAPos+90);
		SetDlgItemText(IDC_EDIT_hangle, msg);
		msg.Format(_T("%8.4f"), gEPos);
		SetDlgItemText(IDC_EDIT_vangle,msg);
		msg.Format(_T("%d"), gZOOM);
		SetDlgItemText(IDC_EDIT_zoom, msg);
		msg.Format(_T("%d"), gFocus);
		SetDlgItemText(IDC_EDIT_FOCUS, msg);
		break;
	case 2:
		if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
		{
			addr=0x02;
			GetDlgItem(IDC_BUTTON_setzoom)->SetWindowText("红外相机变倍");
		}
		else if(((CButton*)GetDlgItem(IDC_RADIO_visiblelightcam))->GetCheck())
		{
			addr=0x01;
			GetDlgItem(IDC_BUTTON_setzoom)->SetWindowText("可见光相机变倍");
		}
		msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,addr,0x00,0x55,0x00,0x00,0x55+addr);
		m_csDataSend=msg;
		SendData(msg);
		break;
	case 3:
		msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x51,0x00,0x00,0x52);
		m_csDataSend=msg;
		SendData(msg);
		break;
	case 4:
		msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x53,0x00,0x00,0x54);
		m_csDataSend=msg;
		SendData(msg);
		break;
	case 5:
		if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
		{
			addr=0x02;
			GetDlgItem(IDC_BUTTON_FOCUS)->SetWindowText("红外相机聚焦");
		}
		else if(((CButton*)GetDlgItem(IDC_RADIO_visiblelightcam))->GetCheck())
		{
			addr=0x01;
			GetDlgItem(IDC_BUTTON_FOCUS)->SetWindowText("可见光相机聚焦");
		}
		msg.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,addr,0x00,0x65,0x00,0x00,0x65+addr);
		m_csDataSend=msg;
		SendData(msg);
		break;
	case 6:
		CaptureImg();
		break;
	case 7:
		msg.Format("%8.4f;%8.4f;%d",gAPos,gEPos,gZOOM);
		msglenth=msg.GetLength();
		m_socket.SendTo(msg,msglenth,gParam.LocalSendPort,gParam.ip);
		break;
	case 8:
		gIsReady=true;
		break;
    CDialog::OnTimer(nIDEvent);
	}
}

void CSerialTransDlg::OnBnClickedButtonCapimg()
{
	//// TODO: 在此添加控件通知处理程序代码
	
	gFilename="抓拍图.bmp";
	CaptureImg();
}
void CSerialTransDlg::CaptureImg()
{
	if(!PathIsDirectory(m_strFilePath+"\\抓拍图"))
		CreateDirectory(m_strFilePath+"\\抓拍图",NULL);
	//UpdateData(TRUE);
	char PicName[256] = {0};
	char infPicName[256]={0};
	int iPicType = m_coPicType.GetCurSel();
	CString sTime;
	CTime CurTime = CTime::GetCurrentTime();
	sTime.Format("%04d%02d%02d%02d%02d%02d",CurTime.GetYear(),CurTime.GetMonth(),CurTime.GetDay(), \
	    CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond());
	//组建jpg结构
	WORD wPicSize,wPicQuality;
    wPicSize = (WORD)m_coJpgSize.GetCurSel();
	wPicQuality = (WORD)m_coJpgQuality.GetCurSel();
	pGlobleCtrl->CaptrueImg(m_strFilePath+"\\抓拍图\\KJ"+sTime,m_strFilePath+"\\抓拍图\\HW"+sTime,iPicType,wPicSize,wPicQuality);
	return;	
}
void CSerialTransDlg::SendData(CString data)
{
	pGlobleCtrl->m_iCurInterface = m_comboInterface.GetCurSel();
	pGlobleCtrl->SendData(data);
}

void CSerialTransDlg::OnBnClickedButtonsetzoom()
{
	// TODO: 在此添加控件通知处理程序代码
	byte addr;
	if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
	{
		addr=0x02;
		GetDlgItem(IDC_BUTTON_setzoom)->SetWindowText("红外相机变倍");
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_visiblelightcam))->GetCheck())
	{
		addr=0x01;
		GetDlgItem(IDC_BUTTON_setzoom)->SetWindowText("可见光相机变倍");
	}
	UpdateData(true);
	int iZ;
	BYTE z1,z2,checksum;
	CString zset;
	iZ=(int)(m_Zoom);
	z1=(iZ>>8)&0xFF;
	z2=iZ&0xFF;
	checksum=addr+0x00+0x4F+z1+z2;
	zset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,addr,0x00,0x4F,z1,z2,checksum);
	SendData(zset);
}

void CSerialTransDlg::OnCbnSelchangeComboPicType()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSel = m_coPicType.GetCurSel();
	if(0 == iSel)  //bmp
	{   
		GetDlgItem(IDC_STATIC_JPGPARA)->EnableWindow(FALSE);
        m_coJpgSize.EnableWindow(FALSE);
		m_coJpgQuality.EnableWindow(FALSE);
	}
	else if(1 == iSel)          //jpg
	{
		GetDlgItem(IDC_STATIC_JPGPARA)->EnableWindow(TRUE);
        m_coJpgSize.EnableWindow(TRUE);
		m_coJpgQuality.EnableWindow(TRUE);
	}
}

void CSerialTransDlg::OnBnClickedButtoninscap()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!bInsCap)
	{
		GetDlgItem(IDC_BUTTON_inscap)->SetWindowText("停止连续抓拍");
		SetTimer(6,1000,0);
		bInsCap=true;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_inscap)->SetWindowText("连续抓拍");
		KillTimer(6);
		bInsCap=false;
	}
}

void CSerialTransDlg::OnBnClickedButtonFocus()
{
	// TODO: 在此添加控件通知处理程序代码
		byte addr;
	if(((CButton*)GetDlgItem(IDC_RADIO_infraredcam))->GetCheck())
	{
		addr=0x02;
		GetDlgItem(IDC_BUTTON_FOCUS)->SetWindowText("红外相机聚焦");
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_visiblelightcam))->GetCheck())
	{
		addr=0x01;
		GetDlgItem(IDC_BUTTON_FOCUS)->SetWindowText("可见光相机聚焦");
	}
	UpdateData(true);
	int iF;
	BYTE f1,f2,checksum;
	CString fset;
	iF=(int)(m_Focus);
	f1=(iF>>8)&0xFF;
	f2=iF&0xFF;
	checksum=addr+0x00+0x6F+f1+f2;
	fset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,addr,0x00,0x6F,f1,f2,checksum);
	SendData(fset);
}

void CSerialTransDlg::OnBnClickedButtonstarttrace()
{
    // TODO: 在此添加控件通知处理程序代码
    gIsReady=true;
}


void CSerialTransDlg::OnBnClickedButtonstoptrace()
{
    // TODO: 在此添加控件通知处理程序代码
    gIsReady=false;
}


void CSerialTransDlg::OnBnClickedButtonopenlaser()
{
    // TODO: 在此添加控件通知处理程序代码
	BYTE checksum;
	CString laserset;
	checksum=0x01+0x00+0x09+0x00+0x02;
	laserset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x09,0x00,0x02,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, laserset);	
	m_csDataSend=laserset;
	pGlobleCtrl->SendData(laserset);
}


void CSerialTransDlg::OnBnClickedButtoncloselaser()
{
    // TODO: 在此添加控件通知处理程序代码
    	BYTE checksum;
	CString laserset;
	checksum=0x01+0x00+0x07+0x00+0xe2;
	laserset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x07,0x00,0xe2,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, laserset);	
	m_csDataSend=laserset;
	pGlobleCtrl->SendData(laserset);
}


void CSerialTransDlg::OnBnClickedButtonaddlaserarea()
{
    // TODO: 在此添加控件通知处理程序代码
    BYTE checksum;
	CString laserset;
	checksum=0x01+0x04+0x00+0x00+0x00;
	laserset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x04,0x00,0x00,0x00,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, laserset);	
	m_csDataSend=laserset;
	pGlobleCtrl->SendData(laserset);
}


void CSerialTransDlg::OnBnClickedButtonminuslaserarea()
{
    // TODO: 在此添加控件通知处理程序代码
    BYTE checksum;
	CString laserset;
	checksum=0x01+0x02+0x00+0x00+0x00;
	laserset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x02,0x00,0x00,0x00,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, laserset);	
	m_csDataSend=laserset;
	pGlobleCtrl->SendData(laserset);
}


void CSerialTransDlg::OnBnClickedButtondistance()
{
    // TODO: 在此添加控件通知处理程序代码
        	BYTE checksum;
	CString laserset;
	checksum=0x01+0x00+0x07+0x00+0xe4;
	laserset.Format("%02x %02x %02x %02x %02x %02x %02x",0xFF,0x01,0x00,0x07,0x00,0xe4,checksum);
	SetDlgItemText(IDC_EDIT_DATA_SEND, laserset);	
	m_csDataSend=laserset;
	pGlobleCtrl->SendData(laserset);
}



