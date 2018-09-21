/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  TClientAPI.h
* @brief
*       ���ܣ�
*       <li>��TClient����ӿ�</li>
*/


#ifndef TCLIENT_INCLUDE
#define TCLIENT_INCLUDE

/************************ͷ�ļ�********************************/

/************************�궨��********************************/
#define UC      unsigned char
#define US      unsigned short
#define UI       unsigned int

//������̬������
#define DLL_FUNC_INT extern "C" __declspec(dllexport) int
#define DLL_FUNC_SHORT extern "C" __declspec(dllexport) short
#define DLL_FUNC_VOID extern "C" __declspec(dllexport) void

const int LISTEN_PORT = 8000;    //ts����TClient�Ķ˿�
#define SERVER_WAY_TCP          0                                //tcp
#define SERVER_WAY_UDP            1                                //udp
#define SERVER_WAY_UART            2                                //����
#define SERVER_WAY_USB          3                                //usb
#define SERVER_WAY_DLL          4                                //��̬��

#define REPORT_LOCATION_SIZE    16                                //��״̬��С
#define REPORT_MSG_SIZE         128                                //����Ϣ��С
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
/*  Modified brief: �򴮿ں�����256�����������Ƴ��ȸ�Ϊ7λ*/
#define UART_NAME_BUF_LEN        7                               //�������Ƴ���
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/
#define IP_BUF_LEN              16                              //IP��ַ
#define DLL_BUF                 800                                //���ݸ���̬����ַ���,�Էָ����','���ָ��
/*Modified by tangxp for BUG No.0002259 on 2007.9.25 [begin]*/
/*  Modified brief:��·�����ȸ�ΪWINDOWS·����������MAX_PATH(260)*/
#define NAME_SIZE               MAX_PATH                                //���ֵĴ�С
/*Modified by tangxp for BUG No.0002259 on 2007.9.25 [end]*/
#define EXT_INFO_LEN         128                                //srvcfg��չ�ֶΣ����ڼ�¼Arch��
#define GUID_LEN                 39
#define MAX_CHANNEL_COUNT        10                                //ͨ�������

#define MAX_REG_TYPE            254                             //��������ע������ֵ 255��Ϊ����Ч
#define PACKET_DATASIZE         (1008+3*1024)                    //�����ݴ�С
#define PACKET_HEADSIZE         16                                //��ͷ��С

//����ֵ����
#define TS_SUCCESS                              0                   //���óɹ�
#define TS_DATACOMING                           0                  //�����ݵ���

#define TS_SOCKET_ERROR                         -1                 //socket����
#define TS_UNKNOW_ERROR                         -2                 //һ�����
#define TS_SYSTEM_ERROR                         -3                 /*�����ǶԻ�����ӳ������Ҳ�����Ƕ��̵߳���API����Ĵ���*/
#define TS_LAUNCH_TS_FAILURE               -4                 //����TSʧ��

