// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
//#include <windows.h>
#ifdef _DEBUG
#pragma comment (lib,"../lib/Debug/x64/libharu.lib")
#else
#pragma comment (lib,"../lib/Release/x64/libharu.lib")
#endif
#include "setjmp.h"
#include "hpdf.h"

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
