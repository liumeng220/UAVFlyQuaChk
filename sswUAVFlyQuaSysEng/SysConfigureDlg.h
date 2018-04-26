#pragma once
#include "afxcmn.h"
#include "SysConfigureDataDlg.h"
#include "SysConfigureParaDlg.h"
#include "MyFunctions.h"
// CSysConfigureDlg �Ի���

class CSysConfigureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSysConfigureDlg)

public:
	CSysConfigureDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSysConfigureDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SYSTEM_CONFIGURE };
protected:
	void InitItemPos();
	void ResetItemPos();
	POINT old;
	SSWstuSysConfigData m_sswSysCfgData;
	SSWstuSysConfigPara m_sswSysCfgPara;
private:

	CSysConfigureDataDlg* m_pDlgSysConfigData;
	CSysConfigureParaDlg* m_pDlgSysConfigPara;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabSysConfig;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabSystemConfigure(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