#define TS_UNKNOW_DATA                          -6                 //�յ�δ֪��Ϣ�����������������Ϣ���������ϴλظ�����Ϣ�ȡ�
#define TS_NULLPTR_ENCONTERED                   -7                 //�������ֿ�ָ��
#define TS_FILE_NOT_EXIST                       -8                 //ts, ����DLL ���ļ�������
#define TS_INVALID_LINK_HANDLE                  -9                 //��Ч�����Ӿ��
#define TS_INVALID_SERVICE_ID                   -10                //��Ч�ķ���ID��
#define TS_TIME_OUT                             -11                //��ʱ
#define TS_INVALID_FILE_NAME                    -12                //��Ч�ļ���  :�糬ʱ��������֧��100�ַ����ļ���
#define TS_DEVICE_CANNOT_USE                    -13                //�豸Ŀǰ�����á�
#define TS_REC_BUF_LIMITED                      -14                 //���յ������ݴ��ڽ��ջ�������С
#define TS_RECV_DATA_ERROR                         -15                 //��������ʱ��������
#define TS_INVALID_SERVICE_NAME                 -16                 //�Ƿ��ķ�������
#define TS_UNKNOW_DATA_FORMAT                   -17                 //�յ������ݰ��ǷǷ���ʽ
#define TS_INVALID_CH_COUNT                     -18                 //�Ƿ���ͨ������
#define TS_INVALID_CH_ID                        -19                 //�Ƿ���ͨ����
#define TS_INVALID_IP                           -20                 //�Ƿ���IP
#define TS_INVALID_UART_PORT                    -21                 //�Ƿ��Ĵ���
#define TS_SERVICE_NOT_EXIST                    -22                 //ָ���ķ��񲻴���,
#define TS_SERVICE_NAME_REPEATED                -23                 //��������,      
#define TS_UNKNOW_COMM_WAY                      -24                 //δ֪��ͨ�ŷ�ʽ
#define TS_UNKNOW_PROTOCOL                      -25                 //δ֪��ͨ��Э��         
#define TS_FILE_ALREADY_EXITS                      -26                //�ļ��Ѿ�����(�ļ��ϴ�)
#define TS_SRV_CFG_NOT_ENOUGH                   -27                 //�������ò�ȫ
/*Modifide by tangxp on 2007.9.26 [begin]*/
/*  Modified brief:����GUID�������� */
#define TS_INVALID_GUID                         -28                 //GUID����
/*Modifide by tangxp on 2007.9.26 [end] */
#define TS_ISNOT_TS_SERVER                      -29                 //�Զ˲���TS������                                                                   
#define TS_SRV_NUM_EXPIRE                       -30                 //�����ķ����Ѿ��ﵽ�������       
#define TS_DLL_BUF_LOST                         -31                 //DLL��������û���Ϊ��
#define TS_CH_SW_FUNC_LOCKED_BY_OHTER    -32                 //ͨ���л����ܱ�����������
#define TS_SERVICE_CONFIG_ERR               -33               //�������ô���,��DLL�������õĻ��������
#define TS_INVALID_IP_PORT                      -34              //��Ч��IP�˿�
#define TS_INVALID_DLL_NAME                   -35             //��Ч��DLL�ļ���                   
#define TS_SEND_DATA_FAILURE                 -36             //������Ϣʧ��
#define TS_CHANGE_CHANNEL_FAILURE     -37             //ͨ���л�ʧ��
#define TS_SRV_CHANNEL_SWITCH_DISABLE     -38           //ͨ���л�������
#define TS_INVALID_REG_TYPE                    -39              //��Ч��ע������

//�ļ����͹���״̬
#define TS_FILE_BEGIN                              1           //�ļ����Ϳ�ʼ
#define TS_FILE_CONTINUE                       2           //�ļ�������
#define TS_FILE_END                                  3          //�ļ�������
#define TS_FILE_BEGIN_END                      4         //�ļ��Ƚ�С����һ�����Ϳ��Դ�����

/************************���Ͷ���******************************/

//����BOOL����
//WINDOWS.h�ﶨ����FALSE/TRUEΪ�꣬���������ض���
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
enum __BOOL
{
    FALSE,
    TRUE
};
typedef __BOOL _BOOL;

//ͨ�ŷ�ʽ��Э�鷽ʽ������
enum _PROTOCOL
{
    TCP,
    UDP,
    UART,
    USB,
    DLL,
    BUTT = 255
};
typedef _PROTOCOL PROTOCOL;

//TSϵͳ��־���������
#ifndef LOGLEVE
#define LOGLEVE
enum _LogLevel
{
    LDEBUG,
    LINFO,
    LWARNING,
    LERROR,
    LFATAL,
    LBUTT = 255
};
typedef _LogLevel LogLevel;
#endif

