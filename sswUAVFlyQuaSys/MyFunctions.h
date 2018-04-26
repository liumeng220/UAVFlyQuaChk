#pragma once
#include "sswUAVFlyQua/sswUAVFlyQua.h"
#include "sswPdfLib.h"
class CMainFrame;
class CsswUAVFlyQuaSysDoc;
class CsswUAVFlyQuaSysView;

CMainFrame* GetMainFramHand();
CsswUAVFlyQuaSysDoc* GetDocHand();
CsswUAVFlyQuaSysView* GetViewHand();

//��ȡ��������ļ�
bool FunReadCmrFile(CString strCmrFilePath,SSWstuCmrFileExtend& sswCmrFileExtend, vector<SSWstuCmrInfo>& vecCmrInfo);
//��ȡPOS�����ļ�
bool FunReadPosFile(CString strPosFilePath,SSWstuPosFileExtend& sswPosFileExtend, vector<SSWstuPosInfo>& vecPosIngo);
//����ϵͳ�����ļ�
bool FunSaveSysConfigFile(SSWstuSysConfigData& sysCfgData, SSWstuSysConfigPara& sysCfgPara);
//����ϵͳ�����ļ�
bool FunReadSysConfigFile(SSWstuSysConfigData& sysCfgData, SSWstuSysConfigPara& sysCfgPara);
//���������־
void FunOutPutWndLog(CMainFrame* pMain, CString strLog, bool bAdd);
//ɾ��Ŀ¼���ļ�
bool FunRemoveDirAndFile(CString strFolderPath);

void FunVectorPt2VertexXY(void *pVector, int nDim, double *&pV);
void FunCreateVertexColor(int nPtNum, double r,double g,double b, double *&pC);


//���ֻص�����ʵ��
void FunCallback4FlyQuaProcess(const char *pInfoMain, float dPercentMain, const char *pInfoSub, float dPercentSub, bool bAdd2LogWnd = false);
void FunCallback4FlyQuaProcessBundle(const char *pInfoMain, float dPercentMain, const char *pInfoSub, float dPercentSub);



void FunCallBackTest(FlyQuaCallBackFun pFun);

//ͨ�������ļ���������
bool FunCreatePrj4ParaFile(CsswUAVFlyQuaPrj &FlyQuaPrj, CString strParaFilePath);