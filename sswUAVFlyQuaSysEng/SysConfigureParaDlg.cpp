// SysConfigParaDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sswUAVFlyQuaSysEng.h"
#include "SysConfigureParaDlg.h"
#include "afxdialogex.h"


// CSysConfigureParaDlg �Ի���

IMPLEMENT_DYNAMIC(CSysConfigureParaDlg, CDialogEx)

CSysConfigureParaDlg::CSysConfigureParaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSysConfigureParaDlg::IDD, pParent)

{

	m_fMinOverlapY = 60.0f;
	m_fMinOverlapX = 30.0f;
	m_fMaxHeiDifInArea = 50.0f;
	m_fMaxHeiDifLinkInStrip = 30.0f;
	m_fMaxStripBlend = 3.0f;
	m_fMaxPitch = 15.0f;
	m_fMaxPitch10Per = 10.0f;
	m_fMaxYaw = 5.0f;
	m_fMaxYaw15Per = 10.0f;
	m_nMaxYaw20Num = 3;
	m_fImgValidJudgeTimesOfRms = 3;
	m_nMatchPtNumThresold = 1;
	m_fMaxErrOfStationPane = 15.0f;
	m_fRmsErrOfStationPane = 10.0f;

	m_fMaxAngleErrRatio = 5.0f;
	m_fMaxMissAreaRatio = 2.0f;
}

CSysConfigureParaDlg::~CSysConfigureParaDlg()
{
}

void CSysConfigureParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MIN_OVERLAP_Y, m_fMinOverlapY);
	DDV_MinMaxFloat(pDX, m_fMinOverlapY, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MIN_OVERLAP_X, m_fMinOverlapX);
	DDV_MinMaxFloat(pDX, m_fMinOverlapX, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MAX_HEIDIF_AREA, m_fMaxHeiDifInArea);
	DDX_Text(pDX, IDC_EDIT_MAX_HEIDIF_STRIPLINK, m_fMaxHeiDifLinkInStrip);
	DDX_Text(pDX, IDC_EDIT_MAX_STRIPBLEND, m_fMaxStripBlend);
	DDX_Text(pDX, IDC_EDIT_MAX_PITCH, m_fMaxPitch);
	DDV_MinMaxFloat(pDX, m_fMaxPitch, 0, 360);
	DDX_Text(pDX, IDC_EDIT_MAX_PITCH10_PER, m_fMaxPitch10Per);
	DDV_MinMaxFloat(pDX, m_fMaxPitch10Per, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MAX_YAW, m_fMaxYaw);
	DDV_MinMaxFloat(pDX, m_fMaxYaw, 0, 360);
	DDX_Text(pDX, IDC_EDIT_MAX_YAW15_PER, m_fMaxYaw15Per);
	DDV_MinMaxFloat(pDX, m_fMaxYaw15Per, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MAX_YAW20_NUM, m_nMaxYaw20Num);
	DDX_Text(pDX, IDC_EDIT_VALIDJUDGE_THRESOLD, m_fImgValidJudgeTimesOfRms);
	DDV_MinMaxFloat(pDX, m_fImgValidJudgeTimesOfRms, 0, 10);
	DDX_Text(pDX, IDC_EDIT_MATCH_PTNUM_THRESOLD, m_nMatchPtNumThresold);
	DDX_Text(pDX, IDC_EDIT_MAXERR_PANE, m_fMaxErrOfStationPane);
	DDX_Text(pDX, IDC_EDIT_RMSERR_PANE, m_fRmsErrOfStationPane);
	DDX_Text(pDX, IDC_EDIT_ANGLEERR_RATIO, m_fMaxAngleErrRatio);
	DDV_MinMaxFloat(pDX, m_fMaxAngleErrRatio, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MIASSAREA_RATIO, m_fMaxMissAreaRatio);
		DDV_MinMaxFloat(pDX, m_fMaxMissAreaRatio, 0, 100);
}


