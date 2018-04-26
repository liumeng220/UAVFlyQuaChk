#ifndef ZG_SIFT_EX_H_68013
#define ZG_SIFT_EX_H_68013

#  ifdef ZG_BUILD_DLL
#      define ZG_EXPORT  __declspec(dllexport)
#  else
#      define ZG_EXPORT  __declspec(dllimport)
#endif

/// �����û���κζ��壬ֻ��Ϊ����VS2010��Class View�п�����ʾ ZGSiftEx��
/// ��������ҵ�����ļ�
class ZG_EXPORT ZGSiftEx
{
public:
    ZGSiftEx(void);
    ~ZGSiftEx(void);
};

/// ����ص����������������Ϣ
typedef void (CALLBACK* ZG_CallBack)(const char *pInfoMain, float nPercentMain, const char *pInfoSub, float nPercentSub);

/// ������һ��ʾ����ע�͵��ˣ���Ϊͷ�ļ�����ò�Ҫ�к������塣
//void CALLBACK SiftCallBack_example(const char *pInfoMain, float nPercentMain, const char *pInfoSub, float nPercentSub)
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// ��������ȡ�����Զ���������Ҫ���裡
#define IMAGE_LAYER_ORIGIN  0x1   /// ���� 5616*3744  -> ��߾��ȣ���������4D��Ʒ�Ƽ���
                                  /// ��С֮��ֱ���:
#define IMAGE_LAYER_S_2x2   0x2   ///     2808*1872  -> �����еȣ��ٶ��еȣ�����ָ����⣡
#define IMAGE_LAYER_S_4x4   0x4   ///     1404*936   -> ����ٶȣ����ȵͣ���ƴ�Ƽ���
#define IMAGE_LAYER_S_8x8   0x8   ///      702*468   -> ͼƬ̫С�ˣ�ǿ�Ҳ��Ƽ�������

/// ������������������ǵ���Ӱ���������ȡ
/// ����ɹ��򷵻�0�� ����ʧ�ܡ�
ZG_EXPORT int SiftFeatureDetector_SingleImage(
    bool  bEnglish,
    char  *strImageFilename,               /// Ӱ����
    char  *strKeypointPath,                /// ������Ĵ���ļ���
    int   nThreadsNum = 8,                 /// �߳�����Ĭ����8���߳�
    int   nStartLayer = 1,                 /// ��ʼ������1����ԭͼ��2������С2x2����4������С4x4��
    int   nNMS_Window = 50                 /// NMS�ֲ���������ƣ��������������������
    );

/// ������������������Ƕ��Ӱ���������ȡ��ʹ�����߳��Ż�����������ٶȸ��졣
/// ����ɹ��򷵻�0�� ����ʧ�ܡ�
ZG_EXPORT int SiftFeatureDetector_ImageList(
    bool  bEnglish,
    char  *strImageListFilename,           /// Ӱ���б�
    char  *strImagePath,                   /// Ӱ����ļ���
    char  *strKeypointPath,                /// ������Ĵ���ļ���
    char  *strThumbnailPath = NULL,        /// ����ͼ����·��
    int   nThreadsNum = 8,                 /// �߳�����Ĭ����8���߳�
    int   nStartLayer = 1,                 /// ��ʼ������1����ԭͼ��2������С2x2����4������С4x4��
    int   nNMS_Window = 50,                /// NMS�ֲ���������ƣ��������������������
    bool  bDoNotDetectIfKeyExist = false,  /// ���key�ļ����ڣ��Ͳ���������ȡ
    ZG_CallBack outputInfo = NULL,         /// Callback
    char * strLogFilename = NULL           /// Log File
    );
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// ������ƥ�䴮�㡱���Զ���������Ҫ���裡

/// ������������������ǡ�����ƥ�䡱
/// ����ɹ��򷵻�0�� ����ʧ�ܡ�
ZG_EXPORT int SiftMatch_ImageList(
    bool  bEnglish,
    char  *strImageListFilename,           /// Ӱ���б�
    char  *strKeypointPath,                /// ������Ĵ���ļ���
    char  *strMatchMatrixFilename,         /// ƥ�����txt�ļ�����¼��N*N��1��0, 1����ƥ��
    char  *strMatchResultFilename,         /// ƥ��Ľ���ļ�
    int   nThreadsNum = 8,                 /// �߳���
    bool  bDoGeometryCheck = true,         /// ���μ�飬ͨ���� F ����
    float fTolerateDistance = 10.0f,       /// ���̵� F ���������ֵ��Ĭ����10�����أ����û�о������䣬��Ҫ���ô�һ�㡣
    ZG_CallBack outputInfo = NULL,         /// Callback
    char * strLogFilename = NULL           /// Log File
    );

/// ������������������ǡ����㡱
/// ����ɹ��򷵻�0�� ����ʧ�ܡ�
ZG_EXPORT int MatchResult_Choose_To_ZGOUT(
    bool  bEnglish,
    char  *strImageListFilename,           /// Ӱ���б�
    char  *strKeypointPath,                /// ������Ĵ���ļ���
    char  *strMatchResultFilename,         /// ƥ��Ľ���ļ�
    char  *strZGOUT_MatchFilename,         /// ZG_OUT ��ʽ��ƥ�����ļ�
    int  nPickPointGridXnum = 21,          /// Ĭ������21*21�����ӣ�ÿ������5����
    int  nPickPointGridYnum = 21,
    int  nPickPointNumPerGrid = 5         /// ������κ�һ�����0�������㣬����ȫ����
    );
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#endif