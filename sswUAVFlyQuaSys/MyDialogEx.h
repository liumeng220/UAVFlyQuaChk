#pragma once


// CMyDialogEx �Ի���

class CMyDialogEx : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDialogEx)

public:
	CMyDialogEx(CWnd* pParent = NULL);   // ��׼���캯��
	CMyDialogEx(UINT nIDTemplate, CWnd *pParent = NULL);
	virtual ~CMyDialogEx();

// �Ի�������
	enum { IDD = IDD_MYDIALOGEX };
protected:
	void InitItemPos();
	void ResetItemPos();
	POINT old;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
