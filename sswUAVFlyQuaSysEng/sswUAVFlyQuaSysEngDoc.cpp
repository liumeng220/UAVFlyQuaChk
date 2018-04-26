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

// sswUAVFlyQuaSysEngDoc.cpp : CsswUAVFlyQuaSysEngDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "sswUAVFlyQuaSysEng.h"
#endif
#include "MainFrm.h"
#include "sswUAVFlyQuaSysEngDoc.h"
#include "sswUAVFlyQuaSysEngView.h"
#include <propkey.h>
#include "CoorConv.hpp"
#include "PrjNewDlg.h"
#include "GradientProgressCtrl.h"
#include "MyMultiThread.h"
#include "MyFunctions.h"
#include <omp.h>
#include <io.h>
#include "gdal201/ogrsf_frmts.h"
#include "gdal201/ogr_spatialref.h"

#include "PdfViewDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CsswUAVFlyQuaSysEngDoc
extern CGradientProgressCtrl g_wndProgressCtrl;
IMPLEMENT_DYNCREATE(CsswUAVFlyQuaSysEngDoc, CDocument)

	BEGIN_MESSAGE_MAP(CsswUAVFlyQuaSysEngDoc, CDocument)
		ON_COMMAND(ID_BUTTON_PRJ_NEW, &CsswUAVFlyQuaSysEngDoc::OnButtonPrjNew)
		ON_COMMAND(ID_BUTTON_RUN_FLYQUA_CHECK, &CsswUAVFlyQuaSysEngDoc::OnButtonRunFlyquaCheck)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_RUN_FLYQUA_CHECK, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonRunFlyquaCheck)
		ON_COMMAND(ID_BUTTON_FLYQUA_REPORT, &CsswUAVFlyQuaSysEngDoc::OnButtonFlyquaReport)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_FLYQUA_REPORT, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonFlyquaReport)
		ON_COMMAND(ID_BUTTON_PRJ_OPEN, &CsswUAVFlyQuaSysEngDoc::OnButtonPrjOpen)
// 		ON_COMMAND(ID_COMBO_CHECK_METHOD, &CsswUAVFlyQuaSysEngDoc::OnComboCheckMethod)
// 		ON_UPDATE_COMMAND_UI(ID_COMBO_CHECK_METHOD, &CsswUAVFlyQuaSysEngDoc::OnUpdateComboCheckMethod)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_PRJ_NEW, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjNew)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_PRJ_OPEN, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjOpen)
		ON_COMMAND(ID_BUTTON_PRJ_SAVE, &CsswUAVFlyQuaSysEngDoc::OnButtonPrjSave)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_PRJ_SAVE, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjSave)
		ON_COMMAND(ID_EDIT_MEAN_HEIGHT, &CsswUAVFlyQuaSysEngDoc::OnEditMeanHeight)
		ON_UPDATE_COMMAND_UI(ID_EDIT_MEAN_HEIGHT, &CsswUAVFlyQuaSysEngDoc::OnUpdateEditMeanHeight)
		ON_COMMAND(ID_FILE_NEW, &CsswUAVFlyQuaSysEngDoc::OnFileNew)
		ON_COMMAND(ID_FILE_OPEN, &CsswUAVFlyQuaSysEngDoc::OnFileOpen)
		ON_COMMAND(ID_FILE_SAVE, &CsswUAVFlyQuaSysEngDoc::OnFileSave)
		ON_COMMAND(ID_BUTTON_DATA_ORDER, &CsswUAVFlyQuaSysEngDoc::OnButtonDataOrder)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_DATA_ORDER, &CsswUAVFlyQuaSysEngDoc::OnUpdateButtonDataOrder)
// 		ON_COMMAND(ID_EDIT_MATCHPOINT_NUM_THRESOLD, &CsswUAVFlyQuaSysEngDoc::OnEditMatchpointNumThresold)
// 		ON_UPDATE_COMMAND_UI(ID_EDIT_MATCHPOINT_NUM_THRESOLD, &CsswUAVFlyQuaSysEngDoc::OnUpdateEditMatchpointNumThresold)
	END_MESSAGE_MAP()

	// CsswUAVFlyQuaSysEngDoc ����/����

	CsswUAVFlyQuaSysEngDoc::CsswUAVFlyQuaSysEngDoc()
	{
		// TODO: �ڴ����һ���Թ������
		m_bPrjLoaded=false;
		m_bInRunning = false;
		m_pView=NULL;
		m_pMain=NULL;
		m_nImgNum = 0;
		m_dMeanHei = 0;
		m_eFlyQuaChkMethod = BY_ADJUSTMENT;
		m_bTransXYInView = false;
//		m_nMatchPointNumThresold = 8;
	}

	CsswUAVFlyQuaSysEngDoc::~CsswUAVFlyQuaSysEngDoc()
	{
	}

	BOOL CsswUAVFlyQuaSysEngDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: �ڴ�������³�ʼ������
		// (SDI �ĵ������ø��ĵ�)
		if(m_pMain==NULL) m_pMain = GetMainFramHand();
		return TRUE;
	}




	// CsswUAVFlyQuaSysEngDoc ���л�

	void CsswUAVFlyQuaSysEngDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: �ڴ���Ӵ洢����
		}
		else
		{
			// TODO: �ڴ���Ӽ��ش���
		}
	}

