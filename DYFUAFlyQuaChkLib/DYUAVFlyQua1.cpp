// DYUAVFlyQuaLib.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "DYUAVFlyQua.h"
#include "gdal201/gdal.h"
#include "gdal201/gdal_priv.h"
#include "gdal201/ogrsf_frmts.h"
#include "gdal201/ogr_spatialref.h"
#include "pugixml.hpp"
#include "CoorConv.hpp"
#include <omp.h>
#include "Matrix.h"
#include "sswPDFLib.h"
#include "conio.h"
#include "MyFunctions.h"

inline bool ReadLine(const char* mbuf, char*line, int maxlen)
{
	int len = 0;
	while(len<maxlen-1&&*(mbuf+len)!='\r')
	{
		*(line+len)=*(mbuf+len);
		len++;
	}
	*(line+len)='\0';
	if(strlen(line)<=0) return false;
	//	mbuf+=strlen(line)+2;
	return true;
}

//����ֱ��-ƽ�淽�̲���
void FunCalLineParas(Point2D pt1, Point2D pt2, double &a, double &b, double &c)
{
	if (pt1.x == pt2.x)
	{
		b = 0;
		a = 1;
		c = -pt1.x;
	}
	else
	{
		b = 1;
		a = -(pt1.y -pt2.y) / (pt1.x - pt2.x);
		c = -a*pt1.x - pt1.y;
	}
}
void FunCalLineParasByLsm(vector<Point2D> vecPt, double &a, double &b, double &c)
{
	int num = vecPt.size();
	int nCount = 0;
	for (int i = 0; i<num; i++)
	{
		if(vecPt[i].x==vecPt[0].x&&vecPt[i].y!=vecPt[0].y) 
			nCount++;
	}
	if(nCount==num)  //ֱ�߷���Ϊx=a;
	{
		a = 1;
		b = 0;
		c = vecPt[0].x;
		return;
	}
	//ֱ�߷���Ϊy=ax+b
	double *A = new double [num * 2]; memset(A, 0, num * 2 * sizeof(double));
	double *X = new double[2 * 1]; memset(X, 0, 2 * 1 * sizeof(double));
	double *L = new double[num * 1]; memset(L, 0, num * 1 * sizeof(double));
	double *AT = new double[2 * num]; memset(AT, 0, num * 2 * sizeof(double));
	double *ATA = new double[2 * 2]; memset(ATA, 0, 2 * 2 * sizeof(double));
	double *ATL = new double[2 * 1]; memset(ATL, 0, 2 * 1 * sizeof(double));
	for (int i = 0; i < num; i++) {
		A[i * 2 + 0] = vecPt[i].x;
		A[i * 2 + 1] = 1;
		L[i * 1 + 0] = vecPt[i].y;
	}
	CMatrix MatrixApp;
	MatrixApp.MatrixTranspose(A, AT, num, 2);
	MatrixApp.MatrixMulti(AT, A, ATA, 2, num, 2);
	MatrixApp.MatrixMulti(AT, L, ATL, 2, num, 1);

	MatrixApp.MatrixInversion_General(ATA, 2);
	MatrixApp.MatrixMulti(ATA, ATL, X, 2, 2, 1);
	a = X[0];
	b = -1;
	c = X[1];
	delete[]A;
	delete[]X;
	delete[]L;
	delete[]AT ;
	delete[]ATA;
	delete[]ATL;
}
void FunCalPaneParasBylsm(vector<Point3D> vecPt, double &a, double &b, double &c, double &d)
{
	int num = vecPt.size();
	//ƽ�淽��Z=aX+bY+c
	double *A = new double [num * 3]; memset(A, 0, num * 3 * sizeof(double));
	double *X = new double[3 * 1]; memset(X, 0, 3 * 1 * sizeof(double));
	double *L = new double[num * 1]; memset(L, 0, num * 1 * sizeof(double));
	double *AT = new double[3 * num]; memset(AT, 0, num * 3 * sizeof(double));
	double *ATA = new double[3 * 3]; memset(ATA, 0, 3 * 3 * sizeof(double));
	double *ATL = new double[3 * 1]; memset(ATL, 0, 3 * 1 * sizeof(double));
	for (int i = 0; i < num; i++) {
		A[i * 3 + 0] = vecPt[i].X;
		A[i * 3 + 1] = vecPt[i].Y;
		A[i * 3 + 2] = 1;
		L[i * 1 + 0] = vecPt[i].Z;
	}
	CMatrix MatrixApp;
	MatrixApp.MatrixTranspose(A, AT, num, 3);
	MatrixApp.MatrixMulti(AT, A, ATA, 3, num, 3);
	MatrixApp.MatrixMulti(AT, L, ATL, 3, num, 1);

	MatrixApp.MatrixInversion_General(ATA, 3);
	MatrixApp.MatrixMulti(ATA, ATL, X, 3, 3, 1);
	a = X[0];
	b = X[1];
	c = -1;
	d = X[2];
	delete[]A;
	delete[]X;
	delete[]L;
	delete[]AT ;
	delete[]ATA;
	delete[]ATL;
}
CString FunCreateStripLabel(int nStripIdx, int nIdxInStrip)
{
	CString strLabel;strLabel.Format("%s_%s","00","000");
	if(nStripIdx==InvalidValue||nIdxInStrip==InvalidValue) return" ";
	CString strStripIdx; strStripIdx.Format("%d",nStripIdx+1);
	CString strIdxInStrip; strIdxInStrip.Format("%d",nIdxInStrip+1);
#pragma omg parallel for
	for (int i = 0; i<strStripIdx.GetLength(); i++)
	{
		strLabel.SetAt((strLabel.GetLength()-1)/2-strStripIdx.GetLength()+i,strStripIdx.GetAt(i));
	}
#pragma omg parallel for
	for (int i = 0; i<strIdxInStrip.GetLength(); i++)
	{
		strLabel.SetAt(strLabel.GetLength()-strIdxInStrip.GetLength()+i,strIdxInStrip.GetAt(i));
	}
	return strLabel;
}
double  FunCalDataRms(vector<double> vData, double dMean)
{
	double dSum2 = 0;
	for (int i = 0; i<vData.size(); i++)
	{
		dSum2+=pow(vData[i]-dMean,2);
	}
	if(vData.size()>0) return sqrt(dSum2/vData.size());
	else return 0;
}
//����ͬ����ָ�������˹�ϵ
void FunCalTopoParasByLsm(vector<Point2D> vecPt1, vector<Point2D> vecPt2, double **CoePara)
{
	int num = vecPt1.size();
	double *A = new double [num * 2 * 6]; memset(A, 0, num * 2 * 6 * sizeof(double));
	double *X = new double[6 * 1]; memset(X, 0, 6 * 1 * sizeof(double));
	double *L = new double[num * 2]; memset(L, 0, num * 2 * sizeof(double));
	double *AT = new double[6 * 2 * num]; memset(AT, 0, num * 6 * 2 * sizeof(double));
	double *ATA = new double[6 * 6]; memset(ATA, 0, 6 * 6 * sizeof(double));
	double *ATL = new double[6 * 1]; memset(ATL, 0, 6 * 1 * sizeof(double));
	for (int i = 0; i < num; i++) {
		A[i * 12 + 0] = vecPt1[i].x;
		A[i * 12 + 1] = vecPt1[i].y;
		A[i * 12 + 2] = 1;
		A[i * 12 + 9]  = vecPt1[i].x;
		A[i * 12 + 10] = vecPt1[i].y;
		A[i * 12 + 11] = 1;
		L[i * 2 + 0] = vecPt2[i].x;
		L[i * 2 + 1] = vecPt2[i].y;
	}
	CMatrix MatrixApp;
	MatrixApp.MatrixTranspose(A, AT, num*2, 6);
	MatrixApp.MatrixMulti(AT, A, ATA, 6, num*2, 6);
	MatrixApp.MatrixMulti(AT, L, ATL, 6, num*2, 1);
	MatrixApp.MatrixInversion_General(ATA, 6);
	MatrixApp.MatrixMulti(ATA, ATL, X, 6, 6, 1);
	*CoePara = X;
	delete[]A;
	delete[]L;
	delete[]AT;
	delete[]ATA;
	delete[]ATL;
}
void FunImgPtTopoTrans(Point2D pt1, double* CoePara, Point2D& pt2)
{
	pt2.x = CoePara[0]*pt1.x+CoePara[1]*pt1.y+CoePara[2];
	pt2.y = CoePara[3]*pt1.x+CoePara[4]*pt1.y+CoePara[5];
}
void FunArrangeAdjPts(vector<CDYUAVImage>& vecImgInfo,vector<DYstuAdjPtInfo> &vecAdjPts)
{
	for (int i = 0; i<vecAdjPts.size(); i++)
	{
		for (int j = 0; j<vecAdjPts[i].nOverlayCount; j++)
		{
			int nImgIdx = vecAdjPts[i].vecImgIdx[j];
			vecImgInfo[nImgIdx].m_ImgAdjPts.vecAdjPtIdx.push_back(i);
			vecImgInfo[nImgIdx].m_ImgAdjPts.vecImgPtIdxInAdjPt.push_back(j);		
			for (int jj = 0; jj<vecAdjPts[i].nOverlayCount; jj++)
			{
				bool bExist = false;
				if(jj==j) continue;
				for(int k = 0; k<vecImgInfo[nImgIdx].m_ImgAdjPts.vecLinkImgIdx.size(); k++)
				{
					if(vecImgInfo[nImgIdx].m_ImgAdjPts.vecLinkImgIdx[k]==vecAdjPts[i].vecImgIdx[jj])
					{
						bExist=true;
						vecImgInfo[nImgIdx].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k].push_back(i);
						vecImgInfo[nImgIdx].m_ImgAdjPts.vvImgPtIdxInAdjPt[k].push_back(j);
						vecImgInfo[nImgIdx].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k].push_back(jj);
						break;
					}
				}
				if(!bExist)
				{
					vecImgInfo[nImgIdx].m_ImgAdjPts.vecLinkImgIdx.push_back(vecAdjPts[i].vecImgIdx[jj]);
					vector<int> vAdjIdx4LinkImg; vAdjIdx4LinkImg.push_back(i);
					vector<int> vImgPtIdxInAdjPt; vImgPtIdxInAdjPt.push_back(j);
					vector<int> vLinkImgPtIdxInAdjPt; vLinkImgPtIdxInAdjPt.push_back(jj);
					vecImgInfo[nImgIdx].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg.push_back(vAdjIdx4LinkImg);
					vecImgInfo[nImgIdx].m_ImgAdjPts.vvImgPtIdxInAdjPt.push_back(vImgPtIdxInAdjPt);
					vecImgInfo[nImgIdx].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt.push_back(vLinkImgPtIdxInAdjPt);
					vector<int>().swap(vAdjIdx4LinkImg); 
					vector<int>().swap(vImgPtIdxInAdjPt); 
					vector<int>().swap(vLinkImgPtIdxInAdjPt); 
				}
			}

		}
	}

	for (int i = 0; i<vecImgInfo.size(); i++)
	{
		if(!vecImgInfo[i].m_bValid) continue;
		for (int j = 0; j<vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg.size()-1; j++)
		{
			if(vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg.size()==0) break;
			int nSum1 = vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[j].size();
			for (int k = j+1; k<vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg.size(); k++)
			{
				int nSum2 = vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k].size();
				if(nSum1<nSum2)
				{
					nSum1 = nSum2;
					swap(vecImgInfo[i].m_ImgAdjPts.vecLinkImgIdx[j],vecImgInfo[i].m_ImgAdjPts.vecLinkImgIdx[k]);
					swap(vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[j],vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k]);
					swap(vecImgInfo[i].m_ImgAdjPts.vvImgPtIdxInAdjPt[j],vecImgInfo[i].m_ImgAdjPts.vvImgPtIdxInAdjPt[k]);
					swap(vecImgInfo[i].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[j],vecImgInfo[i].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k]);
				}
			}
		}
	}
#pragma omp parallel for
	for (int i = 0; i<vecImgInfo.size(); i++)
	{
		if(!vecImgInfo[i].m_bAdjed) continue;
		if(vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx.size()==0) continue;
		int nAdjPtNum = vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx.size();
		vecImgInfo[i].m_ImgAdjPts.vecImgPtResX.resize(nAdjPtNum);
		vecImgInfo[i].m_ImgAdjPts.vecImgPtResY.resize(nAdjPtNum);
		vecImgInfo[i].OpenImg(vecImgInfo[i].m_strImgPath);
		int nCols = vecImgInfo[i].GetCols();
		int nRows = vecImgInfo[i].GetRows();
		for (int j = 0; j<nAdjPtNum; j++)
		{
			int nAdjPtIdx = vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx[j];
			int nImgPtIdx = vecImgInfo[i].m_ImgAdjPts.vecImgPtIdxInAdjPt[j];
			Point2D ptImg0 = vecAdjPts[nAdjPtIdx].vecImgPt[nImgPtIdx];
			Point3D ptAdjXYZ = Point3D(vecAdjPts[nAdjPtIdx].X,vecAdjPts[nAdjPtIdx].Y,vecAdjPts[nAdjPtIdx].Z);
			Point2D ptImg1 = vecImgInfo[i].XYZ2Img(nCols,nRows,vecImgInfo[i].m_ImgCmrInfo.f,ptAdjXYZ,true);
			Point3D ptXYZ = vecImgInfo[i].Img2XYZ(nCols,nRows,vecImgInfo[i].m_ImgCmrInfo.f,ptImg0,ptAdjXYZ.Z,true);
			vecImgInfo[i].m_ImgAdjPts.vecImgPtResX[j]=ptImg1.x-ptImg0.x;
			vecImgInfo[i].m_ImgAdjPts.vecImgPtResY[j]=ptImg1.y-ptImg0.y;
		}
		vecImgInfo[i].CloseImg();
	}
}
void FunFitHei4ImgAdjPts4ImgPt(vector<DYstuAdjPtInfo>& vecAdjPtsAll, double dMeanZOfCq,DYstuAdjPt4Img& ImgAdjPtInfo, Point2D ptImgAimPt, double& Z)
{
	if(ImgAdjPtInfo.vecAdjPtIdx.size()==0)
	{
		Z=dMeanZOfCq;
		return;
	}
	double dMinDis = 1e10;
	for (int i = 0; i<ImgAdjPtInfo.vecAdjPtIdx.size(); i++)
	{
		Point2D ptImg;
		int nAdjPtIdx = ImgAdjPtInfo.vecAdjPtIdx[i];
		int nImgPtIdx = ImgAdjPtInfo.vecImgPtIdxInAdjPt[i];
		ptImg=vecAdjPtsAll[nAdjPtIdx].vecImgPt[nImgPtIdx];
		double dis = sqrt(pow(ptImg.x-ptImgAimPt.x,2)+pow(ptImg.y-ptImgAimPt.y,2));
		if(dMinDis>dis)
		{
			dMinDis=dis;
			Z = vecAdjPtsAll[nAdjPtIdx].Z;
		}
	}
}
void FunFitHei4ImgAdjPts4ObjPt(vector<DYstuAdjPtInfo>& vecAdjPtsAll, double dMeanZOfCq,DYstuAdjPt4Img& ImgAdjPtInfo, Point2D ptObjAimPt, double& Z)
{
	if(ImgAdjPtInfo.vecAdjPtIdx.size()==0)
	{
		Z=dMeanZOfCq;
		return;
	}
	double dMinDis = 1e10;
	for (int i = 0; i<ImgAdjPtInfo.vecAdjPtIdx.size(); i++)
	{
		Point2D ptObj;
		int nAdjPtIdx = ImgAdjPtInfo.vecAdjPtIdx[i];
		ptObj.x=vecAdjPtsAll[nAdjPtIdx].X;
		ptObj.y=vecAdjPtsAll[nAdjPtIdx].Y;
		double dis = sqrt(pow(ptObj.x-ptObjAimPt.x,2)+pow(ptObj.y-ptObjAimPt.y,2));
		if(dMinDis>dis)
		{
			dMinDis=dis;
			Z = vecAdjPtsAll[nAdjPtIdx].Z;
		}
	}
}
void FunTransPolygonVertexOrder(Point3D* &pVertex3d, int nCount /*=4*/)
{
	if(nCount!=4) return;	
	vector<int> vIdxNear;
	Point2D*pVertex2d = new Point2D[nCount];
	Point3D*pVertec3d_2 = new Point3D[nCount];
	for(int i = 0; i<nCount; i++)
	{
		pVertex2d[i].x = pVertex3d[i].X;
		pVertex2d[i].y = pVertex3d[i].Y;
		pVertec3d_2[i] = pVertex3d[i];
	}
	Point2D pt1 = pVertex2d[0];
	for (int j = 1; j<nCount; j++)
	{
		int nSide = 0;
		Point2D pt2 = pVertex2d[j];
		double a,b,c;
		FunCalLineParas(pt1,pt2,a,b,c);
		for (int k = 1; k<nCount; k++)
		{
			if(k==j) continue;
			if(a*pVertex2d[k].x+b*pVertex2d[k].y+c>0)
				nSide++;
			else 
				nSide--;
		}
		if(fabs(nSide*1.0)==nCount-2)
		{
			vIdxNear.push_back(j);
		}
	}
	if(vIdxNear.size()==0) return;
	int nIdx;
	for (int i = 1; i<nCount; i++)
	{
		bool bNear = false;
		for (int j = 0; j<vIdxNear.size(); j++)
		{
			if(i==vIdxNear[j])
			{
				bNear = true;
				continue;
			}
		}
		if(!bNear) nIdx = i;
	}
	pVertex3d[0]=pVertec3d_2[0];
	pVertex3d[1]=pVertec3d_2[vIdxNear[0]];
	pVertex3d[2]=pVertec3d_2[nIdx];
	pVertex3d[3]=pVertec3d_2[vIdxNear[1]];
	delete[]pVertec3d_2;
	delete[]pVertex2d;
	vector<int>().swap(vIdxNear);
}
OGRPolygon* FunCreatePolygon(void* pVertex, int nDim, int nCount)
{
	OGRPolygon* pPolygon = NULL;
	if((nDim!=2&&nDim!=3)||nCount<3) 
	{
		pPolygon=NULL;
		return NULL;
	}
	pPolygon = (OGRPolygon*) OGRGeometryFactory::createGeometry(wkbPolygon);
	OGRLinearRing * pRing = (OGRLinearRing*) OGRGeometryFactory::createGeometry(wkbLinearRing);
	if(pRing==NULL||pPolygon==NULL)
	{
		pPolygon = NULL; 
		return NULL;
	}
	if(nDim==2)
	{
		Point2D *pV = (Point2D*) pVertex;
		for (int i = 0; i<nCount; i++)
		{
			pRing->addPoint(pV[i].x,pV[i].y);
		}
	}
	else if(nDim==3)
	{
		Point3D *pV = (Point3D*) pVertex;
		for (int i = 0; i<nCount; i++)
		{
			pRing->addPoint(pV[i].X,pV[i].Y);
		}
	}
	pRing->closeRings();
	pPolygon->addRing(pRing);
	if(pRing) OGRGeometryFactory::destroyGeometry(pRing);
	return pPolygon;
}
Point2D* FunPolygonIntersect(OGRPolygon* pPolygon1, OGRPolygon* pPolygon2, int& nPtNum)
{
	Point2D* pPtInterVertex = NULL;
	if(pPolygon1==NULL||pPolygon2==NULL) return false;
	OGRGeometry *pInterGeometry = pPolygon1->Intersection(pPolygon2);
	if(pInterGeometry==NULL) 
	{
		OGRGeometryFactory::destroyGeometry(pPolygon1);
		OGRGeometryFactory::destroyGeometry(pPolygon2);
		return NULL;
	}
	if(strcmp("POLYGON",pInterGeometry->getGeometryName()) != 0)
	{
		OGRGeometryFactory::destroyGeometry(pInterGeometry);
		OGRGeometryFactory::destroyGeometry(pPolygon1);
		OGRGeometryFactory::destroyGeometry(pPolygon2);
		return NULL;
	}
	OGRPolygon *pInterPolygon12 = (OGRPolygon*)pInterGeometry;
	OGRLinearRing *pInterRing12 = pInterPolygon12->getExteriorRing();
	nPtNum = pInterRing12->getNumPoints() - 1;
	if(nPtNum<3) return NULL;
	if(pPtInterVertex!=NULL)
	{
		delete[] pPtInterVertex;
		pPtInterVertex = NULL;
	}
	pPtInterVertex = new Point2D[nPtNum];
	for (int i = 0;i<nPtNum;  i++)
	{
		OGRPoint ogrpt;
		pInterRing12->getPoint(i,&ogrpt);
		pPtInterVertex[i].x = ogrpt.getX();
		pPtInterVertex[i].y = ogrpt.getY();
	}
	return pPtInterVertex;
}
//��������
void FunSortByX(int nCount, float *&pX, float *&pY)
{
	for (int i = 0; i<nCount-1; i++)
	{
		for (int j = i+1;j <nCount; j++)
		{
			if(*(pX+i)>*(pX+j))
			{
				float tmp = *(pX+i);
				*(pX+i)=*(pX+j);
				*(pX+j)=tmp;
				tmp = *(pY+i);
				*(pY+i)=*(pY+j);
				*(pY+j)=tmp;
			}
		}
	}

}
void FunSortByX(int nCount, float *&pX, float *&pY,float *&pZ)
{
	for (int i = 0; i<nCount-1; i++)
	{
		for (int j = i+1;j <nCount; j++)
		{
			if(*(pX+i)>*(pX+j))
			{
				float tmp = *(pX+i);
				*(pX+i)=*(pX+j);
				*(pX+j)=tmp;
				tmp = *(pY+i);
				*(pY+i)=*(pY+j);
				*(pY+j)=tmp;
				tmp = *(pZ+i);
				*(pZ+i)=*(pZ+j);
				*(pZ+j)=tmp;
			}
		}
	}

}

