
#include "stdafx.h"
#include "Parameter.h"
#include "afxcmn.h"

//#include "ComAssistDlg.h"

CParameter::CParameter(void)
{
	int iPicNum=0;//Set channel NO.1
	int EA=0;//水平角度
	int AZ=0;//俯仰角度
	int WaitTime=15;//等待时间
	int EAmove=30,AZmove=10;//单步转动角度
	LONG nPort=-1;
	FILE *Videofile=NULL;
	FILE *Audiofile=NULL;
	char filename[100];//文件路径
	HWND hPlayWnd=NULL;
	CString ErrorNum;
	CString gFilename=' ';
	CString m_strFilePath;
	//bool gIsReady=false,gIsCap=false;// true时存储图像
	bool gPM=false;
	byte recvdata[1024],data[7];
	long recvlenth;
}


CParameter::~CParameter(void)
{
}
bool CParameter:: GetParam()
{
	//CString strPath;

	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\Setting.ini";


	LocalPort = GetPrivateProfileInt(_T("System"), _T("本地接收端口"),8000, strPath); 
	LocalSendPort = GetPrivateProfileInt(_T("System"), _T("本地发送端口"),6666, strPath); 
	GetPrivateProfileString(_T("System"),_T("IP地址"), _T("127.0.0.1"), ip.GetBuffer(MAX_PATH), MAX_PATH, strPath);
    PeerPort = GetPrivateProfileInt(_T("System"), _T("远程端口"),8003, strPath); 
	GetPrivateProfileString(_T("System"), _T("设备型号"), _T("HKVS"), DeviceType.GetBuffer(MAX_PATH), MAX_PATH, strPath);	
	GetSetting(DeviceType,strPath,&C1);
	ROIx = GetPrivateProfileInt(DeviceType, _T("ROIx"),128, strPath); 
	ROIy = GetPrivateProfileInt(DeviceType, _T("ROIy"),128, strPath); 
	//DeviceNum=GetPrivateProfileInt(_T("设备"), _T("设备号"), 1, strPath);

	return true;
}
bool CParameter::SaveParam()
{
	CString pp,lp,sp,roix,roiy;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\Setting.ini";

	//ip.Format(_T("%d.%d.%d.%d"),field[0], field[1], field[2], field[3]);
	pp.Format(_T("%d"),PeerPort);
	lp.Format(_T("%d"),LocalPort);
	sp.Format(_T("%d"),LocalSendPort);
	roix.Format(_T("%d"), ROIx);
	roiy.Format(_T("%d"), ROIy);
	WritePrivateProfileString(_T("System"), _T("设备型号"), DeviceType, strPath);
	WritePrivateProfileString(_T("System"), _T("IP地址"), ip, strPath);
	WritePrivateProfileString(_T("System"), _T("远程网络端口"),pp, strPath);
	WritePrivateProfileString(_T("System"), _T("本地接收端口"),lp, strPath);
	WritePrivateProfileString(_T("System"), _T("本地发送端口"),sp, strPath);
	WriteSettingFile(DeviceType,strPath,&C1);
	WritePrivateProfileString(DeviceType, _T("ROIx"),roix, strPath);
	WritePrivateProfileString(DeviceType, _T("ROIy"),roiy, strPath);
	return true;
}
void CParameter::GetSetting(CString text, CString strPath,Device *a)
{
	 a->Com = GetPrivateProfileInt(text, _T("串口号"), 1, strPath);
	 a->Baud= GetPrivateProfileInt(text, _T("波特率"), 9600, strPath);
	 GetPrivateProfileString(text, _T("校验位"), _T("N"), a->Parity.GetBuffer(MAX_PATH), MAX_PATH, strPath);	
	 a->Databits = GetPrivateProfileInt(text, _T("数据位"), 8, strPath);
	 a->Stopbits = GetPrivateProfileInt(text, _T("停止位"), 1, strPath); 
	 a->PSpeed = GetPrivateProfileInt(text,_T("水平速度"), 15, strPath);
	 a->TSpeed = GetPrivateProfileInt(text,_T("俯仰速度"), 15, strPath);
	 GetPrivateProfileString(text,_T("登陆IP1"), _T("192.168.0.64"), a->IP1.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	 GetPrivateProfileString(text,_T("登陆IP2"), _T("192.168.0.74"), a->IP2.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	 GetPrivateProfileString(text,_T("密码"), _T("admin12345"), a->Password.GetBuffer(MAX_PATH), MAX_PATH, strPath);
}
void CParameter::WriteSettingFile(CString text, CString strPath,Device *a)
{
	CString nCom, nBaud, cParity, nDatabits, nStopbits,ps,ts;
	ts.Format(_T("%d"),a->TSpeed);
	ps.Format(_T("%d"),a->PSpeed);
	nCom.Format(_T("%d"), a->Com);
	nBaud.Format(_T("%d"), a->Baud);
	cParity=a->Parity;
	nDatabits.Format(_T("%d"), a->Databits);
	nStopbits.Format(_T("%d"), a->Stopbits);
	WritePrivateProfileString(text,_T("俯仰速度"), ts, strPath);
	WritePrivateProfileString(text,_T("水平速度"), ps, strPath);
	WritePrivateProfileString(text, _T("串口号"), nCom, strPath);
	WritePrivateProfileString(text, _T("波特率"), nBaud, strPath);
	WritePrivateProfileString(text, _T("校验位"), cParity, strPath);
	WritePrivateProfileString(text, _T("数据位"), nDatabits, strPath);
	WritePrivateProfileString(text, _T("停止位"), nStopbits, strPath);
	WritePrivateProfileString(text, _T("登陆IP1"),a->IP1, strPath);
	WritePrivateProfileString(text, _T("登陆IP2"),a->IP2, strPath);
}//写入配置文件