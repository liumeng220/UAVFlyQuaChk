#ifndef dy_uav_flyqua_h
#define dy_uav_flyqua_h
#pragma once

#include "MyDataBaseType.h"
#include "DYUAVFlyQuaChkPro.h"

class CDYImage
{
public:
	CDYImage();
	virtual ~CDYImage();
public:
	bool OpenImg(CString strImgPath, bool bReadOnly=true);
	void CloseImg();
	int  GetRows();
	int  GetCols();
	int  GetBandCount();
	int  GetBitSize();
	bool ReadImg(int nStartCol, int nStartRow, int nCols, int nRows, int nMemWidth, int nMemHeight, void*pMem);
	bool SaveImg(CString strSavePath, int nCols, int nRows, int nBands, BYTE*pMem,const char *pszFormat = "JPEG");
protected:
	CString findImageTypeGDAL(CString strDetImgName);
private:
	void* m_pDataset;
};

class CDYUAVImage:public CDYImage
{
public:
public:
	CDYUAVImage();
	virtual ~CDYUAVImage();

public:
	void CalRotMatrixByPOK(DYstuPosInfo& DYPosInfo);
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
	CString m_strStripLabel;                  
	DYstuCmrInfo m_ImgCmrInfo;                   //�������
	DYstuPosInfo m_ImgPosInfo;                   //Ӱ��POS��Ϣ
	DYstuPosInfo m_AdjPosInfo;                   //����POS
	DYstuAdjPt4Img  m_ImgAdjPts;                 //������
	bool m_bValid;                             //�Ƿ���Ч
	bool m_bAdjed;                             //�Ƿ����ƽ��

	Point2D m_pImgVertexPt2d4[4];
	Point3D m_pImgVertexPt3d4[4];


};

class CDYUAVFlyQuaPrj
{
public:
	CDYUAVFlyQuaPrj();
	~CDYUAVFlyQuaPrj();

public:
	//�������ݼ����ò���
	bool LoadData2Prj(CString strImportDataXmlPath);
	//�����������
	void ClearPrjData();
	//�������ò���
	void CheckImgValidState();
	//ɾ����ЧӰ��
	void DeleteInvalidImg();
	//��������ʼ����
	bool RunFlyQuaChk(double Z = 0, /*int nMatchPtNumThresold = 8,*/ DYFLYQUAMETHOD eMethod = BY_ADJUSTMENT,FlyQuaCallBackFun pFun=NULL);
	//�����ʼ챨��
	bool SaveReport2File(CString strCqViewImgPath,CString strOverlapYErrViewImgPath,CString strOverlapXErrViewImgPath,CString strPosErrViewImgPath,
		CString strPitchErrViewImgPath,CString strYawViewImgPath,CString strRepPath,DYFLYQUAMETHOD eMethod = BY_ADJUSTMENT);
	//��ȡ���ò�����Ϣ
	DYstuSysConfigPara GetPara()
	{
		return m_FlyQuaParaSet;
	}
	vector<CDYUAVImage> GetImgInfo(){return m_vecImgInfo;}
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
	vector<DYstuAdjPtInfo> GetAllAdjPts(){return m_vecAdjPts;}
	//��ȡ��Ӱ��POS���
	vector<DYstuPosInfo> GetImgPosErr(){return m_vecAdjImgPosErr;}
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
protected:
	void ClearAdjInfo();
	void ClearStripInfo();
	void ClearReportInfo(DYFLYQUAMETHOD eMethod);
	bool ReadImpXml();
	bool ReadAdjRep(FlyQuaCallBackFun pFun=NULL);
	void CalBundleImgList2ImgInfo();

	void CalImagePitch();
	void CalImageYaw(double Z, DYFLYQUAMETHOD eMethod);
	void CalImageFlyHeiErr(DYFLYQUAMETHOD eMethod);
	void CalImageFlyDisErr(DYFLYQUAMETHOD eMethod);
	bool CalImagePosErr(DYFLYQUAMETHOD eMethod = BY_ADJUSTMENT);
	void CalStripCqPaneErr(DYFLYQUAMETHOD eMethod = BY_ADJUSTMENT);
	void CalStripOverlaps(double Z, DYFLYQUAMETHOD eMethod,FlyQuaCallBackFun pFun = NULL);
	
