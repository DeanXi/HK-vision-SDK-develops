// UDP.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"		// 主符号

#include "UDP.h"

#include <cstdlib>
// UDP

UDP::UDP()
{
}

UDP::~UDP()
{
}


// UDP 成员函数

void UDP::SetDlg(void *pDlg)
{
	m_pDlg=pDlg;
}

void UDP::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	int nn=0,i,j,k;
	char str[100]={0};
	char s1[10]={0};
	char s2[10]={0};
	char s3[10]={0};
	


	CString strtest;  
	strtest.Format(_T("%s"),str); 
	int len=strlen(str);
	//AfxMessageBox(strtest);

	for(i=0;i<len;i++)
	{
		if(str[i]==';')
		{
			nn=0;
		    break;
			//AfxMessageBox(s1);
		}
		s1[i]=str[i];
	}
	for(j=i+1;j<len;j++)
	{
		if(str[j]==';')
		{
			nn=0;
		    break;
			//AfxMessageBox(s2);
		}
		s2[nn]=str[j];
		nn++;
	}
	for(k=j+1;k<len;k++)
	{
		if(str[k]==';')
		{
			nn=0;
		    break;
			//AfxMessageBox(s3);
		}
		s3[nn]=str[k];
		nn++;
	}
	if(!strcmp(s1,"POS"))
	{
		ha=atof(s2);
		va=atof(s3);
	}
	//CWnd *pWnd=CWnd::FindWindow(ComAssistDlg,_T("Bb")); 
	//SendMessage(ComAssistDlg,);
	//CComAssistDlg *pDlg;
	//pDlg=(CComAssistDlg *)m_pDlg;

	CSocket::OnReceive(nErrorCode);
}