//�ʼ챨�浥��Ŀ��¼
enum PdfListContent
{
	PdfList_CQPreView = 1,
	PdfList_POSValue = 2,
	PdfList_Overlap = 3,
	PdfList_OverlapBad = 4,
	PdfList_ElseBad =5,
	PdfList_TimeUsed = 6
};
void NewListInfoMem2Pdf(int nCols,int nRows,char **&pStrInfo/*, float *&pr, float *&pg, float *&pb*/)
{
	pStrInfo = new char *[nRows*nCols];
	for (int i = 0; i<nCols*nRows; i++)
	{
		pStrInfo[i] = new char[MAX_SIZE_FILE_LINE];
	}
}
void DeleteListInfoMem2Pdf(int nCols,int nRows,char **&pStrInfo/*, float *pr,float *pg, float *pb*/)
{
	for (int i = 0; i<nCols*nRows; i++)
	{
		delete[]pStrInfo[i];
	}
	delete[]pStrInfo;
	// 	delete[]pr;
	// 	delete[]pg;
	// 	delete[]pb;
}
/*********************************************/
/* CDYImage */
/*********************************************/
static bool g_bRegisterGdal = false;
CDYImage::CDYImage()
{
	m_pDataset = NULL;
	if(!g_bRegisterGdal)
	{
		GDALAllRegister();         //����GDAL��ȡͼƬ����Ҫ����ע��  
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //����֧������·��
		g_bRegisterGdal = true;
	}
}
CDYImage::~CDYImage()
{
	if(m_pDataset) CloseImg();
}
CString CDYImage::findImageTypeGDAL(CString strDetImgName)
{
	CString strExt=FunGetFileExt(strDetImgName);
	strExt.MakeLower();
	if     (strExt=="bmp") return "BMP";
	else if(strExt=="jpg"|| strExt=="jpeg") return "JPEG";
	else if(strExt=="png") return "PNG";
	else if(strExt=="tif"||strExt=="tiff")  return "GTiff";
	else if(strExt=="gif") return "GIF";
	else return""; 
}
bool CDYImage::OpenImg(CString strImgPath, bool bReadOnly/* =true */)
{
	if(!PathFileExists(strImgPath)) return false;
	GDALAccess eAcess = bReadOnly?GA_ReadOnly:GA_Update;
	m_pDataset=(GDALDataset*) GDALOpen(strImgPath,eAcess);
	if(m_pDataset==NULL) return false;
	return true;
}
void CDYImage::CloseImg()
{
	if(m_pDataset) 
	{
		GDALClose((GDALDataset*)m_pDataset);
		m_pDataset=NULL;
	}
}
int  CDYImage::GetRows()
{
	if(m_pDataset==NULL) return InvalidValue;
	return ((GDALDataset*)m_pDataset)->GetRasterYSize();
}
int  CDYImage::GetCols()
{
	if(m_pDataset==NULL) return InvalidValue;
	return ((GDALDataset*)m_pDataset)->GetRasterXSize();
}
int  CDYImage::GetBandCount()
{

	if(m_pDataset==NULL) return InvalidValue;
	return ((GDALDataset*)m_pDataset)->GetRasterCount();
}
int  CDYImage::GetBitSize()
{

	if(m_pDataset==NULL) return InvalidValue;
	return GDALGetDataTypeSize(((GDALDataset*)m_pDataset)->GetRasterBand(1)->GetRasterDataType())/8;
}
bool CDYImage::ReadImg(int nStartCol, int nStartRow, int nCols, int nRows, int nMemWidth, int nMemHeight, void*pMem)
{
	if(m_pDataset==NULL) return false;
	GDALDataset* pDataset = (GDALDataset*)m_pDataset;
	nStartCol=max(nStartCol,0); nCols = min(GetCols(),nCols);
	nStartRow=max(nStartRow,0); nRows = min(GetRows(),nRows);
	int nBands=GetBandCount();
	int nBitSize=GetBitSize();	
	int *pBandMap = new int[nBands];
	for (int i=0; i<nBands; i++)
	{
		*(pBandMap+i)=i+1;;
	}

	BYTE* pRead = (BYTE*)pMem;
	if(GDT_ERROR==pDataset->RasterIO(GF_Read, nStartCol, nStartRow, nCols, nRows, pRead, nMemWidth, nMemHeight,GDT_Byte,
		nBands, pBandMap, nBands*nBitSize,nBands*nBitSize*nMemWidth,nBitSize)) return false;
	return true;
}
bool CDYImage::SaveImg(CString strSavePath, int nCols, int nRows, int nBands, BYTE*pMem,const char *pszFormat /* = JPEG */)
{
	if(strcmp(pszFormat,"GTiff")==0)
	{
		GDALDriver *poDriver=GetGDALDriverManager()->GetDriverByName(pszFormat);  
		if( poDriver == NULL ) return false;
		GDALDataset *WriteDataSet = poDriver->Create(strSavePath, nCols,nRows,nBands,GDT_Byte,NULL);  
		int pBandMap[3]={1,2,3};
		if(WriteDataSet->RasterIO(GF_Write,0,0,nCols,nRows,pMem,nCols,nRows,GDT_Byte,nBands,NULL,nBands*1,nCols*nBands*1,1)==CE_Failure )  
		{  
			return false;  
		}  
		GDALClose(poDriver);   
		GDALClose(WriteDataSet); WriteDataSet=NULL;  
		return true; 
	}
	else
	{
		if (strSavePath.IsEmpty() || pMem == NULL || nCols <1 || nRows < 1 || nBands < 1) 
		{
			return false;
		}

		CString GType = findImageTypeGDAL(strSavePath);  
		if (GType.IsEmpty())  { return false; }  
		GDALDriver *pMemDriver = NULL;  
		pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");  
		if( pMemDriver == NULL ) { return false; }  

		GDALDataset * pMemDataSet = pMemDriver->Create("",nCols,nRows,3,GDT_Byte,NULL);  
		GDALRasterBand *pBand = NULL;  
		int nLineCount = nCols * 3;  
		int pBandMap[3]={1,2,3};
		CPLErr err = pMemDataSet->RasterIO(GF_Write,0,0,nCols,nRows,pMem,nCols,nRows,GDT_Byte,3,pBandMap,nBands*1,nLineCount,1);
		if(err==CE_Failure) return false;
		GDALDriver *pDstDriver = NULL;  
		pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);  
		if (pDstDriver == NULL) { return false; }  

		pDstDriver->CreateCopy(strSavePath,pMemDataSet,FALSE, NULL, NULL, NULL);  
		GDALClose(pMemDataSet);   
		return true;
	}
}

/*********************************************/
/* CDYUAVImage */
/*********************************************/
CDYUAVImage::CDYUAVImage()
{
	m_nImgIdx =InvalidValue;           
	m_nStripIdx = InvalidValue;			
	m_nIdxInStrip = InvalidValue;					

	m_strImgPath.Empty();    
	m_strStripLabel = " ";
	m_ImgCmrInfo = DYstuCmrInfo();           
	m_ImgPosInfo=DYstuPosInfo();            
	m_AdjPosInfo=DYstuPosInfo();            
	vector<int>().swap(m_ImgAdjPts.vecLinkImgIdx);
	vector<vector<int>>().swap(m_ImgAdjPts.vvAdjPtIdx4EachLinkImg);
	vector<vector<int>>().swap(m_ImgAdjPts.vvImgPtIdxInAdjPt);
	vector<vector<int>>().swap(m_ImgAdjPts.vvLinkImgPtIdxInAdjPt);
	m_bValid = true;                         
	m_bAdjed = false;                         
	// 	m_pImgVertexPt2d4 = NULL;
	// 	m_pThumbImgVertexPt2d4 = NULL;
	//m_pImgVertexPt3d4 = NULL;
}
CDYUAVImage::~CDYUAVImage()
{
	// 	if(m_pImgVertexPt2d4) 
	// 	{
	// 		delete[]m_pImgVertexPt2d4;
	// 		m_pImgVertexPt2d4=NULL;
	// 	}
	// 	if(m_pThumbImgVertexPt2d4) 
	// 	{
	// 		delete[]m_pThumbImgVertexPt2d4;
	// 		m_pThumbImgVertexPt2d4=NULL;
	// 	}
	for(int i = 0; i<4; i++)
	{
		m_pImgVertexPt2d4[i]=Point2D();
	}
	m_nImgIdx =InvalidValue;           
	m_nStripIdx = InvalidValue;			
	m_nIdxInStrip = InvalidValue;					

	m_strImgPath.Empty();
	m_ImgCmrInfo = DYstuCmrInfo();           
	m_ImgPosInfo=DYstuPosInfo();            
	m_AdjPosInfo=DYstuPosInfo();            

	vector<int>().swap(m_ImgAdjPts.vecLinkImgIdx);
	vector<vector<int>>().swap(m_ImgAdjPts.vvAdjPtIdx4EachLinkImg);
	vector<vector<int>>().swap(m_ImgAdjPts.vvImgPtIdxInAdjPt);
	vector<vector<int>>().swap(m_ImgAdjPts.vvLinkImgPtIdxInAdjPt);

	m_bValid = true;                         
	m_bAdjed = false;  
}
void CDYUAVImage::CalRotMatrixByPOK(DYstuPosInfo& DYPosInfo)
{
	double phi = DegToRad(DYPosInfo.phi), omega = DegToRad(DYPosInfo.omg), kappa = DegToRad(DYPosInfo.kap);
	double cp = cos(phi), sp = sin(phi);
	double co = cos(omega), so = sin(omega);
	double ck = cos(kappa), sk = sin(kappa);
	DYPosInfo.R[0] = cp*ck - so*sp*sk; DYPosInfo.R[1] = -cp*sk - so*sp*ck; DYPosInfo.R[2] = -co*sp;
	DYPosInfo.R[3] = co*sk; DYPosInfo.R[4] = co*ck; DYPosInfo.R[5] = -so;
	DYPosInfo.R[6] = sp*ck + so*cp*sk; DYPosInfo.R[7] = -sp*sk + so*cp*ck; DYPosInfo.R[8] = co*cp;
}
Point2D CDYUAVImage::XYZ2Img(int nCols, int nRows, double f, Point3D &ptXYZ, bool bAdjed)
{
	DYstuPosInfo DYPosInfo; 
	if(!bAdjed)	DYPosInfo = m_ImgPosInfo;
	else DYPosInfo = m_AdjPosInfo;
	if(DYPosInfo.R[0]==InvalidValue) CalRotMatrixByPOK(DYPosInfo);
	double Xs, Ys, Zs;
	Xs = DYPosInfo.Xs;
	Ys = DYPosInfo.Ys;
	Zs = DYPosInfo.Zs;
	double _X = ptXYZ.X;
	double _Y = ptXYZ.Y;
	double _Z = ptXYZ.Z;
	double X_new = DYPosInfo.R[0] * (_X - Xs) + DYPosInfo.R[3] * (_Y - Ys) + DYPosInfo.R[6] * (_Z - Zs);
	double Y_new = DYPosInfo.R[1] * (_X - Xs) + DYPosInfo.R[4] * (_Y - Ys) + DYPosInfo.R[7] * (_Z - Zs);
	double Z_new = DYPosInfo.R[2] * (_X - Xs) + DYPosInfo.R[5] * (_Y - Ys) + DYPosInfo.R[8] * (_Z - Zs);
	double x, y;
	x = -f*X_new / Z_new+nCols/2;
	y = -f*Y_new / Z_new+nRows/2;
	Point2D	ptImg = Point2D(x, y);
	return ptImg;
}
Point3D CDYUAVImage::Img2XYZ(int nCols, int nRows,double f, Point2D &ptImg, double Z, bool bAdjed)
{
	DYstuPosInfo DYPosInfo =m_ImgPosInfo;
	if(!bAdjed) DYPosInfo = m_ImgPosInfo;
	else DYPosInfo = m_AdjPosInfo;
	if(DYPosInfo.R[0]==InvalidValue) CalRotMatrixByPOK(DYPosInfo);
	double Xs, Ys, Zs;
	Xs = DYPosInfo.Xs;
	Ys = DYPosInfo.Ys;
	Zs = DYPosInfo.Zs;
	double _px = ptImg.x-nCols/2;
	double _py = ptImg.y-nRows/2;
	double _Z = Z;
	double A1 = DYPosInfo.R[2] * _px + DYPosInfo.R[0] * f;
	double B1 = DYPosInfo.R[5] * _px + DYPosInfo.R[3] * f;
	double M1 = (DYPosInfo.R[2] * _px + DYPosInfo.R[0] * f)*Xs +
		(DYPosInfo.R[5] * _px + DYPosInfo.R[3] * f)*Ys -
		(DYPosInfo.R[8] * _px + DYPosInfo.R[6] * f)*(_Z - Zs);

	double A2 = DYPosInfo.R[2] * _py + DYPosInfo.R[1] * f;
	double B2 = DYPosInfo.R[5] * _py + DYPosInfo.R[4] * f;
	double M2 = (DYPosInfo.R[2] * _py + DYPosInfo.R[1] * f)*Xs +
		(DYPosInfo.R[5] * _py + DYPosInfo.R[4] * f)*Ys -
		(DYPosInfo.R[8] * _py +  DYPosInfo.R[7] * f)*(_Z - Zs);
	double _X = (M1*B2 - M2*B1) / (A1*B2 - A2*B1);
	double _Y = (M1*A2 - M2*A1) / (A2*B1 - A1*B2);
	Point3D ptXYZ = Point3D(_X, _Y, Z);
	return ptXYZ;
}
void CDYUAVImage::CalImg4VertexPt3d(double* pZ, bool bAdjed, bool bThumOrNot, Point3D *&pVertex3d)
{
	Point2D *pVerPtImg4 = GetImgVertexPt2d4(bThumOrNot);
	double f = m_ImgCmrInfo.f;
	for (int i = 0; i<4; i++)
	{
		pVertex3d[i] = Img2XYZ(pVerPtImg4[2].x,pVerPtImg4[1].y,f,pVerPtImg4[i],*(pZ+i),bAdjed);
	}
}
Point2D* CDYUAVImage::GetImgVertexPt2d4(bool bThumOrNot)
{
			if(m_pImgVertexPt2d4[0]==Point2D())
		{
			//		m_pImgVertexPt2d4 = new Point2D[4];

			OpenImg(m_strImgPath);
			int nCols = GetCols();
			int nRows = GetRows();
			CloseImg();
			m_pImgVertexPt2d4[0]=Point2D(0,0);
			m_pImgVertexPt2d4[1]=Point2D(0,nRows);
			m_pImgVertexPt2d4[2]=Point2D(nCols,nRows);
			m_pImgVertexPt2d4[3]=Point2D(nCols,0);

		}
		return m_pImgVertexPt2d4;
}
Point3D* CDYUAVImage::GetImgVertexPt3d4(double* pZ, bool bAdjed, bool bThumOrNot)
{
	Point3D *pVertex3d = new Point3D[4];
	CalImg4VertexPt3d( pZ, bAdjed, bThumOrNot,pVertex3d);
	return pVertex3d;
}

static bool XmlNodeJudge(pugi::xml_node& Node)
{
	if(Node==NULL)
	{
		FunOutPutLogInfo("�����ļ���������");
		return false;
	}
	return true;
}

#define ImageNumLimite 100
CDYUAVFlyQuaPrj::CDYUAVFlyQuaPrj()
{
	ClearPrjData();
}
CDYUAVFlyQuaPrj::~CDYUAVFlyQuaPrj()
{
	ClearPrjData();
}
void CDYUAVFlyQuaPrj::ClearPrjData()
{
	m_strImpXmlPath.Empty();
	m_strCmrFilePath.Empty();
		m_strBundleAdjRepPath.Empty();
	vector<CDYUAVImage>().swap(m_vecImgInfo);
	vector<CDYUAVImage>().swap(m_vecInvalidImgInfo);
	m_nImgNum = 0;
	m_nInvalidNum = 0;
	m_nInAdjedNum = 0;
	m_dRmsPosErr = 0;
	m_nStripNum = 0;
	m_nMaxStripIdx = 0;
	m_bReManageImages = true;
	m_ImgCmrInfo = DYstuCmrInfo();       //Ӱ���������
	vector<int>().swap(m_vecStripIdx);
	vector<int>().swap(m_vecImgNumInEachStrip);
	vector<vector<int>>().swap(m_vvImgIdxInEachStrip);
	vector<int>().swap(m_vecBundleImgList2ImgInfo);
	vector<CString>().swap(m_vecBundleImgName);
	ClearReportInfo(BY_ADJUSTMENT);
	ClearStripInfo();
	ClearAdjInfo();
	m_dTimeSecsWholeProcess=0;
	m_dTimeSecsTimeReadAdjRep=0;
	m_dTimeSecsRunFlyQuaChk=0;
	m_dTimeSecsWriteChkRep=0;
	m_nMatchPointNumThresold = 5;
}

