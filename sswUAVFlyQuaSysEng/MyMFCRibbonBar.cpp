// MyMFCRibbonBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sswUAVFlyQuaSysEng.h"
#include "MyMFCRibbonBar.h"


// CMyMFCRibbonBar

IMPLEMENT_DYNAMIC(CMyMFCRibbonBar, CMFCRibbonBar)

CMyMFCRibbonBar::CMyMFCRibbonBar()
{

}

CMyMFCRibbonBar::~CMyMFCRibbonBar()
{
}


BEGIN_MESSAGE_MAP(CMyMFCRibbonBar, CMFCRibbonBar)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CMyMFCRibbonBar ��Ϣ�������




void CMyMFCRibbonBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������
}