#ifdef SHARED_HANDLERS

	// ����ͼ��֧��
	void CsswUAVFlyQuaSysEngDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
	{
		// �޸Ĵ˴����Ի����ĵ�����
		dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

		CString strText = _T("TODO: implement thumbnail drawing here");
		LOGFONT lf;

		CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		pDefaultGUIFont->GetLogFont(&lf);
		lf.lfHeight = 36;

		CFont fontDraw;
		fontDraw.CreateFontIndirect(&lf);

		CFont* pOldFont = dc.SelectObject(&fontDraw);
		dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
		dc.SelectObject(pOldFont);
	}

	// ������������֧��
	void CsswUAVFlyQuaSysEngDoc::InitializeSearchContent()
	{
		CString strSearchContent;
		// ���ĵ����������������ݡ�
		// ���ݲ���Ӧ�ɡ�;���ָ�

		// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
		SetSearchContent(strSearchContent);
	}

	void CsswUAVFlyQuaSysEngDoc::SetSearchContent(const CString& value)
	{
		if (value.IsEmpty())
		{
			RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
		}
		else
		{
			CMFCFilterChunkValueImpl *pChunk = NULL;
			ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
			if (pChunk != NULL)
			{
				pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
				SetChunkValue(pChunk);
			}
		}
	}

#endif // SHARED_HANDLERS

	// CsswUAVFlyQuaSysEngDoc ���

