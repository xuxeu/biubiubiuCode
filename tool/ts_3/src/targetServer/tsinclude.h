/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tsInclulde.h
 * @Version        :  1.0.0
 * @Brief           :  ���ڰ���TS�еĳ���ͷ�ļ�
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006��12��20�� 16:55:21
 * @History        : 
 *   
 */
#ifndef _TSINCLUDE_H_
#define _TSINCLUDE_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSemaphore>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QTextStream> 
#include <QTextCodec>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include "windows.h"
#include "setupapi.h"
//#include "util.h"
#include "common.h"
/*Modified by tangxp on 2008.1.14 [begin]*/
/*  Modified brief: ���ӻ���ͷ�ļ�*/
//#include "Mutex.h"
/*Modified by tangxp on 2008.1.14 [end]*/
#define  _CRT_SECURE_NO_DEPRECATE          //����sprintf�ຯ���澯

#define UC unsigned char
#define US unsigned short
#define UI unsigned int
#define UL unsigned long

//�ļ����͹���״̬
#ifndef _FILE_PROCESS_FLAG
#define _FILE_PROCESS_FLAG
#define TS_FILE_BEGIN                              1           //�ļ����Ϳ�ʼ
#define TS_FILE_CONTINUE                       2           //�ļ�������
#define TS_FILE_END                                  3          //�ļ�������
#define TS_FILE_BEGIN_END                      4         //�ļ��Ƚ�С����һ�����Ϳ��Դ�����
#endif

#define MAX_KEY_LENGTH                  255
#define MAX_VALUE_NAME                  16383
//ts����saע��������
#define MAX_REG_TYPE  254
#define INVALID_REG_TYPE  255
#define MAX_CHANNEL_COUNT           10

/*Modified by tangxp on 2007.11.12 [begin]*/
/*  brief: ���ӳ�פģ���ע������*/
#define PSA_REG_TYPE    254
/*Modified by tangxp on 2007.11.12 [end]*/

/****************�������ֵ************************/
#define MAX_ERR_NUM                             100

#define TS_SUCCESS                              0                   //���óɹ�
#define TS_SOCKET_ERROR                         -1                 //socket����

#define TS_UNKNOW_ERROR                         -2                 //һ�����
#define TS_SYSTEM_ERROR                         -3                 /*�����ǶԻ�����ӳ������Ҳ�����Ƕ��̵߳���API����Ĵ���*/

#define TS_UNKNOW_DATA                          -6                 //�յ�δ֪��Ϣ�����������������Ϣ���������ϴλظ�����Ϣ�ȡ�
#define TS_NULLPTR_ENCONTERED                   -7                 //�������ֿ�ָ��
#define TS_FILE_NOT_EXIST                       -8                 //ts, ����DLL ���ļ�������
#define TS_INVALID_SERVICE_ID                   -10                //��Ч�ķ���ID��
#define TS_TIME_OUT                             -11                //��ʱ
#define TS_INVALID_FILE_NAME                    -12                //��Ч�ļ���  :�糬ʱ��������֧��100�ַ����ļ���
#define TS_DEVICE_CANNOT_USE                    -13                //�豸Ŀǰ�����á�
#define TS_REC_BUF_LIMITED                      -14                 //���յ������ݴ��ڽ��ջ�������С
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
     
#define TS_SRV_CFG_NOT_ENOUGH                   -27                 //�������ò�ȫ

#define TS_SRV_NUM_EXPIRE                       -30                 //�����ķ����Ѿ��ﵽ�������       
#define TS_DLL_BUF_LOST                         -31                 //DLL��������û���Ϊ��
#define TS_CH_SW_FUNC_LOCKED_BY_OHTER           -32                 //ͨ���л����ܱ�����������
#define TS_SERVICE_CONFIG_ERR                   -33               //�������ô���,��DLL�������õĻ��������
#define TS_INVALID_IP_PORT                      -34              //��Ч��IP�˿�
#define TS_INVALID_DLL_NAME                     -35             //��Ч��DLL�ļ���                   
#define TS_SEND_DATA_FAILURE                    -36             //������Ϣʧ��
#define TS_CHANGE_CHANNEL_FAILURE               -37             //ͨ���л�ʧ��
#define TS_SRV_CHANNEL_SWITCH_DISABLE     -38           //ͨ���л�������
#define TS_INVALID_REG_TYPE                     -39              //��Ч��ע������

#define FAILURE_ACK_LEN                             3                         //ʧ�ܻظ��ĳ���Exx

