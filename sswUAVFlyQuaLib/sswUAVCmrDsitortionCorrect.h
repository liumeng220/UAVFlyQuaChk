#pragma once
#include "sswUAVFlyQua.h"
#define MAX_ADD_DISTORTION_ITER 10
static enum eDistortionReturn
{
	SSWSuccess=0,
	SSWPathErr=1,
	SSWOpenErr=2,
	SSWReadErr=3,
	SSWCmrErr=4,
	SSWPosErr=5,
	SSWDataErr=6,
	SSWSaveErr=7
};
static CString ReturnDat2Info(eDistortionReturn eReturn)
{
	switch(eReturn)
	{
	case SSWSuccess: return "����ɹ���";
	case SSWPathErr: return "Ӱ��·������";
	case SSWOpenErr: return "Ӱ���ʧ�ܣ�";
	case SSWReadErr: return "Ӱ���ȡʧ�ܣ�";
	case SSWCmrErr:  return "�����������";
	case SSWPosErr:  return "POS��������";
	case SSWDataErr: return "������������";
	case SSWSaveErr: return "�ļ�����ʧ�ܣ�";
	}
}
class CsswUAVCmrDsitortionCorrect :
	public CsswUAVImage
{
public:
	CsswUAVCmrDsitortionCorrect();
	~CsswUAVCmrDsitortionCorrect();

	eDistortionReturn LoadImageInfo(CString strImgPath, SSWstuCmrInfo& sswCmrInfo, CString strSavePath, float fScale = 1.0);
	eDistortionReturn RunDistortionCorrect(SSWstuCmrInfo& cmrNew, bool bChkExist = false);
	string GetResImgPath(){ return m_strSavePath;}
protected:
	void Remove_Distortion_Level3_2(double xori,double yori, double &xdst, double &ydst);
	void Add_Distortion_Level3_2(double xori,double yori, double &xdst, double &ydst);
protected:
	bool m_bDataReady;
	CString m_strImgPath;
	CString m_strSavePath;
	float m_fScale;
	SSWstuCmrInfo m_CmrInfo;
};
