#pragma once

// Logwriter ÃüÁîÄ¿±ê

class Logwriter : public CObject
{
public:
	Logwriter();
	virtual ~Logwriter();
private:
	CString m_strFilePath;
public:
	void WriteString(CString str);
	void WriteString(float h0,float h1);
    void WriteString(int h0,int h1);
	void WriteString(TCHAR *ch);
	//void WriteString(CString str, int nLine, TCHAR *fileName);
	//void WriteString(TCHAR *ch, int nLine, TCHAR *fileName);
};