#define ERR_UNKNOW_ERR                          "E01"                                     //δ֪����
#define ERR_SYSTEM_ERR                            "E01"                                    //ϵͳ����������
#define ERR_UNKNOW_DATA                         "E02"                                   //δ֪����
#define ERR_FILE_NOT_EXIST                      "E03"                                   //�ļ�������
#define ERR_INVALID_SERVICE_ID                  "E04"                               //�Ƿ��ķ���ID
#define ERR_TIME_OUT                            "E05"                                           //��ʱ
#define ERR_INVALID_FILE_NAME                   "E06"                                  //�Ƿ����ļ���
#define ERR_DEVICE_CANNOT_USE                   "E07"                               //�豸������
#define ERR_REC_BUF_LIMITED                     "E08"                                   //���ջ�������
#define ERR_INVALID_CH_ID                         "E09"                                   //�Ƿ���ͨ����
#define ERR_INVALID_SERVICE_NAME                "E10"                               //�Ƿ��ķ�����
#define ERR_INVALID_IP                          "E11"                                           //�Ƿ�IP
#define ERR_INVALID_UART_PORT                   "E12"                                   //�Ƿ����ڶ˿�
#define ERR_SERVICE_NOT_EXIST                   "E13"                                   //���񲻴���
#define ERR_SERVICE_NAME_REPEATED               "E14"                               //��������
#define ERR_UNKNOW_COMM_WAY                     "E15"                               //δ֪��ͨ�ŷ�ʽ
#define ERR_UNKNOW_PROTOCOL                     "E16"                                  //δ֪Э��
#define ERR_CH_SW_FUNC_LOCKED_BY_OHTER          "E17"                                 //ͨ���л����ܱ���������
#define ERR_SRV_CFG_NOT_ENOUGH                  "E18"                               //�������ò�ȫ
#define ERR_SRV_CFG_ERR                         "E19"                                           //�������ô���
#define ERR_SRV_NUM_EXPIRE                      "E20"                                       //�����ķ��������ﵽ����
#define ERR_DLL_BUF_LOST                        "E21"                                           //ȱ��DLL��BUF����
#define ERR_CHANGE_CHANNEL_FAILURE              "E23"                                       //ͨ���л�ʧ��

#define ERR_SERVICE_IS_ALREADY_EXIT             "E24"                                      //�����Ѿ�����
#define ERR_SRV_CHANNEL_SWITCH_DISABLE          "E25"                                 //��������ͨ���л�
#define ERR_FILE_ALREADY_EXIT                               "E26"                                   //�ļ��Ѿ�����(�����ϴ��ļ�ʱ)
//���Ʒָ���
//const char CONTROL_FLAG[] = ",";
const char CONTROL_FLAG_CHAR = ',';
const char ANSWER_FAIL[] = "E01"; 
const char SUCCESS_ACK[] = "OK";
#define CONTROL_FLAG_LEN         strlen(CONTROL_FLAG)
#define SUCCESS_ACK_LEN     strlen(SUCCESS_ACK)
#define ANSWER_FAIL_LEN    strlen(ANSWER_FAIL)

#define DIDLEN      sizeof(US)             //�Ի�ID���ֽڳ���
#define CONTROL_COMMAND_LEN      1                         //����ʶ����ĳ���
#define FILEBUFSIZE 3*1024             //�����ļ�����ʱÿ�δ��͵����ݴ�С
#define EXT_INFO_LEN         128                                //srvcfg��չ�ֶΣ����ڼ�¼Arch��

//����ͨ�ŷ�ʽ

