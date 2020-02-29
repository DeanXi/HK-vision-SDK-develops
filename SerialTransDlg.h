// SerialTransDlg.h : header file
//

#if !defined(AFX_SERIALTRANSDLG_H__52949285_99B6_4B1A_A2A0_842D1930650D__INCLUDED_)
#define AFX_SERIALTRANSDLG_H__52949285_99B6_4B1A_A2A0_842D1930650D__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "HCNetSDK.h"

#include "GeneralDef.h"
#include <Afxmt.h> 
#include "Logwriter.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <winsock2.h>
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CSerialTransDlg dialog


class CSerialTransDlg : public CDialog
{
// Construction
public:
	CSerialTransDlg(CWnd* pParent = NULL);	// standard constructor

public:
	int m_iCurSerialType;
	int m_iSerialChan;
	//int m_iCurInterface;      //发送接口，0-长连接，需建立透明通道  1-不需要建立透明通道
	
	BOOL DoLogin();
	BOOL m_bIsLogin;
	LOCAL_DEVICE_INFO m_struDeviceInfo,m_struDeviceInfo1;
	char m_DataBuf[DATABUF_LEN];
	int m_DataLen;
	afx_msg static UINT UDPRemoteListenThread(LPVOID lpParam);
	void SetHAngle(double HAngle);
	void SetVAngle(double VAngle);
    CWinThread * pUDPRemoteLisenThread;
// Dialog Data
	//{{AFX_DATA(CSerialTransDlg)
	enum { IDD = IDD_SERIALTRANS_DIALOG };
	CListCtrl	m_RecvDataList;
	CComboBox	m_comboInterface;
	CComboBox	m_comboChan;
	CComboBox	m_comboSerialType;
	CIPAddressCtrl	m_ctrlDevIp;
	UINT	m_nDevPort;
	CString	m_csUser;
	CString	m_csPWD;
	CString	m_csDataSend;
	UINT	m_iSendInterval;
	BOOL	m_bSendContinual;
public:

	afx_msg void OnButtonSend();
	//}}AFX_DATA
	afx_msg BOOL PreTranslateMessage(MSG *pMsg);
	BOOL PreTranslateMsg(MSG *pMsg);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialTransDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLogin();
	afx_msg void OnSelchangeComboSerialType();
	afx_msg void OnButtonSerialstart();
	afx_msg void OnSelchangeComboChan();
	afx_msg void OnButtonSerialstop();
	//}}AFX_VIRTUAL
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialTransDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//回传数据//////////////////////////////////////////////////////
	LONG lUserID,lUserID1;
	CComboBox n_Channel;
	NET_DVR_DEVICECFG m_struCfg;
	CStatic m_picplay;
	CBitmap m_cBackGround;
	CBitmap	m_cOverlay;
	short iPChannel;
	CIPAddressCtrl m_ctrlDeviceIP;
	CString m_csUserName;
	CString m_csPassword;
	short   m_nLoginPort;
	//回传//////////////////////////////////////////////////////////////
	int IMAGE_PORT;
public:
	bool m_isFullScreen;
	CRect m_Old;
	void StartPlay();
	void StopPlay();
	afx_msg void OnBnClickedButtonSetted();
	afx_msg void OnBnClickedButtonQh();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonset();

    POINT old;

	void SendData(CString Data);
	afx_msg void OnBnClickedButtonCapimg();
	double m_ha;
	double m_va;
	LONG m_lPlayHandle;
	float m_dbldisplayh;
	double m_dbdisplayv;
	double m_Zoom;
	afx_msg void OnBnClickedButtonsetzoom();
	CIPAddressCtrl m_ctrlDevIP;
	CComboBox m_coPicType;
	CComboBox m_coJpgSize;
	CComboBox m_coJpgQuality;
	afx_msg void OnCbnSelchangeComboPicType();
	afx_msg void OnBnClickedButtoninscap();
	BOOL bInsCap;
	void CaptureImg();
	afx_msg void OnBnClickedButtonFocus();
	double m_Focus;
    afx_msg void OnBnClickedButtonstarttrace();
    afx_msg void OnBnClickedButtonstoptrace();
    afx_msg void OnBnClickedButtonopenlaser();
    afx_msg void OnBnClickedButtoncloselaser();
    afx_msg void OnBnClickedButtonaddlaserarea();
    afx_msg void OnBnClickedButtonminuslaserarea();
    afx_msg void OnBnClickedButtondistance();
    afx_msg void OnBnClickedButtonfocusup();
    afx_msg void OnBnClickedButtonaddzoom();
    afx_msg void OnBnClickedButtonfocusdown();
    afx_msg void OnBnClickedButtonlesszoom();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALTRANSDLG_H__52949285_99B6_4B1A_A2A0_842D1930650D__INCLUDED_)