	double CalOverlapsByPos(int nIdx1, int nIdx2, double Z, DYOVERLAPDIRECTION eDirection);
	double CalOverlapsByMatch(int nIdx1, int nIdx2, DYOVERLAPDIRECTION eDirection);
	double CalOverlapsByAdjustment(int nIdx1, int nIdx2, DYOVERLAPDIRECTION eDirection);
	double CalOverlapByInterVertex(Point2D *pVertexPts, int nPtNum, double dFullLen, DYOVERLAPDIRECTION eDirection);
	void AnalysisFlyQuaChkParas(int nMatchPtNumThresold);
	void CheckMissArea(double Z, DYFLYQUAMETHOD eMethod);
	///////////////////������������///////////////////////
public:
	//void CalCqVertexByProject(float Z);
	
protected:
	CString m_strImpXmlPath;
	CString m_strCmrFilePath;
	CString m_strBundleAdjRepPath;
	CString m_strFlyQuaChkRepPath;
	vector<CDYUAVImage> m_vecImgInfo;
	vector<CDYUAVImage> m_vecInvalidImgInfo;
	vector<int> m_vecImgIdxInvalid;
	vector<int> m_vecBundleImgList2ImgInfo;
	vector<int> m_vecBundleSuccess2ImgInfo;
	vector<CString> m_vecBundleImgName;
	int m_nImgNum;
	int m_nInvalidNum;
	int m_nInAdjedNum;
	int m_nStripNum;
	int m_nMaxStripIdx;
	bool m_bReManageImages;
	vector<int> m_vecStripIdx;
	DYstuCmrInfo m_ImgCmrInfo;       //Ӱ���������
	DYstuCmrInfo m_ThumbCmrInfo;     //����ͼ�������
	vector<int> m_vecImgNumInEachStrip;
	vector<vector<int>> m_vvImgIdxInEachStrip; //�����е�Ӱ��������Ϣ�����к���ʱ���ı�m_vecImgInfo�е�Ӱ��˳��
	vector<DYstuAdjPtInfo> m_vecAdjPts;
	double m_dMeanHeiCq;  //����������߳̾�ֵ
	/************�ʼ�ʱ��ͳ��*************/
	double m_dTimeSecsWholeProcess;
	double m_dTimeSecsTimeReadAdjRep;
	double m_dTimeSecsRunFlyQuaChk;
	double m_dTimeSecsWriteChkRep;
	///////////////////////////////////////////////////////////////////////////
	/************�ʼ�������*************/
	DYstuDeviceFileExtend m_DevExtend;
	DYstuSysConfigPara m_FlyQuaParaSet;

	vector<double> m_vecImagePitch;                 //Ӱ�����
	vector<vector<double>> m_vvStripImageYaw;       //����Ӱ������
	vector<vector<double>> m_vvStripImageFlyHeiErr; //��������Ӱ�񺽸߲�
	vector<DYstuPosInfo>  m_vecAdjImgPosErr;       //Ӱ��POS���
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
	int m_nImgNumPitch10;                        //���>10��Ӱ����
	int m_nImgNumYaw15;                          //����>15��Ӱ����
	vector<int> m_vecImgNumInEachStripYaw20;     //ÿ����������>20��Ӱ����

	int m_nMatchPointNumThresold;
	vector<int> m_vecImgIdxLowMatchPointNum;     //ƥ������Ӱ������

	vector<vector<Point3D>> m_vvMissAreaVertex;  //����©���ǵ�����
	vector<vector<int>> m_vvMissAreaImgIdx;      //����©�����Ӱ������
	vector<vector<Point3D>> m_vvAreaVertex;      //���ɲ�����Χ-���������߽�
	vector<vector<Point3D>> m_vvImgVertex3d;     //Ӱ���Ľǵ�
};

class CMemTxtFile
{
public:
	CMemTxtFile();
	~CMemTxtFile();
public:
	bool OpenFile(CString strFilePath);
	void CloseFile();
	DWORD GetFileSize(){return m_dwFileSize;}
	char* GetFileHead(){return m_lpbFile;}
private:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	char* m_lpbFile;
	DWORD m_dwFileSize;
};

#endif