typedef struct COMCONFIG
{
    PROTOCOL way;                                    //ͨ�ŷ�ʽ��ѡ��,0��ʾtcp��ʽ,1��ʾudp��ʽ,2��ʾ����,4��ʶ��̬�ⷽʽ
    PROTOCOL protocolType;                           //Э������

    struct
    {
        //�����������(Ŀ�����)
        char ip[IP_BUF_LEN];                    //IP��ַ
        UI port;                                //�˿ں�
        UI timeout;                            //��ʱ
        UI retry;                                //���Դ���
    } NET;

    struct
    {
        //���ڲ�������(������)
        char channel[UART_NAME_BUF_LEN];        //����
        UI baud;                                //������
        UI timeout;                            //��ʱ
        UI retry;                                //���Դ���
    } UART;

    struct
    {
        char dllName[NAME_SIZE];                //��̬������,���Դ�·��
        //��̬�ⷽʽͨ��
        char buf[DLL_BUF];                        /*��̬��Ļ�����
                                                  ��д��ʽ��[aid,name,isSupportPrivateProtocol,configfile,]
                                                  ˵�����£�
                                                  aid:  ��ICEͨ�ŵ�ͨ�Ŵ���SA ��aid
                                                  name: ��ICEͨ�ŵ�ͨ�Ŵ���SA ��name
                                                  isSupportPrivateProtocol: �Ƿ�֧����չЭ�飬������1
                                                  configfile: �����ļ���, �����ts��loadfileĿ¼�£���ʹ��tclient_uploadfile���������ļ�����ֱ��д�ļ���������д��·�����ļ���*/
    } DLL;

    struct
    {
        char GUID[GUID_LEN];              /*USB�豸��GUID�ţ�
                                                            ��sprintf��GUID�Ÿ�ʽ����16�����ַ���
                                                            ����ʽ��BUF��, ��"873fdf-61a8-......."*/
    } USB;
    COMCONFIG* next;                            //ͨ��������Ϣ
} comConfig ;

typedef struct server_config
{
    char            isLog;                            //�Ƿ���Ҫ��־0,����¼,1:��¼
    char        name[NAME_SIZE];                //����
    char        extInfo[EXT_INFO_LEN];          //��ϵ�ṹ�ȣ���Ŀ�������ΪTA����0*;��Ŀ�������ΪICE����1*
    US        aid;                            //aid��
    UI            state;                            //״̬����TS�ϱ����ֱ�Ϊ1(ACTIVE)��0(INACTIVE)��2(DELETED)
    UI            reportType;                        //�ϱ����ͣ���TS�ϱ���Ϊע������
    UI            currentChannel;                    //��ǰͨ����
    UI            channelCount;                    //ͨ����
    COMCONFIG*    pComConfig;                        //��һ����ڵ㣬��Ŀ���Ϊ��ͨ������ָ����һ��ͨ����ͨ������
} ServerCfg;


/************************�ӿ�����******************************/

/**
 * ����: �ر�ָ��������
 * @param fd  ���Ӿ��
 * @return :
         TS_INVALID_LINK_HANDLE: ����fd����Ч�ľ��
         TS_SOCKET_ERROR:  �������TS�����ʱSOCEKT���������رշ��ظ�ֵ
         TS_SUCCESS:    �ɹ��رա�
 */
DLL_FUNC_INT TClient_close(US fd) ;

/**
 * ����: �����ݷ��͵�ָ����Ŀ������Ӧ�ô���
 * @param fd  ���Ӿ��
 * @param des_aid  Ŀ��aid��
 * @param des_said Ŀ��said��
 * @param src_said Դsaid��
 * @param data  ���ݰ�
 * @param size  ���ݰ���С
 * @return
        ����������ݳɹ�����ʵ�ʷ��͵����ݵ��ֽڴ�С
        TS_NULLPTR_ENCONTERED: �����ݵĻ���Ϊ��ָ��
        TS_INVALID_LINK_HANDLE: ��������Ӿ����Ϊ��
        TS_SOCKET_ERROR:  ����ʱSOCKET��������
 */
DLL_FUNC_INT TClient_putpkt(US fd, US des_aid, US des_said, US src_said,
                            const char* data, UI size) ;

/**
 * ����: ����Ŀ����������
 * @param fd  ���Ӿ��
 * @param des_aid  Ŀ��aid��
 * @param des_said Ŀ��said��
 * @param src_said Դsaid��
 * @param data  ���ݰ�
 * @param dataSize  �������Ĵ�С
 * @return
        ����ɹ��㷵��ʵ�ʵõ����ݰ����ֽڴ�С
        TS_NULLPTR_ENCONTERED:���ջ����ָ��Ϊ��.
        TS_INVALID_LINK_HANDLE: ��Ч�ľ����
        TS_SOCKET_ERROR:�հ�ʱ����SOCKET����.
        TS_REC_BUF_LIMITED:  ���ջ���Ĵ�СС��ʵ�ʽ��յ������ݵ��ֽڴ�С
 */
DLL_FUNC_INT TClient_getpkt(US fd, US* src_aid, US* des_said, US* src_said,
                            char* data, UI dataSize) ;