//����������
const char GETID='I';                                                       //���ư�GETID��ʶ���
const char  CREATE_SERVER='C';                                  //���ư�createServer��ʶ���
const char  CREATE_SERVERWITHDLL='T';                   //���ư�createServerWithDll��ʶ���
const char  CLOSE_TS='D';                                               //�ر�ts��ʶ���
const char  DELETE_SERVER='R';                                   //���ư�deleteServer��ʶ���
const char  CONNECT_FLAG='?';                                   //����TClient��ʶ���
const char  QUERY_SERVER_STATE='Q';                     //���ư�queryServerState��ʶ���
const char  SEARCH_SERVER_COUNT='S';                    //���ư�searchServerCount��ʶ���
const char  ACTIVE_SERVER='A';                                   //���ư�TClient_activeServer��ʶ���
const char  REST_SERVER='P';                                        //���ư�TClient_inActiveServer��ʶ���
const char  MODIFY_SERVER='M';                                  //���ư�TClient_modifyServer��ʶ���
const char  REGISTER_SERVER='E';                                //���ư�TClient_register��ʶ���
const char  UNREGISTER_SERVER='U';                          //���ư�TClient_unregister��ʶ���
const char  SET_SERVER_TYPE='s';                                //���ư�TClient_setRegisterType��ʶ���
const char  CLEAR_SERVER_TYPE='c';                          //���ư�TClient_clearRegisterType��ʶ���
const char  CHANGE_CHANNEL_TYPE ='V';                   //���ư�TClient_changeChannelType��ʶ���
//TS3.2����������
const char  QUERY_HOST_COMM_CONFIG = 'p';         //���ư�TClient_queryHostSerialPort��ʶ���
const char  SET_TARGET_SWITCH_FLAG = 'a';           //���ư�TClient_setTargetChannelSwitchStateʶ���
const char  SET_TS_SYSTEM_LOGOUT_LEVEL  = 'L';    //���ư�TClient_setSystemLogLevel��ʶ���
const char  SET_SERVICE_PACK_LOG_FLAG = 'l';         //���ư�TClient_SetServiceAgentLog��ʶ���
const char  CLOSE_LINK = 'd';                                     //���ư�TClient_Close������ʶ���
const char  UPLOAD_FILE = 'F';                                    //���ư�TClient_UpLoadFile������ʶ����
const char  DOWNLOAD_FILE = 'f';                                //���ư�TClient_DownLoadFile������ʶ����
const char  DELETE_FILE = 'K';                                        //���ư�TClient_DeleteFile������ʶ����

/********************�ṹ/�������Ͷ���**********************/

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

#ifndef _SERVER_CONFIG
#define _SERVER_CONFIG
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
/*  Modified brief: �򴮿ں�����256�����������Ƴ��ȸ�Ϊ7λ*/
#define UART_NAME_BUF_LEN        7                               //�������Ƴ���
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/
#define IP_BUF_LEN              16                              //IP��ַ
#define DLL_BUF                 800                                //���ݸ���̬����ַ���,�Էָ����','���ָ��
#define NAME_SIZE               100                                //���ֵĴ�С

typedef struct COMCONFIG 
{
    PROTOCOL way;									//ͨ�ŷ�ʽ��ѡ��,0��ʾtcp��ʽ,1��ʾudp��ʽ,2��ʾ����,4��ʶ��̬�ⷽʽ
    PROTOCOL protocolType;                           //Э������
    							
    union
    {
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
            char buf[DLL_BUF];                        //��̬��Ļ�����
        } DLL;
    }config;
    COMCONFIG* next;							//ͨ��������Ϣ
}comConfig ;

typedef struct server_config 
{
	char			isLog;							//�Ƿ���Ҫ��־
	char		name[NAME_SIZE];				//����
        char        extInfo[EXT_INFO_LEN];          //��ϵ�ṹ��
	US		aid;							//aid��
        UI            state;                            //״̬
        UI            reportType;                        //�ϱ�����
        UI            currentChannel;                    //��ǰͨ����
        UI            channelCount;                    //ͨ����
	COMCONFIG*	pComConfig;						//��һ����ڵ�
}ServerCfg;
#endif 


typedef enum _winversion
{
    UNKNOW,
    WIN95,
    WIN98,
    WINME,
    WINNT,
    WIN2K,
    WINXP,
    WIN2003
}WINVERSION;

extern WINVERSION getWindowsVersion();

#define GUID_LEN                 37

/*����LambdaICE��GUID*/
DEFINE_GUID(LambdaICE_GUID, 
0x873fdf, 0x61a8, 0x11d1, 0xaa, 0x5e, 0x0, 0xc0, 0x4f, 0xb1, 0x72, 0x8b);
/*****************************************************************************/
extern char* getErrStrByErrValue(int errValue );

/**
 * @Funcname:  guidToString
 * @brief        :  ��GUIDת�����ַ���
 * @para1      : guidData, guid����
 * @para2      : upcaseת���ɴ�Сд��ʽ
 * @para3      : strGuid, ת������ַ������������û�����
 * @para4      : buflen, �û�����Ļ����С
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��27�� 10:44:03
 *   
**/
bool guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen);
/**
 * @Funcname:  stringToGUID
 * @brief        :  ���ַ�����ʽ��GUIDת����GUID�ṹ����
 * @para1      : strGuid, �ַ�����ʽ��GUID
 * @para2      : len �ַ����ĳ��� 
 * @para3      : guidData, ת����Ľṹ
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��27�� 10:44:03
 *   
**/
bool stringToGUID(char *strGuid, unsigned short len, GUID *guidData);

#endif /*_TSINCLUDE_H_*/

