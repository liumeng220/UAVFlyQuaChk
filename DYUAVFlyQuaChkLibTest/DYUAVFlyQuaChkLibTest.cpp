// DYUAVFlyQuaChkLibTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DYUAVFlyQuaChkPro/DYUAVFlyQuaChkPro.h"

void FunCallback4FlyQua(const char *pInfoMain, float fPercentMain)
{
	char strLog[1024];
	sprintf(strLog, "%s==> %.2lf\n",pInfoMain,fPercentMain);
	printf(strLog);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=3) 
	{
		printf("��������\n����1�������ļ�·��\n����2���������·��\n");
		return -1;
	}
	DYstuSysConfigPara para;
	para.fMinOverlapX=8;
	para.fMinOverlapY=53;
	para.fMaxYaw = 5;
	DYUpdateCfgFile(&para);

	if(!DYUAVFlyQuaChkPro(argv[1],argv[2],FunCallback4FlyQua)) printf("Error!");
	return 0;
}