/**
 * ����: �ṩ������ͨ�ŷ�ʽ, �û������趨һ��ʱ���timeout
 * @param fd  ���Ӿ��
 * @param timeout  �ȴ�ʱ��,��λ:��
 * @return
        TS_DATACOMING:��ָ����ʱ���������ݵ���
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_TIME_OUT: ָ����ʱ����û�����ݵ���
        TS_SOCKET_ERROR:�ڲ�ѯSOCKET����ʱSOCKET����,�����ǶԶ˹ر�����.
 */
DLL_FUNC_INT TClient_wait(US fd, int timeout) ;

/**
 * ����: ��ָ��IP��ַ�Ͷ˿ںŵ�TS��������
 * @param IP  TS���ڵ�IP��ַ
 * @param port  �˿ں�, 0��ʶ����Ĭ�϶˿�port, ��0��ʾָ���˿�
 * @return
        �����ɹ����ش���0��ʾ�õ���aid��
        TS_NULLPTR_ENCONTERED :�����IP����Ϊ��ָ��
        TS_INVALID_IP: IPΪ��ЧIP,��0.0.0.0/255.255.255.255��.
        TS_SOCKET_ERROR: ��ͼ�ͶԶ˽������ӵ�ʱ��SOCKET��������
        TS_ISNOT_TS_SERVER:  �Զ�û������TS����.
        TS_UNKNOW_DATA  :�Զ˷��ص���Ϣ��δ֪��Ϣ��,���ʽ����ʶ��
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: ��TClient_open���������ֶ�����(short)��Ϊ����(int)*/
DLL_FUNC_INT TClient_open(const char* IP, US port = 0) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */

/**
 * ����:  ����Ŀ���������ֲ��Ҷ�Ӧ��ID��
 * @param fd  ���Ӿ��
 * @param name  Ŀ������, һ����0��β���ַ���
 * @return
    �����ɹ����ط����Ӧ��Ŀ������ID��
    TS_NULLPTR_ENCONTERED: nameΪ��ָ��
    TS_INVALID_LINK_HANDLE : ��Ч�����Ӿ��
    TS_INVALID_SERVICE_NAME: name�ַ������ȴ�Сϵͳ�涨�����ֳ���
    TS_UNKNOW_DATA: δ֪�����ݸ�ʽ�����ݰ�
    TS_SYSTEM_ERROR:  ϵͳ����������,�п������յ������ݰ��ĶԻ�
                                    ID�����ڴ��Ĳ�һ��,Ҳ�п��ܷ��ص�����Ч��AID��
    TS_TIME_OUT:  ��ָ�����հ�ʱ����û���յ����ݰ�


���͸�ʽ��
                            -------------------
                            |DID|, |I name|, |
                            -------------------
���ո�ʽ��
                          �ظ����ݰ���ʽ
                            -------------------
                            |DID|, |OK|, |serverID|,|
                            -------------------
*/
DLL_FUNC_INT TClient_getServerID(US fd, char* name) ;


/**
 * ����: ��TM�д���һ��DLL��ʽ��Ŀ���������.
 * @param fd  ���Ӿ��
 * @param pServerConfig: �����������ýṹָ��
 * @return
        ��������ɹ�����AID��
        TS_INVALID_LINK_HANDLE: ��Ч�ľ����
        TS_UNKNOW_DATA: ��Ч������,һ�������ݳ���С����������С����
        TS_UNKNOW_DATA_FORMAT:  �޷�ʶ������ݸ�ʽ
        TS_SYSTEM_ERROR: ϵͳ��������
        TS_TIME_OUT: ��ָ��ʱ����û���յ�����
        �Լ�����TS���صľ������ֵ

���͸�ʽ: "DID, TserviceName,  dllName,  isLog,  DllBuf,"
���ո�ʽ: "DID, OK, aid,"
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: ��TClient_createServerWithDll���������ֶ�����(short)��Ϊ����(int)*/
DLL_FUNC_INT TClient_createServerWithDll(US fd, ServerCfg* pServerConfig) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end]*/


