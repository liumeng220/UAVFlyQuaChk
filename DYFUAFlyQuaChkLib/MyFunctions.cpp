#include "StdAfx.h"
#include "MyFunctions.h"
#include <io.h>

bool FunSaveSysConfigFile(DYstuSysConfigPara& sysCfgPara)
{
	CString strCfgFilePath = FunGetSysCfgPath(); 
	strCfgFilePath = FunGetFileFolder(FunGetThisExePath())+"\\"+FunGetFileName(FunGetThisExePath(),false)+"_FlyQua.ini";
	FILE* pfW = fopen(strCfgFilePath,"w");

	if(pfW == NULL) 
	{
		FunOutPutLogInfo("ϵͳ������Ϣ����ʧ�ܣ��ļ��򿪴���");
		return false;
	}
	fprintf(pfW,"*SysCfg_Para*\n");
	fprintf(pfW,"[ValidJudgeTimesOfRms=float]               %.3f\n",sysCfgPara.fValidJudgeTimesOfRms);
	fprintf(pfW,"[Min_MatchPointNum2Pass=uint]              %d\n",sysCfgPara.nMatchPtNumThresold);
	fprintf(pfW,"[Max_ErrOfStationPane=float]               %.3f\n",sysCfgPara.fMaxErrStationPane);
	fprintf(pfW,"[Rms_ErrOfStationPane=float]               %.3f\n",sysCfgPara.fRmsErrStationPane);
	fprintf(pfW,"[Min_overlap_y=float]                      %.3f\n",sysCfgPara.fMinOverlapY);
	fprintf(pfW,"[Min_overlap_x=float]                      %.3f\n",sysCfgPara.fMinOverlapX);
	fprintf(pfW,"[Max_hei_dif_in_area=float]                %.3f\n",sysCfgPara.fMaxHeiDifInArea);
	fprintf(pfW,"[Max_hei_dif_link_in_strip=float]          %.3f\n",sysCfgPara.fMaxHeiDifLinkInStrip);
	fprintf(pfW,"[MAX_strip_blend=float]                    %.3f\n",sysCfgPara.fMaxStripBlend);
	fprintf(pfW,"[Max_pitch=float]                          %.3f\n",sysCfgPara.fMaxPitch);
	fprintf(pfW,"[Max_pitch10_percent=float]                %.3f\n",sysCfgPara.fMaxPitch10Per);
	fprintf(pfW,"[Max_yaw=float]                            %.3f\n",sysCfgPara.fMaxYaw);
	fprintf(pfW,"[Max_yaw15_percent=float]                  %.3f\n",sysCfgPara.fMaxYaw15Per);
	fprintf(pfW,"[Max_yaw20_number=uint]                    %d\n",sysCfgPara.nMaxYaw20Num);
	fprintf(pfW,"[Max_MissArea_ratio=float]                 %.3f\n",sysCfgPara.fMissAreaRatio);
	fprintf(pfW,"[Max_AngleErr_ratio=float]                 %.3f\n",sysCfgPara.fAngleErrRatio);
	FunOutPutLogInfo(CString("ϵͳ�����ļ�����ɹ���"));
	fclose(pfW); pfW = NULL;
	return true;
}
bool FunReadSysConfigFile(DYstuSysConfigPara& sysCfgPara)
{
	CString strCfgFilePath= FunGetSysCfgPath();
	strCfgFilePath = FunGetFileFolder(FunGetThisExePath())+"\\"+FunGetFileName(FunGetThisExePath(),false)+"_FlyQua.ini";
	FILE* pfR = fopen(strCfgFilePath,"r");
	if(pfR == NULL)
	{
		FunOutPutLogInfo("ϵͳϵͳ�����ļ���ȡʧ�ܣ��ļ��򿪴���");
		return false;
	}
	char line[MAX_SIZE_FILE_LINE];
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	if(strcmp(line,"*SysCfg_Para*")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���1�д���*SysCfg_Para*����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	int nParts;  double dat = 0;
	char strHead[MAX_SIZE_FILE_LINE]; memset(strHead,0,MAX_SIZE_FILE_LINE);
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[ValidJudgeTimesOfRms=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���2�д���[ValidJudgeTimesOfRms=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fValidJudgeTimesOfRms=dat;
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Min_MatchPointNum2Pass=uint]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���3�д���[Min_MatchPointNum2Pass=uint] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.nMatchPtNumThresold=(int)dat;
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_ErrOfStationPane=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���4�д���[MAX_ErrOfStationPane=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxErrStationPane=dat;
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Rms_ErrOfStationPane=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���5�д���[Rms_ErrOfStationPane=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fRmsErrStationPane=dat;
	

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Min_overlap_y=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���6�д���[Min_overlap_y=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMinOverlapY=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Min_overlap_x=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���7�д���[Min_overlap_x=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMinOverlapX=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_hei_dif_in_area=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���8�д���[Max_hei_dif_in_area=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxHeiDifInArea=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_hei_dif_link_in_strip=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���9�д���[Max_hei_dif_link_in_strip=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxHeiDifLinkInStrip=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		return false;
		fclose(pfR); pfR=NULL;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[MAX_strip_blend=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���10�д���[MAX_strip_blend=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxStripBlend=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_pitch=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���11�д���[Max_pitch=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxPitch=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_pitch10_percent=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���12�д���[Max_pitch10_percent=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxPitch10Per=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_yaw=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���13�д���[Max_yaw=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxYaw=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		fclose(pfR); pfR=NULL;
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_yaw15_percent=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���14�д���[Max_yaw15_percent=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMaxYaw15Per=dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		fclose(pfR); pfR=NULL;  
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_yaw20_number=uint]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���15�д���[Max_yaw20_number=uint] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.nMaxYaw20Num=(int)dat;

	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		fclose(pfR); pfR=NULL;  
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_MissArea_ratio=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���16�д���[Max_MissArea_ratio=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fMissAreaRatio=dat;
	if(!fgets(line,MAX_SIZE_FILE_LINE,pfR))
	{
		fclose(pfR); pfR=NULL;  
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���������");
		return false;
	}
	line[strlen(line)-1] = '\0';
	nParts=sscanf(line,"%s %lf",strHead, &dat);
	if(nParts!=2||strcmp(strHead,"[Max_AngleErr_ratio=float]")!=0)
	{
		FunOutPutLogInfo("ϵͳ�����ļ���ȡʧ�ܣ��ļ���16�д���[Max_AngleErr_ratio=float] value����");
		fclose(pfR); pfR=NULL;
		return false;
	}
	sysCfgPara.fAngleErrRatio=dat;

	fclose(pfR); pfR=NULL;
	FunOutPutLogInfo("ϵͳ�����ļ���ȡ�ɹ���");
	return true;
}

bool FunRemoveDirAndFile(CString strFolderPath)
{
//	CString strfBat = FunGetFileFolder(FunGetThisExePath())+"\\RemoveBat.bat";
//	FILE *fBat = fopen(strfBat,"w");
//	CString strDeletecmd = "rd \/s \/q "+ strFolderPath+"\\";
//	fprintf(fBat,"%s",strDeletecmd);
//	fclose(fBat);
//	CString strVbs = FunGetFileFolder(strfBat)+"\\RemoveBat.vbs";
 //	system(strDeletecmd);
//	char strCmd[1024]; strcpy(strCmd,strDeletecmd);
	CString  strDeletecmd= "/c rd "+strFolderPath+" /s /q";
//	ShellExecute(NULL, NULL, "cmd.exe", strDeletecmd, NULL, SW_HIDE);
// 	STARTUPINFO   si; PROCESS_INFORMATION   pi;
// 	ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si); ZeroMemory(&pi, sizeof(pi));
//  	if (!CreateProcess(NULL, /*NULL*/strCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
// 	{
// 		int n = GetLastError();
// 		return FALSE;
// 	}
	while(PathFileExists(strFolderPath)) 
		Sleep(10);
	return TRUE;
}


void FunVectorPt2VertexXY(void *pVector, int nDim, double *&pV)
{
	if(nDim==2)
	{
		vector<Point2D> vecPt2d = *(vector<Point2D>*)pVector;
		for (int i = 0; i<vecPt2d.size(); i++)
		{
			*(pV+2*i+0) = vecPt2d[i].x;
			*(pV+2*i+1) = vecPt2d[i].y;
		}
		// 		*(pV+2*vecPt2d.size()+0) = vecPt2d[0].x;
		// 		*(pV+2*vecPt2d.size()+1) = vecPt2d[0].y;
		vector<Point2D>().swap(vecPt2d);
	}
	else if(nDim==3)
	{
		vector<Point3D> vecPt3d = *(vector<Point3D>*)pVector;
		for (int i = 0; i<vecPt3d.size(); i++)
		{
			*(pV+2*i+0) = vecPt3d[i].X;
			*(pV+2*i+1) = vecPt3d[i].Y;
		}
		// 		*(pV+2*vecPt3d.size()+0) = vecPt3d[0].X;
		// 		*(pV+2*vecPt3d.size()+1) = vecPt3d[0].Y;
		vector<Point3D>().swap(vecPt3d);
	}
}
void FunCreateVertexColor(int nPtNum, double r,double g,double b, double *&pC)
{
	for (int i = 0; i<nPtNum; i++)
	{
		*(pC+i*4+0)=r;
		*(pC+i*4+1)=g;
		*(pC+i*4+2)=b;
		*(pC+i*4+3)=0.5*255;
	}
}

Point2D ptCln2Geo(Point2D ptClnCenter2Geo, float fScale, CRect rectCln, CPoint& ptCln)
{
	Point2D ptGeo;
	ptGeo.x = ptClnCenter2Geo.x + 1.0*(ptCln.x - rectCln.CenterPoint().x) / fScale;
	ptGeo.y = ptClnCenter2Geo.y - 1.0*(ptCln.y - rectCln.CenterPoint().y) / fScale;
	return ptGeo;
}
CPoint ptGeo2Cln(Point2D ptClnCenter2Geo, float fScale, CRect rectCln, Point2D& ptGeo)
{
	CPoint ptCln;
	ptCln.x = (ptGeo.x - ptClnCenter2Geo.x) * fScale + rectCln.CenterPoint().x;
	ptCln.y = (ptClnCenter2Geo.y - ptGeo.y) * fScale + rectCln.CenterPoint().y;
	return ptCln;
}
void CalLineParas(void* p1, void* p2, int nDims, float &a, float &b, float &c)
{
	if(nDims<2||nDims>3) return;
	CPoint pt1,pt2;
	if(nDims==2) 
	{
		pt1 = *(CPoint*)p1;
		pt2 = *(CPoint*)p2;
	}else
	{
		if(nDims==3)
		{
			pt1 = CPoint(((Point3D*)p1)->X, ((Point3D*)p1)->Y);
			pt2 = CPoint(((Point3D*)p2)->X, ((Point3D*)p2)->Y);
		}
	}
	if (pt1.x == pt2.x)
	{
		b = 0;
		a = 1;
		c = -pt1.x;
	}
	else
	{
		b = 1;
		a = -1.0*(pt1.y -pt2.y) / (pt1.x - pt2.x);
		c = -1.0*a*pt1.x - pt1.y;
	}
}
double LineY2X(double y, float a, float b, float c)
{
	if(a==0) return 0;
	return -(b*y+c)/a;
}
