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

// sswUAVFlyQuaSysDoc.h : CsswUAVFlyQuaSysDoc ��Ľӿ�
//


#pragma once
#include "MyFunctions.h"
class CMainFrame;
class CsswUAVFlyQuaSysView;

// class CsswUAVFlyQua;
// class CsswUAVImage;

struct stuOverlap
{
	int nIdx1;
	int nIdx2;
	double dOverlapX;
	double dOverlapY;
};
class CsswUAVFlyQuaSysDoc : public CDocument
{
protected: // �������л�����
	CsswUAVFlyQuaSysDoc();
	DECLARE_DYNCREATE(CsswUAVFlyQuaSysDoc)

protected:
	void ClearData();
	void UnionParaFormat();
public:
	bool PrjNew(CString strPath,vector<CsswUAVImage> &vecImgInfo, SSWstuSysConfigPara para, bool bWithCmr, bool bAutoManageImages);
// ����
public:
	/**2016-12-28**/
	bool m_bCmrFile;
	float m_fOverlapRatio4;

	CsswUAVFlyQuaPrj m_FlyQuaPrj;
	CString m_strPrjFilePath;
	vector<CsswUAVImage> m_vecImgInfo;
	vector<CString> m_vecInvalidImgPath;
	CString m_strMatchAdjFolder;
	CString m_strImgListFilePath;
	CString m_strPosListFilePath;
	CString m_strMatchMatrixFilePath;
	CString m_strAdjReportFilePath;
	CString m_strFlyQuaRepPath;
	bool m_bTransXYInView;
	bool m_bPrjLoaded;
	bool m_bAutoManageImages;
	bool m_bInRunning;
	int m_nImgNum;
	double m_dMeanHei;
//	int m_nMatchPointNumThresold;
	SSWFLYQUAMETHOD m_eFlyQuaChkMethod;
	bool GetConfigInfo(SSWstuSysConfigData &data, SSWstuSysConfigPara &para);
	void InitViewRect();
	void FillPosRender();
	void UpdatePrjTree();
	void ReSortStrips();
	void ManageImgFile5();
	void ManageImgFile3();
// ����
public:
	CsswUAVFlyQuaSysView* m_pView;
	CMainFrame* m_pMain;
// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CsswUAVFlyQuaSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnButtonPrjNew();
	afx_msg void OnButtonRunFlyquaCheck();
	afx_msg void OnUpdateButtonRunFlyquaCheck(CCmdUI *pCmdUI);
	afx_msg void OnButtonFlyquaReport();
	afx_msg void OnUpdateButtonFlyquaReport(CCmdUI *pCmdUI);
	afx_msg void OnButtonPrjOpen();
// 	afx_msg void OnComboCheckMethod();
// 	afx_msg void OnUpdateComboCheckMethod(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonPrjNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonPrjOpen(CCmdUI *pCmdUI);
	afx_msg void OnButtonPrjSave();
	afx_msg void OnUpdateButtonPrjSave(CCmdUI *pCmdUI);
	afx_msg void OnEditMeanHeight();
	afx_msg void OnUpdateEditMeanHeight(CCmdUI *pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnButtonDataOrder();
	afx_msg void OnUpdateButtonDataOrder(CCmdUI *pCmdUI);
// 	afx_msg void OnEditMatchpointNumThresold();
// 	afx_msg void OnUpdateEditMatchpointNumThresold(CCmdUI *pCmdUI);
};