#ifdef _DEBUG
	void CsswUAVFlyQuaSysEngDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CsswUAVFlyQuaSysEngDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif //_DEBUG


	// CsswUAVFlyQuaSysEngDoc ����
	bool CsswUAVFlyQuaSysEngDoc::GetConfigInfo(SSWstuSysConfigData &data, SSWstuSysConfigPara &para)
	{
		if(!FunReadSysConfigFile(data,para))
		{
			para = SSWstuSysConfigPara();
			return false;
		}
		return true;
	}
	void CsswUAVFlyQuaSysEngDoc::InitViewRect()
	{
		double l,r,t,b;
		r=t=-1e10;
		l=b=1e10;
		for (int i = 0; i<m_vecImgInfo.size(); i++)
		{
			l=min(l,m_vecImgInfo[i].m_ImgPosInfo.Xs);
			r=max(r,m_vecImgInfo[i].m_ImgPosInfo.Xs);
			t=max(t,m_vecImgInfo[i].m_ImgPosInfo.Ys);
			b=min(b,m_vecImgInfo[i].m_ImgPosInfo.Ys);
		}
		m_pView->m_RectPosGeo = CRectD(l-100,t+100,r+100,b-100);
		m_pView->m_RectCqViewGeo = m_pView->m_RectPosGeo;
		m_pView->m_ptClnCenter2Geo = m_pView->m_RectCqViewGeo.CenterPoint();
		m_pView->m_fZoomRateCln2Geo = min(m_pView->m_RectCln.Width()*1.0/m_pView->m_RectCqViewGeo.Width(),
			m_pView->m_RectCln.Height()*1.0/m_pView->m_RectCqViewGeo.Height())*0.7;
		//m_pView->ResetRectCln2Geo();
		m_pView->m_fZoomRateCln2GeoMin = m_pView->m_fZoomRateCln2Geo/20;
		m_pView->m_fZoomRateCln2GeoMax = m_pView->m_fZoomRateCln2Geo*20;
		m_pMain->m_wndImgAndPt.InitRepView();

	}
	void CsswUAVFlyQuaSysEngDoc::FillPosRender()
	{
		m_vecImgInfo=m_FlyQuaPrj.GetImgInfo();
		GLdouble* pv = new GLdouble[m_vecImgInfo.size()*2];
		GLdouble* pc = new GLdouble[m_vecImgInfo.size()*4];
		for (int i = 0; i<m_vecImgInfo.size(); i++)
		{
			*(pv+2*i+0)=m_vecImgInfo[i].m_ImgPosInfo.Xs;
			*(pv+2*i+1)=m_vecImgInfo[i].m_ImgPosInfo.Ys;
			if(!m_vecImgInfo[i].m_bValid)   //ת����ЧӰ��
			{
				*(pc+4*i+0)=100;
				*(pc+4*i+1)=0;
				*(pc+4*i+2)=0;
				*(pc+4*i+3)=255;
			}else
				if(m_vecImgInfo[i].m_nStripIdx==InvalidValue)  //��ת�򺽴�ʧ��
				{
					*(pc+4*i+0)=255;
					*(pc+4*i+1)=110;
					*(pc+4*i+2)=180;
					*(pc+4*i+3)=255;
				}else
					if(m_vecImgInfo[i].m_nStripIdx%2)  //��������
					{
						*(pc+4*i+0)=0;
						*(pc+4*i+1)=100;
						*(pc+4*i+2)=0;
						*(pc+4*i+3)=255;
					}else
					{
						*(pc+4*i+0)=0;
						*(pc+4*i+1)=0;
						*(pc+4*i+2)=180;
						*(pc+4*i+3)=255;
					}
		}
		m_pView->m_PosRender.FillVectorVBO(pv,pc,m_vecImgInfo.size());
		delete[]pv; pv=NULL;
		delete[]pc; pc=NULL;

		m_pView->Invalidate();


	}
	void CsswUAVFlyQuaSysEngDoc::UpdatePrjTree()
	{
		vector<CString> vecWndTreeItem(2+m_FlyQuaPrj.GetStripNum());
		CString strItem;strItem.Format("Proj-%s",m_strPrjFilePath);
		vecWndTreeItem[0]=strItem;
		strItem.Format("Image Number of Flight Adjustment-%d",m_FlyQuaPrj.GetInvalidImgNum());
		vecWndTreeItem[1]=strItem;
		for (int i = 0; i<m_FlyQuaPrj.GetStripNum(); i++)
		{
			strItem.Format("Strip_%d-%d",i+1,m_FlyQuaPrj.GetImgNumInStrip(i));
			vecWndTreeItem[i+2] = strItem;
		}
		m_pMain->ShowWndTree(vecWndTreeItem);
		vector<CString>().swap(vecWndTreeItem);
	}
	void CsswUAVFlyQuaSysEngDoc::ReSortStrips()
	{
		g_wndProgressCtrl.ShowWindow(true);
		m_FlyQuaPrj.ReOrderStrips(m_pView->m_LineParaA,m_pView->m_LineParaB,m_pView->m_LineParaC,FunCallback4FlyQuaProcess);
		m_FlyQuaPrj.SavePrj2File(m_strPrjFilePath); 
		FillPosRender();
		m_pView->m_bSaveView2Img=true;
		CString str; str.Format("Strip sorting successful, and the strip number is %d",m_FlyQuaPrj.GetStripNum());
		FunOutPutWndLog(m_pMain,str,true);
		UpdatePrjTree();
		g_wndProgressCtrl.ShowWindow(false);
	}
	void CsswUAVFlyQuaSysEngDoc::ManageImgFile()
	{
		FunCallback4FlyQuaProcess("Managing Image File",1,"Invalid images",0,true);
		vector<CsswUAVImage> vecImgInfo = m_FlyQuaPrj.GetImgInfo();
		vector<int> vecAimIdx;
		CString strImgFolder = FunGetFileFolder(m_strPrjFilePath)+"\\ImageManagement";	
		CString strImgInvalidFolder = strImgFolder+"\\1_ImageInvalid";
		CString strImgPitchErrFolder = strImgFolder+"\\2_ImagePitchErr";
		CString strImgPosErrFolder = strImgFolder+"\\3_ImagePosErr";
		CString strImgAdjFolder = strImgFolder+"\\4_ImageAdjErr";
		CString strImgPassFolder = strImgFolder+"\\5_ImagePassCheck";	
		while(PathFileExists(strImgFolder)) FunRemoveDirAndFile(strImgFolder);
		while(!PathFileExists(strImgFolder)) FunCreateDir4Path(strImgFolder);
		while(!PathFileExists(strImgInvalidFolder)) FunCreateDir4Path(strImgInvalidFolder);
		while(!PathFileExists(strImgPitchErrFolder)) FunCreateDir4Path(strImgPitchErrFolder);
		while(!PathFileExists(strImgPosErrFolder)) FunCreateDir4Path(strImgPosErrFolder);
		while(!PathFileExists(strImgAdjFolder)) FunCreateDir4Path(strImgAdjFolder);
		while(!PathFileExists(strImgPassFolder)) FunCreateDir4Path(strImgPassFolder);

		FunCallback4FlyQuaProcess("Managing Image File",2,"Invalid images",0,false);
		/*��ЧӰ��*/
		CString strImgInvalidList = strImgInvalidFolder+"\\1_InvalidList.txt";	
		FunCreateDir4Path(FunGetFileFolder(strImgInvalidList));
		FILE *pfW = fopen(strImgInvalidList,"w");
		for (int i = 0; i<vecImgInfo.size(); i++)
		{
			if(!vecImgInfo[i].m_bValid)
			{
				CString strOldPath = vecImgInfo[i].m_strImgPath;
				if(!PathFileExists(strOldPath)) continue;
				CString strNewPath = strImgInvalidFolder+"\\"+FunGetFileName(strOldPath,true);
				CopyFile(strOldPath,strNewPath,TRUE);
				fprintf(pfW,"%s\n",strNewPath);
			}
		}
		fclose(pfW);

		FunCallback4FlyQuaProcess("Managing Image File",25,"Error pitch images",0,false);
		/*��ǹ���Ӱ��*/

		CString strImgPitchErrList = strImgPitchErrFolder+"\\2_PitchErrList.txt";
		FunCreateDir4Path(FunGetFileFolder(strImgPitchErrList));
		pfW = fopen(strImgPitchErrList,"w");
		vecAimIdx = m_FlyQuaPrj.GetImgIdxHeiPitch();
		for (int i = 0; i<vecAimIdx.size(); i++)
		{
			FunCallback4FlyQuaProcess("Managing Image File",25+i*25/vecAimIdx.size(),"Pitch error images",0,false);
			CString strOldPath = vecImgInfo[vecAimIdx[i]].m_strImgPath;
			if(!PathFileExists(strOldPath)) continue;
			CString strNewPath = strImgPitchErrFolder+"\\"+FunGetFileName(strOldPath,true);
			CopyFile(strOldPath,strNewPath,FALSE);
			fprintf(pfW,"%s\n",strNewPath);
		}
		fclose(pfW);
		vector<int>().swap(vecAimIdx);

		FunCallback4FlyQuaProcess("Managing Image File",50,"POS error images",0,false);
		/*Pos�����쳣Ӱ��*/
	
		CString strImgPosErrList = strImgPosErrFolder+"\\3_PosErrList.txt";	
				FunCreateDir4Path(FunGetFileFolder(strImgPosErrList));
		pfW = fopen(strImgPosErrList,"w");
		vecAimIdx = m_FlyQuaPrj.GetImgIdxHeiPosErr();
		for (int i = 0; i<vecAimIdx.size(); i++)
		{
			FunCallback4FlyQuaProcess("Managing Image File",50+i*25/vecAimIdx.size(),"POS error images",0,false);
			CString strOldPath = vecImgInfo[vecAimIdx[i]].m_strImgPath;
			if(!PathFileExists(strOldPath)) continue;
			CString strNewPath = strImgPosErrFolder+"\\"+FunGetFileName(strOldPath,true);
			CopyFile(strOldPath,strNewPath,FALSE);
			fprintf(pfW,"%s\n",strNewPath);
		}
		fclose(pfW);
		vector<int>().swap(vecAimIdx);

		FunCallback4FlyQuaProcess("Managing Image File",75,"Adjustment error images",0,false);
		/*��������Ӱ��*/
		CString strImgAdjErrList = strImgAdjFolder+"\\4_AdjErrList.txt";
		FunCreateDir4Path(FunGetFileFolder(strImgAdjErrList));
		pfW = fopen(strImgAdjErrList,"w");
		vecAimIdx = m_FlyQuaPrj.GetImgIdxLowAdjPoint();
		for (int i = 0; i<vecAimIdx.size(); i++)
		{
			FunCallback4FlyQuaProcess("Managing Image File",75+i*10/vecAimIdx.size(),"Adjustment error images",0,false);
			CString strOldPath = vecImgInfo[vecAimIdx[i]].m_strImgPath;
			if(!PathFileExists(strOldPath)) continue;
			CString strNewPath = strImgAdjFolder+"\\"+FunGetFileName(strOldPath,true);
			CopyFile(strOldPath,strNewPath,FALSE);
			fprintf(pfW,"%s\n",strNewPath);
		}
		fclose(pfW);
		vector<int>().swap(vecAimIdx);

		FunCallback4FlyQuaProcess("Managing Image File",85,"Quality images",0,false);
		vecAimIdx = m_FlyQuaPrj.GetImgIdxPassCheck();
		/*����Ӱ��*/
		CString strImgPassCheckList = strImgPassFolder+"\\5_PassCheckList.txt";
				FunCreateDir4Path(FunGetFileFolder(strImgPassCheckList));
		pfW = fopen(strImgPassCheckList,"w");
		for (int i = 0; i<vecAimIdx.size(); i++)
		{
			FunCallback4FlyQuaProcess("Managing Image File",85+i*15/vecAimIdx.size(),"Quality images",0,false);
			CString strOldPath = vecImgInfo[vecAimIdx[i]].m_strImgPath;
			if(!PathFileExists(strOldPath)) continue;
			CString strNewPath = strImgPassFolder+"\\"+FunGetFileName(strOldPath,true);
			CopyFile(strOldPath,strNewPath,FALSE);
			fprintf(pfW,"%s\n",strNewPath);
		}
		fclose(pfW);
		vector<int>().swap(vecAimIdx);
		if(vecImgInfo.size()>0) vector<CsswUAVImage>().swap(vecImgInfo);
		FunCallback4FlyQuaProcess("Managing Image File",100,"Completed",0,false);
		m_pMain->UpdateStatusLabel("Ready");
	}


	void CsswUAVFlyQuaSysEngDoc::ClearData()
	{
		vector<CsswUAVImage>().swap(m_vecImgInfo);
		vector<CString>().swap(m_vecInvalidImgPath);
		//		m_bPrjLoaded = false;
		m_bTransXYInView = false;
		m_nImgNum = 0;
		m_bCmrFile = true;
		m_fOverlapRatio4 = 0;
		//	m_dMeanHei = 0;
		m_FlyQuaPrj.ClearPrjData();
		m_pView->DeleteCurLineInfo();
	}
	void CsswUAVFlyQuaSysEngDoc::UnionParaFormat()
	{
#pragma omp parallel for
		for (int i = 0; i<m_vecImgInfo.size(); i++)
		{
			if(m_vecImgInfo[i].m_ImgPosInfo.eFormatCoordinate==LBH)
			{
				UTMCoor xy;
				int zone = (m_vecImgInfo[i].m_ImgPosInfo.Xs + 180.0) / 6 + 1;
				if(m_vecImgInfo[i].m_ImgPosInfo.eFormatAngleLBH==DEG) 
					LatLonToUTMXY(DegToRad(m_vecImgInfo[i].m_ImgPosInfo.Ys), DegToRad(m_vecImgInfo[i].m_ImgPosInfo.Xs), zone, xy);
				else
					LatLonToUTMXY(m_vecImgInfo[i].m_ImgPosInfo.Ys, m_vecImgInfo[i].m_ImgPosInfo.Xs, zone, xy);
				m_vecImgInfo[i].m_ImgPosInfo.Xs = xy.x;
				m_vecImgInfo[i].m_ImgPosInfo.Ys = xy.y;
			}
			if(m_vecImgInfo[i].m_ImgPosInfo.eFormatAnglePOK == RAD)
			{
				m_vecImgInfo[i].m_ImgPosInfo.phi = RadToDeg(m_vecImgInfo[i].m_ImgPosInfo.phi);
				m_vecImgInfo[i].m_ImgPosInfo.omg = RadToDeg(m_vecImgInfo[i].m_ImgPosInfo.omg);
				m_vecImgInfo[i].m_ImgPosInfo.kap = RadToDeg(m_vecImgInfo[i].m_ImgPosInfo.kap);
			}
			// 			if (m_vecImgInfo[i].m_ImgCmrInfo.eUnitx0y0 != PIX&&m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize != PIX)  //x0,y0ת��Ϊ���ص�λ
			// 			{
			// 				m_vecImgInfo[i].m_ImgCmrInfo.x0= m_vecImgInfo[i].m_ImgCmrInfo.x0*m_vecImgInfo[i].m_ImgCmrInfo.eUnitx0y0/m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize/m_vecImgInfo[i].m_ImgCmrInfo.pixsize;
			// 				m_vecImgInfo[i].m_ImgCmrInfo.y0= m_vecImgInfo[i].m_ImgCmrInfo.y0*m_vecImgInfo[i].m_ImgCmrInfo.eUnitx0y0/m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize/m_vecImgInfo[i].m_ImgCmrInfo.pixsize;
			// 			}
			if (m_vecImgInfo[i].m_ImgCmrInfo.eUnitf != PIX&&m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize != PIX) //f,fx,fyת��Ϊ���ص�λ
			{
				m_vecImgInfo[i].m_ImgCmrInfo.f = m_vecImgInfo[i].m_ImgCmrInfo.f*m_vecImgInfo[i].m_ImgCmrInfo.eUnitf / m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize / m_vecImgInfo[i].m_ImgCmrInfo.pixsize;
				m_vecImgInfo[i].m_ImgCmrInfo.fx = m_vecImgInfo[i].m_ImgCmrInfo.fx*m_vecImgInfo[i].m_ImgCmrInfo.eUnitf / m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize / m_vecImgInfo[i].m_ImgCmrInfo.pixsize;
				m_vecImgInfo[i].m_ImgCmrInfo.fy = m_vecImgInfo[i].m_ImgCmrInfo.fy*m_vecImgInfo[i].m_ImgCmrInfo.eUnitf / m_vecImgInfo[i].m_ImgCmrInfo.eUnitpixsize / m_vecImgInfo[i].m_ImgCmrInfo.pixsize;
			}
			m_vecImgInfo[i].m_ImgCmrInfo.eUnitx0y0=PIX;
			m_vecImgInfo[i].m_ImgCmrInfo.eUnitf=PIX;
			m_vecImgInfo[i].m_ImgPosInfo.eFormatCoordinate=XYZ;
			m_vecImgInfo[i].m_ImgPosInfo.eFormatAnglePOK=DEG;
			m_vecImgInfo[i].m_ImgPosInfo.eFormatAngleLBH=DEG;
		}
	}

	bool CsswUAVFlyQuaSysEngDoc::PrjNew(CString strPath, vector<CsswUAVImage> &vecImgInfo, SSWstuSysConfigPara para, bool bWithCmr)
	{
		ClearData();
		m_strPrjFilePath=strPath;
		m_vecImgInfo=vecImgInfo;
		UnionParaFormat();
		if(!m_FlyQuaPrj.LoadData2Prj(m_vecImgInfo,m_strPrjFilePath,para,bWithCmr))
		{
			CString strInfo; strInfo.Format("Project creating failed, log file %s",FunGetSyslogPath());
			MessageBoxEx(NULL,strInfo,"Error",MB_OK, MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US));
			m_pMain->UpdateStatusLabel("Project creating failed!");
			return false;
		}

		m_FlyQuaPrj.CheckImgValidState();
		if(!m_FlyQuaPrj.SavePrj2File(m_strPrjFilePath))
		{
			CString strInfo; strInfo.Format("Project creating failed, log file %s",FunGetSyslogPath());
			MessageBoxEx(NULL,strInfo,"Error",MB_OK, MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US));
			m_pMain->UpdateStatusLabel("Project creating failed!");
			return false;
		}
		m_vecImgInfo = m_FlyQuaPrj.GetImgInfo();
		m_nImgNum = m_vecImgInfo.size();
		m_pMain->ShowRibbonValue();
		m_pMain->m_wndDataChart.OnComboDatachartExtend();
		m_pMain->m_wndImgAndPt.DeleteAllRender();
		if(!m_bPrjLoaded) m_pMain->CreatePaneWnd();
		m_bPrjLoaded=true;
		m_pView->DeleteViewVBO();
		m_pView->m_PosRender.CreateVectorVBO(m_vecImgInfo.size(),vPoint,true);
		InitViewRect();
		FillPosRender();
		UpdatePrjTree();
		FunOutPutWndLog(m_pMain,("Project creating successful=" +m_strPrjFilePath),true);
		return true;
	}

	void CsswUAVFlyQuaSysEngDoc::OnButtonPrjNew()
	{
		if(m_pView==NULL) m_pView=GetViewHand();
		if(m_pMain==NULL) m_pMain=GetMainFramHand();
		if(m_pView!=NULL) m_pView->m_pDoc=this;
		if(m_pMain!=NULL) m_pMain->m_pDoc = this;
		if(m_pMain->m_wndImgAndPt.m_pDoc==NULL) m_pMain->m_wndImgAndPt.m_pDoc = this;
		m_pMain->UpdateStatusLabel("Creating project...");
		SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US),SORT_DEFAULT));
		CPrjNewDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			SSWstuCmrInfo cmr;
			if(!dlg.GetCmrInfo(cmr, m_bCmrFile))
			{
				ClearData();
				MessageBoxEx(NULL,"Image focallength parsing error, project creating failed!","Error",MB_OK, MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US));
				return;
			}
			if(!PrjNew(dlg.GetPrjFilePath(),dlg.LoadImg2Prj(),dlg.GetFlyQuaPara(),m_bCmrFile))
				return;
			m_bPrjLoaded = true;
			m_pView->m_bStripResorted = false;
		}
		m_pMain->UpdateStatusLabel("Ready");

	}
	void CsswUAVFlyQuaSysEngDoc::OnButtonPrjOpen()
	{
		// TODO: �ڴ���������������
		if(m_pView==NULL) m_pView=GetViewHand();
		if(m_pMain==NULL) m_pMain=GetMainFramHand();
		if(m_pView!=NULL) m_pView->m_pDoc=this;
		if(m_pMain!=NULL) m_pMain->m_pDoc = this;
		if(m_pMain->m_wndImgAndPt.m_pDoc==NULL) m_pMain->m_wndImgAndPt.m_pDoc = this;
		m_pMain->UpdateStatusLabel("Setting project file...");
		SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US),SORT_DEFAULT));
		CFolderPickerDialog dlg1;
		CString FilePathName;
		CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
			NULL, 
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			(LPCTSTR)_TEXT("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||"),
			NULL);
		if(dlg.DoModal()==IDOK)
		{
			FilePathName = dlg.GetPathName(); //�ļ�����������FilePathName��
			if(FilePathName==m_strPrjFilePath) return;	
			ClearData();
			if(!m_FlyQuaPrj.LoadPrj4File(FilePathName))
			{
				CString strInfo; strInfo.Format("Project open failed, log file %s",FunGetSyslogPath());
				MessageBoxEx(NULL,strInfo,"Error",MB_OK, MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US));
				m_pMain->UpdateStatusLabel("Project file open failed!");
				return;
			}
			SSWstuSysConfigPara para_cfg;
			SSWstuSysConfigData data_cfg;
			if(GetConfigInfo(data_cfg,para_cfg))
			{
				if(!(para_cfg==m_FlyQuaPrj.GetPara()))
				{
					CString strInfo; strInfo.Format("Software configuration is different from the project file, whether or not to update the software?");
					if(MessageBoxEx(NULL,strInfo,"Message",MB_YESNO,MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US))==IDYES )
					{
						FunSaveSysConfigFile(data_cfg,m_FlyQuaPrj.GetPara());
					}
				}
			}
			if(!m_bPrjLoaded) m_pMain->CreatePaneWnd();
			m_bPrjLoaded=true;
			m_strPrjFilePath = FilePathName;