/**
 * ����: ��TM�д���һ��Ŀ���������.
 * @param fd  ���Ӿ��
 * @param pServerCfg  Ŀ���������Ϣ
 * @return
        ��������ɹ�����AID��
        TS_INVALID_LINK_HANDLE: ��Ч�ľ����
        TS_UNKNOW_DATA: ��Ч������,һ�������ݳ���С����������С����
        TS_UNKNOW_DATA_FORMAT:  �޷�ʶ������ݸ�ʽ
        TS_SYSTEM_ERROR: ϵͳ��������
        TS_TIME_OUT: ��ָ��ʱ����û���յ�����
        TS_NULLPTR_ENCONTERED: pServerCfg��pServerCfg->pComConfigָ��Ϊ��
        TS_INVALID_SERVICE_NAME:  û��ָ��������������
        TS_INVALID_IP: ��������Ч��IP
        �Լ�����TS���صľ������ֵ

���͸�ʽ: |DID|, Csa����,  ��־,ͨ����, +����wayѡ�����������Ϣ(�Էָ��)
���ո�ʽ:      |DID|, |OK|, |aid|
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: ��TClient_createServer���������ֶ�����(short)��Ϊ����(int)*/
DLL_FUNC_INT TClient_createServer(US fd, ServerCfg* pServerCfg) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end]*/


/**
 * ����: �ر�ָ����TS
 * @param fd   ���Ӿ��
 * @return
        TS_SUCCESS�����ɹ�
        TS_INVALID_LINK_HANDLE:  ��Ч�����Ӿ����
        TS_SOCKET_ERROR:   �������ݰ�ʱSOCKET��������
 */
DLL_FUNC_INT TClient_closeTM(US fd) ;

/**
 * ����: ͨ���л�
 * @param fd   ���Ӿ��
 * @param AID  Ŀ������AID��
 * @param channelType  Ҫ�л�����ͨ��ID
 * @return
        TS_SUCCESS: �����ɹ�
        TS_INVALID_SERVICE_ID:��Ч�ķ���ID
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_CHANGE_CHANNEL_FAILURE  : ͨ���л�ʧ��
        TS_SRV_CHANNEL_SWITCH_DISABLE: ͨ���л�������,���Լ�����
        TS_CH_SW_FUNC_LOCKED_BY_OHTER: ͨ���л����ܱ�����������
        �Լ�����һЩTS����ֵ
 */
DLL_FUNC_INT TClient_changeChannelType(US fd,  US aid, UC channelType);

/**
 * ����: ɾ��TMһ��Ŀ���������.
 * @param fd   ���Ӿ��
 * @param AID  Ŀ������AID��
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_SERVICE_ID:��Ч�ķ���ID
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        �Լ�����TS���ص�һЩ�������ֵ
���͸�ʽ: DID,RID,
���ո�ʽ:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_deleteServer(US fd, US aid) ;

/**
 * ����: �޸�һ���Ѿ�������TS�е�Ŀ���������
 * @param fd   ���Ӿ��
 * @param pServerCfg   ָ��Ŀ��server�����ýṹ, �Ǹ��������
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_SERVICE_ID:��Ч�ķ���ID
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_SERVICE_CONFIG_ERR : ָ����ͨ��������Ϣ����
        TS_UNKNOW_COMM_WAY: ͨ������ָ����ͨ�ŷ�ʽ����
        �Լ�����TS���ص�һЩ�������ֵ

 ���͸�ʽ:
 NET/UART��ʽ
 -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|isLog|,|channelCount|,|curChannel|,|channelConfig..............|
 -----------------------------------------------------------------------------
 DLL��ʽ
  -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|isLog|,|way|,|DllBuf|
 -----------------------------------------------------------------------------
���ո�ʽ: DID,OK/Exx,
*/
DLL_FUNC_INT TClient_modifyServer(US fd, ServerCfg* pServerCfg) ;

/**
 * ����: ͨ��ָ��Ŀ�����ID�Ų��Ҹ�Ŀ��������ò�����״̬��Ϣ
 * @param fd   ���Ӿ��
 * @param pServerCfg[OUT]   ָ��Ŀ��server�����ýṹ, �Ǹ��������
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_NULLPTR_ENCONTERED:  pServerCfgΪ��ָ��
        TS_INVALID_SERVICE_ID:  ��Ч��AID
        TS_UNKNOW_DATA_FORMAT:  �Զ˷����������ݰ���ʽ����ʶ��
        TS_UNKNOW_COMM_WAY:  �Զ˷����������ݰ��Ĵ����ͨ�ŷ�ʽ����ʶ��
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ
 ���͸�ʽ��
 -------------
 |DID|,|Qaid|,|
 -------------
��ѯ���񷵻صĸ�ʽΪ��
TCP/UDP/UART
OK, name, state, log, chcount, curch, config of each ch..
DLL
OK, name, state, log, chcount, curch, protocol, way, dllname, outbuf, inbuf,
*/
DLL_FUNC_INT TClient_queryServer(US fd, ServerCfg* pServerCfg) ;


