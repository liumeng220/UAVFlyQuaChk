#ifndef ssw_uav_flyqua_h
#define ssw_uav_flyqua_h
#pragma once

#include "MyDataBaseType.h"

#ifdef  SSWUAVFLYQUA_EXPORTS
#define SSWUAVFLY __declspec(dllexport)
#else
#define SSWUAVFLY __declspec(dllimport)
#endif

enum SSWMatchMatrixMethod
{//ƥ�������㷽��
	SSW_POS_DIS = 0,
	SSW_AREA_LINK = 1,
	SSW_STRIP_IDX = 2
};




class SSWUAVFLY CsswImage
{
public:
	CsswImage();
	virtual ~CsswImage();
public:
	bool OpenImg(CString strImgPath, bool bReadOnly=true);
	void CloseImg();
	int  GetRows();
	int  GetCols();
	int  GetBandCount();
	int  GetBitSize();
	float GetFocalLength(SSWUNITTYPE eUnit=PIX);
	bool ReadImg(int nStartCol, int nStartRow, int nCols, int nRows, int nMemWidth, int nMemHeight, void*pMem);
	bool SaveImg(CString strSavePath, int nCols, int nRows, int nBands, BYTE*pMem,const char *pszFormat = "JPEG");
protected:
	CString findImageTypeGDAL(CString strDetImgName);
private:
	void* m_pDataset;
};

class SSWUAVFLY CsswUAVImage:public CsswImage
{
public:
public:
	CsswUAVImage();
	virtual ~CsswUAVImage();

public:
	void CalRotMatrixByPOK(SSWstuPosInfo& sswPosInfo);
	Point2D XYZ2Img(int nCols, int nRows, double f, Point3D &ptXYZ, bool bAdjed);
	Point3D Img2XYZ(int nCols, int nRows,double f, Point2D &ptImg, double Z, bool bAdjed);
	Point2D* GetImgVertexPt2d4(bool bThumOrNot);
	Point3D* GetImgVertexPt3d4(double* pZ, bool bAdjed, bool bThumOrNot);
	//Point3D* GetImgVertexPt3d42(double Z0,bool bAdjed, bool bThumOrNot);
protected:
	void CalImg4VertexPt3d(double* pZ, bool bAdjed, bool bThumOrNot, Point3D *&pVertex3d);
public:									        
	Point3D m_PrincipalPoint;                     //������
	int m_nImgIdx;                                //Ӱ������
	int m_nStripIdx;						        //��������
	int m_nIdxInStrip;						      						
	CString m_strImgPath;                         //Ӱ��·��
	CString m_strImgName;
	CString m_strThumbPath;                       //����ͼ·��
	CString m_strThumbName;
	CString m_strStripLabel;                  
	SSWstuCmrInfo m_ImgCmrInfo;                   //�������
	SSWstuCmrInfo m_ThumbCmrInfo;                 //����ͼ�������
	SSWstuPosInfo m_ImgPosInfo;                   //Ӱ��POS��Ϣ
	SSWstuPosInfo m_AdjPosInfo;                   //����POS
	SSWstuAdjPt4Img  m_ImgAdjPts;                 //������
	bool m_bValid;                             //�Ƿ���Ч
	bool m_bAdjed;                             //�Ƿ����ƽ��

	Point2D m_pImgVertexPt2d4[4];
	Point2D m_pThumbImgVertexPt2d4[4];
	Point3D m_pImgVertexPt3d4[4];


};