// 			if(m_FlyQuaPrj.GetInvalidImgNum()==0)
// 			{
// 				m_FlyQuaPrj.CheckImgValidState();
// 				m_FlyQuaPrj.SavePrj2File(m_strPrjFilePath);
// 			}
			m_vecImgInfo=m_FlyQuaPrj.GetImgInfo();
			m_nImgNum = m_vecImgInfo.size();
			m_pMain->ShowRibbonValue();

			if(m_FlyQuaPrj.GetStripNum()>0)
			{
				m_pView->m_bStripResorted=true;
			}else
			{
				m_pView->m_bStripResorted = false;
			}
			m_pView->DeleteCurLineInfo();
			m_pView->DeleteViewVBO();
			m_pView->m_PosRender.CreateVectorVBO(m_vecImgInfo.size(),vPoint,true);
			m_pMain->m_wndDataChart.OnComboDatachartExtend();
			m_pMain->m_wndImgAndPt.DeleteAllRender();
			//	m_pView->m_PosRectRender.CreateVectorVBO(5,vLine,true);
			InitViewRect();
			FillPosRender();

			UpdatePrjTree();
			m_pView->SaveView2Img(m_pView->m_strCqViewImagePath);
			FunOutPutWndLog(m_pMain,("Project open successful=" +m_strPrjFilePath),true);
		}
		m_pMain->UpdateStatusLabel("Ready");