/**
 * ����: ����ָ���Ĵ�������״̬��Ŀ�����
 * @param fd   ���Ӿ��
 * @param aid   Ŀ���aid��
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_SERVICE_ID:  ��Ч��AID
        TS_UNKNOW_DATA_FORMAT:  �Զ˷����������ݰ���ʽ����ʶ��
        TS_UNKNOW_COMM_WAY:  �Զ˷����������ݰ��Ĵ����ͨ�ŷ�ʽ����ʶ��
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ
 ���͸�ʽ: DID, Aaid,
���ո�ʽ: DID, OK/Exx,
*/
DLL_FUNC_INT TClient_activeServer(US fd, US aid) ;

/**
 *   �� ��    ���� TClient_activeServerWithDll
 *   ����˵���� ����ָ���Ĵ�������״̬��DLL_SA
 *   ��      ��1   ��fd:���Ӿ��
 *   ��      ��2   ��aid:Ŀ������Aid��
 *   ��      ��3   ��char* anserInfo:  TC�յ���TS����������Ϣ
 *   ��      ��4   ��infolen[in/out]  ָ��answerInfo���ֽڴ�С��
                                        ������ʵ�����ݵ��ֽڴ�С
 *   ��  ��   ֵ��
                                ��������ɹ��򷵻ؽ��յ��Ļظ��ֽ���
                                TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
                                TS_NULLPTR_ENCONTERED:  answerInfoΪ��ָ��
                                TS_INVALID_SERVICE_ID:  ��Ч��AID
                                TS_UNKNOW_DATA_FORMAT:  �Զ˷����������ݰ���ʽ����ʶ��
                                TS_UNKNOW_COMM_WAY:  �Զ˷����������ݰ��Ĵ����ͨ�ŷ�ʽ����ʶ��
                                TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
                                TS_REC_BUF_LIMITED:  ָ����answerInfo�ռ䳤�Ȳ���
                                �Լ�����TS���ص�һЩ�������ֵ
       ���͸�ʽ: DID,Aaid,
 *    ���ո�ʽ:DID,Exx,  // DID, OK, outbuflen, outbuf...,
 *
**/
DLL_FUNC_INT TClient_activeServerWithDll(US fd,  US aid,  char* answerInfo,  int infolen);


/**
 * ����: ����ָ���Ĵ��ڼ���״̬��Ŀ�����
 * @param fd   ���Ӿ��
 * @param aid   Ŀ���aid��
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_SERVICE_ID:  ��Ч��AID
        TS_UNKNOW_DATA_FORMAT:  �Զ˷����������ݰ���ʽ����ʶ��
        TS_UNKNOW_COMM_WAY:  �Զ˷����������ݰ��Ĵ����ͨ�ŷ�ʽ����ʶ��
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ
 ���͸�ʽ:DID, Paid,
���ո�ʽ:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_inActiveServer(US fd, US aid) ;

/**
 * ����: ����TS
 * @param path   TS�Ĵ�·�����ļ���
 * @return
        TS_SUCCESS:   ���سɹ�
        TS_NULLPTR_ENCONTERED: path :Ϊ��ָ��
        TS_INVALID_FILE_NAME: �ļ�����Сϵͳ�涨���ļ�����С
        TS_FILE_NOT_EXIST:  �ļ�����Ӧ��TS�ļ�������
        TS_LAUNCH_TS_FAILURE:  ����TS����ʧ��
 */
DLL_FUNC_INT TClient_launchTM(const char* path) ;