class SSWUAVFLY CsswUAVFlyQuaPrj
{
public:
	CsswUAVFlyQuaPrj();
	~CsswUAVFlyQuaPrj();

public:
	//��������
	bool LoadData2Prj(vector<CsswUAVImage> &vecImg,CString strPrjPath, SSWstuSysConfigPara para, bool bWithCmr);
	//�����ʼ칤��
	bool SavePrj2File(CString strSavePath);
	//
	bool SavePrj2File4DYDll(CString strSavePath);
	//�����ʼ칤��
	bool LoadPrj4File(CString strFilePath);
	//�����������
	void ClearPrjData();
	//�������ò���
	void SetPara2Prj(SSWstuSysConfigPara para);
	//���Ӱ����Ч�ԣ����й��̵������������Ӱ��Ϊ��Ч
	void CheckImgValidState();
	//ɾ����ЧӰ��
	void DeleteInvalidImg();
	//����Ӱ����Ч��
	void SetImgValidState(vector<vector<Point2D>>vvAreaVertex2Select, bool bIsToInValid = true, FlyQuaCallBackFun pFun = NULL);
	//���ź���
	bool ReOrderStrips(double dLine_a, double dLine_b, double dLine_c,FlyQuaCallBackFun pFun = NULL);
	bool ReOrderStripByHand(vector<vector<Point2D>> vvAreaVertex2Select,FlyQuaCallBackFun pFun = NULL);
	//������������Ӱ���ز�����������ͼ
	void CmrDistortionCorrect(float fScale = 4.0, bool bChkExist = true,FlyQuaCallBackFun pFun=NULL);
	//����ƥ�����
	void CalMatchMatrix(int nMatCount, int nDim = 3, int nNearst = 10,SSWMatchMatrixMethod eMethod = SSW_POS_DIS);
	//����ͼƥ�估ƽ�����
	bool BundleAdjustment(sswUAVMatchAndAdjPara para, bool bChkExist = true);
	//��������ʼ����
	bool RunFlyQuaChk(double Z = 0, /*int nMatchPtNumThresold = 8,*/ SSWFLYQUAMETHOD eMethod = BY_ADJUSTMENT,FlyQuaCallBackFun pFun=NULL);
	//�����ʼ챨��
	bool SaveReport2File1(CString strCqViewImgPath,CString strOverlapYErrViewImgPath,CString strOverlapXErrViewImgPath,CString strPosErrViewImgPath,
		CString strPitchErrViewImgPath,CString strYawViewImgPath,SSWFLYQUAMETHOD eMethod);
	bool SaveReport2File_20170108(CString strCqViewImgPath,CString strOverlapYErrViewImgPath,CString strOverlapXErrViewImgPath,CString strPosErrViewImgPath,
		CString strPitchErrViewImgPath,CString strYawViewImgPath,SSWFLYQUAMETHOD eMethod, float fOverlap4Ratio);
	bool SaveReport2File20170108_(CString strCqViewImgPath,CString strOverlapYErrViewImgPath,CString strOverlapXErrViewImgPath,CString strPosErrViewImgPath,
		CString strPitchErrViewImgPath,CString strYawViewImgPath,CString strMatchLowPointImgPath);

