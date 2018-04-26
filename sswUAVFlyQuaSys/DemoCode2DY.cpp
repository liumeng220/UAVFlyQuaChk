//��������زο�����
//��Ҫʹ��gdal��ogr
#include "gdal201/gdal.h"
#include "gdal201/gdal_priv.h"
#include "gdal201/ogrsf_frmts.h"
#include "gdal201/ogr_spatialref.h"

/************************************************************************/
/* �ڵ㹹������                                                                     */
//vecPoint->point[]
/************************************************************************/
//pVertex: point[]
//nCount: �����
//���ض���ζ���
OGRPolygon* FunCreatePolygon(void* pVertex,  int nCount, int nDim = 2)
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

/************************************************************************/
/* �������pos����                                                                     */
/************************************************************************/

OGRPolygon * pPolygon = FunCreatePolygon(void* pVertex,  int nCount, int nDim = 2) ; //���ɶ����
for(int i = 0; i<vecPos.size(); i++) //����POS��
{
	OGRPoint ogrPt;  //ogrPos��
	ogrPt.setX(vecPos[i]..Xs);
	ogrPt.setY(vecPos[i]..Ys);

	OGRGeometry *pInet=NULL;   // �󽻽��

		pInet=pPolygon->Intersection(&ogrPt);  //����������
		if(pInet==NULL) 
			{
				//Ϊ��ѡ�����ö�Ӧ����
		}
		CString strName = pInet->getGeometryName();
		if(strName=="POINT")   
		{
			//��ѡ�����ö�Ӧ����
}

/************************************************************************/
/* VBO����                                                                     */
/************************************************************************/
		//����
		CreateVectorVBO(int _nVertexNum, sswVectorType _eType = vLine, bool _bSupportVBO = true);   //����ʸ������VBO

		FillVectorVBO(GLdouble *_pV, GLdouble* _pColor, int _nVertexNum);//���VBO����
		Display(float nSize = 1,int nGap = 0);
		DeleteVBO();
