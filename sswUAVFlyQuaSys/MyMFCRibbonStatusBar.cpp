#include "StdAfx.h"
#include "MyMFCRibbonStatusBar.h"


CMyMFCRibbonStatusBar::CMyMFCRibbonStatusBar(void)
{
}


CMyMFCRibbonStatusBar::~CMyMFCRibbonStatusBar(void)
{
}
BEGIN_MESSAGE_MAP(CMyMFCRibbonStatusBar, CMFCRibbonStatusBar)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


void CMyMFCRibbonStatusBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������
}