/**
 * ����:���߿�ѡ�õ������߳������ڸ÷�����
 *              �õ�TS�Զ��ϱ�����Ϣ��
 *               ��������Ϣ, Ҳ������״̬
 *               �ڴ�ռ��ɵ�����������
 * @param fd   ���Ӿ��
 * @param pServerCfg    �ϱ���Ϣͷָ��
 * @return
         ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_SERVICE_ID:  ��Ч��AID
        TS_UNKNOW_DATA_FORMAT:  �Զ˷����������ݰ���ʽ����ʶ��
        TS_UNKNOW_COMM_WAY:  �Զ˷����������ݰ��Ĵ����ͨ�ŷ�ʽ����ʶ��
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ

 ��ʽ:sa��AID��, ����, �ϱ�����, ״̬, ��־, ͨ�ŷ�ʽ, ���ݲ�ͬ��ͨ�ŷ�ʽ��������Ϣ(�Էָ�����ָ�)
 */
DLL_FUNC_INT TClient_getSAReport(US fd, ServerCfg *pServerCfg, UI timeout = 0) ;

/**
 * ����: TS����ע��, ע���, TS����TC�Զ�������Ϣ��״̬
 * @param fd   ���Ӿ��
 * @param regType   ע������
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_REG_TYPE:  ��Ч��ע�����ͺ�
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ

���͸�ʽ: DID,Etype,
���ո�ʽ: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_register(US fd, int regType) ;

/**
 * ����: ȡ��ĳ�����͵�ע��Ǽ�
 * @param fd   ���Ӿ��
 * @param regType   ע������
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_REG_TYPE:  ��Ч��ע�����ͺ�
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ

 ���͸�ʽ��DID,UregType,
���ո�ʽ��DID,OK/Exx,
*/
DLL_FUNC_INT TClient_unregister(US fd, int regType);

/**
 * ����: ����sa��ע������
 * @param fd   ���Ӿ��
 * @param aid   Ҫ����sa��aid��
 * @param registerType   ����
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_REG_TYPE:  ��Ч��ע�����ͺ�
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ

 ������ʽ: DID,s aid, registerType,
 ���ո�ʽ: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_setRegisterType(US fd, US aid, int registerType) ;

/**
 * ����: ���sa��ע������
 * @param fd   ���Ӿ��
 * @param aid   Ҫ����sa��aid��
 * @param registerType   Ҫע������
 * @return
        ��������ɹ��򷵻�TS_SUCCESS
        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
        TS_INVALID_REG_TYPE:  ��Ч��ע�����ͺ�
        TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
        �Լ�����TS���ص�һЩ�������ֵ

 ������ʽ: DID,c aid, registerType,
 ���ո�ʽ: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_clearRegisterType(US fd, US aid,  int registerType) ;

/**
 * @Funcname:  Tclient_queryHostSerialPort
 * @brief        :  ��ѯTS���������Ĵ�������
 * @para1      : fd  ���Ӿ����
 * @para2      : buf  ���ջ���
 * @para3      : �����ɹ�:size  ���ջ�����ֽڴ�С
 * @return      :
                    �����ɹ����ؽ��ջ����е�ʵ���ֽڴ�С
                    TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
                    TS_TIME_OUT:  ָ��ʱ����û���յ����ݰ�
                    TS_REC_BUF_LIMITED: ������ָ���Ĵ�СС��ʵ�ʽ��յ����ֽڴ�С
                    �Լ�����TS���ص�һЩ�������ֵ

    ���͸�ʽ: DID, p,
    ���ո�ʽ: |DID|,|OK|,|COM1|,|COM2|,|COM3|,|����DID,Exx,
 */
DLL_FUNC_INT Tclient_queryHostSerialPort(US fd,  char *recvBuf,  US size);

/**
 * @Funcname:  TClient_setTargetChannelSwitchState
 * @brief        :  ����Ŀ���ͨ���л���־
 * @para1      : fd : ���Ӿ��
 * @para2      : aid  Ŀ�����ID��
 * @para3      :  flag :false��ֹͨ���л�true:����ͨ���л�
 * @return      :
                        TS_SUCESS ���óɹ�,
                        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
                        TS_TIMEOUT: ��ȡ�ظ���Ϣ����ʱ
                        TS_CH_SW_FUNC_LOCKED_BY_OHTER: ͨ���л��������ͻ�������
                        �Լ�����һЩ����ֵ

    ������ʽ:DID,a aid, flag
    ���ո�ʽ:DID,OK,/Exx,
 *
**/
DLL_FUNC_INT TClient_setTargetChannelSwitchState(US fd,  US aid,  _BOOL flag);