	//��ȡ���ò�����Ϣ
	SSWstuSysConfigPara GetPara()
	{
		return m_FlyQuaParaSet;
	}
	//��ȡ�ʼ챨��·��
	CString GetCheckRepPath(SSWFLYQUAMETHOD eMethod)
	{
		m_strFlyQuaChkRepPath = FunGetFileFolder(m_strPrjXmlPath)+"\\"+FunGetFileName(m_strPrjXmlPath,false)+"_����������鱨��.pdf";
// 		if(eMethod==BY_POS) m_strFlyQuaChkRepPath+="_ByPos.pdf";else
// 			if(eMethod==BY_MATCH) m_strFlyQuaChkRepPath+="_ByMatch.pdf";else
// 				if(eMethod==BY_ADJUSTMENT) m_strFlyQuaChkRepPath+="_ByBundle.pdf";
		return m_strFlyQuaChkRepPath;
	}
	//��ȡƥ���ļ�·��
	CString GetMatchPointPath(){return m_strMatchPointPath;}
	//��ȡƽ����·��
	CString GetAdjRepPath(){return m_strBundleAdjRepPath;}
	vector<CsswUAVImage> GetImgInfo(){return m_vecImgInfo;}
	int GetImgNum(){return m_nImgNum;}
	int GetValidImgNum(){return m_nImgNum-m_nInvalidNum;}
	int GetInvalidImgNum(){return m_nInvalidNum;}
	int GetStripNum(){return m_nStripNum;}
	int GetImgNumInStrip(int nIdx){
		if(nIdx<0) return 0;
		else if(nIdx>=m_nStripNum) return 0;
		else return m_vecImgNumInEachStrip[nIdx];
	}
	int GetImgIdx(int nStripIdx, int nIdxInStrip)
	{
		return m_vvImgIdxInEachStrip[nStripIdx][nIdxInStrip];
	}
	int GetInvalidImgIdx(int nIdxInInvalid)
	{
		if(m_nInvalidNum<0) return 0;
		if(nIdxInInvalid<0) return 0;
		if(nIdxInInvalid>=m_nInvalidNum) return 0;
		return m_vecImgIdxInvalid[nIdxInInvalid];
	}
	//��ȡ������Ӱ������
	vector<vector<int>> GetStripImgIdx(){return m_vvImgIdxInEachStrip;}
	//��ȡ���п�����
	vector<SSWstuAdjPtInfo> GetAllAdjPts(){return m_vecAdjPts;}
	//��ȡ��Ӱ��POS���
	vector<SSWstuPosInfo> GetImgPosErr(){return m_vecAdjImgPosErr;}
	//��ȡ��Ӱ��POS�������
	vector<double> GetImgPosDisErr(){return m_vecAdjImgPosDisErr;}
	//��ȡ�����ص���
	vector<vector<double>> GetImgOverlapInsStrip(){return m_vvOverlapsInsStrips;}
	//��ȡ�����ص���
	vector<vector<double>> GetImgOverlapBetStrip(){return m_vvOverlapsBetStrips;}
	//��ȡӰ�����
	vector<double> GetImgPitch(){return m_vecImagePitch;}
	//��ȡ������Ӱ������
	vector<vector<double>> GetStripImgYaw(){return m_vvStripImageYaw;}
	//��ȡ©������ڵ�
	vector<vector<Point3D>> GetMissAreaVertex(){return m_vvMissAreaVertex;}
	//��ȡ��������νڵ�
	vector<vector<Point3D>> GetAreaVertex(){return m_vvAreaVertex;}
	vector<vector<Point3D>> GetImgVertex3d(){return m_vvImgVertex3d;}
	//��ȡ�����ص��Ȳ��ϸ�Ӱ������
	vector<int> GetImgIdxLowOverlapInsStrips(){return m_vecImgIdxLowOverlapInsStrips;}
	//��ȡ�����ص��Ȳ��ϸ�Ӱ������
	vector<int> GetImgIdxLowOverlapBetStrips(){return m_vecImgIdxLowOverlapBetStrips;}
	//��ȡPOS���ϸ�Ӱ������
	vector<int> GetImgIdxHeiPosErr(){return m_vecImgIdxHeiPosErr;}
	//��ȡ��ǲ��ϸ�Ӱ������
	vector<int> GetImgIdxHeiPitch(){return m_vecImgIdxHeiPitch;}
	//��ȡ���ǲ��ϸ�Ӱ������
	vector<int> GetImgIdxHeiYaw(){return m_vecImgIdxHeiYaw;}
	//��ȡ��������Ӱ������
	vector<int> GetImgIdxLowAdjPoint(){return m_vecImgIdxLowMatchPointNum;}
	//��ȡ�ʼ�ͨ��Ӱ������
	vector<int> GetImgIdxPassCheck(){return m_vecImgIdxPassCheck;}
	//��ȡ����Ӱ���п�������
	vector<int>GetImgPointNum(){return m_vecPtNumInImgs;}

