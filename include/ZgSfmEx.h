#ifndef  ZG_SFM_EX_H_19274
#define  ZG_SFM_EX_H_19274

#  ifdef ZG_BUILD_DLL
#      define ZG_EXPORT  __declspec(dllexport)
#  else
#      define ZG_EXPORT  __declspec(dllimport)
#endif

/// �����û���κζ��壬ֻ��Ϊ����VS2010��Class View�п�����ʾ ZGSiftEx��
/// ��������ҵ�����ļ�
class ZgSfmEx
{
public:
    ZgSfmEx(void);
    ~ZgSfmEx(void);
};

/// ����ص����������������Ϣ
typedef void (CALLBACK* ZG_CallBack)(const char *pInfoMain, float nPercentMain, const char *pInfoSub, float nPercentSub);

/// ������һ��ʾ����ע�͵��ˣ���Ϊͷ�ļ�����ò�Ҫ�к������塣
//void CALLBACK SfmCallBack_example(const char *pInfoMain, float nPercentMain, const char *pInfoSub, float nPercentSub)
//{
//    // ZhongGuan Note : you can put information into your CListBox or CProgressBar.
//    // We promise that nPercentMain and nPercentSub belong in [0.00 ~ 100.00].
//    if (pInfoMain)
//    {
//        printf("�����ȣ�%.2f%%  %s\n",
//            nPercentMain, pInfoMain);
//    }
//    if (pInfoSub)
//    {
//        printf("�ӽ��ȣ�%.2f%%  %s\n",
//            nPercentSub, pInfoSub);
//    }
//    // ע�⣺��Ҫ����������κ�ֻ���ڽ����߳��е��õĺ�������Invalidate(FALSE)��
//    // �����Ҫˢ�½��棬��ʹ��::PostMessage����������Ϣ��
//}
#ifndef CAM_LEVEL_DEFINE_ZG
#define CAM_LEVEL_DEFINE_ZG
#define Cam_Level0     0
#define Cam_Level1     1
#define Cam_Level2     2
#define Cam_Level3_1   3
#define Cam_Level3_2   4
#define Cam_Level4     5
#define Cam_Level5_1   6
#define Cam_Level5_2   7
#define Cam_Level6     8

#define CamParamNum_Level0     1
#define CamParamNum_Level1     3
#define CamParamNum_Level2     5
#define CamParamNum_Level3_1   6
#define CamParamNum_Level3_2   7
#define CamParamNum_Level4     8
#define CamParamNum_Level5_1   9
#define CamParamNum_Level5_2   10
#define CamParamNum_Level6     11
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// ����0��ζ�ųɹ�������ʧ�ܣ�
ZG_EXPORT int Sfm_ImageList(
    bool  bEnglish,
    char  *strImageListFilename,   /// *\imageList.txt
    char  *strMatchZgoutFilename,  /// *\Freenet_Match.zgout��ƥ�䴮���ļ����������Զ��ģ�Ҳ�������ֶ�׷�ӵ�
    char  *strResultFilename,      /// BundleAdjustment\*; *\Freenet.zgout
    char  *strGPSFilename = NULL,  /// GPS�ع����ļ������ڸ���ƽ��
    int    nThreadsNum = 8,        /// �߳���
    int    nInitPair1 = -1,        /// ��ʼ�����Ϊ-1����ζ���Զ�ѡ���ʼ���
    int    nInitPair2 = -1,
    bool  bFixedCameraInnerPara = true,  /// �̶�����ڲ���
    ZG_CallBack outputInfo = NULL,       /// �ص����������������Ϣ
    char  *strOldResultFilename = NULL,  /// *\*.zgout�ļ����ɵĽ����������ھɵĽ���������
    char * strLogFilename = NULL         /// Log File
    );

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//  Return 0 if succeed, and return other value if failed.
ZG_EXPORT int CSba_zgout(
    bool  bEnglish,
    char  *strImageListFilename,   /// *\imageList.txt
    char  *strSBA_InitFilename,    /// *\Freenet.zgout
    char  *strSBA_FinalFilename,   /// *\Freenet_SBA.zgout
    char  *strCtrlPointsFilename,  /// *\ControlPoints
    char  *strGPSFilename = NULL,  /// *\POS, GPS�ع����ļ������ڸ���ƽ��
    int   nThreadsNum = 8,         /// �߳���
    bool  bFixedCameraInnerPara = true,  /// �̶�����ڲ���
    int   nCameraInnerParaLevel = Cam_Level5_2,
    double dGPS_Precise = 1.0,       /// GPS�����龫�ȣ���λ����
    double dINS_Precise = 0.1,       /// INS�����龫�ȣ���λ�Ƕ�
    double dControl_Precise = 0.01,  /// ���Ƶ�����龫�ȣ���λ����
    bool  bTrustRegion = true,
    int   nMaxIterNum  = 100,
    ZG_CallBack outputInfo = NULL,       /// �ص����������������Ϣ
    char * strLogFilename = NULL,         /// Log 
    double IgnoreGPS_LargeThan_ThisRatio = 3.0  /// ����GPS�д������������
    );
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ZG_EXPORT void  Output_ControlPoint_Report(
    bool  bEnglish,
    char  *strImageListFilename,   /// *\imageList.txt
    char  *strSBA_FinalFilename,   /// *\Freenet_SBA.zgout
    char  *strCtrlPointsFilename,  /// *\ControlPoints
    char  *strOutputReport,        /// *\Report.txt
    int   nCameraInnerParaLevel = Cam_Level5_2
    );



#endif
