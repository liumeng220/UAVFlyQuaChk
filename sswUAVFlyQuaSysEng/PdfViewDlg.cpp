// PdfViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sswUAVFlyQuaSysEng.h"
#include "PdfViewDlg.h"
#include "afxdialogex.h"

#include "MyFunctions.h"
#include "sswUAVFlyQuaSysEngDoc.h"
// CPdfViewDlg �Ի���

IMPLEMENT_DYNAMIC(CPdfViewDlg, CDialogEx)

CPdfViewDlg::CPdfViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPdfViewDlg::IDD, pParent)
{
//	m_pDoc = NULL;
}

CPdfViewDlg::~CPdfViewDlg()
{
}

void CPdfViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPdfViewDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(ID_MSG_SHOW_PDF, OnMsgShowPdf)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPdfViewDlg ��Ϣ�������
LRESULT CPdfViewDlg::OnMsgShowPdf(WPARAM wParam,LPARAM lParam)
{
	CString str = *(CString*)lParam;
	ShowPdf(str);
	return 1;
}

int CPdfViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_PdfWeb.Create(NULL,WS_CHILD|WS_VISIBLE,rectDummy,this, 2))
	{
		TRACE0("PDF window error\n");
		return -1;      // δ�ܴ���
	}
	return 0;
}

void CPdfViewDlg::ShowPdf(CString strPath)
{
	m_PdfWeb.Navigate(strPath,NULL,NULL,NULL,NULL);
//	m_PdfWeb.UpdateData();
}
BOOL CPdfViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
	CenterWindow();
	int nWndX = GetSystemMetrics(SM_CXSCREEN);
	int nWndY = GetSystemMetrics(SM_CYSCREEN);
	int nDlgX = min(800, nWndX);
	int nDlgY = nDlgX * 3 / 4;
	CRect rectWnd; GetWindowRect(rectWnd);
    SetWindowPos (NULL, -1, -1, nDlgX, nDlgY, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	CenterWindow();
// 	if(m_pDoc->m_eFlyQuaChkMethod==BY_POS) SetWindowText("�ʼ챨��-����POS�ʼ�");else
// 		if(m_pDoc->m_eFlyQuaChkMethod==BY_MATCH) SetWindowText("�ʼ챨��-����ƥ����ʼ�");else
// 			if(m_pDoc->m_eFlyQuaChkMethod==BY_ADJUSTMENT) SetWindowText("�ʼ챨��-����ƽ����ʼ�");
// 	m_PdfWeb.Navigate(m_pDoc->m_strFlyQuaRepPath,NULL,NULL,NULL,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPdfViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	m_PdfWeb.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	// TODO: �ڴ˴������Ϣ����������
}

void CPdfViewDlg::ReleasePdf()
{
	m_PdfWeb.ExternalRelease();
	m_PdfWeb.ExternalDisconnect();
}
void CPdfViewDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ReleasePdf();
	CDialogEx::OnClose();
//	ShowWindow(FALSE);
}
