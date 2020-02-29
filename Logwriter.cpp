// Logwriter.cpp : ʵ���ļ�
//
#include "afxdialogex.h"
#include "stdafx.h"
#include "Logwriter.h"
// CLogWriter
// shell api����Ŀ¼�������
#include "shlwapi.h" 
#pragma comment(lib, "shlwapi.lib")

// Logwriter

Logwriter::Logwriter()
{
	GetModuleFileName(NULL, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	m_strFilePath.ReleaseBuffer();
	int nPos = m_strFilePath.ReverseFind('\\');
	m_strFilePath = m_strFilePath.Left(nPos);
	m_strFilePath += "\\log.txt";
	//m_strFilePath = _T(".\\log");   // �ļ��е�·������������Եģ������Ǿ��Եġ�
	////   �ж�·���Ƿ����   
	//if   (! PathIsDirectory(m_strFilePath))   
	//{   
	//		if   (! CreateDirectory(m_strFilePath, NULL))   
	//		{   
 //               if (AfxMessageBox(_T("��־·������ʧ��..."), MB_ICONSTOP, 0)== IDYES)   
	//				return;   
 //           }   
	//} 
	//m_strFilePath += _T("\\log.txt"); 
}

Logwriter::~Logwriter()
{
}


// Logwriter ��Ա����
//void Logwriter::WriteString(TCHAR *ch, int nLine, TCHAR *fileName)
//{
//	CString str;
//	str.Format(_T("%s"), ch);
//	WriteString(str,nLine,fileName);
//}

//void Logwriter::WriteString(CString str, int nLine, TCHAR *fileName)
//{
//	CString strAdd;
//	CTime ti = CTime::GetCurrentTime();
//	strAdd.Format(_T("%d/%2d/%2d  %2d:%2d:%2d  "), ti.GetYear(), ti.GetMonth(), ti.GetDay(), ti.GetHour(), ti.GetMinute(), ti.GetSecond());
//	
//	CFile t;
//	if(!t.Open(m_strFilePath, CFile::modeWrite, NULL))
//	{
//		t.Open(m_strFilePath, CFile::modeWrite | CFile::modeCreate, NULL);
//	}
//	str = strAdd + str;
//	strAdd.Format(_T("  line: %d    file: %s\r\n"), nLine, fileName);
//	str += strAdd;
//	
//	t.SeekToEnd();
//	t.Write(str.GetBuffer(str.GetLength()), str.GetLength());
//	t.Close();
//}

void Logwriter::WriteString(CString str)
{
	CString strAdd;
	CTime ti = CTime::GetCurrentTime();
	strAdd.Format(_T("%d/%2d/%2d %2d:%2d:%2d  "), ti.GetYear(), ti.GetMonth(), ti.GetDay(), ti.GetHour(), ti.GetMinute(), ti.GetSecond());
	str = strAdd + str + _T("\r\n");
	CFile t;
	if(!t.Open(m_strFilePath, CFile::modeWrite, NULL))
	{
		t.Open(m_strFilePath, CFile::modeWrite | CFile::modeCreate, NULL);
	}
	
	t.SeekToEnd();
	t.Write(str.GetBuffer(str.GetLength()), str.GetLength());
	t.Close();
}

void Logwriter::WriteString(TCHAR *ch)
{
	CString str;
	str.Format(_T("%s"), ch);
	WriteString(str);
}
void Logwriter::WriteString(float h0,float h1)
{
	CString str;
	str.Format(_T("%15.6f\n%15.6f\n"),h0,h1);
	WriteString(str);
}
void Logwriter::WriteString(int h0,int h1)
{
	CString str;
	str.Format(_T("%d\n%d\n"),h0,h1);
	WriteString(str);
}