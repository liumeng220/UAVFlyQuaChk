// MyTreeCtrlEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sswUAVFlyQuaSys.h"
#include "MainFrm.h"
#include "sswUAVFlyQuaSysDoc.h"
#include "sswUAVFlyQuaSysView.h"
#include "MyTreeCtrlEx.h"
#include "MyFunctions.h"
// CMyTreeCtrlEx

IMPLEMENT_DYNAMIC(CMyTreeCtrlEx, CTreeCtrl)

CMyTreeCtrlEx::CMyTreeCtrlEx()
{

}

CMyTreeCtrlEx::~CMyTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrlEx, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CMyTreeCtrlEx::OnNMClick)
END_MESSAGE_MAP()



// CMyTreeCtrlEx ��Ϣ�������




void CMyTreeCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);//ת��Ϊ�ͻ�����
	UINT uFlags;
	HTREEITEM CurrentItem;

	CurrentItem = HitTest(point, &uFlags);//��õ�ǰ����ڵ��ITEM

	if(CurrentItem)
	{
		int nData = GetItemData(CurrentItem);
		
		if(nData>=0)  //��ЧӰ��
		{
			vector<CString> vecItemInfo;
			if(nData==0)
				for (int i = 0; i<GetDocHand()->m_nImgNum; i++)
				{
					if(GetDocHand()->m_vecImgInfo[i].m_bValid==false)
					{
						vecItemInfo.push_back(FunGetFileName(GetDocHand()->m_vecImgInfo[i].m_strImgPath,true));
					}
				}
			else 
			{
				vector<int> vImgIdx = GetDocHand()->m_FlyQuaPrj.GetStripImgIdx()[nData-1];
				for (int i = 0; i<vImgIdx.size(); i++)
				{
					vecItemInfo.push_back(FunGetFileName(GetDocHand()->m_vecImgInfo[vImgIdx[i]].m_strImgPath,true));
				}
				vector<int>().swap(vImgIdx);
			}
		//	GetMainFramHand()->m_wndStripImg.LoadData(vecItemInfo);
			GetMainFramHand()->ShowStripList(vecItemInfo);
			vector<CString>().swap(vecItemInfo);
		}
		GetViewHand()->m_nStripIdxSelected = nData-1;
		GetViewHand()->ShowSeletctedStrip();
		GetViewHand()->Invalidate();
	}
	*pResult = 0;
}
