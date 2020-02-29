#include "stdafx.h"

#pragma once
//#include "winsock2.h"

class CParameter
{
public:
	CParameter(void);
	~CParameter(void);
public:
	//int Baud1, Baud2,Baud3;       //波特率1控制2水平3俯仰
	//int Com1, Com2,Com3;        //串口号
	//CString Parity1;
	//CString Parity2;
	//CString Parity3;    //校验
	//int Databits1, Databits2,Databits3;   //数据位
	//int Stopbits1, Stopbits2,Stopbits3;   //停止位
	
	CString ip,strPath,DeviceType;
	int LocalPort,PeerPort,LocalSendPort;
	int DeviceNum;
	bool GetParam();
	bool SaveParam();
    struct Device
	{

		int Baud;
		int Com;
		CString Parity;
		CString IP1,IP2,Password;
		int Databits;
		int Stopbits;
		int TSpeed,PSpeed;
	};
	Device C1;
	int ROIx,ROIy;
	void GetSetting(CString text, CString strPath,Device *a);
	void WriteSettingFile(CString text, CString strPath,Device *a);
};

