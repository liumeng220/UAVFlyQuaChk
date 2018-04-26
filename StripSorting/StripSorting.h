#ifndef strip_sorting_h
#define strip_sorting_h
#pragma once
#include <vector>
using namespace std;
#ifdef  SSWUAVFLYQUA_EXPORTS
#define SSWUAVFLY __declspec(dllexport)
#else
#define SSWUAVFLY __declspec(dllimport)
#endif
struct SSWUAVFLY stuPosInfo
{
	double Xs,Ys,Zs;
	double phi,omg,kap;
	int nStripIdx;   //�������
	int nIdxInStrip; //������
	bool bValid;     //�Ƿ���봦��  2017-03-22
};
/************************************************************************/
/*�����ֻ�ֱ�����ź���
ֱ�߷��̣�
ax+by+c=0;
����������
line_a,line_b,line_c��Ӧֱ�߷�����������
vecPosInfo POS����
/************************************************************************/

void SSWUAVFLY strip_sorting(double line_a, double line_b, double line_c,vector<stuPosInfo>& vecPosInfo);


#endif

