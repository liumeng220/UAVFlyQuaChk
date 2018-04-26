/************************************************************************/
/* Ŀ��: ���˻�����Ӱ���ʼ�����㷨��                                   */
/* ʱ��: 2016-12-08                                                     */
/* ��λ: �人���ƿƼ����޹�˾                                           */
/************************************************************************/
#pragma once

#ifdef  DYUAVFLYQUACHKLIB_EXPORTS
#define DYUAVFLY __declspec(dllexport)
#else
#define DYUAVFLY __declspec(dllimport)
#endif 

/************************************************************************/
/* �������ݽṹ                                                         */
/************************************************************************/
//���ȵ�λ
enum DYUAVFLY DYUNITTYPE
{
	PIX = 0,
	UM  = 1,
	MM  = 1000,
	CM  = 10000,
	M   = 1000000
};
//��������
enum DYUAVFLY DYCOORDINATEFORMAT
{
	LBH = 0,
	XYZ = 1
};
//�Ƕ�����
enum DYUAVFLY DYANGLEFORMAT
{
	DEG = 0,
	RAD = 1
};
//�����POS�豸��Ϣ����
struct DYUAVFLY DYstuDeviceFileExtend
{
	int pCfeColMap[13];              //����ļ������������� cmr_label, x0, y0, f, fx, fy, pixsize, k1, k2, pa, p2, a, b
	DYUNITTYPE UnitIntrinsicParas;   //����ڲ�����λx0, y0, f, fx, fy, pixsize

	DYCOORDINATEFORMAT FormatCoor;  //POS��������
	DYANGLEFORMAT FormatAngleLBH;   //POS����Ϊ��γ��ʱ����γ�ȽǶ�����
	DYANGLEFORMAT FormatAnglePOK;   //POS��̬������
	DYstuDeviceFileExtend()
	{
		*pCfeColMap = 12;
		for (int i = 1; i<13; i++)
		{
			*(pCfeColMap+i) = i-1;
		}
		UnitIntrinsicParas = MM;
	
		FormatCoor      = LBH;
		FormatAngleLBH  = DEG;
		FormatAnglePOK  = DEG;
	}
};
//�����ʼ������ֵ
struct DYUAVFLY DYstuSysConfigPara
{
	float fMinOverlapY;               //�����ص�����С��ֵ
	float fMinOverlapX;               //�����ص�����С��ֵ
	float fMaxHeiDifInArea;           //������󺽸߲���ֵ
	float fMaxHeiDifLinkInStrip;      //��������Ӱ����󺽸߲���ֵ
	float fMaxStripBlend;             //���������������ֵ
	float fMaxPitch;                  //����Ӱ����������ֵ
	float fMaxPitch10Per;             //����Ӱ����ǳ���10����������ֵ   -��δʹ��
	float fMaxYaw;                    //����Ӱ�����������ֵ
	float fMaxYaw15Per;				  //�������ǳ���15��Ӱ�����ռ����ֵ   -��δʹ��
	int   nMaxYaw20Num;               //�������ǳ���20��Ӱ���������       -��δʹ��
	float fValidJudgeTimesOfRms;      //����Ӱ����Ч���ж���ֵ
	int   nMatchPtNumThresold;        //��������ƥ�������С��ֵ
	float fMaxErrStationPane;         //����ƽ������վ���ƫ����ֵ
	float fRmsErrStationPane;         //����ƽֱ����վƫ������������ֵ
	float fMissAreaRatio;
	float fAngleErrRatio;
	DYstuSysConfigPara()
	{
		fMinOverlapY = 60.0f;
		fMinOverlapX = 30.0f;
		fMaxHeiDifInArea = 50.0f;
		fMaxHeiDifLinkInStrip = 30.0f;
		fMaxStripBlend = 3.0f;
		fMaxPitch = 15.0f;
		fMaxPitch10Per = 10.0f;
		fMaxYaw = 5.0f;
		fMaxYaw15Per = 10.0f;
		nMaxYaw20Num = 3;
		fValidJudgeTimesOfRms=2.0f;
		nMatchPtNumThresold = 8;
		fMaxErrStationPane = 15;
		fRmsErrStationPane = 10;
		fMissAreaRatio = 2.0f;
		fAngleErrRatio = 5.0f;
	}
	bool operator==(DYstuSysConfigPara para){
		return (fMinOverlapY == para.fMinOverlapY && 
			fMinOverlapX == para.fMinOverlapX && 
			fMaxHeiDifInArea == para.fMaxHeiDifInArea && 
			fMaxHeiDifLinkInStrip == para.fMaxHeiDifLinkInStrip && 
			fMaxStripBlend == para.fMaxStripBlend && 
			fMaxPitch == para.fMaxPitch && 
			fMaxPitch10Per == para.fMaxPitch10Per && 
			fMaxYaw == para.fMaxYaw && 
			fMaxYaw15Per == para.fMaxYaw15Per && 
			nMaxYaw20Num == para.nMaxYaw20Num &&
			nMatchPtNumThresold == para.nMatchPtNumThresold &&
			fMissAreaRatio == para.fMissAreaRatio &&
			fAngleErrRatio == para.fAngleErrRatio );}
};

/************************************************************************/
/* �ⲿ�ӿں���                                                         */
/* �ص�����ʾ����void* pCallBackFun(const char* strInfo, float fPercent)*/
/************************************************************************/
#ifndef _dll_functions
#define _dll_functions
//�޸�������Ϣ-��Ĭ��ֵ���ɰ�������
/*******����1���ʼ������ֵ*/
/*******����2���ص�����ָ��*/
void DYUAVFLY DYUpdateCfgFile(DYstuSysConfigPara *cfgPara = NULL, void *pCallBackFun = NULL);
//ִ���ʼ����
/*******����1���ʼ�������Ϣ�ļ�·��*/
/*******����2���ʼ챨�汣��·��*/
/*******����3���ص�����ָ��*/
/*******����ֵ��ture-�ϸ�false-���ϸ�*/
bool DYUAVFLY DYUAVFlyQuaChkPro(char* strImpFilePath, char* strReportSavePath, void *pCallBackFun = NULL);
#endif