/**
 * @Funcname:  DLL_FUNC_INT  TClient_SetSystemLogLevel
 * @brief        :  ����TSϵͳ��־���������
 * @para1      : fd:  ���Ӿ����
 * @para2      : level ��־����
 * @return      :
                        TS_SUCESS ���óɹ�,
                        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
                        TS_SOCKET_ERROR: ���������ʱSOCKET����

    ������ʽ:DID,L level,
    �޻ظ�����
 *
**/
DLL_FUNC_INT  TClient_setSystemLogLevel(US fd,  LogLevel level);

/**
 * @Funcname:  TClient_SetServiceAgentLog
 * @brief        :  ����ָ����������ݰ���־���
 * @para1      : fd : ���Ӿ��
 * @para2      : aid  Ŀ�����ID��
 * @para3      :  logflag :0�����ָ���������־1:���ָ���������Ϣ��־
 * @return      :
                        TS_SUCESS ���óɹ�
                        TS_INVALID_LINK_HANDLE: ��Ч�����Ӿ����
                        TS_SOCKET_ERROR: ���������ʱSOCKET����

    ������ʽ:DID,l aid,logflag,
    ���ո�ʽ: DID, Ok/Exx,
 *
**/
DLL_FUNC_INT  TClient_SetServiceAgentLog(US fd, US aid, _BOOL logflag);

/**
 * @Funcname:  TClient_UpLoadFile
 * @brief        :  ����ָ���ļ���TS������
 * @para1      : fd: ��TS�����������Ӿ��
 * @para2      : filename��Ҫ���͵��ļ���
 * @para3      : isReplace�Ƿ���Ҫ����ͬ���ļ�0,������;1: ����
 * @para4      : savedname, ������TS�ϵ��ļ���
 * @return      :
                        TS_SUCCESS���ͳɹ�
                        TS_FILE_ALREADY_EXIT�Ѿ�����ͬ���ļ�
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��2�� 14:51:36
 *
**/
DLL_FUNC_INT TClient_UpLoadFile(US fd, const char* filename, const char *savedname, int isReplace = 1);

/**
 * @Funcname:  TClient_DownLoadFile
 * @brief        :  ��TS�����������ļ������߶�
 * @para1      : fd: ��TS�����������Ӿ��
 * @para2      :  filename��Ҫ���ص��ļ���
 * @para3      : savefilename�����ļ�����Ҫ������ļ���
                        (��ȫ·��,���򽫱�����TS���еĵ�ǰĿ¼)
 * @return      :
                         TS_SUCCESS���ͳɹ�
                         TS_FILE_ALREADY_EXITS�Ѿ�����ͬ���ļ�
                         TS_FILE_NOT_EXISTָ�����ļ�������
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��2�� 15:00:31
 *
**/
DLL_FUNC_INT TClient_DownLoadFile(US fd, const char*filename, const char*savefilename);

/**
 * @Funcname:  TClient_DeleteFile
 * @brief        :  ��TS������ɾ��ָ���ļ�
 * @para1      : fd: ��TS�����������Ӿ��
 * @para2      :  filename��Ҫ���ص��ļ���
 * @return      :
                         TS_SUCCESS���ͳɹ�
                         TS_FILE_NOT_EXISTָ�����ļ�������
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��11�� 11:00:31
 *
**/
DLL_FUNC_INT TClient_DeleteFile(US fd, const char*filename);

/**
 * @Funcname:  guidToString
 * @brief        :  ��GUIDת�����ַ���
 * @para1      : guidData, guid����
 * @para2      : upcaseת���ɴ�Сд��ʽ
 * @para3      : strGuid, ת������ַ������������û�����
 * @para4      : buflen, �û�����Ļ����С
 * @return      : �ɹ�����0��ʧ�ܷ���-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��27�� 10:44:03
 *
**/
DLL_FUNC_INT guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen);
/**
 * @Funcname:  stringToGUID
 * @brief        :  ���ַ�����ʽ��GUIDת����GUID�ṹ����
 * @para1      : strGuid, �ַ�����ʽ��GUID
 * @para2      : len �ַ����ĳ���
 * @para3      : guidData, ת����Ľṹ
 * @return      : �ɹ�����0��ʧ�ܷ���-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��27�� 10:44:03
 *
**/
DLL_FUNC_INT stringToGUID(char *strGuid, unsigned short len, GUID *guidData);


#endif