BEGIN_MESSAGE_MAP(CSysConfigureParaDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSysConfigureParaDlg ��Ϣ�������
void CSysConfigureParaDlg::InitItemPos()
{
	CRect rect;
	GetClientRect(&rect);     //ȡ�ͻ�����С    
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	cy = rt.bottom;
	MoveWindow(0, 0, cx, cy);
}

void CSysConfigureParaDlg::ResetItemPos()
{
	float fsp[2];
	POINT Newp; //��ȡ���ڶԻ���Ĵ�С  
	CRect recta;
	GetClientRect(&recta);     //ȡ�ͻ�����С    
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / old.x;
	fsp[1] = (float)Newp.y / old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //���Ͻ�  
	CPoint OldBRPoint, BRPoint; //���½�  
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //�г����пؼ�    
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//ȡ��ID  
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	old = Newp;
}

void CSysConfigureParaDlg::UpdateItems()
{
	m_fMinOverlapY=m_SysCfgPara.fMinOverlapY;
	m_fMinOverlapX = m_SysCfgPara.fMinOverlapX;
	m_fMaxHeiDifInArea = m_SysCfgPara.fMaxHeiDifInArea;
	m_fMaxHeiDifLinkInStrip = m_SysCfgPara.fMaxHeiDifLinkInStrip;
	m_fMaxStripBlend = m_SysCfgPara.fMaxStripBlend;
	m_fMaxPitch = m_SysCfgPara.fMaxPitch;
	m_fMaxPitch10Per = m_SysCfgPara.fMaxPitch10Per;
	m_fMaxYaw = m_SysCfgPara.fMaxYaw;
	m_fMaxYaw15Per = m_SysCfgPara.fMaxYaw15Per;
	m_nMaxYaw20Num = m_SysCfgPara.nMaxYaw20Num;
	m_fImgValidJudgeTimesOfRms = m_SysCfgPara.fValidJudgeTimesOfRms;
	m_nMatchPtNumThresold = m_SysCfgPara.nMatchPtNumThresold;
	m_fMaxErrOfStationPane = m_SysCfgPara.fMaxErrStationPane;
	m_fRmsErrOfStationPane = m_SysCfgPara.fRmsErrStationPane;
	m_fMaxMissAreaRatio = m_SysCfgPara.fMissAreaRatio;
	m_fMaxAngleErrRatio = m_SysCfgPara.fAngleErrRatio;
	UpdateData(FALSE);
}
void CSysConfigureParaDlg::UpdateParas()
{
	UpdateData(TRUE);
	m_SysCfgPara.fMinOverlapY=m_fMinOverlapY;
	m_SysCfgPara.fMinOverlapX=m_fMinOverlapX;
	m_SysCfgPara.fMaxHeiDifInArea=m_fMaxHeiDifInArea;
	m_SysCfgPara.fMaxHeiDifLinkInStrip=m_fMaxHeiDifLinkInStrip;
	m_SysCfgPara.fMaxStripBlend=m_fMaxStripBlend;
	m_SysCfgPara.fMaxPitch=m_fMaxPitch;
	m_SysCfgPara.fMaxPitch10Per=m_fMaxPitch10Per;
	m_SysCfgPara.fMaxYaw=m_fMaxYaw;
	m_SysCfgPara.fMaxYaw15Per=m_fMaxYaw15Per;
	m_SysCfgPara.nMaxYaw20Num=m_nMaxYaw20Num;
	m_SysCfgPara.fValidJudgeTimesOfRms=m_fImgValidJudgeTimesOfRms;
	m_SysCfgPara.nMatchPtNumThresold=m_nMatchPtNumThresold;
	m_SysCfgPara.fMaxErrStationPane=m_fMaxErrOfStationPane;
	m_SysCfgPara.fRmsErrStationPane=m_fRmsErrOfStationPane;
	m_SysCfgPara.fMissAreaRatio = m_fMaxMissAreaRatio;
	m_SysCfgPara.fAngleErrRatio=m_fMaxAngleErrRatio;
}
BOOL CSysConfigureParaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
//	InitItemPos();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSysConfigureParaDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	ResetItemPos();
	// TODO: �ڴ˴������Ϣ����������
}