//		m_FlyQuaPrj.SavePrj2File4DYDll(m_strPrjFilePath+"DYTest.xml");
	}
	void CsswUAVFlyQuaSysEngDoc::OnFileNew()
	{
		// TODO: �ڴ���������������
		OnButtonPrjNew();
	}
	void CsswUAVFlyQuaSysEngDoc::OnFileOpen()
	{
		// TODO: �ڴ���������������
		OnButtonPrjOpen();
	}
	void CsswUAVFlyQuaSysEngDoc::OnFileSave()
	{
		// TODO: �ڴ���������������
		OnButtonPrjSave();
	}

	void CsswUAVFlyQuaSysEngDoc::OnButtonRunFlyquaCheck()
	{
		// TODO: �ڴ���������������
		m_pView->m_bActStripResort = false;
		CString strMethod;
		if(m_eFlyQuaChkMethod==BY_POS) strMethod="By POS";
		if(m_eFlyQuaChkMethod==BY_MATCH) strMethod="By Matching";
		if(m_eFlyQuaChkMethod==BY_ADJUSTMENT) strMethod="By Adjustment";
		CString str; str.Format("Run flight quality control, method: %s",strMethod);
		FunOutPutWndLog(m_pMain,str,true);
		DeleteFile(m_pView->m_strCqViewImagePath);
		if(m_eFlyQuaChkMethod==BY_MATCH&&PathFileExists(m_FlyQuaPrj.GetMatchPointPath()))
		{
			switch(MessageBoxEx(NULL,"Matching result file is exist, whether or not to rebuild it?","Message",MB_YESNOCANCEL ,MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US)))
			{
			case IDYES:
				DeleteFile(m_FlyQuaPrj.GetAdjRepPath());
				break;
			case IDNO:
				break;
			default:
				return;
			}
		}else
			if(m_eFlyQuaChkMethod==BY_ADJUSTMENT&&PathFileExists(m_FlyQuaPrj.GetAdjRepPath()))
			{
				switch(MessageBoxEx(NULL,"Adjustment result file is exist, whether or not to rebuild it?","Message",MB_YESNOCANCEL,MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US)))
				{
				case IDYES:
					DeleteFile(m_FlyQuaPrj.GetAdjRepPath());
					break;
				case IDNO:
					break;
				default:
					return;
				}
				// 				if(MessageBox(NULL,"��⵽�ʼ칤�����Ѵ���ƽ�����ļ����Ƿ��������ɣ�","��ʾ",MB_YESNOCANCEL)==IDYES)
				// 				{
				// 					DeleteFile(m_FlyQuaPrj.GetAdjRepPath());
				// 				}
				// 				else if(MessageBox(NULL,"��⵽�ʼ칤�����Ѵ���ƽ�����ļ����Ƿ��������ɣ�","��ʾ",MB_YESNOCANCEL)==IDCANCEL)
				// 					return;
			}
			m_bInRunning = true;
			HANDLE hThread;DWORD IdThread;
			hThread = CreateThread(NULL, 0,
				(LPTHREAD_START_ROUTINE)MultiThreadFlyQuaChkFun,
				(LPVOID*)this, 0, &IdThread);
