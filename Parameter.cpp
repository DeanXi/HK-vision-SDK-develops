
#include "stdafx.h"
#include "Parameter.h"
#include "afxcmn.h"

//#include "ComAssistDlg.h"

CParameter::CParameter(void)
{
	int iPicNum=0;//Set channel NO.1
	int EA=0;//ˮƽ�Ƕ�
	int AZ=0;//�����Ƕ�
	int WaitTime=15;//�ȴ�ʱ��
	int EAmove=30,AZmove=10;//����ת���Ƕ�
	LONG nPort=-1;
	FILE *Videofile=NULL;
	FILE *Audiofile=NULL;
	char filename[100];//�ļ�·��
	HWND hPlayWnd=NULL;
	CString ErrorNum;
	CString gFilename=' ';
	CString m_strFilePath;
	//bool gIsReady=false,gIsCap=false;// trueʱ�洢ͼ��
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


	LocalPort = GetPrivateProfileInt(_T("System"), _T("���ؽ��ն˿�"),8000, strPath); 
	LocalSendPort = GetPrivateProfileInt(_T("System"), _T("���ط��Ͷ˿�"),6666, strPath); 
	GetPrivateProfileString(_T("System"),_T("IP��ַ"), _T("127.0.0.1"), ip.GetBuffer(MAX_PATH), MAX_PATH, strPath);
    PeerPort = GetPrivateProfileInt(_T("System"), _T("Զ�̶˿�"),8003, strPath); 
	GetPrivateProfileString(_T("System"), _T("�豸�ͺ�"), _T("HKVS"), DeviceType.GetBuffer(MAX_PATH), MAX_PATH, strPath);	
	GetSetting(DeviceType,strPath,&C1);
	ROIx = GetPrivateProfileInt(DeviceType, _T("ROIx"),128, strPath); 
	ROIy = GetPrivateProfileInt(DeviceType, _T("ROIy"),128, strPath); 
	//DeviceNum=GetPrivateProfileInt(_T("�豸"), _T("�豸��"), 1, strPath);

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
	WritePrivateProfileString(_T("System"), _T("�豸�ͺ�"), DeviceType, strPath);
	WritePrivateProfileString(_T("System"), _T("IP��ַ"), ip, strPath);
	WritePrivateProfileString(_T("System"), _T("Զ������˿�"),pp, strPath);
	WritePrivateProfileString(_T("System"), _T("���ؽ��ն˿�"),lp, strPath);
	WritePrivateProfileString(_T("System"), _T("���ط��Ͷ˿�"),sp, strPath);
	WriteSettingFile(DeviceType,strPath,&C1);
	WritePrivateProfileString(DeviceType, _T("ROIx"),roix, strPath);
	WritePrivateProfileString(DeviceType, _T("ROIy"),roiy, strPath);
	return true;
}
void CParameter::GetSetting(CString text, CString strPath,Device *a)
{
	 a->Com = GetPrivateProfileInt(text, _T("���ں�"), 1, strPath);
	 a->Baud= GetPrivateProfileInt(text, _T("������"), 9600, strPath);
	 GetPrivateProfileString(text, _T("У��λ"), _T("N"), a->Parity.GetBuffer(MAX_PATH), MAX_PATH, strPath);	
	 a->Databits = GetPrivateProfileInt(text, _T("����λ"), 8, strPath);
	 a->Stopbits = GetPrivateProfileInt(text, _T("ֹͣλ"), 1, strPath); 
	 a->PSpeed = GetPrivateProfileInt(text,_T("ˮƽ�ٶ�"), 15, strPath);
	 a->TSpeed = GetPrivateProfileInt(text,_T("�����ٶ�"), 15, strPath);
	 GetPrivateProfileString(text,_T("��½IP1"), _T("192.168.0.64"), a->IP1.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	 GetPrivateProfileString(text,_T("��½IP2"), _T("192.168.0.74"), a->IP2.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	 GetPrivateProfileString(text,_T("����"), _T("admin12345"), a->Password.GetBuffer(MAX_PATH), MAX_PATH, strPath);
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
	WritePrivateProfileString(text,_T("�����ٶ�"), ts, strPath);
	WritePrivateProfileString(text,_T("ˮƽ�ٶ�"), ps, strPath);
	WritePrivateProfileString(text, _T("���ں�"), nCom, strPath);
	WritePrivateProfileString(text, _T("������"), nBaud, strPath);
	WritePrivateProfileString(text, _T("У��λ"), cParity, strPath);
	WritePrivateProfileString(text, _T("����λ"), nDatabits, strPath);
	WritePrivateProfileString(text, _T("ֹͣλ"), nStopbits, strPath);
	WritePrivateProfileString(text, _T("��½IP1"),a->IP1, strPath);
	WritePrivateProfileString(text, _T("��½IP2"),a->IP2, strPath);
}//д�������ļ