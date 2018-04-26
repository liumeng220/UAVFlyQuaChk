// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

// 	// ����ѡ�����:
// 	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
// 	{
// 		TRACE0("δ�ܴ������ѡ�����\n");
// 		return -1;      // δ�ܴ���
// 	}

	// �����������:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

// 	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
// 		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
// 		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
// 	{
// 		TRACE0("δ�ܴ����������\n");
// 		return -1;      // δ�ܴ���
// 	}
	if (!m_wndOutputLog.Create(dwStyle, rectDummy, this, 2))
	{
		TRACE0("δ�ܴ����������\n");
		return -1;      // δ�ܴ���
	}
	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// ���б��ڸ��ӵ�ѡ�:
// 	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
// 	ASSERT(bNameValid);
//	m_wndTabs.AddTab(&m_wndOultputBuild, strTabName, (UINT)0);
// 	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
// 	ASSERT(bNameValid);
// 	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
// 	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
// 	ASSERT(bNameValid);
// 	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);

	// ʹ��һЩ�����ı���д���ѡ�(���踴������)
//	FillLogWindow();
// 	FillDebugWindow();
// 	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// ѡ��ؼ�Ӧ��������������:
	m_wndOutputLog.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillLogWindow(CString strLog, bool bAdd)
{
	if(!bAdd&&m_wndOutputLog.GetCount()>0)
	{
		m_wndOutputLog.DeleteString(m_wndOutputLog.GetCount()-1);
	}
	m_wndOutputLog.AddString(strLog);
	if(m_wndOutputLog.GetCount()>1)
		m_wndOutputLog.SetCurSel(m_wndOutputLog.GetCount()-1);
	m_wndOutputLog.SetRedraw(TRUE);
}
void COutputWnd::UpdateFonts()
{
	m_wndOutputLog.SetFont(&afxGlobalData.fontRegular);
// 	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
// 	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
	m_bScroll = false;
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList ��Ϣ�������

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("�������"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("������"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


BOOL COutputList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(!m_bScroll) return TRUE;
	else return CListBox::OnEraseBkgnd(pDC);
	m_bScroll = false;
}


void COutputList::OnPaint()
{
//	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CListBox::OnPaint()
	CPaintDC dc(this); // device context for painting

	CRect   rect; 
	GetClientRect(&rect); 

	CDC   MemDC;//�ڴ�DC 
	CBitmap   MemBmp; 
	MemDC.CreateCompatibleDC(&dc); 
	MemBmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	MemDC.SelectObject(&MemBmp); 
	//��ʱ������λͼֻ��һ�ź�ɫ��ֽ
	//�Ȼ��ϱ���    
	MemDC.FillSolidRect(rect, RGB(255, 255, 255));
	//�ٻ���ͼ��
	//��һ���ǵ���Ĭ�ϵ�OnPaint(),��ͼ�λ����ڴ�DC���� 
	DefWindowProc(WM_PAINT, (WPARAM)MemDC.m_hDC, (LPARAM)0); 

	//��� 
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY); 

	MemDC.DeleteDC(); 
	MemBmp.DeleteObject(); 
}


void COutputList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bScroll = true;
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}


void COutputList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bScroll =true;
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL COutputList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_bScroll = true;
	return CListBox::OnMouseWheel(nFlags,zDelta,pt);
}

void COutputWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������
}