bool CDYUAVFlyQuaPrj::LoadData2Prj(CString strImportDataXmlPath)
{
	if(strImportDataXmlPath.IsEmpty()||!PathFileExists(strImportDataXmlPath)) return false;
	ClearPrjData();
	FunReadSysConfigFile(m_DevExtend,m_FlyQuaParaSet);
	m_strImpXmlPath=strImportDataXmlPath;
	if(!ReadImpXml()){ClearPrjData(); return false;}
	for (int i = 0; i<m_nImgNum; i++)
	{
		m_vecImgInfo[i].m_strStripLabel=FunCreateStripLabel(m_vecImgInfo[i].m_nStripIdx,m_vecImgInfo[i].m_nIdxInStrip);
	}
	return true;
}
bool CDYUAVFlyQuaPrj::ReadImpXml()
{
	pugi::xml_document ImpXmlDoc;
	pugi::xml_parse_result ret = ImpXmlDoc.load_file(m_strImpXmlPath,pugi::parse_full,pugi::encoding_utf8);
	if(ret.status!=pugi::status_ok)
	{
		FunOutPutLogInfo("��������xml��ʧ�ܣ�");
		return false;
	}
	pugi::xml_node NodeRoot = ImpXmlDoc.child("Import_data");
	XmlNodeJudge(NodeRoot);
	CDYUAVImage DYImg;
	DYImg.m_ImgPosInfo.eFormatAngleLBH = m_DevExtend.FormatAngleLBH;
	DYImg.m_ImgPosInfo.eFormatAnglePOK = m_DevExtend.FormatAnglePOK;
	DYImg.m_ImgPosInfo.eFormatCoordinate = m_DevExtend.FormatCoor;
	DYImg.m_ImgCmrInfo = m_ImgCmrInfo;
	for (pugi::xml_node NodeImage=NodeRoot.first_child(); NodeImage!=NULL; NodeImage=NodeImage.next_sibling())
	{
		CString NodeLabel = NodeImage.name();
		if(NodeLabel =="CameraFilePath")
		{
			m_strCmrFilePath = NodeImage.text().as_string();
			vector<DYstuCmrInfo> vecCmr;
			if(!FunReadCmrFile(m_strCmrFilePath,m_DevExtend,vecCmr))
			{
				FunOutPutLogInfo("�����������");
				return false;
			}
			m_ImgCmrInfo = vecCmr[0];
			DYImg.m_ImgCmrInfo = m_ImgCmrInfo;
			vector<DYstuCmrInfo>().swap(vecCmr);

		}else
			if(NodeLabel=="AdjReportPath") m_strBundleAdjRepPath = NodeImage.text().as_string();else
				if(NodeLabel=="IfReManageImage")
				{
					CString strValue = NodeImage.text().as_string();
					if(strValue == "false") m_bReManageImages=false;
					else m_bReManageImages=true;
				}else
					if(NodeLabel=="StripNum")
					{
						m_nStripNum = NodeImage.text().as_int();
						if(m_nStripNum>0) m_vecImgNumInEachStrip.resize(m_nStripNum);
					}else
					if(NodeLabel=="Image"){
						for (pugi::xml_node Child=NodeImage.first_child(); Child!=NULL; Child=Child.next_sibling())
						{
							CString ChildLabel = Child.name();
							if(ChildLabel=="ID") DYImg.m_nImgIdx=Child.text().as_int();else
								if(ChildLabel=="Path")
								{
									DYImg.m_strImgPath=Child.text().as_string();
									DYImg.m_strImgName = FunGetFileName(DYImg.m_strImgPath,true).MakeLower();
								}else
									if(ChildLabel=="Pos")
									{
										vector<CString>vP=FunStrTok(Child.text().as_string()," ");
										if(vP.size()!=6)
										{
											FunOutPutLogInfo("Ӱ��POS��Ϣ�������󣬹����ļ���ʧ�ܣ�");
											return false;
										}
										DYImg.m_ImgPosInfo.Xs=atof(vP[0]);DYImg.m_ImgPosInfo.omg=atof(vP[3]);
										DYImg.m_ImgPosInfo.Ys=atof(vP[1]);DYImg.m_ImgPosInfo.phi=atof(vP[4]);
										DYImg.m_ImgPosInfo.Zs=atof(vP[2]);DYImg.m_ImgPosInfo.kap=atof(vP[5]);
										vector<CString>().swap(vP);
									}else
										if(ChildLabel=="StripIdx")
										{
											DYImg.m_nStripIdx=Child.text().as_uint();

										}else
											if(ChildLabel=="IdxInStrip")
											{
												DYImg.m_nIdxInStrip=Child.text().as_uint();
											}else
												if(ChildLabel=="Adjustment")
												{
													CString strValue = Child.text().as_string();
													if(strValue == "false") DYImg.m_bAdjed=false;
													else DYImg.m_bAdjed=true;
												}
						}
						m_vecImgInfo.push_back(DYImg);
						if(!DYImg.m_bValid) m_vecImgIdxInvalid.push_back(m_vecImgInfo.size()-1);
					}
					for (int i = 0; i<m_nStripNum; i++)
					{
						char strStripLabel[100];sprintf(strStripLabel,"ImgNumInStrip_%d",i);
						if(NodeLabel==strStripLabel)
						{
							m_vecImgNumInEachStrip[i] = NodeImage.text().as_int();
						}
					}
	}
	m_nImgNum = m_vecImgInfo.size();
	if(m_nStripNum>0)
	{
		m_vvImgIdxInEachStrip.resize(m_nStripNum);	
		m_vecStripIdx.resize(m_nStripNum);
		m_nMaxStripIdx=m_nStripNum;
		for (int i = 0; i<m_nStripNum; i++)
		{
			m_vvImgIdxInEachStrip[i].resize(m_vecImgNumInEachStrip[i]);
			m_vecStripIdx[i]=i;
		}

#pragma omp parallel for
		for (int i = 0; i<m_vecImgInfo.size(); i++)
		{
			if(!m_vecImgInfo[i].m_bValid||m_vecImgInfo[i].m_nStripIdx==InvalidValue) continue;
			int nStriIdx = m_vecImgInfo[i].m_nStripIdx;
			int nIdxInStrip= m_vecImgInfo[i].m_nIdxInStrip;
			m_vvImgIdxInEachStrip[nStriIdx][nIdxInStrip]=m_vecImgInfo[i].m_nImgIdx;
		}
	}
	return true;
}

void CDYUAVFlyQuaPrj::CalBundleImgList2ImgInfo()
{
	m_vecBundleImgList2ImgInfo.resize(m_vecBundleImgName.size());
#pragma omp parallel for
	for (int i = 0; i<m_vecBundleImgName.size(); i++)
	{
		for(int j = 0; j<m_vecImgInfo.size(); j++)
		{
			if(m_vecBundleImgName[i]==m_vecImgInfo[j].m_strImgName)
			{
				m_vecBundleImgList2ImgInfo[i] = j;
			}
		}
	}
}

void CDYUAVFlyQuaPrj::CalImagePitch()
{
	if(m_nImgNum<=0) return;
	m_vecImagePitch.assign(m_nImgNum,InvalidValue);
#pragma omp parallel for
	for(int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<=i) continue;
		m_vecImagePitch[i]=max(m_vecImgInfo[i].m_ImgPosInfo.phi,m_vecImgInfo[i].m_ImgPosInfo.omg);
	}
}
void CDYUAVFlyQuaPrj::CalImageYaw(double Z, DYFLYQUAMETHOD eMethod)
{
	m_vvStripImageYaw.resize(m_nStripNum);
	//#pragma omp parallel for
	for (int i = 0; i<m_nStripNum; i++)
	{
		m_vvStripImageYaw[i].assign(m_vecImgNumInEachStrip[i],0);
		for (int j = 1; j<m_vecImgNumInEachStrip[i]; j++)
		{
			int nImgIdx1 = m_vvImgIdxInEachStrip[i][j-1];
			int nImgIdx2 = m_vvImgIdxInEachStrip[i][j];
			if(m_vecImgInfo.size()<nImgIdx1||m_vecImgInfo.size()<nImgIdx2) break;
			int nCount = 0; double dSum = 0;
			vector<double> vecYaw;
			switch(eMethod)
			{
			case BY_POS:{
				//Point3D pVertexPt3d[4]; 
				double pZ[4]; memset(pZ,Z,4*sizeof(double));
				Point3D *pVertexPt3d=	m_vecImgInfo[nImgIdx1].GetImgVertexPt3d4(pZ,false,false);
				Point2D *pVertexImg4 = m_vecImgInfo[nImgIdx1].GetImgVertexPt2d4(false);
				Point2D pVertexPt2d1[4],pVertexPt2d2[4];
				for (int ii = 0; ii<4; ii++)
				{
					pVertexPt2d1[ii]=m_vecImgInfo[nImgIdx1].XYZ2Img(pVertexImg4[2].x,pVertexImg4[1].y,m_vecImgInfo[nImgIdx1].m_ImgCmrInfo.f,pVertexPt3d[ii],false);
					pVertexPt2d2[ii]=m_vecImgInfo[nImgIdx2].XYZ2Img(pVertexImg4[2].x,pVertexImg4[1].y,m_vecImgInfo[nImgIdx2].m_ImgCmrInfo.f,pVertexPt3d[ii],false);
				}
				for (int ii = 0; ii<3; ii++)
				{
					for (int jj = ii+1; jj<4; jj++)
					{
						//					if(m_vecImgInfo.size()<=i) return;
						Point2D pt11 = pVertexPt2d1[ii];
						Point2D pt12 = pVertexPt2d1[jj];
						Point2D pt21 = pVertexPt2d2[ii];
						Point2D pt22 = pVertexPt2d2[jj];
						double a1 = atan((pt12.y-pt11.y)/(pt12.x-pt11.x));
						double a2 = atan((pt22.y-pt21.y)/(pt22.x-pt21.x));
						double da = RadToDeg(a1)-RadToDeg(a2);
						dSum+=da;
						nCount++;
					}
				}
						}
						break;
			case BY_MATCH:
				for (int k = 0; k<m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vecLinkImgIdx.size(); k++)
				{
					if(nImgIdx1==m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vecLinkImgIdx[k])
					{
						for (int p = 0; p<m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k].size()-1; p+=2)
						{
							//				if(m_vecImgInfo.size()<=k) return;
							int nAdjId1 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k][p];
							int nAdjId2 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k][p+1];
							int nImgPtIdx11 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvImgPtIdxInAdjPt[k][p];
							int nImgPtIdx12 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvImgPtIdxInAdjPt[k][p+1];
							int nImgPtIdx21 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k][p];
							int nImgPtIdx22 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k][p+1];
							Point2D pt11 = m_vecAdjPts[nAdjId1].vecImgPt[nImgPtIdx11];
							Point2D pt12 = m_vecAdjPts[nAdjId2].vecImgPt[nImgPtIdx12];
							Point2D pt21 = m_vecAdjPts[nAdjId1].vecImgPt[nImgPtIdx21];
							Point2D pt22 = m_vecAdjPts[nAdjId2].vecImgPt[nImgPtIdx22];
							// 							double a1 = atan((pt12.y-pt11.y)/(pt12.x-pt11.x));
							// 							double a2 = atan((pt22.y-pt21.y)/(pt22.x-pt21.x));
							double a1,a2;
							if(pt12.x!=pt11.x) a1 = atan((pt12.y-pt11.y)/(pt12.x-pt11.x));
							else a1 = 0;
							if(pt22.x!=pt21.x) a2 = atan((pt22.y-pt21.y)/(pt22.x-pt21.x));
							else a2 = 0;
							double da = RadToDeg(a1)-RadToDeg(a2);
							dSum+=da;
							vecYaw.push_back(da);
							nCount++;
						}
						break;
					}
				}
				break;
			case BY_ADJUSTMENT:
				for (int k = 0; k<m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vecLinkImgIdx.size(); k++)
				{
					if(nImgIdx1==m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vecLinkImgIdx[k])
					{
						for (int p = 0; p<m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k].size()-1; p+=2)
						{
							int nAdjId1 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k][p];
							int nAdjId2 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[k][p+1];
							int nImgPtIdx11 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvImgPtIdxInAdjPt[k][p];
							int nImgPtIdx12 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvImgPtIdxInAdjPt[k][p+1];
							int nImgPtIdx21 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k][p];
							int nImgPtIdx22 = m_vecImgInfo[nImgIdx2].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[k][p+1];
							Point2D pt11 = m_vecAdjPts[nAdjId1].vecImgPt[nImgPtIdx11];
							Point2D pt12 = m_vecAdjPts[nAdjId2].vecImgPt[nImgPtIdx12];
							Point2D pt21 = m_vecAdjPts[nAdjId1].vecImgPt[nImgPtIdx21];
							Point2D pt22 = m_vecAdjPts[nAdjId2].vecImgPt[nImgPtIdx22];
							double a1,a2;
							if(pt12.x!=pt11.x) a1 = atan((pt12.y-pt11.y)/(pt12.x-pt11.x));
							else a1 = 0;
							if(pt22.x!=pt21.x) a2 = atan((pt22.y-pt21.y)/(pt22.x-pt21.x));
							else a2 = 0;
							double da = RadToDeg(a1)-RadToDeg(a2);
							dSum+=da;
							nCount++;
						}
						break;
					}
				}
				break;
			default:
				break;
			}
			if(nCount>0) m_vvStripImageYaw[i][j]=dSum/nCount;
			vector<double>().swap(vecYaw);
		}
	}

}
void CDYUAVFlyQuaPrj::CalImageFlyHeiErr(DYFLYQUAMETHOD eMethod)
{
	if(m_nImgNum<=0||m_nStripNum<=0) return;
	m_vvStripImageFlyHeiErr.resize(m_nStripNum);
	//	m_vecStripImageFlyHeiMaxErr.resize(m_nStripNum);

#pragma omp parallel for
	for (int i= 0; i<m_nStripNum; i++)
	{
		if(m_vvStripImageFlyHeiErr.size()<i)continue;
		m_vvStripImageFlyHeiErr[i].assign(m_vecImgNumInEachStrip[i],0);
		for (int j = 1; j<m_vecImgNumInEachStrip[i]; j++)
		{
			if(m_vvStripImageFlyHeiErr.size()<i)continue;
			if(m_vecImgNumInEachStrip.size()<i) continue;
			int nImgIdx1 = m_vvImgIdxInEachStrip[i][j-1];
			int nImgIdx2 = m_vvImgIdxInEachStrip[i][j];
			DYstuPosInfo DYPosInfo1, DYPosInfo2;
			if (m_vecImgInfo.size()<nImgIdx1||m_vecImgInfo.size()<nImgIdx2) 
			{
				continue;
			}
			if(eMethod==BY_POS||eMethod==BY_MATCH)
			{
				DYPosInfo1=m_vecImgInfo[nImgIdx1].m_ImgPosInfo;
				DYPosInfo2=m_vecImgInfo[nImgIdx2].m_ImgPosInfo;
			}
			else
			{
				DYPosInfo1=m_vecImgInfo[nImgIdx1].m_AdjPosInfo;
				DYPosInfo2=m_vecImgInfo[nImgIdx2].m_AdjPosInfo;
			}
			m_vvStripImageFlyHeiErr[i][j]=DYPosInfo2.Zs-DYPosInfo1.Zs;

		}
		//	m_vecStripImageFlyHeiMaxErr[i]=dMaxHei-dMinHei;
	}
	double dMaxHei = -1e10, dMinHei = 1e10;
	for (int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<=i) break;
		if(!m_vecImgInfo[i].m_bValid) continue;
		DYstuPosInfo DYPosInfo1;
		if(eMethod==BY_POS||eMethod==BY_MATCH)
		{
			DYPosInfo1=m_vecImgInfo[i].m_ImgPosInfo;
		}
		else
		{
			DYPosInfo1=m_vecImgInfo[i].m_AdjPosInfo;
		}
		dMaxHei=max(dMaxHei,DYPosInfo1.Zs);
		dMinHei=min(dMinHei,DYPosInfo1.Zs);
	}
	m_dMaxHeiErr = dMaxHei-dMinHei;
}
void CDYUAVFlyQuaPrj::CalImageFlyDisErr(DYFLYQUAMETHOD eMethod)
{
	if(m_nImgNum<=0||m_nStripNum<=0) return;
	m_vvStripImageFlyDisErr.resize(m_nStripNum);
	m_vecStripBlendPara.assign(m_nStripNum,InvalidValue);
#pragma omp parallel for
	for (int i = 0; i<m_nStripNum; i++)
	{	
		if(m_vecImgInfo.size()<i) continue;
		double dMaxDis = InvalidValue;
		m_vvStripImageFlyDisErr[i].assign(m_vecImgNumInEachStrip[i],InvalidValue);
		double strip_a,strip_b,strip_c;
		vector<Point2D> vPt2d; vPt2d.resize(m_vecImgNumInEachStrip[i]);
		for (int j = 0; j<m_vecImgNumInEachStrip[i]; j++)
		{
			if(m_vecImgInfo.size()<=i) break;
			Point2D pt2d;
			int nImgIdx = m_vvImgIdxInEachStrip[i][j];
			DYstuPosInfo DYPosInfo;
			if(eMethod==BY_POS||eMethod==BY_MATCH)
			{
				DYPosInfo=m_vecImgInfo[nImgIdx].m_ImgPosInfo;
			}
			else
			{
				DYPosInfo=m_vecImgInfo[nImgIdx].m_AdjPosInfo;
			}
			pt2d.x = DYPosInfo.Xs;
			pt2d.y = DYPosInfo.Ys;
			vPt2d[j]=pt2d;
		}
		FunCalLineParasByLsm(vPt2d,strip_a,strip_b,strip_c);
		double M = sqrt(pow(strip_a,2)+pow(strip_b,2)+pow(strip_c,2));
		for (int j = 0; j<m_vecImgNumInEachStrip[i]; j++)
		{
			double dis = fabs(strip_a*vPt2d[j].x+strip_b*vPt2d[j].y+strip_c)/M;
			m_vvStripImageFlyDisErr[i][j]=dis;
			dMaxDis = max(dMaxDis,dis);
		}
		double dStripLen = sqrt(pow(vPt2d[0].x-vPt2d[vPt2d.size()-1].x,2)+pow(vPt2d[0].y-vPt2d[vPt2d.size()-1].y,2));
		m_vecStripBlendPara[i]=dMaxDis/dStripLen;
	}
	cprintf("========Blend of strips========\n");
	for (int i = 0; i<m_vecStripBlendPara.size(); i++)
	{
		CString strInfo;
		strInfo.Format("Strip_%d��%.2lf%%\n",i,m_vecStripBlendPara[i]*100);
		cprintf(strInfo);
	}
}
bool CDYUAVFlyQuaPrj::CalImagePosErr(DYFLYQUAMETHOD eMethod /* = BY_ADJUSTMENT */)
{
	if(eMethod!=BY_ADJUSTMENT)
	{
		m_vecAdjImgPosDisErr.assign(m_nImgNum-m_nInvalidNum,0);
		return false;
	}
	m_vecAdjImgPosErr.resize(m_nInAdjedNum);
	m_vecAdjImgPosDisErr.resize(m_nInAdjedNum);
#pragma omp parallel for
	for (int i = 0; i<m_nInAdjedNum; i++)
	{
		int nImgIdx = m_vecBundleSuccess2ImgInfo[i];
		if(!m_vecImgInfo[nImgIdx].m_bValid) continue;
		m_vecAdjImgPosErr[i] = m_vecImgInfo[nImgIdx].m_ImgPosInfo-m_vecImgInfo[nImgIdx].m_AdjPosInfo;
		m_vecAdjImgPosErr[i].strLabel=m_vecImgInfo[nImgIdx].m_ImgPosInfo.strLabel;
		m_vecAdjImgPosDisErr[i]=sqrt(pow(m_vecAdjImgPosErr[i].Xs,2)+pow(m_vecAdjImgPosErr[i].Ys,2)+pow(m_vecAdjImgPosErr[i].Zs,2));
	}
	return true;
}
void CDYUAVFlyQuaPrj::CalStripCqPaneErr(DYFLYQUAMETHOD eMethod /* = BY_ADJUSTMENT */)
{
	//////////////////////////////////////////////////
	///�����
	//m_vecStripPanePara;
	vector<Point3D> vecPt3d;
	double dSumX=0,dSumY=0,dSumZ=0;
	for (int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<i) return;
		if(!m_vecImgInfo[i].m_bValid) continue;
		Point3D pt3d;
		if(eMethod==BY_ADJUSTMENT&&m_vecImgInfo[i].m_bAdjed)
		{
			pt3d.X=m_vecImgInfo[i].m_AdjPosInfo.Xs;
			pt3d.Y=m_vecImgInfo[i].m_AdjPosInfo.Ys;
			pt3d.Z=m_vecImgInfo[i].m_AdjPosInfo.Zs;
		}
		else
		{
			pt3d.X=m_vecImgInfo[i].m_ImgPosInfo.Xs;
			pt3d.Y=m_vecImgInfo[i].m_ImgPosInfo.Ys;
			pt3d.Z=m_vecImgInfo[i].m_ImgPosInfo.Zs;
		}
		dSumX+=pt3d.X;
		dSumY+=pt3d.Y;
		dSumZ+=pt3d.Z;
		vecPt3d.push_back(pt3d);
	}
	if(vecPt3d.size()<3) return;
	for (int i = 0; i<vecPt3d.size(); i++)
	{
		vecPt3d[i].X-=dSumX/vecPt3d.size();
		vecPt3d[i].Y-=dSumY/vecPt3d.size();
		vecPt3d[i].Z-=dSumZ/vecPt3d.size();
	}
	double a,b,c,d;
	FunCalPaneParasBylsm(vecPt3d,a,b,c,d);
	/*vector<double> vecDis2Pane(vecPt3d.size(), 0);*/
	m_vecDisImgAdjPos2PosPane.assign(vecPt3d.size(),0);
	double dSum = 0, dMean = 0, dRms = 0;
	double dMax = -1e10; 
	double M = sqrt(pow(a,2)+pow(b,2)+pow(c,2)+pow(d,2));
	for (int i = 0; i<vecPt3d.size(); i++)
	{
		double dis = /*fabs*/(a*vecPt3d[i].X+b*vecPt3d[i].Y+c*vecPt3d[i].Z+c)/M;
		dSum+=fabs(dis);
		dMax=max(dMax,fabs(dis));
		m_vecDisImgAdjPos2PosPane[i]=dis;
	}
	if(vecPt3d.size()>0) dMean = dSum/vecPt3d.size();
	dRms = FunCalDataRms(m_vecDisImgAdjPos2PosPane,dMean);
	m_dMaxPaneDifErr = dMax;
	m_dRmsPaneDifErr = dRms;
	cprintf("========Pane info of area========\n");
	cprintf("MaxDis  = %.2lfm\n",dMax);
	cprintf("MeanDis = %.2lfm\n",dMean);
	cprintf("RmsDis  = %.2lfm\n",dRms);

	vector<Point3D>().swap(vecPt3d);
}


