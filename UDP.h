#pragma once

// UDP ����Ŀ��

class UDP : public CSocket
{
public:
	UDP();
	virtual ~UDP();
	virtual void OnReceive(int nErrorCode);

	void SetDlg(void *pDlg);
protected:
	double ha,va;
	void *m_pDlg;
};