// 			CString strImgFolder = FunGetFileFolder(m_strPrjFilePath)+"\\ImageManagement";
// 				FunRemoveDirAndFile(strImgFolder);
// 			while(PathFileExists(strImgFolder)) Sleep(100);
			
	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonRunFlyquaCheck(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(m_bPrjLoaded);
		else if(m_pView->m_bStripResorted) pCmdUI->Enable(!m_bInRunning);
		else pCmdUI->Enable(m_pView->m_bStripResorted);
	}
	void CsswUAVFlyQuaSysEngDoc::OnButtonFlyquaReport()
	{
		// TODO: �ڴ���������������	
		m_strFlyQuaRepPath = m_FlyQuaPrj.GetCheckRepPath(m_eFlyQuaChkMethod);
		CString strOpenBat = FunGetFileFolder(m_strFlyQuaRepPath)+"open.bat";
		FILE *pfW = fopen(strOpenBat,"w"); 
		fprintf(pfW,"start %s\ndel %0",m_strFlyQuaRepPath);
		fclose(pfW); Sleep(10);//strOpenBat = "start "+m_strFlyQuaRepPath;
		ShellExecute(NULL, NULL, strOpenBat, NULL, NULL, SW_HIDE);

//		system(m_strFlyQuaRepPath);
// 		if(!PathFileExists(m_strFlyQuaRepPath))
// 		{
// 			CString strInfo;
// 			if(m_eFlyQuaChkMethod==BY_ADJUSTMENT) strInfo = "����ƽ����ʼ챨�治���ڣ�������ִ���ʼ������";else
// 				if(m_eFlyQuaChkMethod==BY_MATCH) strInfo = "����ƥ����ʼ챨�治���ڣ�������ִ���ʼ������";else
// 					if(m_eFlyQuaChkMethod==BY_POS) strInfo = "����POS���ʼ챨�治���ڣ�������ִ���ʼ������";
// 			MessageBox(strInfo);
// 		}
// 		else
// 		{
// 			m_pMain->SendMessage(ID_MSG_OPEN_REPORT);
//		}

	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonFlyquaReport(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(m_bPrjLoaded);
		else pCmdUI->Enable(!m_bInRunning);
	}
// 	void CsswUAVFlyQuaSysEngDoc::OnComboCheckMethod()
// 	{
// 		// TODO: �ڴ���������������
// 		CMFCRibbonComboBox *pComb = NULL;
// 		pComb = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,m_pMain->m_wndRibbonBar.FindByID(ID_COMBO_CHECK_METHOD));
// 		m_eFlyQuaChkMethod = SSWFLYQUAMETHOD(pComb->GetCurSel());
// 	}
// 	void CsswUAVFlyQuaSysEngDoc::OnUpdateComboCheckMethod(CCmdUI *pCmdUI)
// 	{
// 		// TODO: �ڴ������������û����洦��������
// 		if(!m_bPrjLoaded) pCmdUI->Enable(FALSE);
// 		else pCmdUI->Enable(!m_bInRunning);
// 	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjNew(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(!m_bPrjLoaded);
		else pCmdUI->Enable(!m_bInRunning);
	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjOpen(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(!m_bPrjLoaded);
		else pCmdUI->Enable(!m_bInRunning);
	}
	void CsswUAVFlyQuaSysEngDoc::OnButtonPrjSave()
	{
		// TODO: �ڴ���������������
		if(m_bPrjLoaded) m_FlyQuaPrj.SavePrj2File(m_strPrjFilePath);
	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonPrjSave(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(m_bPrjLoaded);
		else pCmdUI->Enable(!m_bInRunning);
	}
	void CsswUAVFlyQuaSysEngDoc::OnEditMeanHeight()
	{
		// TODO: �ڴ���������������
		CMFCRibbonEdit* pEdit = NULL; CString str;
		pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit,m_pMain->m_wndRibbonBar.FindByID(ID_EDIT_MEAN_HEIGHT));
		m_dMeanHei=atof(pEdit->GetEditText());
	}
	void CsswUAVFlyQuaSysEngDoc::OnUpdateEditMeanHeight(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		if(!m_bPrjLoaded) pCmdUI->Enable(FALSE);
		else pCmdUI->Enable(!m_bInRunning);
	}




	void CsswUAVFlyQuaSysEngDoc::OnButtonDataOrder()
	{
		// TODO: �ڴ���������������

	}


	void CsswUAVFlyQuaSysEngDoc::OnUpdateButtonDataOrder(CCmdUI *pCmdUI)
	{
		// TODO: �ڴ������������û����洦��������
		pCmdUI->Enable(FALSE);
		// 		if(!m_bPrjLoaded) pCmdUI->Enable(false);else
		// 		if(m_bInRunning) pCmdUI->Enable(false);else
		// 			if(m_pView->m_vecCqExtRingRender.size()==0) pCmdUI->Enable(false);
		// 			else pCmdUI->Enable(true);
	}


// 	void CsswUAVFlyQuaSysEngDoc::OnEditMatchpointNumThresold()
// 	{
// 		// TODO: Add your command handler code here
// 		CMFCRibbonEdit* pEdit = NULL; CString str;
// 		pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit,m_pMain->m_wndRibbonBar.FindByID(ID_EDIT_MATCHPOINT_NUM_THRESOLD));
// 		m_nMatchPointNumThresold=atoi(pEdit->GetEditText());
// 	}
// 
// 
// 	void CsswUAVFlyQuaSysEngDoc::OnUpdateEditMatchpointNumThresold(CCmdUI *pCmdUI)
// 	{
// 		// TODO: Add your command update UI handler code here
// 		if(!m_bPrjLoaded) pCmdUI->Enable(FALSE);
// 		else if(m_bInRunning) pCmdUI->Enable(FALSE);
// 		else if(m_eFlyQuaChkMethod==BY_POS) pCmdUI->Enable(FALSE);
// 		else pCmdUI->Enable(TRUE);
// 	}