	//�Ƿ��Զ�����Ӱ��
	bool ifAutoManageImages(){return m_bAutoManageImages;}
	void setAutoManageImages(bool bAuto){m_bAutoManageImages=bAuto;}
protected:
	void ClearAdjInfo();
	void ClearStripInfo();
	void ClearReportInfo(SSWFLYQUAMETHOD eMethod);
	bool ReadPrjXml();
	bool ReadImpXml();
	bool SavePrjXml();
	bool SaveImpXml();
	bool SaveImgList();
	bool SavePosList();
	bool SaveMatchMatrix();

	void MatchMatrixByPosDis(int nMatCount, int nDim = 3, int nNearst = 10);
	void MatchMatrixByAreaLink(int nMatCount, int nDim = 3);
	bool ReadImgList();
	bool ReadAdjRep(FlyQuaCallBackFun pFun=NULL);
	bool ReadMatchPoint(FlyQuaCallBackFun pFun = NULL);
	void CalBundleImgList2ImgInfo();

	void CalImagePitch();
	void CalImageYaw(double Z, SSWFLYQUAMETHOD eMethod);
	void CalImageFlyHeiErr(SSWFLYQUAMETHOD eMethod);
	void CalImageFlyDisErr(SSWFLYQUAMETHOD eMethod);
	bool CalImagePosErr(SSWFLYQUAMETHOD eMethod = BY_ADJUSTMENT);
	void CalStripCqPaneErr(SSWFLYQUAMETHOD eMethod = BY_ADJUSTMENT);
	void CalStripOverlaps(double Z, SSWFLYQUAMETHOD eMethod,FlyQuaCallBackFun pFun = NULL);
	