double CDYUAVFlyQuaPrj::CalOverlapsByPos(int nIdx1, int nIdx2, double Z, DYOVERLAPDIRECTION eDirection)
{
	double dOverlap = 0;
	if(nIdx1>=m_nImgNum||nIdx2>=m_nImgNum||nIdx1<0||nIdx2<0) return dOverlap;
	if(!m_vecImgInfo[nIdx1].m_bValid||!m_vecImgInfo[nIdx2].m_bValid) return dOverlap;
	double pZ[4]; memset(pZ,Z,4*sizeof(double));
	Point3D* pVerPt4_1 = m_vecImgInfo[nIdx1].GetImgVertexPt3d4(pZ,false,false);
	Point3D* pVerPt4_2 = m_vecImgInfo[nIdx2].GetImgVertexPt3d4(pZ,false,false);
	OGRPolygon *pPolygon1 = NULL, *pPolygon2 = NULL;
	pPolygon1 = FunCreatePolygon(pVerPt4_1,3,4);
	if(pPolygon1==NULL) return dOverlap;
	pPolygon2=FunCreatePolygon(pVerPt4_2,3,4);
	if(pPolygon2==NULL)
	{
		OGRGeometryFactory::destroyGeometry(pPolygon1);
		return dOverlap;
	}
	Point2D *pInterVetexPt=NULL; int nPtNum = 0;
	pInterVetexPt=FunPolygonIntersect(pPolygon1,pPolygon2,nPtNum);
	if(pInterVetexPt==NULL) return dOverlap;
	//������Χ��img2 Ͷ��img1	
	m_vecImgInfo[nIdx1].OpenImg(m_vecImgInfo[nIdx1].m_strImgPath);
	double dFullLenX = 0, dFullLenY=0;
	dFullLenX = m_vecImgInfo[nIdx1].GetCols();
	dFullLenY = m_vecImgInfo[nIdx1].GetRows();
	for (int i = 0; i<nPtNum; i++)
	{
		if(m_vecImgInfo.size()<=i) return 0;
		Point3D pt3d = Point3D(pInterVetexPt[i].x,pInterVetexPt[i].y,Z);
		pInterVetexPt[i] = m_vecImgInfo[nIdx1].XYZ2Img(dFullLenX,dFullLenY,m_vecImgInfo[nIdx1].m_ImgCmrInfo.f,pt3d,false);
	}
	m_vecImgInfo[nIdx1].CloseImg();
	if(eDirection==DIRECTION_Y)
	{
		if(CalOverlapByInterVertex(pInterVetexPt,nPtNum,dFullLenX,DIRECTION_X)>0.6)
			dOverlap = CalOverlapByInterVertex(pInterVetexPt, nPtNum,dFullLenY,DIRECTION_Y);
	}
	else if(eDirection==DIRECTION_X)
	{
		if(CalOverlapByInterVertex(pInterVetexPt,nPtNum,dFullLenY,DIRECTION_Y)>0.3)
			dOverlap = CalOverlapByInterVertex(pInterVetexPt, nPtNum,dFullLenX,DIRECTION_X);
	}
	dOverlap = min(1,dOverlap);
	dOverlap = max(0,dOverlap);
	return dOverlap;
}
double CDYUAVFlyQuaPrj::CalOverlapsByMatch(int nIdx1, int nIdx2, DYOVERLAPDIRECTION eDirection)
{
	double dOverlap = 0;
	if(nIdx1>=m_nImgNum||nIdx2>=m_nImgNum||nIdx1<0||nIdx2<0) return dOverlap;
	if(!m_vecImgInfo[nIdx1].m_bValid||!m_vecImgInfo[nIdx2].m_bValid) return dOverlap;
	vector<Point2D> vecPt2d1,vecPt2d2;
	for (int i = 0; i<m_vecImgInfo[nIdx1].m_ImgAdjPts.vecLinkImgIdx.size(); i++)
	{
		int nLinImgIdx = m_vecImgInfo[m_vecImgInfo[nIdx1].m_ImgAdjPts.vecLinkImgIdx[i]].m_nImgIdx;
		if(nLinImgIdx==nIdx2)
		{
			for (int j = 0; j<m_vecImgInfo[nIdx1].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[i].size(); j++)
			{	
				if(m_vecImgInfo.size()<=i) return 0;
				int nAdjIdx, nImgPtIdx1, nImgPtIdx2;
				nAdjIdx=m_vecImgInfo[nIdx1].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg[i][j];
				nImgPtIdx1 = m_vecImgInfo[nIdx1].m_ImgAdjPts.vvImgPtIdxInAdjPt[i][j];
				nImgPtIdx2 = m_vecImgInfo[nIdx1].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt[i][j];
				vecPt2d1.push_back(m_vecAdjPts[nAdjIdx].vecImgPt[nImgPtIdx1]);
				vecPt2d2.push_back(m_vecAdjPts[nAdjIdx].vecImgPt[nImgPtIdx2]);
			}
			break;
		}
	}
	if(vecPt2d1.size()<3) 
	{
		//		return CalOverlapsByPos(nIdx1,nIdx2,m_dMeanHeiCq,eDirection);
		return dOverlap;
	}
	double *CoePara6 = new double[6]; memset(CoePara6,InvalidValue,6*sizeof(double));
	FunCalTopoParasByLsm(vecPt2d2,vecPt2d1,&CoePara6);
	Point2D pImgVertexPt2To1[4];
	Point2D* pImgVertexPt2 = m_vecImgInfo[nIdx2].GetImgVertexPt2d4(true);
	for (int i = 0; i<4; i++)
	{
		FunImgPtTopoTrans(pImgVertexPt2[i],CoePara6,pImgVertexPt2To1[i]);
	}
	double dFullLenX = 0, dFullLenY=0;
	m_vecImgInfo[nIdx1].OpenImg(m_vecImgInfo[nIdx1].m_strImgPath);
	dFullLenX = m_vecImgInfo[nIdx1].GetCols();
	dFullLenY = m_vecImgInfo[nIdx1].GetRows();
	m_vecImgInfo[nIdx1].CloseImg();
	if(eDirection==DIRECTION_Y)
	{
		if(CalOverlapByInterVertex(pImgVertexPt2To1,4,dFullLenX,DIRECTION_X)>0.6)
			dOverlap = CalOverlapByInterVertex(pImgVertexPt2To1,4,dFullLenY,DIRECTION_Y);
	}
	else if(eDirection==DIRECTION_X)
	{
		if(CalOverlapByInterVertex(pImgVertexPt2To1,4,dFullLenY,DIRECTION_Y)>0.3)
			dOverlap = CalOverlapByInterVertex(pImgVertexPt2To1,4,dFullLenX,DIRECTION_X);
	}
	dOverlap = min(1,dOverlap);
	dOverlap = max(0,dOverlap);
	delete[]CoePara6;
	vector<Point2D>().swap(vecPt2d1);
	vector<Point2D>().swap(vecPt2d2);
	return dOverlap;
}
double CDYUAVFlyQuaPrj::CalOverlapsByAdjustment(int nIdx1, int nIdx2, DYOVERLAPDIRECTION eDirection)
{
	double dOverlap = 0;
	if(nIdx1>=m_nImgNum||nIdx2>=m_nImgNum||nIdx1<0||nIdx2<0) return dOverlap;
	if(!m_vecImgInfo[nIdx1].m_bValid||!m_vecImgInfo[nIdx2].m_bValid) return dOverlap;
	//	Point3D pVerPt4_1[4],pVerPt4_2[4];

	double pZ[4]; memset(pZ,0,4*sizeof(double));
	Point2D *pImgVertexPt = m_vecImgInfo[nIdx1].GetImgVertexPt2d4(true);
	for (int i = 0; i<4; i++)
	{
		FunFitHei4ImgAdjPts4ImgPt(m_vecAdjPts,m_dMeanHeiCq,m_vecImgInfo[nIdx1].m_ImgAdjPts,pImgVertexPt[i],pZ[i]);
	}
	//m_vecImgInfo[nIdx1].CalImg4VertexPt3d(pZ,true,pVerPt4_1,true);
	Point3D* pVerPt4_1 = m_vecImgInfo[nIdx1].GetImgVertexPt3d4(pZ,true,true);
	pImgVertexPt = m_vecImgInfo[nIdx2].GetImgVertexPt2d4(true);
	for (int i = 0; i<4; i++)
	{
		FunFitHei4ImgAdjPts4ImgPt(m_vecAdjPts,m_dMeanHeiCq,m_vecImgInfo[nIdx2].m_ImgAdjPts,pImgVertexPt[i],pZ[i]);
	}
	//	m_vecImgInfo[nIdx2].CalImg4VertexPt3d(pZ,true,pVerPt4_2,true);
	Point3D* pVerPt4_2 = m_vecImgInfo[nIdx1].GetImgVertexPt3d4(pZ,true,true);
	OGRPolygon *pPolygon1 = NULL, *pPolygon2 = NULL;
	pPolygon1 = FunCreatePolygon(pVerPt4_1,3,4);
	if(pPolygon1==NULL) return dOverlap;
	pPolygon2=FunCreatePolygon(pVerPt4_2,3,4);
	if(pPolygon2==NULL)
	{
		OGRGeometryFactory::destroyGeometry(pPolygon1);
		return dOverlap;
	}
	Point2D *pInterVetexPt=NULL; int nPtNum = 0;
	pInterVetexPt=FunPolygonIntersect(pPolygon1,pPolygon2,nPtNum);
	if(pInterVetexPt==NULL) return dOverlap;
	//������Χ��img2 Ͷ��img1��
	double dFullLenX = 0, dFullLenY=0;
	m_vecImgInfo[nIdx1].OpenImg(m_vecImgInfo[nIdx1].m_strImgPath);
	dFullLenX = m_vecImgInfo[nIdx1].GetCols();
	dFullLenY = m_vecImgInfo[nIdx1].GetRows();
	for (int i = 0; i<nPtNum; i++)
	{
		double Z = 0;
		FunFitHei4ImgAdjPts4ObjPt(m_vecAdjPts,m_dMeanHeiCq,m_vecImgInfo[nIdx1].m_ImgAdjPts,pInterVetexPt[i],Z);
		Point3D pt3d = Point3D(pInterVetexPt[i].x,pInterVetexPt[i].y,Z);
		pInterVetexPt[i] = m_vecImgInfo[nIdx1].XYZ2Img(dFullLenX,dFullLenY,m_vecImgInfo[nIdx1].m_ImgCmrInfo.f,pt3d,true);
	}
	m_vecImgInfo[nIdx1].CloseImg();

	if(eDirection==DIRECTION_Y)
	{
		if(CalOverlapByInterVertex(pInterVetexPt,nPtNum,dFullLenX,DIRECTION_X)>0.6)
			dOverlap = CalOverlapByInterVertex(pInterVetexPt, nPtNum,dFullLenY,DIRECTION_Y);
	}
	else if(eDirection==DIRECTION_X)
	{
		if(CalOverlapByInterVertex(pInterVetexPt,nPtNum,dFullLenY,DIRECTION_Y)>0.3)
			dOverlap = CalOverlapByInterVertex(pInterVetexPt, nPtNum,dFullLenX,DIRECTION_X);
	}
	dOverlap = min(1,dOverlap);
	dOverlap = max(0,dOverlap);
	return dOverlap;
}
double CDYUAVFlyQuaPrj::CalOverlapByInterVertex(Point2D *pVertexPts, int nPtNum, double dFullLen, DYOVERLAPDIRECTION eDirection)
{
	double dOverlap = 0;
	if(nPtNum<3) return dOverlap;
	if(dFullLen<=0) return dOverlap;
	switch(eDirection)
	{
	case DIRECTION_X:{
		for (int i = 0; i<nPtNum-1; i++)
		{
			for (int j = i; j<nPtNum; j++)
			{
				if(pVertexPts[i].x>pVertexPts[j].x)	
				{
					Point2D ptTmp = pVertexPts[i];
					pVertexPts[i]=pVertexPts[j];
					pVertexPts[j]=ptTmp;
				}
			}
		}
		double dMaxX1, dMaxX2, dMinX1, dMinX2;
		if(nPtNum >= 4)
		{
			dMaxX1 = min(dFullLen,pVertexPts[nPtNum-1].x);
			dMaxX2 = min(dFullLen,pVertexPts[nPtNum-2].x);
			dMinX1 = max(0,pVertexPts[0].x);
			dMinX2 = max(0,pVertexPts[1].x);
		}
		else if(nPtNum == 3)
		{
			dMaxX1 = min(dFullLen,pVertexPts[2].x);
			dMaxX2 = min(dFullLen,pVertexPts[2].x);
			dMinX1 = max(0,pVertexPts[0].x);
			dMinX2 = max(0,pVertexPts[1].x);
		}
		dOverlap = (dMaxX1+dMaxX2-dMinX1-dMinX2)/dFullLen/2;
					 }
					 break;
	case DIRECTION_Y:{
		for (int i = 0; i<nPtNum-1; i++)
		{
			for (int j = i; j<nPtNum; j++)
			{
				if(pVertexPts[i].y>pVertexPts[j].y)	
				{
					Point2D ptTmp = pVertexPts[i];
					pVertexPts[i]=pVertexPts[j];
					pVertexPts[j]=ptTmp;
				}
			}
		}
		double dMaxY1, dMaxY2, dMinY1, dMinY2;
		if(nPtNum >= 4)
		{	
			dMaxY1 = min(dFullLen,pVertexPts[nPtNum-1].y);
			dMaxY2 = min(dFullLen,pVertexPts[nPtNum-2].y);
			dMinY1 = max(0,pVertexPts[0].y);
			dMinY2 = max(0,pVertexPts[1].y);

		}
		else if(nPtNum == 3)
		{
			dMaxY1 = min(dFullLen,pVertexPts[2].y);
			dMaxY2 = min(dFullLen,pVertexPts[2].y);
			dMinY1 = max(0,pVertexPts[0].y);
			dMinY2 = max(0,pVertexPts[1].y);
		}
		dOverlap = (dMaxY1+dMaxY2-dMinY1-dMinY2)/dFullLen/2;
					 }
					 break;
	default:
		break;
	}
	return dOverlap;
}
void CDYUAVFlyQuaPrj::AnalysisFlyQuaChkParas(int nMatchPtNumThresold)
{
	m_vecImgNumInEachStripYaw20.assign(m_nStripNum,0);
	double dMinHei=1e10, dMaxHei=-1e10;
	vector<bool> vecPassedbool;
	vecPassedbool.assign(m_nImgNum,true);
	for (int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<=i) return;
		if(!m_vecImgInfo[i].m_bValid)
		{
			vecPassedbool[i]=false;
			continue;
		}
		int nStripIdx = m_vecImgInfo[i].m_nStripIdx;
		int nIdxInStrip = m_vecImgInfo[i].m_nIdxInStrip;
		int nImgIdx = m_vecImgInfo[i].m_nImgIdx;
		//�����ص��Ȳ��ϸ�Ӱ��
		if(m_vvOverlapsInsStrips[nStripIdx][nIdxInStrip]*100<m_FlyQuaParaSet.fMinOverlapY)
		{
			m_vecImgIdxLowOverlapInsStrips.push_back(nImgIdx);
		}
		//�����ص��Ȳ��ϸ�Ӱ��
		if(m_vvOverlapsBetStrips[nStripIdx][nIdxInStrip]*100<m_FlyQuaParaSet.fMinOverlapX)
		{
			m_vecImgIdxLowOverlapBetStrips.push_back(nImgIdx);
		}
		//��ǲ��ϸ�Ӱ��
		if(m_vecImagePitch[nImgIdx]>m_FlyQuaParaSet.fMaxPitch)
		{
			m_vecImgIdxHeiPitch.push_back(nImgIdx);
			vecPassedbool[nImgIdx]=false;
		}
		if(m_vecImagePitch[nImgIdx]>10)
		{
			m_nImgNumPitch10++;
		}
		//���ǲ��ϸ�Ӱ��
		if(fabs(m_vvStripImageYaw[nStripIdx][nIdxInStrip])>m_FlyQuaParaSet.fMaxYaw)
		{
			m_vecImgIdxHeiYaw.push_back(nImgIdx);
		}
		if(fabs(m_vvStripImageYaw[nStripIdx][nIdxInStrip])>15)
		{
			m_nImgNumYaw15++;
		}
		if(fabs(m_vvStripImageYaw[nStripIdx][nIdxInStrip])>20)
		{
			m_vecImgNumInEachStripYaw20[nStripIdx]++;
		}
		//���и߶Ȳ��ϸ�Ӱ��
		if(m_vvStripImageFlyHeiErr[nStripIdx][nIdxInStrip]>m_FlyQuaParaSet.fMaxHeiDifLinkInStrip)
		{
			m_vecImgIdxHeiHeiErr.push_back(nImgIdx);
		}
		//����ƥ������Ӱ��
		if(m_vecImgInfo[nImgIdx].m_ImgAdjPts.vecAdjPtIdx.size()<nMatchPtNumThresold)
		{
			m_vecImgIdxLowMatchPointNum.push_back(nImgIdx);
		}
	}
	//POS���ϸ�Ӱ��
	if(m_vecAdjImgPosErr.size()>0)
	{
		int num = 0; double dSum = 0, dMean = 0;
		for (int i = 0; i<m_vecAdjImgPosErr.size(); i++)
		{
			//			if(!m_vecImgInfo[i].m_bValid||!m_vecImgInfo[i].m_bAdjed) continue;
			dSum+=m_vecAdjImgPosDisErr[i];
			num++;
		}
		dMean=dSum/num; dSum=0; /*m_nInAdjedNum = num;*/
		for (int i = 0; i<m_vecAdjImgPosErr.size(); i++)
		{
			//		if(!m_vecImgInfo[i].m_bValid||!m_vecImgInfo[i].m_bAdjed) continue;
			dSum+=pow(m_vecAdjImgPosDisErr[i]-dMean,2);
		}
		m_dRmsPosErr = sqrt(dSum/num);
		m_dMeanPosErr = dMean;
		for (int i = 0; i<m_vecAdjImgPosErr.size(); i++)
		{
			if(fabs(m_vecAdjImgPosDisErr[i]-dMean)>3*m_dRmsPosErr)
			{
				m_vecImgIdxHeiPosErr.push_back(m_vecBundleSuccess2ImgInfo[i]);
				m_vecBundleIdxHeiPosErr.push_back(i);
				vecPassedbool[m_vecBundleSuccess2ImgInfo[i]]=false;
			}
		}
	}
	//�ʼ�ͨ��Ӱ��
	for (int i = 0; i<vecPassedbool.size(); i++)
	{
		if(vecPassedbool[i] == true)
		{
			m_vecImgIdxPassCheck.push_back(i);
		}
	}
	vector<bool>().swap(vecPassedbool);
}
void CDYUAVFlyQuaPrj::CheckMissArea(double Z, DYFLYQUAMETHOD eMethod)
{
	vector<vector<Point3D>>().swap(m_vvMissAreaVertex);
	vector<vector<int>>().swap(m_vvMissAreaImgIdx);
	int nInerRingNum = 0;
	OGRGeometry *pGeoCombine = NULL;
	OGRPolygon *pPolygon1 = NULL;
	OGRLinearRing *pRing = NULL;
	double pZ[4]; for(int i = 0; i<4; i++) pZ[i]=Z;

	m_vvImgVertex3d.resize(m_nImgNum);
	int nCount = 0;
	switch(eMethod)
	{
	case BY_ADJUSTMENT:
		for (int i = 0; i<m_nImgNum; i++)
		{
			if(m_vecImgInfo.size()<=i) return;
			if(!m_vecImgInfo[i].m_bValid) continue;
			memset(pZ,0,4*sizeof(double));
			Point2D *pImgVertexPt = m_vecImgInfo[i].GetImgVertexPt2d4(true);
			double dSum = 0;
			for (int j = 0; j<4; j++)
			{
				FunFitHei4ImgAdjPts4ImgPt(m_vecAdjPts,m_dMeanHeiCq,m_vecImgInfo[i].m_ImgAdjPts,pImgVertexPt[j],pZ[j]);
				dSum+=pZ[j];
			}
			Point3D* pVerPt4=m_vecImgInfo[i].GetImgVertexPt3d4(pZ,true,true);
			FunTransPolygonVertexOrder(pVerPt4,4);
			for (int ii = 0; ii<4; ii++)
			{
				m_vvImgVertex3d[i].push_back(pVerPt4[ii]);
			}
			pPolygon1 = FunCreatePolygon(pVerPt4,3,4);
			if(pVerPt4) delete[]pVerPt4;
			OGRwkbGeometryType geotype = pPolygon1->getGeometryType();
			if(geotype!=wkbPolygon) continue;
			if(pPolygon1==NULL) continue;
			if(pGeoCombine==NULL) pGeoCombine = (OGRGeometry*)pPolygon1;
			else pGeoCombine= pGeoCombine->Union(pPolygon1);
			nCount++;
		}
		break;
	default:
		for (int i = 0; i<m_nImgNum; i++)
		{
			if(m_vecImgInfo.size()<=i) return;
			if(!m_vecImgInfo[i].m_bValid) continue;
			Point3D* pVerPt4 = m_vecImgInfo[i].GetImgVertexPt3d4(pZ,false,false);
			for (int ii = 0; ii<4; ii++)
			{
				m_vvImgVertex3d[i].push_back(pVerPt4[ii]);
			}
			pPolygon1 = FunCreatePolygon(pVerPt4,3,4);
			if(pVerPt4) delete[]pVerPt4;
			if(pPolygon1==NULL) continue;
			if(pGeoCombine==NULL) pGeoCombine=(OGRGeometry*)pPolygon1;
			else pGeoCombine=pGeoCombine->Union(pPolygon1);
		}
		break;
	}
	if(pGeoCombine==NULL) return;
	OGRwkbGeometryType geotype=pGeoCombine->getGeometryType();//
	vector<OGRPolygon*> vecpMissPolygon;
	if(geotype==wkbPolygon)
	{
		m_vvAreaVertex.resize(1); 
		pGeoCombine->closeRings();
		pRing = (OGRLinearRing*)(((OGRPolygon*)pGeoCombine)->getExteriorRing());
		for (int i = 0; i<pRing->getNumPoints(); i++)
		{
			OGRPoint ogrpt; pRing->getPoint(i,&ogrpt);
			double x = ogrpt.getX();
			double y = ogrpt.getY();
			m_vvAreaVertex[0].push_back(Point3D(x,y,Z));
		}

		nInerRingNum = ((OGRPolygon*)pGeoCombine)->getNumInteriorRings();
		for (int i = 0; i<nInerRingNum; i++)
		{
			pRing = (OGRLinearRing*)(((OGRPolygon*)pGeoCombine)->getInteriorRing(i));
			vector<Point3D> vecPoint3d;
			for (int i = 0; i<pRing->getNumPoints(); i++)
			{
				Point3D pt; OGRPoint ogrpt;
				pRing->getPoint(i,&ogrpt);
				pt.X = ogrpt.getX();
				pt.Y = ogrpt.getY();
				pt.Z = Z;
				vecPoint3d.push_back(pt);
			}
			m_vvMissAreaVertex.push_back(vecPoint3d);
			vector<Point3D>().swap(vecPoint3d);

			OGRPolygon *pPolygonMiss =(OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
			pPolygonMiss->addRing(pRing);
			vecpMissPolygon.push_back(pPolygonMiss);
		}
	}
	else if(geotype==wkbMultiPolygon)
	{
		OGRMultiPolygon *pMultiPolygon = (OGRMultiPolygon *)pGeoCombine;
		pMultiPolygon->closeRings();
		int nRef = pMultiPolygon->getNumGeometries();
		m_vvAreaVertex.resize(nRef);
		for (int i = 0; i<nRef; i++)
		{
			OGRGeometry *pGeometry = pMultiPolygon->getGeometryRef(i);
			pGeometry->closeRings();
			geotype=pGeometry->getGeometryType();
			if(geotype==wkbPolygon)
			{
				pPolygon1=(OGRPolygon*)pGeometry;
				pRing = (OGRLinearRing *)pPolygon1->getExteriorRing();
				for (int j = 0; j<pRing->getNumPoints(); j++)
				{
					OGRPoint ogrpt; pRing->getPoint(j,&ogrpt);
					double x = ogrpt.getX();
					double y = ogrpt.getY();
					m_vvAreaVertex[i].push_back(Point3D(x,y,Z));
				}
				nInerRingNum = pPolygon1->getNumInteriorRings();
				for (int j = 0; j<nInerRingNum; j++)
				{
					pRing = pPolygon1->getInteriorRing(j);

					vector<Point3D> vecPoint3d;
					for (int i = 0; i<pRing->getNumPoints(); i++)
					{
						Point3D pt; OGRPoint ogrpt;
						pRing->getPoint(i,&ogrpt);
						pt.X = ogrpt.getX();
						pt.Y = ogrpt.getY();
						pt.Z=Z;
						vecPoint3d.push_back(pt);
					}
					m_vvMissAreaVertex.push_back(vecPoint3d);
					vector<Point3D>().swap(vecPoint3d);
					OGRPolygon *pPolygonMiss = (OGRPolygon *)OGRGeometryFactory::createGeometry(wkbPolygon);
					pPolygonMiss->addRing(pRing);
					vecpMissPolygon.push_back(pPolygonMiss);
				}
			}
		}
	}
	for (int i = 0; i<vecpMissPolygon.size(); i++)
	{
		vector<int> vecImgIdx;
		for (int j = 0; j<m_nImgNum; j++)
		{
			if(!m_vecImgInfo[j].m_bValid) continue;
			Point3D *pVertex3d = m_vecImgInfo[j].GetImgVertexPt3d4(pZ,false,(eMethod==BY_POS?false:true));
			OGRPolygon* pPolygon = FunCreatePolygon(pVertex3d,3,4);
			if(pPolygon->getGeometryType()==wkbPolygon)
			{
				OGRGeometry *pInter = vecpMissPolygon[i]->Intersection(pPolygon);
				geotype = pInter->getGeometryType();
				if(geotype==wkbPolygon||geotype==wkbPoint)
					vecImgIdx.push_back(j); 
			}
			if(pVertex3d) delete[]pVertex3d;
			if(pPolygon) OGRGeometryFactory::destroyGeometry(pPolygon);
		}
		m_vvMissAreaImgIdx.push_back(vecImgIdx);
		vector<int>().swap(vecImgIdx);
	}
	for (int i = 0; i<vecpMissPolygon.size(); i++)
	{
		if(vecpMissPolygon[i]) 
			OGRGeometryFactory::destroyGeometry(vecpMissPolygon[i]);
	}
	vector<OGRPolygon*>().swap(vecpMissPolygon);
	if(pGeoCombine) OGRGeometryFactory::destroyGeometry(pGeoCombine);
	//	if(pPolygon1) OGRGeometryFactory::destroyGeometry(pPolygon1);
	//	if(pRing) OGRGeometryFactory::destroyGeometry(pRing);
}

void CDYUAVFlyQuaPrj::CheckImgValidState()
{
	if(m_nImgNum==0) return;
	double sum = 0, mean = 0, rms=0;
	vector<double> vDSinkap(m_nImgNum,0);
	for (int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<=i) return;

		vDSinkap[i]=fabs(sin(DegToRad(m_vecImgInfo[i].m_ImgPosInfo.kap)));
		sum+=vDSinkap[i];
		m_vecImgInfo[i].m_bValid=true;
	}
	mean = sum/m_nImgNum; sum=0;
	rms = FunCalDataRms(vDSinkap,mean);
	for (int i = 0; i<m_nImgNum; i++)
	{
		if(m_vecImgInfo.size()<=i) return;

		if(fabs(mean-vDSinkap[i])>m_FlyQuaParaSet.fValidJudgeTimesOfRms*rms)
		{
			m_vecImgInfo[i].m_bValid=false;
			m_nInvalidNum++;
			m_vecImgIdxInvalid.push_back(i);
			m_vecInvalidImgInfo.push_back(m_vecImgInfo[i]);
		}
	}
}
void CDYUAVFlyQuaPrj::DeleteInvalidImg()
{
	for (int i = 0; i<m_vecImgIdxInvalid.size(); i++)
	{
		for (int j = 0; j<m_vecImgInfo.size(); j++)
		{
			if(m_vecImgInfo[j].m_nImgIdx==m_vecImgIdxInvalid[i])
			{
				m_vecImgInfo.erase(m_vecImgInfo.begin()+j);
				m_vecImgIdxInvalid.erase(m_vecImgIdxInvalid.begin()+i);
				i--;
				j--;
				m_nImgNum--;
				break;
			}
		}
	}
	m_nInvalidNum = 0;
}
void CDYUAVFlyQuaPrj::CalStripOverlaps(double Z, DYFLYQUAMETHOD eMethod,FlyQuaCallBackFun pFun /* = NULL */)
{
	if(m_nImgNum<=0||m_nStripNum<=0) return;
	vector<vector<double>>().swap(m_vvOverlapsInsStrips);
	m_vvOverlapsInsStrips.resize(m_nStripNum);
	vector<vector<double>>().swap(m_vvOverlapsBetStrips);
	m_vvOverlapsBetStrips.resize(m_nStripNum);
	for (int i = 0; i<m_nStripNum; i++)
	{
		m_vvOverlapsBetStrips[i].assign(m_vecImgNumInEachStrip[i],0);
		m_vvOverlapsInsStrips[i].assign(m_vecImgNumInEachStrip[i],0);
	}
	//���㺽���ص���
	int nCount = 0;
	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);
	//#pragma omp parallel for
	for (int i = 0; i<m_nStripNum; i++)
	{
		//		EnterCriticalSection(&cs);
		nCount++;	
		/*		LeaveCriticalSection(&cs);*/
		CString strInfo;
		if(pFun!=NULL)
		{
			strInfo.Format("���ڼ��㺽���ص���...%-3.1lf%%",40+nCount*20.0/m_nStripNum);
			pFun(strInfo,40+nCount*20.0/m_nStripNum,NULL,40+nCount*20.0/m_nStripNum,false);
		}	
		if(m_vecImgInfo.size()<=i) continue;
		for (int j = 0; j<m_vecImgNumInEachStrip[i]; j++)
		{
			if(m_vecImgInfo.size()<=i) break;
			int nImgIdx1 = m_vvImgIdxInEachStrip[i][j];
			int nImgIdx2; double dMaxOverlapY = 0;
			for (int jj = max(j-2,0); jj<min(j+2,m_vecImgNumInEachStrip[i]-1); jj++)
			{
				if(jj==j) continue;
				nImgIdx2 = m_vvImgIdxInEachStrip[i][jj];
				double dOverlapY = 0;
				switch(eMethod)
				{
				case BY_POS:
					dOverlapY = CalOverlapsByPos(nImgIdx1,nImgIdx2,Z,DIRECTION_Y);
					break;
				case BY_MATCH:
					dOverlapY = CalOverlapsByMatch(nImgIdx1,nImgIdx2,DIRECTION_Y);
					if(dOverlapY==0)
						dOverlapY = CalOverlapsByPos(nImgIdx1,nImgIdx2,Z,DIRECTION_Y);
					break;
				case BY_ADJUSTMENT:
					dOverlapY = CalOverlapsByAdjustment(nImgIdx1,nImgIdx2,DIRECTION_Y);
					break;
				default:
					break;
				}
				dMaxOverlapY = max(dMaxOverlapY,dOverlapY);
			}

			m_vvOverlapsInsStrips[i][j]=dMaxOverlapY;
		}

	}
	//////////////////////////////////////////////////////
	//���������ص���
	vector<vector<int>> vvStripLinkStripIdx;
	for (int i = 0; i<m_nStripNum; i++)
	{
		vector<int> vecLinkStripIdx;
		for (int ii = max(i-2,0); ii<min(m_nStripNum,i+2); ii++)
		{
			if(m_vecImgInfo.size()<=i) break;
			if(ii==i) continue;
			vecLinkStripIdx.push_back(ii);
		}
		vvStripLinkStripIdx.push_back(vecLinkStripIdx);
		vector<int>().swap(vecLinkStripIdx);
	}
	nCount=0;
	//#pragma omp parallel for
	for (int i = 0; i<m_nStripNum; i++)
	{
		if(m_vecImgInfo.size()<=i) continue;
		//	EnterCriticalSection(&cs);
		nCount++;		
		//	LeaveCriticalSection(&cs);
		if(pFun!=NULL)
		{
			CString strInfo;
			strInfo.Format("���ڼ��������ص���...%-3.1lf%%",60+nCount*20.0/m_nStripNum);
			pFun(strInfo,60+nCount*20.0/m_nStripNum,NULL,60+nCount*20.0/m_nStripNum,false);
		}	
		vector<int> vecLinkStripIdx = vvStripLinkStripIdx[i];
		for (int j = 0; j<m_vecImgNumInEachStrip[i]; j++)
		{
			int nBetStripCount=0;
			int nImgIdx1 = m_vvImgIdxInEachStrip[i][j];
			double dMaxOverlapBetStrip=0;
			if(m_vecImgInfo.size()<=nImgIdx1) break;
			for (int ii = 0; ii<vecLinkStripIdx.size(); ii++)
			{
				double dSum=0;int nCount = 0;
				for (int jj = 0; jj<m_vecImgNumInEachStrip[vecLinkStripIdx[ii]]; jj++)
				{
					if(m_vecImgInfo.size()<=nImgIdx1) break;
					int nImgIdx2 = m_vvImgIdxInEachStrip[vecLinkStripIdx[ii]][jj];
					double dOverlapX = 0;
					switch(eMethod)
					{
					case BY_POS:
						dOverlapX=CalOverlapsByPos(nImgIdx1,nImgIdx2,Z,DIRECTION_X);
						break;
					case BY_MATCH:
						dOverlapX=CalOverlapsByMatch(nImgIdx1,nImgIdx2,DIRECTION_X);
						if(dOverlapX==0)
							dOverlapX = CalOverlapsByPos(nImgIdx1,nImgIdx2,Z,DIRECTION_X);
						break;
					case BY_ADJUSTMENT:
						dOverlapX=CalOverlapsByAdjustment(nImgIdx1,nImgIdx2,DIRECTION_X);
						break;
					default:
						break;
					}
					if(dOverlapX>0)
					{
						dMaxOverlapBetStrip=max(dMaxOverlapBetStrip,dOverlapX);
					}
				}
			}
			m_vvOverlapsBetStrips[i][j]=dMaxOverlapBetStrip;
		}
		vector<int>().swap(vecLinkStripIdx);

	}
	vector<vector<int>>().swap(vvStripLinkStripIdx);
	DeleteCriticalSection(&cs);
}

