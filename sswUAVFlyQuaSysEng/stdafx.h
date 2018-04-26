// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��
#include <afxdisp.h>
#include <afxdisp.h>



//�ַ�����ǩ
//#define SOFTWARE_LABEL "@2016 UAV Flight Quality Check System by SSW Group"
#define SOFTWARE_LABEL "@2016 UAV Flight Quality Check System"
//��ʱ�����
#define TIME_ID_SYSTIME 1
#define TIME_ID_IMGPOSMATCH 2
#define TIME_ID_IMGMATCHADJ 3
//�Զ�����Ϣ
#define ID_MSG_IMGPOSMATCH          (WM_USER+1000)
#define ID_MSG_UPDATE_STATUSLABEL   (WM_USER+1001)
#define ID_MSG_RESORT_STRIPS        (WM_USER+1002)
#define ID_MSG_FILL_WND_LOG         (WM_USER+1003)
#define ID_MSG_CREATE_TEXTURES      (WM_USER+1004)
#define ID_MSG_CREATE_ADJPTVBO      (WM_USER+1005)
#define ID_MSG_FILL_DATACHART       (WM_USER+1006)
#define ID_MSG_CREATE_MISSRENDER    (WM_USER+1007)
#define ID_MSG_FILL_REPRENDER       (WM_USER+1008)
#define ID_MSG_SAVE_REPORT          (WM_USER+1009)
#define ID_MSG_OPEN_REPORT          (WM_USER+1010)
#define ID_MSG_SHOW_PDF             (WM_USER+1011)
#define ID_MSG_WNDPROGRESSCTRL_POS  (WM_USER+1012)
#define ID_MSG_WNDPROGRESSCTRL_SHOW (WM_USER+1013)
#define ID_MSG_SET_IMGVALID_STATE   (WM_USER+1014)
#define ID_MSG_CREATE_SELECTRENDER  (WM_USER+1015)
#define SIZE_IMG_TEXTURE  256

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//overlap color list
//0   250 240 230
//1   205 198 115
//2   255 255 0
//3   255 140 0
//4   50 205 50
//5   0 100 0
//11  0 0 205