	double CalOverlapsByPos(int nIdx1, int nIdx2, double Z, SSWOVERLAPDIRECTION eDirection);
	double CalOverlapsByMatch(int nIdx1, int nIdx2, SSWOVERLAPDIRECTION eDirection);
	double CalOverlapsByAdjustment(int nIdx1, int nIdx2, SSWOVERLAPDIRECTION eDirection);
	double CalOverlapByInterVertex(Point2D *pVertexPts, int nPtNum, double dFullLen, SSWOVERLAPDIRECTION eDirection);
	void AnalysisFlyQuaChkParas(int nMatchPtNumThresold);
	void CheckMissArea(double Z, SSWFLYQUAMETHOD eMethod);
	///////////////////������������///////////////////////
public:
	//void CalCqVertexByProject(float Z);
	
protected:
	CString m_strPrjXmlPath;
	CString m_strImpXmlPath;
	CString m_strThumbFolder;
	CString m_strMatchAndAdjFolder;
	CString m_strImgListPath;
	CString m_strPosListPath;
	CString m_strMatchMatrixPath;
	CString m_strBundleAdjRepPath;
	CString m_strMatchPointPath;
	CString m_strFlyQuaChkRepPath;
	vector<CsswUAVImage> m_vecImgInfo;
	vector<CsswUAVImage> m_vecInvalidImgInfo;
	vector<int> m_vecImgIdxInvalid;
	vector<int> m_vecBundleImgList2ImgInfo;
	vector<int> m_vecBundleSuccess2ImgInfo;
	vector<CString> m_vecBundleImgName;
	bool m_bCmrFile;
	bool m_bAutoManageImages;
	int m_nImgNum;
	int m_nInvalidNum;
	int m_nInAdjedNum;
	int m_nStripNum;
	int m_nMaxStripIdx;
	vector<int> m_vecStripIdx;
	SSWstuCmrInfo m_ImgCmrInfo;       //Ӱ���������
	SSWstuCmrInfo m_ThumbCmrInfo;     //����ͼ�������
	SSWstuPosFileExtend m_sswPosFormat;
	vector<int> m_vecImgNumInEachStrip;
	vector<vector<int>> m_vvImgIdxInEachStrip; //�����е�Ӱ��������Ϣ�����к���ʱ���ı�m_vecImgInfo�е�Ӱ��˳��
	int *m_pMatchMatrix;
	vector<SSWstuAdjPtInfo> m_vecAdjPts;
	vector<int> m_vecPtNumInImgs;
	double m_dMeanHeiCq;  //����������߳̾�ֵ
	/************�ʼ�ʱ��ͳ��*************/
	double m_dTimeSecsWholeProcess;
	double m_dTimeSecsCmrDistortion;
	double m_dTimeSecsBundleAdjustment;
	double m_dTimeSecsTimeReadAdjRep;
	double m_dTimeSecsRunFlyQuaChk;
	double m_dTimeSecsWriteChkRep;
	///////////////////////////////////////////////////////////////////////////
	/************�ʼ�������*************/
	SSWstuSysConfigPara m_FlyQuaParaSet;
	SSWstuSysConfigPara m_FlyQuaParaCal;
	vector<double> m_vecImagePitch;                 //Ӱ�����
	vector<vector<double>> m_vvStripImageYaw;       //����Ӱ������
	vector<vector<double>> m_vvStripImageFlyHeiErr; //��������Ӱ�񺽸߲�
	vector<SSWstuPosInfo>  m_vecAdjImgPosErr;       //Ӱ��POS���
	vector<double> m_vecAdjImgPosDisErr;            //Ӱ��POS�������
	vector<vector<double>> m_vvStripImageFlyDisErr; //������ƫ��
	vector<double> m_vecStripBlendPara;             //����������
	vector<vector<double>> m_vvOverlapsInsStrips;   //�����ص���
	vector<vector<double>> m_vvOverlapsBetStrips;   //�����ص���
	vector<double> m_vecStripPanePara;           //����ƽ����-ƽ���POS-Zƫ���
	vector<int> m_vecImgIdxLowOverlapInsStrips;  //�����ص��ȵ�����ֵ��Ӱ������
	vector<int> m_vecImgIdxLowOverlapBetStrips;  //�����ص��ȵ�����ֵ��Ӱ������
	vector<int> m_vecImgIdxHeiPitch;             //��Ǹ�����ֵ��Ӱ������
	vector<int> m_vecImgIdxHeiYaw;               //���Ǹ�����ֵ��Ӱ������
	vector<int> m_vecImgIdxHeiHeiErr;            //���߲������ֵ��Ӱ������
	vector<int> m_vecImgIdxHeiPosErr;            //Pos������3��������Ӱ������
	vector<int> m_vecBundleIdxHeiPosErr;         //Pos������3��������Bundle����
	vector<int> m_vecStripIdxHeiBlend;           //�����ȸ�����ֵ�ĺ�������
	vector<double> m_vecDisImgAdjPos2PosPane;    //ƽ���POS��POSƽ��ľ���
	vector<int> m_vecImgIdxPassCheck;
	double m_dRmsPosErr;
	double m_dMeanPosErr;
	double m_dMaxHeiErr;                         //������󺽸߲�
	double m_dMaxPaneDifErr;                     //�������͹��ֵ-ƽ���POS-ƫ���
	double m_dRmsPaneDifErr;
	double m_dMissAreaRatio;
	int m_nImgNumPitch10;                        //���>10��Ӱ����
	int m_nImgNumYaw15;                          //����>15��Ӱ����
	vector<int> m_vecImgNumInEachStripYaw20;     //ÿ����������>20��Ӱ����

	int m_nMatchPointNumThresold;
	vector<int> m_vecImgIdxLowMatchPointNum;     //��������Ӱ������

	vector<vector<Point3D>> m_vvMissAreaVertex;  //����©���ǵ�����
	vector<vector<int>> m_vvMissAreaImgIdx;      //����©�����Ӱ������
	vector<vector<Point3D>> m_vvAreaVertex;      //���ɲ�����Χ-���������߽�
	vector<vector<Point3D>> m_vvImgVertex3d;     //Ӱ���Ľǵ�
};
#endif