void CDYUAVFlyQuaPrj::ClearAdjInfo()
{
	vector<DYstuAdjPtInfo>().swap(m_vecAdjPts);
	for (int i = 0; i<m_nImgNum; i++)
	{
		m_vecImgInfo[i].m_bAdjed = false;
		vector<int>().swap(m_vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx);
		vector<int>().swap(m_vecImgInfo[i].m_ImgAdjPts.vecLinkImgIdx);
		vector<vector<int>>().swap(m_vecImgInfo[i].m_ImgAdjPts.vvAdjPtIdx4EachLinkImg);
		vector<vector<int>>().swap(m_vecImgInfo[i].m_ImgAdjPts.vvImgPtIdxInAdjPt);
		vector<vector<int>>().swap(m_vecImgInfo[i].m_ImgAdjPts.vvLinkImgPtIdxInAdjPt);
	}
	vector<int>().swap(m_vecBundleSuccess2ImgInfo);
	m_nInAdjedNum = 0;
	m_dRmsPosErr = 0;
	m_dMeanHeiCq = 0;
}
void CDYUAVFlyQuaPrj::ClearStripInfo()
{
	m_nStripNum=0;
	vector<int>().swap(m_vecImgNumInEachStrip);
	vector<vector<int>>().swap(m_vvImgIdxInEachStrip);
	vector<int>().swap(m_vecStripIdx);
#pragma omp parallel for
	for (int i = 0; i<m_nImgNum; i++)
	{
		m_vecImgInfo[i].m_nIdxInStrip=InvalidValue;
		m_vecImgInfo[i].m_nStripIdx=InvalidValue;
		m_vecImgInfo[i].m_strStripLabel = FunCreateStripLabel(InvalidValue,InvalidValue);
	}
	m_nMaxStripIdx=0;
}
void CDYUAVFlyQuaPrj::ClearReportInfo(DYFLYQUAMETHOD eMethod)
{
	vector<double>().swap(m_vecImagePitch);                 
	vector<vector<double>>().swap(m_vvStripImageYaw);       
	vector<vector<double>>().swap(m_vvStripImageFlyHeiErr); 
	vector<vector<double>>().swap(m_vvStripImageFlyDisErr); 
	vector<double>().swap(m_vecStripBlendPara);             
	vector<vector<double>>().swap(m_vvOverlapsInsStrips);   
	vector<vector<double>>().swap(m_vvOverlapsBetStrips); 

	vector<int>().swap(m_vecImgIdxLowOverlapInsStrips);
	vector<int>().swap(m_vecImgIdxLowOverlapBetStrips);
	vector<int>().swap(m_vecImgIdxHeiPitch);
	vector<int>().swap(m_vecImgIdxHeiYaw);
	vector<int>().swap(m_vecImgIdxHeiHeiErr);
	vector<int>().swap(m_vecStripIdxHeiBlend);
	vector<double>().swap(m_vecStripPanePara);
	vector<DYstuPosInfo>().swap(m_vecAdjImgPosErr);
	vector<double>().swap(m_vecAdjImgPosDisErr);
	vector<int>().swap(m_vecImgIdxHeiPosErr);
	vector<int>().swap(m_vecBundleIdxHeiPosErr);
	m_dMeanPosErr  =0;
	m_dRmsPosErr = 0;
	m_nImgNumPitch10 = 0;
	m_nImgNumYaw15 = 0;
	m_dMaxPaneDifErr = 0;
	m_dRmsPaneDifErr = 0;
	m_dMaxHeiErr = 0;
	vector<int>().swap(m_vecImgNumInEachStripYaw20);
	vector<int>().swap(m_vecImgIdxLowMatchPointNum);
	vector<vector<Point3D>>().swap(m_vvMissAreaVertex);
	vector<vector<int>>().swap(m_vvMissAreaImgIdx);
	vector<vector<Point3D>>().swap(m_vvAreaVertex);
	vector<vector<Point3D>>().swap(m_vvImgVertex3d);
	vector<double>().swap(m_vecDisImgAdjPos2PosPane);

	if(eMethod==BY_POS){
		m_dTimeSecsTimeReadAdjRep=0;
	}
	m_dTimeSecsRunFlyQuaChk=0;
	m_dTimeSecsWholeProcess=0;
	m_dTimeSecsWriteChkRep=0;
	m_nMatchPointNumThresold = 8;
	vector<int>().swap(m_vecImgIdxPassCheck);
}
bool CDYUAVFlyQuaPrj::ReadAdjRep(FlyQuaCallBackFun pFun/* =NULL */)
{
	clock_t t1 = clock();
	ClearAdjInfo(); 
	CMemTxtFile txt;
	if(!txt.OpenFile(m_strBundleAdjRepPath))
	{
		FunOutPutLogInfo("��������򿪴������ݶ�ȡʧ�ܣ�");
		return false;
	}
	char line[MAX_SIZE_FILE_LINE];
	const char* tmp = txt.GetFileHead();
	if(!ReadLine(tmp,line,MAX_SIZE_FILE_LINE))
	{
		FunOutPutLogInfo("�����������ж�ȡ�������ݶ�ȡʧ�ܣ�");
		return false;
	}
	tmp+=strlen(line)+2;
	int nImgNum, nPtNum;
	if(2!=sscanf(line,"%d %d",&nImgNum,&nPtNum))
	{
		FunOutPutLogInfo("�����������н����������ݶ�ȡʧ�ܣ�");
		return false;
	}
	if(nImgNum<=0||nPtNum<=0)
	{
		FunOutPutLogInfo("������������������ݶ�ȡʧ�ܣ�");
		return false;
	}
	char strlog[MAX_SIZE_FILE_LINE];
	DYstuPosInfo* pAdjPosInfo = new DYstuPosInfo[nImgNum];
	if(pFun!=NULL) pFun("���ڶ�ȡ��������...",10,NULL,0,true);
	for (int i =0; i<nImgNum; i++)
	{
		DYstuPosInfo DYAdjPos;
		if(!ReadLine(tmp,line,MAX_SIZE_FILE_LINE))
		{	
			sprintf(strlog,"�����������%d��Ӱ����Ϣ��ȡ����",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		tmp+=strlen(line)+2;
		int nWidth, nHeight, df; char strLabel[MAX_SIZE_FILE_LINE];
		if(4!=sscanf(line,"%s %d %d %lf", strLabel,&nWidth,&nHeight,&df))
		{
			sprintf(strlog,"�����������%d��Ӱ����Ϣ��������",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		DYAdjPos.strLabel = strLabel; DYAdjPos.strLabel.MakeLower();
		for (int j = 0; j<2; j++)   //�������������㡢���������Ϣ
		{
			if(!ReadLine(tmp,line,MAX_SIZE_FILE_LINE))
			{
				sprintf(strlog,"�����������%d��Ӱ����Ϣ��ȡ����",i);
				FunOutPutLogInfo(strlog);
				return false;
			}
			tmp+=strlen(line)+2;
		}
		if(!ReadLine(tmp,line,MAX_SIZE_FILE_LINE))
		{
			sprintf(strlog,"�����������%d��Ӱ���ⷽλԪ�ض�ȡ����",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		tmp+=strlen(line)+2;
		if(6!=sscanf(line,"%lf %lf %lf %lf %lf %lf",&DYAdjPos.Xs,&DYAdjPos.Ys,
			&DYAdjPos.Zs,&DYAdjPos.R[0],&DYAdjPos.R[1],&DYAdjPos.R[2]))
		{
			sprintf(strlog,"�����������%d��Ӱ���ⷽλԪ�ؽ�������",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		if(!ReadLine(tmp,line,MAX_SIZE_FILE_LINE))
		{
			sprintf(strlog,"�����������%d��Ӱ���ⷽλԪ�ض�ȡ����",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		tmp+=strlen(line)+2;
		if(6!=sscanf(line,"%lf %lf %lf %lf %lf %lf",&DYAdjPos.R[3],&DYAdjPos.R[4],
			&DYAdjPos.R[5],&DYAdjPos.R[6],&DYAdjPos.R[7],&DYAdjPos.R[8]))
		{
			sprintf(strlog,"�����������%d��Ӱ���ⷽλԪ�ؽ�������",i);
			FunOutPutLogInfo(strlog);
			return false;
		}
		*(pAdjPosInfo+i) = DYAdjPos;
		if(pFun!=NULL)
		{
			CString strInfo;
			if((nImgNum>=10&&i%(m_nImgNum/10)==0)||nImgNum<10)
			{
				strInfo = "���ڶ�ȡ��������...";
				pFun(strInfo,10+10.0*(i+1)/m_nImgNum,NULL,0,false);
			}
		}

	}
	CString *pAdjPtsLine = new CString[nPtNum*2];
	for (int i = 0; i<nPtNum*2; i++)
	{
		ReadLine(tmp,line,MAX_SIZE_FILE_LINE);
		tmp+=strlen(line)+2;
		*(pAdjPtsLine+i) = line;
	}

	m_vecAdjPts.resize(nPtNum);
	double dSumOfHei = 0;
	int nCount = 0;
	for (int i = 0; i<nPtNum; i++)
	{
		nCount++;
		if(pFun!=NULL)
		{
			CString strInfo;
			if((nPtNum>=75&&nCount%(nPtNum*5/75)==0)||nPtNum<75)
			{
				strInfo = "���ڶ�ȡ��������...";
				pFun(strInfo,20+75.0*(nCount+1)/nPtNum,NULL,0,false);
			}
		}
		double X,Y,Z; int r,g,b; int nOverlap;
		sscanf(*(pAdjPtsLine+i*2+0),"%lf %lf %lf %d %d %d",&X,&Y,&Z,&r,&g,&b);
		vector<CString> vParts = FunStrTok(*(pAdjPtsLine+i*2+1),"\t ");
		nOverlap = atoi(vParts[0]);
		DYstuAdjPtInfo AdjPt;
		AdjPt.X=X;AdjPt.Y=Y;AdjPt.Z=Z;
		AdjPt.r=r;AdjPt.g=g;AdjPt.b=b;
		AdjPt.nOverlayCount = nOverlap;
		AdjPt.vecImgIdx.resize(nOverlap);
		AdjPt.vecImgPt.resize(nOverlap);
		dSumOfHei+=Z;
		for (int j = 0; j<nOverlap; j++)
		{
			if(m_vecImgInfo.size()==0) return true;
			int nImgId; double px,py;
//			nImgId = m_vecBundleImgList2ImgInfo[atoi(vParts[j*3+1])];  //��ʵ����
			nImgId = atoi(vParts[j*3+1]);  //����������Ӱ������ImportXml��һ��
			px = atof(vParts[j*3+2]);
			py  = atof(vParts[j*3+3]);
			Point2D pt = Point2D(px,py);
			AdjPt.vecImgIdx[j] = nImgId;
			AdjPt.vecImgPt[j] = pt;
		}
		m_vecAdjPts[i] = AdjPt;
		vector<CString>().swap(vParts);
	}
	txt.CloseFile();
	m_vecBundleSuccess2ImgInfo.resize(nImgNum);
	m_dMeanHeiCq = dSumOfHei/nPtNum;
#pragma omp parallel for
	for (int i = 0; i<nImgNum; i++)
	{
		//  		m_vecImgInfo[m_vecBundleImgList2ImgInfo[i]].m_AdjPosInfo = *(pAdjPosInfo+i);
		//  		m_vecImgInfo[m_vecBundleImgList2ImgInfo[i]].m_bAdjed = true;
		for (int j = 0; j<m_nImgNum; j++)
		{
			if(m_vecImgInfo[j].m_bAdjed) continue;
			if((*(pAdjPosInfo+i)).strLabel==m_vecImgInfo[j].m_strImgName)
			{
				m_vecImgInfo[j].m_AdjPosInfo = *(pAdjPosInfo+i);
				m_vecImgInfo[j].m_bAdjed=true;
				m_vecBundleSuccess2ImgInfo[i]=j;
				break;
			}
		}
	}
	m_nInAdjedNum = nImgNum;
	FunArrangeAdjPts(m_vecImgInfo,m_vecAdjPts);
	CString strInfo="���ڶ�ȡ��������...";
	if(pFun!=NULL)	pFun(strInfo,97,NULL,0,false);

	if(pAdjPosInfo)delete[]pAdjPosInfo;
	if(pAdjPtsLine)delete[]pAdjPtsLine;

	strInfo="���ڶ�ȡ��������...";
	if(pFun!=NULL) pFun(strInfo,100,NULL,0,false);
	clock_t t2 = clock();
	double dt = (double)(t2-t1)/CLOCKS_PER_SEC;
	m_dTimeSecsTimeReadAdjRep = dt;
	sprintf(strlog,"��������ļ���ȡ��ɣ�Ӱ����%d���﷽����%d����ʱ%.3lfs",nImgNum,nPtNum,m_dTimeSecsTimeReadAdjRep);
	if(pFun!=NULL)
	{
		pFun(strlog,100,NULL,0,true);	
	}
	else
	{
		FunOutPutLogInfo(strlog);
	}
	return true;
}

bool CDYUAVFlyQuaPrj::RunFlyQuaChk(double Z /* = 0 */, DYFLYQUAMETHOD eMethod /* = BY_ADJUSTMENT */,FlyQuaCallBackFun pFun/* =NULL */)
{ 
	//	if(pFun) pFun("�����ʼ����",0,NULL,0,false);
	ClearAdjInfo();
	ClearReportInfo(eMethod);
	if(m_nImgNum==0) return false;
	m_nMatchPointNumThresold = m_FlyQuaParaSet.nMatchPtNumThresold;
	clock_t t1 = clock();
	if(eMethod==BY_MATCH) 
	{
// 		if(!ReadMatchPoint(pFun))
// 		{
// 			if(!pFun) FunOutPutLogInfo("ƥ����ȡ�����ʼ����ʧ�ܣ�");
// 			else pFun("ƥ����ȡ�����ʼ����ʧ�ܣ�",0,NULL,0,true);
// 			return false;
// 		}
	}
	else if(eMethod==BY_ADJUSTMENT)
	{
		if(!ReadAdjRep(pFun))
		{
			if(!pFun) FunOutPutLogInfo("���������ȡ�����ʼ����ʧ�ܣ�");
			else pFun("���������ȡ�����ʼ����ʧ�ܣ�",0,NULL,0,true);
		}
	}
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ�����",0,NULL,0,true);
	CalImagePitch();
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ������",10,NULL,0,true);
	CalImageYaw(Z,eMethod/*BY_MATCH*/);
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ�񺽸߲�",20,NULL,0,true);
	CalImageFlyHeiErr(eMethod);
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ��ƫ����",30,NULL,0,true);
	CalImageFlyDisErr(eMethod);
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ���ص���",40,NULL,0,true);
	// 	if(eMethod!=BY_POS) CalStripOverlaps(Z,/*eMethod*/BY_MATCH);
	// 	else CalStripOverlaps(Z,BY_POS);
	switch(eMethod)
	{
	case BY_POS:
		CalStripOverlaps(Z,BY_POS);
		break;
	case BY_MATCH:
		CalStripOverlaps(Z,BY_MATCH);
		break;
	case BY_ADJUSTMENT:
		CalStripOverlaps(Z,BY_MATCH);
		break;
	default:
		break;
	}
	if(pFun!=NULL) pFun("�ʼ�������㣺Ӱ��POS���",80,NULL,0,true);
	CalImagePosErr(eMethod);
	if(pFun!=NULL) pFun("�ʼ�������㣺����ͳ��",90,NULL,0,true);
	AnalysisFlyQuaChkParas(m_nMatchPointNumThresold);
	if(pFun!=NULL) pFun("�ʼ�������㣺©������",95,NULL,0,true);
	CheckMissArea(Z,eMethod);
	if(pFun!=NULL) pFun("�ʼ�������㣺©������",100,NULL,0,false);
	CalStripCqPaneErr(eMethod);
	clock_t t2 = clock();
	double dt = (double)(t2-t1)/CLOCKS_PER_SEC;
	m_dTimeSecsRunFlyQuaChk = dt;
	char strlog[MAX_SIZE_FILE_LINE];
	sprintf(strlog,"�ʼ����ͳ����ϣ���ʱ%.3lfs",m_dTimeSecsRunFlyQuaChk);
	if(pFun!=NULL) pFun(strlog,100,NULL,0,true);
	else FunOutPutLogInfo(strlog);

	/*************************************/
	/*
	FILE*pfWX = fopen(FunGetFileFolder(m_strPrjXmlPath)+"\\AdjPt_X.txt","w");
	FILE*pfWY = fopen(FunGetFileFolder(m_strPrjXmlPath)+"\\AdjPt_Y.txt","w");
	FILE*pfWZ = fopen(FunGetFileFolder(m_strPrjXmlPath)+"\\AdjPt_Z.txt","w");
	FILE*pfWDis = fopen(FunGetFileFolder(m_strPrjXmlPath)+"\\PosDis2Pane.txt","w");
	int nCount = 0;
	if(pfWZ!=NULL)
	{
		for (int i = 0; i<m_vecImgInfo.size(); i++)
		{
			if(!m_vecImgInfo[i].m_bAdjed) continue;
			double Z = m_vecImgInfo[i].m_AdjPosInfo.Zs;
			nCount++;
			fprintf(pfWZ,"%-13.6lf\n",Z);
		}
	}
	nCount = 0;
	for (int i = 0; i<m_vecImgInfo.size(); i++)
	{
		if(!m_vecImgInfo[i].m_bAdjed) continue;
		if(m_vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx.size()==0) continue;
		int nAdjPtNum = m_vecImgInfo[i].m_ImgAdjPts.vecAdjPtIdx.size();
		for (int j = 0; j<nAdjPtNum; j++)
		{
			if(pfWX!=NULL) fprintf(pfWX,"%-13.6lf\n",m_vecImgInfo[i].m_ImgAdjPts.vecImgPtResX[i]);
			if(pfWY!=NULL) fprintf(pfWY,"%-13.6lf\n",m_vecImgInfo[i].m_ImgAdjPts.vecImgPtResY[i]);
			nCount++;
		}
		if(nCount>0) break;
	}
	double dmsan = 0;
	for (int i =0; i<m_vecDisImgAdjPos2PosPane.size(); i++)
	{
		dmsan+=m_vecDisImgAdjPos2PosPane[i];
	}
	dmsan/=m_vecDisImgAdjPos2PosPane.size();
	double rms = FunCalDataRms(m_vecDisImgAdjPos2PosPane,dmsan);
	if(pfWDis!=NULL)
	{
		//	fprintf(pfWDis,"%-13.6lf  %-13.6lf\n",dmsan,rms);
		for (int i = 0; i<m_vecDisImgAdjPos2PosPane.size(); i++)
		{
			fprintf(pfWDis,"%-13.6lf\n",m_vecDisImgAdjPos2PosPane[i]);
		}
	}
	fclose(pfWX);
	fclose(pfWY);
	fclose(pfWZ);
	fclose(pfWDis);
	*/
	return true;
}
bool CDYUAVFlyQuaPrj::SaveReport2File(CString strCqViewImgPath,
	CString strOverlapYErrViewImgPath,CString strOverlapXErrViewImgPath,
	CString strPosErrViewImgPath, CString strPitchErrViewImgPath,
	CString strYawViewImgPath,CString strRepPath,DYFLYQUAMETHOD eMethod/* = BY_ADJUSTMENT*/)
{
	FunOutPutLogInfo("��ʼд��PDF");
	clock_t t1 = clock();
	m_strFlyQuaChkRepPath = strRepPath;
	char *strLine = new char[MAX_SIZE_FILE_LINE*3]; 
	memset(strLine,0,MAX_SIZE_FILE_LINE*3);
	CsswPdf pdf; 
	if(!pdf.InitPdf())
	{
		FunOutPutLogInfo("PDF��ʼ��ʧ�ܣ�");
		return false;
	}
	int nImgIdx =1; 
	char strImgLabelInfo[MAX_SIZE_FILE_LINE]; 
	memset(strImgLabelInfo,0,MAX_SIZE_FILE_LINE);
	int nListCols/* = 2*/, nListRows/* = 5*/;
	vector<CString> vecListInfo;
	if(!pdf.AddPage()) return false;
	pdf.SetLineSpace(50);
	pdf.SetPdfTitle("���˻���������������鱨��",0,0.6,0);
	CString strLogon = FunGetFileFolder(FunGetThisExePath())+"\\logo.dll";
	int nw = 31, nh = 31;
	int nx = pdf.GetPdfPageWidth()-pdf.GetLineSpace();
	int ny = pdf.GetPdfPageHeight()-pdf.GetTitleGap()-pdf.GetLineGap();
	pdf.AddImage(strLogon,1.0,SSW_NONE,nx,ny,nw,nh); 
	sprintf(strLine,"��������ʱ�䣺%s",FunGetSysTimeStr());
	pdf.AddText(strLine,SSW_SONG,1.2,SSW_LEFT,0,false,0,0,0,0,0,0,true);pdf.AddEmptyLine(-1.2);
	pdf.AddText("@2016 UAV Flight Quality Check",SSW_SONG,1.2,SSW_RIGHT,0,false,0.3,0.3,0.3,0,0,0,true);
	pdf.AddEmptyLine(5);

	/**************��������List 2*4 ***************/
	pdf.AddText("�������ݸ���",SSW_HEI,2.5,SSW_LEFT);
	pdf.AddEmptyLine(3);

	nListCols = 4; nListRows = 2;
	vecListInfo.resize(nListCols*nListRows);
	vecListInfo[0*nListCols+0].Format("%s","Ӱ������");
	vecListInfo[0*nListCols+1].Format("%s","��ЧӰ����");
	vecListInfo[0*nListCols+2].Format("%s","��ЧӰ����");
	vecListInfo[0*nListCols+3].Format("%s","������");
	vecListInfo[1*nListCols+0].Format("%d",m_nImgNum);
	vecListInfo[1*nListCols+1].Format("%d",m_nImgNum-m_nInvalidNum);
	vecListInfo[1*nListCols+2].Format("%d",m_nInvalidNum);
	vecListInfo[1*nListCols+3].Format("%d",m_nStripNum);
	pdf.AddText("����Ӱ������",SSW_SONG,1.5,SSW_LEFT);
	pdf.AddEmptyLine(1);
	cprintf("========List info in report========\n");
	cprintf("ImageInfo...");
	pdf.AddList(nListCols,nListRows,vecListInfo);
	cprintf("...Over\n");
	vector<CString>().swap(vecListInfo);
	pdf.AddEmptyLine(1);
	pdf.AddImage(strCqViewImgPath);
	pdf.AddEmptyLine(0.5);
	sprintf(strImgLabelInfo,"ͼ%d. ��������ʾ��ͼ(ע����ɫ��Ϊ��ЧӰ��������ɫ��ʾ�����������ɫ�����Ϊ©������)",nImgIdx);
	pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
	nImgIdx++;
	pdf.AddEmptyLine(5);
	FunOutPutLogInfo("�������ݸ���д����ɣ�");
	/***********************�ʼ������ֵ*************************/
	nListCols = 3; nListRows = 8;
	vecListInfo.resize(nListCols*nListRows);
	vecListInfo[0*nListCols+0]="�ʼ���";
	vecListInfo[0*nListCols+1]="��ֵ˵��";
	vecListInfo[0*nListCols+2]="��ֵ��С";
	vecListInfo[1*nListCols+0]="�������";
	vecListInfo[1*nListCols+1]="ƽ���POSƽ����ƫ�����ֵ(m)";
	vecListInfo[1*nListCols+2].Format("%.2f",m_FlyQuaParaSet.fMaxErrStationPane);
	vecListInfo[2*nListCols+0]="�������";
	vecListInfo[2*nListCols+1]="ƽ���POSƽ����ƫ�������(m)";
	vecListInfo[2*nListCols+2].Format("%.2f",m_FlyQuaParaSet.fRmsErrStationPane);
	vecListInfo[3*nListCols+0]="�����ص���";
	vecListInfo[3*nListCols+1]="Ӱ��ϸ���С�����ص���";
	vecListInfo[3*nListCols+2].Format("%.2f%%",m_FlyQuaParaSet.fMinOverlapY);
	vecListInfo[4*nListCols+0]="�����ص���";
	vecListInfo[4*nListCols+1]="Ӱ��ϸ���С�����ص���";
	vecListInfo[4*nListCols+2].Format("%.2f%%",m_FlyQuaParaSet.fMinOverlapX);
	vecListInfo[5*nListCols+0]="Ӱ�����";
	vecListInfo[5*nListCols+1]="Ӱ��ϸ�������";
	vecListInfo[5*nListCols+2].Format("%.2f",m_FlyQuaParaSet.fMaxPitch);
	vecListInfo[6*nListCols+0]="Ӱ������";
	vecListInfo[6*nListCols+1]="Ӱ��ϸ��������";
	vecListInfo[6*nListCols+2].Format("%.2f",m_FlyQuaParaSet.fMaxYaw);
	vecListInfo[7*nListCols+0]="ƥ������Ӱ��";
	vecListInfo[7*nListCols+1]="ƥ������Ӱ���жϿ���������Сֵ";
	vecListInfo[7*nListCols+2].Format("%d",m_nMatchPointNumThresold);
	pdf.AddText("�Զ��ʼ������ֵ",SSW_SONG,1.5,SSW_LEFT);
	pdf.AddEmptyLine(1);
	cprintf("========List info in report========\n");
	cprintf("Thresold...");
	pdf.AddList(nListCols,nListRows,vecListInfo);
	cprintf("...Over\n");
	vector<CString>().swap(vecListInfo);
	FunOutPutLogInfo("�Զ��ʼ������ֵд����ɣ�");
	pdf.AddEmptyLine(4);
	/***********************�ʼ���*************************/
	pdf.AddText("�ʼ���",SSW_HEI,2,SSW_LEFT);
	pdf.AddEmptyLine(1);
	memset(strLine,'-',MAX_SIZE_FILE_LINE*3);
	pdf.AddText(strLine,SSW_SONG,1,SSW_LEFT,0,true,0.7,0.7,0.7);
	pdf.AddEmptyLine(-2);
	if(eMethod==BY_POS) sprintf(strLine,"�ʼ췽����������POS�Ŀ����ʼ�");else
		if(eMethod==BY_MATCH) sprintf(strLine,"�ʼ췽����������ƥ����ʼ췽��");else
			if(eMethod==BY_ADJUSTMENT) sprintf(strLine,"�ʼ췽����������ƽ��ľ�ȷ�ʼ�");
	pdf.AddText(strLine,SSW_HEI,1.5,SSW_CENTER);
	pdf.AddEmptyLine(4);
	/***********************�ֲ�������*************************/
	pdf.AddText("���ݷֲ������Լ��",SSW_HEI,2);
	pdf.AddEmptyLine(3);
	sprintf(strLine,"��ͼ%d��ʾ������©��������%d��",nImgIdx-1,m_vvMissAreaImgIdx.size());
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1,0,0);
	pdf.AddEmptyLine(1.5);
	if(m_vvMissAreaImgIdx.size()>0)
	{
		int nMissImgNum = 0;
		for (int i = 0; i<m_vvMissAreaImgIdx.size(); i++)
		{
			nMissImgNum+=m_vvMissAreaImgIdx[i].size();
			if(i<m_vvMissAreaImgIdx.size()-1) nMissImgNum++;
		}
		sprintf(strLine,"©�������Ӱ���б�");
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1,0,0);
		pdf.AddEmptyLine(1.5);
		nListCols=4;nListRows=nMissImgNum+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("©�����");
		vecListInfo[0*nListCols+1].Format("Ӱ������");
		vecListInfo[0*nListCols+2].Format("Ӱ����");
		vecListInfo[0*nListCols+3].Format("������Ϣ");
		int nCount = 1;
		for (int i = 0; i<m_vvMissAreaImgIdx.size(); i++)
		{
			for (int j = 0; j<m_vvMissAreaImgIdx[i].size(); j++)
			{
				vecListInfo[nListCols*nCount+0].Format("%d",i);
				vecListInfo[nListCols*nCount+1].Format("%d",m_vvMissAreaImgIdx[i][j]);
				vecListInfo[nListCols*nCount+2].Format("%s",m_vecImgInfo[m_vvMissAreaImgIdx[i][j]].m_strImgName);
				vecListInfo[nListCols*nCount+3].Format("%s",m_vecImgInfo[m_vvMissAreaImgIdx[i][j]].m_strStripLabel);
				nCount++;
			}
			if(i<m_vvMissAreaImgIdx.size()-1)
			{
				vecListInfo[nListCols*nCount+0].Format("---");
				vecListInfo[nListCols*nCount+1].Format("---");
				vecListInfo[nListCols*nCount+2].Format("---");
				vecListInfo[nListCols*nCount+3].Format("---");
				nCount++;
			}
		}
		cprintf("MissArea...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
	}
	pdf.AddEmptyLine(4);
	FunOutPutLogInfo("©������д����ɣ�");
	/***********************�������*************************/	
	if(eMethod==BY_ADJUSTMENT)
	{	
		sprintf(strLine,"����ڲ������ȷ���");
		pdf.AddText(strLine,SSW_HEI,2,SSW_LEFT);
		pdf.AddEmptyLine(3);
		pdf.AddText("���ԭʼ�������",SSW_SONG,1.5,SSW_LEFT);
		pdf.AddEmptyLine(1);
		nListCols = 6; nListRows = 2;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("a");
		vecListInfo[0*nListCols+1].Format("b");
		vecListInfo[0*nListCols+2].Format("k1");
		vecListInfo[0*nListCols+3].Format("k2");
		vecListInfo[0*nListCols+4].Format("p1");
		vecListInfo[0*nListCols+5].Format("p2");
		vecListInfo[1*nListCols+0].Format("%.8lf",m_ImgCmrInfo.a);
		vecListInfo[1*nListCols+1].Format("%.8lf",m_ImgCmrInfo.b);
		vecListInfo[1*nListCols+2].Format("%.8lf",m_ImgCmrInfo.k1);
		vecListInfo[1*nListCols+3].Format("%.8lf",m_ImgCmrInfo.k2);
		vecListInfo[1*nListCols+4].Format("%.8lf",m_ImgCmrInfo.p1);
		vecListInfo[1*nListCols+5].Format("%.8lf",m_ImgCmrInfo.p2);
		cprintf("CameraInfo...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
		pdf.AddEmptyLine(2);
		// 		pdf.AddText("//////////////////////////////////",SSW_SONG,1.5,SSW_CENTER);
		// 		pdf.AddEmptyLine(2);

		sprintf(strLine,"ƽ������վ��ຽ��ƽ�棬������ƫ��%.2lfm(��ֵ%.2f)������ƫ�������%.2lfm(��ֵ%.2f)��",m_dMaxPaneDifErr,m_FlyQuaParaSet.fMaxErrStationPane,m_dRmsPaneDifErr,m_FlyQuaParaSet.fRmsErrStationPane);
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,0);
		pdf.AddEmptyLine(1.5);
		int nPosX = pdf.GetTextWidthInPage(strLine)+pdf.GetLineSpace();
		int nPosY = pdf.GetWriteTopInPage();
		if(m_dMaxPaneDifErr>m_FlyQuaParaSet.fMaxErrStationPane&&m_dRmsPaneDifErr>m_FlyQuaParaSet.fRmsErrStationPane)
		{
			pdf.AddText("����������ϸ�",SSW_SONG,2,SSW_LEFT,0,false,1);
		}
		else
		{
			pdf.AddText("��������ϸ�",SSW_SONG,2,SSW_LEFT,0,false,1);
		}
		pdf.AddEmptyLine(1.5);
		float *pX = new float[m_nInAdjedNum];
		float *pY = new float[m_nInAdjedNum];
		for (int i = 0; i<m_nInAdjedNum; i++)
		{
			pX[i]=m_vecBundleSuccess2ImgInfo[i];
			pY[i]=m_vecDisImgAdjPos2PosPane[i];
		}
		pdf.AddChart(pY,pX,m_vecDisImgAdjPos2PosPane.size(),0,0,1,0,0,1,0,0,"POSƫ�����","Ӱ������","POS����ƫ��ֵ-m");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. ����Ӱ��ƽ����POSƫ��ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(4);
		delete[]pX;
		delete[]pY;
		FunOutPutLogInfo("�����������д����ɣ�");
		/***********************POS����*************************/	
		sprintf(strLine,"POS������Ϣ���ȷ���");
		pdf.AddText(strLine,SSW_HEI,2,SSW_LEFT);
		pdf.AddEmptyLine(3);
		sprintf(strLine,"����ƽ��Ӱ��%d�ţ�ƽ��ǰ��POS�ռ����(Err)��������� RMS = %.2lfm����ֵ%.2lfm",
			m_nInAdjedNum,m_dRmsPosErr,m_dMeanPosErr);
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,true);
		pdf.AddEmptyLine(2);
		//	sprintf(strLine,"���ϸ�Ӱ����%d�ţ�Err > 3*RMS+��ֵ Ϊ���ϸ�",m_vecImgIdxHeiPosErr.size());
		sprintf(strLine,"����3������Err>3*RMS+Mean��Ӱ��%d��",m_vecImgIdxHeiPosErr.size());
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1);
		pdf.AddEmptyLine(1.5);
		if(m_vecImgIdxHeiPosErr.size()>0)
		{	
			nListCols = 6; nListRows = m_vecImgIdxHeiPosErr.size()+1;
			vecListInfo.resize(nListRows*nListCols);
			vecListInfo[0*nListCols+0].Format("Ӱ������");
			vecListInfo[0*nListCols+1].Format("Ӱ����");
			vecListInfo[0*nListCols+2].Format("X���-m");
			vecListInfo[0*nListCols+3].Format("Y���-m");
			vecListInfo[0*nListCols+4].Format("Z���-m");
			vecListInfo[0*nListCols+5].Format("XYZ���-m");
			for (int i = 0; i<nListRows-1; i++)
			{
				vecListInfo[(i+1)*nListCols+0].Format("%d",m_vecImgIdxHeiPosErr[i]);
				vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[m_vecImgIdxHeiPosErr[i]].m_strImgName);
				vecListInfo[(i+1)*nListCols+2].Format("%.2lf",m_vecAdjImgPosErr[m_vecBundleIdxHeiPosErr[i]].Xs);
				vecListInfo[(i+1)*nListCols+3].Format("%.2lf",m_vecAdjImgPosErr[m_vecBundleIdxHeiPosErr[i]].Ys);
				vecListInfo[(i+1)*nListCols+4].Format("%.2lf",m_vecAdjImgPosErr[m_vecBundleIdxHeiPosErr[i]].Zs);
				vecListInfo[(i+1)*nListCols+5].Format("%.2lf",m_vecAdjImgPosDisErr[m_vecBundleIdxHeiPosErr[i]]);
			}
			cprintf("PosInfo...");
			pdf.AddList(nListCols,nListRows,vecListInfo);
			cprintf("...Over\n");
			vector<CString>().swap(vecListInfo);
			pdf.AddEmptyLine(1.5);

			pdf.AddEmptyLine(1);
			pdf.AddImage(strPosErrViewImgPath,2);
			pdf.AddEmptyLine(0.5);
			sprintf(strImgLabelInfo,"ͼ%d. POS����Ӱ��ֲ�(ע����ɫ��Ϊ����Ӱ����ɫ��Ϊ�ǳ���Ӱ��)",nImgIdx);
			pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
			nImgIdx++;
			pdf.AddEmptyLine(3);
		}
		pX = new float[m_nInAdjedNum];
		pY = new float[m_nInAdjedNum];
		for (int i = 0; i<m_nInAdjedNum; i++)
		{
			pX[i]=m_vecBundleSuccess2ImgInfo[i];
			pY[i]=m_vecAdjImgPosDisErr[i];
		}
		pdf.AddChart(pY,pX,m_nInAdjedNum,0,0,1,0,0,1,0,0,"POS�������","Ӱ������","POS�������ֵ-m");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. ����Ӱ��ԭʼPOS�������ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(4);
		delete[]pX;
		delete[]pY;
	}
	FunOutPutLogInfo("POS����д����ɣ�");
	/***********************�ص���*************************/
	sprintf(strLine,"Ӱ���ص�����Ϣ");
	pdf.AddText(strLine,SSW_HEI,2,SSW_LEFT);
	pdf.AddEmptyLine(3);
	CString strOverlapImg = FunGetFileFolder(strCqViewImgPath);
	if(eMethod==BY_ADJUSTMENT) strOverlapImg+="\\OverlapImg_ByBundle.jpg";else
		if(eMethod==BY_MATCH) strOverlapImg+="\\OverlapImg_ByMatch.jpg";else
			if(eMethod==BY_POS) strOverlapImg+="\\OverlapImg_ByPos.jpg";
	pdf.AddImage(strOverlapImg,1,SSW_CENTER,0,0,0,0,true);
	//pdf.AddImage(FunGetFileFolder(strViewImgPath)+"\\OverlapNote.jpg",1.0,SSW_NONE,0,350,40);
	pdf.AddEmptyLine(0.5);
	sprintf(strImgLabelInfo,"ͼ%d. �����ص���ʾ��ͼ",nImgIdx);
	pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
	nImgIdx++;
	pdf.AddEmptyLine(3);

	double dMaxOverlapX, dMinOverlapX,dMaxOverlapY, dMinOverlapY;
	double dMeanOverlapX=0,dMeanOverlapY=0;
	dMaxOverlapX=dMaxOverlapY=-1;
	dMinOverlapX=dMinOverlapY=10;
	float *pOverlapY,*pOverlapX,*pX;
	pOverlapX=new float[m_nImgNum-m_nInvalidNum];
	pOverlapY=new float[m_nImgNum-m_nInvalidNum];
	pX=new float[m_nImgNum-m_nInvalidNum];
	int nCount = 0;
	for (int i = 0; i<m_vvOverlapsBetStrips.size(); i++)
	{
		for (int j = 0; j<m_vvOverlapsBetStrips[i].size(); j++)
		{
			int nStripIdx = i;
			int nIdxInstrip = j;
			dMaxOverlapX = max(dMaxOverlapX,m_vvOverlapsBetStrips[nStripIdx][nIdxInstrip]);
			dMinOverlapX = min(dMinOverlapX,m_vvOverlapsBetStrips[nStripIdx][nIdxInstrip]);
			dMaxOverlapY = max(dMaxOverlapY,m_vvOverlapsInsStrips[nStripIdx][nIdxInstrip]);
			dMinOverlapY = min(dMinOverlapY,m_vvOverlapsInsStrips[nStripIdx][nIdxInstrip]);
			dMeanOverlapX+=m_vvOverlapsBetStrips[nStripIdx][nIdxInstrip];
			dMeanOverlapY+=m_vvOverlapsInsStrips[nStripIdx][nIdxInstrip];
			*(pOverlapX+nCount)=m_vvOverlapsBetStrips[nStripIdx][nIdxInstrip]*100;
			*(pOverlapY+nCount)=m_vvOverlapsInsStrips[nStripIdx][nIdxInstrip]*100;
			*(pX+nCount)=GetImgIdx(nStripIdx,nIdxInstrip);
			nCount++;
		}
	}
	if(nCount>0)
	{
		dMeanOverlapX/=nCount;
		dMeanOverlapY/=nCount;
		sprintf(strLine,"����Ӱ������ص��Ƚ���ͳ�ƣ�");
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT);
		pdf.AddEmptyLine(2);
		sprintf(strLine,"�����ص������ֵ%3.2lf%%����Сֵ%3.2lf%%��ƽ��ֵ%3.2lf%%��",dMaxOverlapY*100,dMinOverlapY*100,dMeanOverlapY*100);
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT);
		pdf.AddEmptyLine(2);
		sprintf(strLine,"�����ص������ֵ%3.2lf%%����Сֵ%3.2lf%%��ƽ��ֵ%3.2lf%%��",dMaxOverlapX*100,dMinOverlapX*100,dMeanOverlapX*100);					
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT);
		pdf.AddEmptyLine(3);
	}
	sprintf(strLine,"�����ص��Ȳ��ϸ�< %.2lf%%��Ӱ��%d��",m_FlyQuaParaSet.fMinOverlapY,m_vecImgIdxLowOverlapInsStrips.size());
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1,0,0);
	if(m_vecImgIdxLowOverlapInsStrips.size()>0)
	{
		pdf.AddEmptyLine(1);
		nListCols=4;nListRows=m_vecImgIdxLowOverlapInsStrips.size()+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("Ӱ������");
		vecListInfo[0*nListCols+1].Format("Ӱ����");
		vecListInfo[0*nListCols+2].Format("������Ϣ");
		vecListInfo[0*nListCols+3].Format("�����ص���");
		for (int i = 0; i<nListRows-1; i++)
		{
			int nImgIdx = m_vecImgIdxLowOverlapInsStrips[i];
			double dOverlap=
				m_vvOverlapsInsStrips[m_vecImgInfo[nImgIdx].m_nStripIdx][m_vecImgInfo[nImgIdx].m_nIdxInStrip];
			vecListInfo[(i+1)*nListCols+0].Format("%d",nImgIdx);
			vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[nImgIdx].m_strImgName);
			vecListInfo[(i+1)*nListCols+2].Format("%s",m_vecImgInfo[nImgIdx].m_strStripLabel);
			vecListInfo[(i+1)*nListCols+3].Format("%.2lf%%",dOverlap*100);
		}
		cprintf("Overlap_Y...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
		pdf.AddEmptyLine(1);
		pdf.AddImage(strOverlapYErrViewImgPath,2);
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. �����ص��Ȳ��ϸ�Ӱ��ֲ�ʾ��ͼ(ע����ɫ��Ϊ����Ӱ����ɫ��Ϊ�ϸ�Ӱ��)",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(3);
	}
	FunOutPutLogInfo("�����ص���д����ɣ�");
	pdf.AddEmptyLine(3);
	sprintf(strLine,"�����ص��Ȳ��ϸ�< %.2lf%%��Ӱ��%d��",m_FlyQuaParaSet.fMinOverlapX,m_vecImgIdxLowOverlapBetStrips.size());
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1,0,0);
	char strlog[MAX_SIZE_FILE_LINE];
	// 	sprintf(strlog,"size = %d",m_vecImgIdxLowOverlapBetStrips.size());
	// 	FunOutPutLogInfo(strlog);
	if(m_vecImgIdxLowOverlapBetStrips.size()>0)
	{
		pdf.AddEmptyLine(1);
		nListCols=4;nListRows=m_vecImgIdxLowOverlapBetStrips.size()+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("Ӱ������");
		vecListInfo[0*nListCols+1].Format("Ӱ����");
		vecListInfo[0*nListCols+2].Format("������Ϣ");
		vecListInfo[0*nListCols+3].Format("�����ص���");
		for (int i = 0; i<nListRows-1; i++)
		{
			int nImgIdx = m_vecImgIdxLowOverlapBetStrips[i];
			double dOverlap=
				m_vvOverlapsBetStrips[m_vecImgInfo[nImgIdx].m_nStripIdx][m_vecImgInfo[nImgIdx].m_nIdxInStrip];
			vecListInfo[(i+1)*nListCols+0].Format("%d",nImgIdx);
			vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[nImgIdx].m_strImgName);
			vecListInfo[(i+1)*nListCols+2].Format("%s",m_vecImgInfo[nImgIdx].m_strStripLabel);
			vecListInfo[(i+1)*nListCols+3].Format("%.2lf%%",dOverlap*100);
		}
		//////////////////////////////////////////////////////////
		cprintf("Overlap_X...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
		pdf.AddEmptyLine(1.5);
		pdf.AddEmptyLine(1);
		pdf.AddImage(strOverlapXErrViewImgPath,2);
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. �����ص��Ȳ��ϸ�Ӱ��ֲ�ʾ��ͼ(ע����ɫ��Ϊ����Ӱ����ɫ��Ϊ�ϸ�Ӱ��)",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(3);
	}
	pdf.AddEmptyLine(3);
	if(nCount>0)
	{
		FunSortByX(nCount,pX,pOverlapY,pOverlapX);
		pdf.AddChart(pOverlapY,pX,nCount,0,0,1,0,0,1.0,0,0,"�����ص���","Ӱ������","�ص���(%)");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. Ӱ�����ص���ͳ��ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(1.5);
		pdf.AddChart(pOverlapX,pX,m_nImgNum-m_nInvalidNum,0,0,1,0,0,1.0,0,0,"�����ص���","Ӱ������","�ص���(%)");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. Ӱ�������ص���ͳ��ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
	}
	pdf.AddEmptyLine(4);
	delete[]pOverlapX;	
	delete[]pOverlapY;
	delete[]pX;
	FunOutPutLogInfo("�����ص���д����ɣ�");
	/***********************�����ʼ���Ϣ*************************/
	pdf.AddText("�����ʼ���Ϣ",SSW_HEI,2);
	pdf.AddEmptyLine(3);

	sprintf(strLine,"��ǲ��ϸ�> %.2lfdeg��Ӱ��%d��",m_FlyQuaParaSet.fMaxPitch,m_vecImgIdxHeiPitch.size());
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1);
	pdf.AddEmptyLine(1);
	if(m_vecImgIdxHeiPitch.size()>0)
	{	
		nListCols=3;nListRows=m_vecImgIdxHeiPitch.size()+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("Ӱ������");
		vecListInfo[0*nListCols+1].Format("Ӱ����");
		vecListInfo[0*nListCols+2].Format("��ǣ�deg��");
		for (int i = 0; i<nListRows-1; i++)
		{
			int nImgIdx = m_vecImgIdxHeiPitch[i];
			vecListInfo[(i+1)*nListCols+0].Format("%d",nImgIdx);
			vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[nImgIdx].m_strImgName);
			vecListInfo[(i+1)*nListCols+2].Format("%.2lf",m_vecImagePitch[nImgIdx]);
		}
		cprintf("PitchInfo...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
		pdf.AddEmptyLine(1);
		pdf.AddEmptyLine(1);
		pdf.AddImage(strPitchErrViewImgPath,2);
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. ��ǲ��ϸ�Ӱ��ֲ�ʾ��ͼ(ע����ɫ��Ϊ����Ӱ����ɫ��Ϊ�ϸ�Ӱ��)",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(3);
	}	
	float *pPitch = new float[m_nImgNum-m_nInvalidNum];
	pX = new float[m_nImgNum-m_nInvalidNum];
	nCount=0;
	for (int i = 0; i<m_vecImagePitch.size(); i++)
	{
		if(!m_vecImgInfo[i].m_bValid)continue;
		*(pPitch+nCount)=m_vecImagePitch[i];
		*(pX+nCount)=i;
		nCount++;
	}
	if(nCount>0)
	{
		pdf.AddChart(pPitch,pX,nCount,0,0,1,0,0,1,0,0,"Ӱ�����","Ӱ������","���ֵ��deg��");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. Ӱ���������ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(3);
	}
	FunOutPutLogInfo("���д����ɣ�");
	sprintf(strLine,"���ǲ��ϸ�> %.2lfdeg��Ӱ��%d��",m_FlyQuaParaSet.fMaxYaw,m_vecImgIdxHeiYaw.size());
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1);
	pdf.AddEmptyLine(1);
	if(m_vecImgIdxHeiYaw.size()>0)
	{
		nListCols=3;nListRows=m_vecImgIdxHeiYaw.size()+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("Ӱ������");
		vecListInfo[0*nListCols+1].Format("Ӱ����");
		vecListInfo[0*nListCols+2].Format("���ǣ�deg��");
		for (int i = 0; i<nListRows-1; i++)
		{
			int nImgIdx = m_vecImgIdxHeiYaw[i];
			vecListInfo[(i+1)*nListCols+0].Format("%d",nImgIdx);
			vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[nImgIdx].m_strImgName);
			vecListInfo[(i+1)*nListCols+2].Format("%.2lf",m_vvStripImageYaw[m_vecImgInfo[nImgIdx].m_nStripIdx][m_vecImgInfo[nImgIdx].m_nIdxInStrip]);
		}
		cprintf("YawInfo...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
		pdf.AddEmptyLine(1);
		pdf.AddImage(strYawViewImgPath,2);
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. ���ǲ��ϸ�Ӱ��ֲ�ʾ��ͼ(ע����ɫ��Ϊ����Ӱ����ɫ��Ϊ�ϸ�Ӱ��)",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
		pdf.AddEmptyLine(3);
	}
	float *pYaw = new float[m_nImgNum-m_nInvalidNum];
	memset(pYaw,0,sizeof(float)*(m_nImgNum-m_nInvalidNum));
	nCount = 0;
	for (int i = 0; i<m_vvStripImageYaw.size(); i++)
	{
		for (int j = 0; j<m_vvStripImageYaw[i].size(); j++)
		{
			int nStripIdx = i;
			int nIdxInStrip = j;
			*(pYaw+nCount)=m_vvStripImageYaw[nStripIdx][nIdxInStrip];
			*(pX+nCount)=GetImgIdx(nStripIdx,nIdxInStrip);
			nCount++;
		}
	}
	FunSortByX(nCount,pX,pYaw);
	if(nCount>0)
	{
		pdf.AddChart(pYaw,pX,m_nImgNum-m_nInvalidNum,0,0,1,0,0,1,0,0,"Ӱ������","Ӱ������","����ֵ��deg��");
		pdf.AddEmptyLine(0.5);
		sprintf(strImgLabelInfo,"ͼ%d. Ӱ����������ͼ",nImgIdx);
		pdf.AddText(strImgLabelInfo,SSW_HEI,1,SSW_CENTER);
		nImgIdx++;
	}
	delete[]pYaw;
	delete[]pPitch;
	delete[]pX;
	FunOutPutLogInfo("����д����ɣ�");
	pdf.AddEmptyLine(3);
	sprintf(strLine,"���з����������ЧӰ��%d��",m_nInvalidNum);
	pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1);
	pdf.AddEmptyLine(1);
	if(m_nInvalidNum>0) 
	{
		nListCols = 2; nListRows = m_nInvalidNum+1;
		vecListInfo.resize(nListCols*nListRows);
		vecListInfo[0*nListCols+0].Format("Ӱ������");
		vecListInfo[0*nListCols+1].Format("Ӱ����");
		int nCount =0;
		for (int i = 0; i<m_nImgNum; i++)
		{
			if(!m_vecImgInfo[i].m_bValid)
			{
				vecListInfo[(nCount+1)*nListCols+0].Format("%d",i);
				vecListInfo[(nCount+1)*nListCols+1].Format("%s",m_vecImgInfo[i].m_strImgName);
				nCount++;
			}
		}
		cprintf("InvalidImg...");
		pdf.AddList(nListCols,nListRows,vecListInfo);
		cprintf("...Over\n");
		vector<CString>().swap(vecListInfo);
	}
	FunOutPutLogInfo("���е���Ӱ��д����ɣ�");
	pdf.AddEmptyLine(3);
	if(eMethod!=BY_POS)
	{
		sprintf(strLine,"ƥ�����ѣ�����ͼƥ�����С��%d��Ӱ��%d��",m_nMatchPointNumThresold,m_vecImgIdxLowMatchPointNum.size());
		pdf.AddText(strLine,SSW_SONG,1.5,SSW_LEFT,0,false,1);
		pdf.AddEmptyLine(1);
		if(m_vecImgIdxLowMatchPointNum.size()>0)
		{
			nListCols = 3; nListRows = m_vecImgIdxLowMatchPointNum.size()+1;
			vecListInfo.resize(nListCols*nListRows);
			vecListInfo[0*nListCols+0].Format("Ӱ������");
			vecListInfo[0*nListCols+1].Format("Ӱ����");
			vecListInfo[0*nListCols+2].Format("ƥ�����");
			for (int i = 0; i<m_vecImgIdxLowMatchPointNum.size(); i++)
			{
				vecListInfo[(i+1)*nListCols+0].Format("%d",m_vecImgIdxLowMatchPointNum[i]);
				vecListInfo[(i+1)*nListCols+1].Format("%s",m_vecImgInfo[m_vecImgIdxLowMatchPointNum[i]].m_strImgName);
				vecListInfo[(i+1)*nListCols+2].Format("%d",m_vecImgInfo[m_vecImgIdxLowMatchPointNum[i]].m_ImgAdjPts.vecAdjPtIdx.size());
			}
			cprintf("MatchErr...");
			pdf.AddList(nListCols,nListRows,vecListInfo);
			cprintf("...Over\n");
			vector<CString>().swap(vecListInfo);
			pdf.AddEmptyLine(1);
		}
		pdf.AddEmptyLine(3);
		FunOutPutLogInfo("ƥ������Ӱ��д����ɣ�");
	}
	/***********************�����ʼ���Ϣ*************************/
	pdf.AddText("�ʼ촦���ʱͳ��",SSW_HEI,2);
	pdf.AddEmptyLine(1);
	clock_t t2 = clock();
	double dt = (double)(t2-t1)/CLOCKS_PER_SEC;
	m_dTimeSecsWriteChkRep=dt;
	m_dTimeSecsWholeProcess=/*m_dTimeSecsCmrDistortion+m_dTimeSecsBundleAdjustment+*/m_dTimeSecsTimeReadAdjRep+m_dTimeSecsRunFlyQuaChk+m_dTimeSecsWriteChkRep;
	nListCols = 2; nListRows = 5;
	vecListInfo.resize(nListCols*nListRows);
	vecListInfo[0*nListCols+0].Format("�������");
	vecListInfo[0*nListCols+1].Format("��ʱ-s");
	vecListInfo[1*nListCols+0].Format("��������");
	vecListInfo[1*nListCols+1].Format("%.3lf",m_dTimeSecsWholeProcess);
