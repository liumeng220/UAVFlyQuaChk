// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_)
#define AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"
//#include "..\\a_Initial\\GeoArithmetic.h"
#define BOOL bool
#define FALSE false
#define TRUE true

//#define max(a,b) ((a) > (b) ? (a) : (b))

#pragma warning(disable : 4018)
#pragma warning(disable : 4305)

struct matrixN
{
	double BTA[3][6],ATB[3][6];
	double NIBTNBU[6];
};

class CMatrix
{
public:
	CMatrix();
	virtual ~CMatrix() ;
	//virtual ~CMatrix() = 0;//2015-12-26,xxw,
//	BOOL FirstTime;
	BOOL ExportCoviaranceMatrix; 

	void MatrixPrint(double *a, int m, int n);//2016-01-04,xxw

	//���þ������㺯��
	BOOL MatrixInversion_General(double *a, int n);
	int  MatrixInversion(double *a, int n);
	void MatrixInvResult(double *N, double *L, double *X, int n);
	BOOL MatrixTranspose(double * A ,double * AT, int M , int N );
	BOOL MatrixMulti(double *a, double *b, double *c, int m, int n, int k);
	void MultiplyMatrixWeight(double *A, double *P, double* AP,int m, int n);
	BOOL MatrixABAT (double * A, double * B , double * C, int M, int N);
	BOOL PartitionMatrixInversion(double *N,double *L,double *X, int photoNumber,int Cmr_ParaNum,int ValidSpcPtNum, int Spc_ParaNum, int IntrinsicParaNum);

	//������ϵ�������� �ⷽλԪ�� �� �ռ�����δ֪�� ���ʱ�ķֿ����淨
	void FourPartMatrixInversion(double *NN, double *L, double *X, int photoNumber,int Cmr_ParaNum,int ValidSpcPtNum, int Spc_ParaNum);
	
	//�����ξ����Ϊ2*2�Ӿ�������ͨ�ó���
	void Sub2by2Matrix_Inversion(double *N,int first,int second);


	//��̬�������
	void Ill_Condition_Matrix_Sovle(double *N, double *L, double *X,double *Qxx, int n,int times, double espAngle,double espPoint);
	//�������������㺯����
	double Matrix_Condition(double *matrix,int m, int n);

	
	//AX = 0 �Ľ�ΪA����С����ֵ��Ӧ��������������V����ֵΪ��������ת�ã��ʶ���������������
	BOOL  SingularValueDecomposition(double *a, int m,int  n, double *u,double * v, double eps, int ka);

	void a_ppp(double *a, double *e, double *s, double *v,int m, int n);
	void a_sss(double fg[2], double cs[2]);

	void Zero(double *p, int n);
	void  Nrml (double*aa,int n,double bb,double*a,double*b, double p=1);
	void Solve(double*a,double*b,double*x,int n,int wide);

//////////////////////////////////////////////////////
//
//�������
//
//a����Ϊm*n�ף�bΪp*q�ף�С�����,
//��b����a�ĵ�(r,c)��Ԫ�ش�(r=0,1����m-1��c=0,1,����n-1)
//
//////////////////////////////////////////////////////
	void FillMatrix(double *a,double *b,int m,int n,int p,int q,int r,int c);

//////////////////////////////////////////////////////
//
//������� ------  ���
//
//a����Ϊm*n�ף�bΪp*q�ף�С�����,
//��b����a�ĵ�(r,c)��Ԫ�ش�(r=0,1����m-1��c=0,1,����n-1)
//
//////////////////////////////////////////////////////
	void FillMatrix_MINUS(double *a,double *b,int m,int n,int p,int q,int r,int c);

/////////////////////////////////////////////////////////////
//��  �ܣ��������
//��  �룺����a��mxn��)���Ͼ���b��mxn�ף����ھ���a��mxn�ף�
//��  ����void������ھ���a��
/////////////////////////////////////////////////////////////
	void MatrixAdd(double *a, double *b,int m, int n);

private:
	void Ldltban1(double*a,double*d,double*l,int n,int wide);
	void Ldltban2 (double*l,double*d,double*b,double*x,int n,int wide);

};

#endif // !defined(AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_)