// 	vecListInfo[2*nListCols+0].Format("�������");
// 	vecListInfo[2*nListCols+1].Format("%.3lf",m_dTimeSecsCmrDistortion);
// 	vecListInfo[3*nListCols+0].Format("ƥ��ƽ��");
// 	vecListInfo[3*nListCols+1].Format("%.3lf",m_dTimeSecsBundleAdjustment);
	vecListInfo[2*nListCols+0].Format("����ƽ���");
	vecListInfo[2*nListCols+1].Format("%.3lf",m_dTimeSecsTimeReadAdjRep);
	vecListInfo[3*nListCols+0].Format("�����ʼ����");
	vecListInfo[3*nListCols+1].Format("%.3lf",m_dTimeSecsRunFlyQuaChk);
	vecListInfo[4*nListCols+0].Format("�����ʼ챨��");
	vecListInfo[4*nListCols+1].Format("%.3lf",m_dTimeSecsWriteChkRep);
	cprintf("TimeInfo...");
	bool b = pdf.AddList(nListCols,nListRows,vecListInfo);
	cprintf("...Over\n");
	vector<CString>().swap(vecListInfo);
	pdf.AddEmptyLine(1);
	FunOutPutLogInfo("����д����ϣ�");
	CString strPath = m_strFlyQuaChkRepPath;

	if(!pdf.SavePdf(strPath))
		return false;
	delete[]strLine;
	return true;
}



CMemTxtFile::CMemTxtFile()
{

}
CMemTxtFile::~CMemTxtFile()
{
	CloseFile();
}
bool CMemTxtFile::OpenFile(CString strFilePath)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_hFileMap = NULL;
	m_lpbFile = NULL;
	m_dwFileSize=0;
	if(!PathFileExists(strFilePath)) return false;
	m_hFile=CreateFile(strFilePath, //·��
		GENERIC_READ,//��
		FILE_SHARE_READ, //����ֻ��
		NULL,//��ȫ����
		OPEN_EXISTING,//�ļ��������
		FILE_FLAG_SEQUENTIAL_SCAN,//����������ʶ��ļ���������Ż�
		NULL);  //�����Ϊ0����ָ��һ���ļ����
	if(INVALID_HANDLE_VALUE==m_hFile) return false;
	m_dwFileSize = ::GetFileSize(m_hFile,NULL);
	m_hFileMap=CreateFileMapping(m_hFile,
		NULL,//��ȫ����
		PAGE_READONLY,//ֻ����ʽ��ӳ��
		0,
		0,//�ô����ļ�ʵ�ʳ���
		NULL);//ָ���ļ�ӳ����������
	if(NULL==m_hFileMap) return false;
	m_lpbFile=(char*)MapViewOfFile(
		m_hFileMap,
		FILE_MAP_READ,
		0,
		0,//ӳ�������ļ�ӳ�����
		0);//0��ʾ����WindowsѰ�ҵ�ַ
	if(NULL==m_lpbFile) return false;
	return true;
}
void CMemTxtFile::CloseFile()
{
	if(m_lpbFile!=NULL) UnmapViewOfFile(m_lpbFile);
	if(m_hFileMap!=NULL) CloseHandle(m_hFileMap);
	if(m_hFile!=INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	m_hFileMap = NULL;
	m_lpbFile = NULL;
	m_dwFileSize=0;
}
