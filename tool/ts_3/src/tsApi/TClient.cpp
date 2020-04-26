/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit,  including
*    copying it,  modifying it,  compiling it,  and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice,  this paragraph,  and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* ʵ��TClient�Ľӿ�
*
* @file     TClient.cpp
* @brief     ʵ��TClient�Ľӿ�
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
* @history:  
            1��v3.2 �汾�����ӶԻ���������ֵ���������ӿڵ�

* ���ţ�ϵͳ��
*
*/

#include "stdafx.h"

//#include "..\..\include\wsocket.h"
#include "wsocket.h"
#include <map>
#include <string>

#include <algorithm>
#include <stdio.h>
#include "windows.h"

#include "TClientAPI.h"
#include "common.h"

using namespace std;


/*****************************���岿��*************************************/

/*Added by qingxh on 2006-10-20 begin*/
#define NULL_PTR  0
#define IsInvalidAID(aid) ((aid < 0 || aid > 32767) ?false:true)
#define CheckInvalidPort(port) (((port<0)||(port>65535))?false:true)
//check the pack by the size , judge the pack is valid one or not
#define CheckValidPackBySize(size) ((size< (int)(DIDLEN + 2* CONTROL_FLAG_LEN +\
                                                                min(ANSWER_SUCEESS_LEN, ANSWER_FAIL_LEN)))?\
                                                                false:true)
//���ע�������Ƿ�Ϸ�
#define checkValidRegType(type) ((type < 0 || type > MAX_REG_TYPE)?false:true)
/*Added End */

//���Ʒָ���
//const char CONTROL_FLAG[] = ",";
char TS_PROGRAM_NAME[] = "ts.exe";
#define CONTROL_FLAG_LEN         strlen(CONTROL_FLAG)
const char ANSWER_SUCEESS[] = "OK";             //���ذ��ɹ����ַ���
#define ANSWER_SUCEESS_LEN         strlen(ANSWER_SUCEESS)

const char ANSWER_FAIL[] = "E01"; 
#define ANSWER_FAIL_LEN    strlen(ANSWER_FAIL)

/**
 ����16����ASCII�ַ���
 */
const char hexchars[]="0123456789abcdef";

#define DIDLEN      sizeof(US)             //�Ի�ID���ֽڳ���
#define FILEBUFSIZE 3*1024             //�����ļ�����ʱÿ�δ��͵����ݴ�С

map<US, WSocket*> mapIntToWSocket;            //���ӵ�AID�ŵ�wsocket����Ĺ���
TMutex                      MtxSocket;                              //mapIntToWSocket�ı�����
map<US,  US> mapLnkToDID;                                //���Ӿ�����Ի�ID��ӳ��
TMutex           mtxDID;                                    //mapLnkToDID�ı�����

//
typedef map<US,  WSocket* > INT2SOCKET;
typedef map<US,  US> LNK2DID;

 int TIMEOUT_GETPKT = 5;                               //���ư����շ��ذ���ʱʱ��, ��λ����
const int MAX_TIME_OUT = 15;            //���ʱʱ��Ϊ15��
const int MIN_TIME_OUT = 5;             //��С�ĳ�ʱʱ��Ϊ5��
/************�������ֵ����Ϣӳ��*****************/
#define MAX_ERR_NUM     100
#define ERR_MSG_CHAR_NUM    3
#define CONTROL_COMMAND_LEN      1                         //����ʶ����ĳ���
const char ERRHEAD[] = "E";

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
const char  QUERY_HOST_COMM_CONFIG = 'p';         //���ư�TClient_queryHostSerialPort��ʶ���
const char  SET_TARGET_SWITCH_FLAG = 'a';           //���ư�TClient_setTargetChannelSwitchStateʶ���
const char  SET_TS_SYSTEM_LOGOUT_LEVEL  = 'L';    //���ư�TClient_setSystemLogLevel��ʶ���
const char  SET_SERVICE_PACK_LOG_FLAG = 'l';         //���ư�TClient_SetServiceAgentLog��ʶ���
const char  CLOSE_LINK = 'd';                                     //���ư�TClient_Close������ʶ���
const char  UPLOAD_FILE = 'F';                                    //���ư�TClient_UpLoadFile������ʶ����
const char  DOWNLOAD_FILE = 'f';                                //���ư�TClient_DownLoadFile������ʶ����
const char  DELETE_FILE = 'K';                                        //���ư�TClient_DeleteFile������ʶ����

#define ERR_UNKNOW_ERR                          "E01"                                     //δ֪����
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
#define ERR_CH_SW_FUNC_LOCKED_BY_OHTER      "E17"                                 //ͨ���л����ܱ���������
#define ERR_SRV_CFG_NOT_ENOUGH                  "E18"                               //�������ò�ȫ
#define ERR_SRV_CFG_ERR                         "E19"                                           //�������ô���
#define ERR_SRV_NUM_EXPIRE                      "E20"                                       //�����ķ��������ﵽ����
#define ERR_DLL_BUF_LOST                        "E21"                                           //ȱ��DLL��BUF����
#define ERR_CHANGE_CHANNEL_FAILURE      "E23"                                       //ͨ���л�ʧ��
#define ERR_SRV_CHANNEL_SWITCH_DISABLE   "E25"                                 //ͨ���л�������
#define ERR_FILE_ALREADY_EXIT                               "E26"                            //�ļ��Ѿ�����(�ļ��ϴ�)
/*Modifide by tangxp on 2007.9.26 [begin]*/
/*  Modified brief:����GUID�������� */
#define ERR_INVALID_GUID                        "E27"                       //GUID����
/*Modifide by tangxp on 2007.9.26 [end] */
typedef struct 
{
    char ErrData[10];
    short ErrValue;
}ERRMSG;
//������Ϣ�������ֵ֮���ӳ��
ERRMSG ERR_MAP[MAX_ERR_NUM] =
{
    {ERR_UNKNOW_ERR,  TS_UNKNOW_ERROR}, 
    {ERR_UNKNOW_DATA,  TS_UNKNOW_DATA}, 
    {ERR_FILE_NOT_EXIST,  TS_FILE_NOT_EXIST}, 
    {ERR_INVALID_SERVICE_ID,  TS_INVALID_SERVICE_ID}, 
    {ERR_TIME_OUT,  TS_TIME_OUT}, 
    {ERR_INVALID_FILE_NAME,  TS_INVALID_FILE_NAME}, 
    {ERR_DEVICE_CANNOT_USE,  TS_DEVICE_CANNOT_USE}, 
    {ERR_REC_BUF_LIMITED,  TS_REC_BUF_LIMITED}, 
    {ERR_INVALID_CH_ID, TS_INVALID_CH_ID},
    {ERR_INVALID_SERVICE_NAME,  TS_INVALID_SERVICE_NAME}, 
    {ERR_INVALID_IP,  TS_INVALID_IP}, 
    {ERR_INVALID_UART_PORT,  TS_INVALID_UART_PORT}, 
    {ERR_SERVICE_NOT_EXIST,  TS_SERVICE_NOT_EXIST}, 
    {ERR_SERVICE_NAME_REPEATED,  TS_SERVICE_NAME_REPEATED}, 
    {ERR_UNKNOW_COMM_WAY,  TS_UNKNOW_COMM_WAY}, 
    {ERR_UNKNOW_PROTOCOL,  TS_UNKNOW_PROTOCOL}, 
    {ERR_SRV_CFG_NOT_ENOUGH,  TS_SRV_CFG_NOT_ENOUGH}, 
    {ERR_SRV_CFG_ERR,  TS_SERVICE_CONFIG_ERR}, 
    {ERR_SRV_NUM_EXPIRE,  TS_SRV_NUM_EXPIRE}, 
    {ERR_DLL_BUF_LOST,  TS_DLL_BUF_LOST}, 
    {ERR_CHANGE_CHANNEL_FAILURE, TS_CHANGE_CHANNEL_FAILURE},
    {ERR_CH_SW_FUNC_LOCKED_BY_OHTER, TS_CH_SW_FUNC_LOCKED_BY_OHTER},
    {ERR_SRV_CHANNEL_SWITCH_DISABLE, TS_SRV_CHANNEL_SWITCH_DISABLE},
    {ERR_FILE_ALREADY_EXIT, TS_FILE_ALREADY_EXITS},
    /*Modifide by tangxp on 2007.9.26 [begin]*/
    /*  Modified brief:����GUID��������ӳ�� */
    {ERR_INVALID_GUID, TS_INVALID_GUID},  
    /*Modifide by tangxp on 2007.9.26 [end] */
    {"", 0}
    
};

/*************ǰ������***************************/
//���ظ���Ϣ���е�DID�뵱ǰӳ����е�DID���бȽ�
//����ظ���Ϣ���е�DIDС�ڵ�ǰӳ����е�DID
//�������һ���ظ���Ϣ, ���������Ϣ�е�DID��С
//��ǰӳ����е�DID���򷵻�ϵͳ����
#define AssertPackDIDAndCurDID(packDID,  curDID) {\
                                                                    if((65535 == packDID) && (0 == curDID))\
                                                                    {\
                                                                        continue;\
                                                                    }\
                                                                    else if(packDID > curDID)\
                                                                    {\
                                                                        return TS_SYSTEM_ERROR;\
                                                                    }\
                                                                    else\
                                                                    {\
                                                                        continue;\
                                                                    }\
                                                                 }



//��ָ���Ĵ����ַ������ش���ֵ
static short returnErrValueFromErrMsg(const char *buf);
//���ָ��IP�Ƿ�Ƿ�IP
static bool checkInvalidIP(const char *ip,  size_t iplen);
//��̬������ʱʱ��
static void adjustSocketTime(bool timeoutFlag);
//���˿���������ڲ��������
static int filtrateError(int errValue);
/*************ʵ�ֲ���*************************/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        WSocket::Init();    //��ʼ��socket����
    return TRUE;
}


/**
 * ����: ��value��ֵ, �����ڻ�����dest��, ռ�õĴ�Сλbytes
 * @param value  Ҫ���������
 * @param dest   ������ָ��
 * @param bytes  Ҫ�������ݵĴ�С
 * @return 0��ʾ�ɹ�
 */
template<class T> int __store(T value, char *dest, size_t bytes) 
{
    int upper_bit = (value < 0 ? 128 : 0);
    size_t i;

    if (value < 0)
    {
        T oldvalue = value;
        value = -value;
        if (oldvalue != -value)
            return 1;
    }

    size_t iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;
    for(i = 0 ; i < iCount ; i++)
    {
        dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
        value = value / 256;
    }

    if (value && value != -1)
        return 1;

    for(; i < bytes ; i++)
        dest[i] = 0;
    dest[bytes - 1] |= upper_bit;
    return 0;
}

/**
 * ����: �ӻ�����dest�ж�ȡbytes�ֽ�, �����浽dest��
 * @param value  Ҫ���������
 * @param dest   ������ָ��
 * @param bytes  Ҫ�������ݵĴ�С
 * @return 0��ʾ�ɹ�
 */
template<class T> int __fetch(T *dest, char *source, size_t bytes) 
{
    T value = 0;
    int i;

    for (i = bytes - 1; (int) i > (sizeof (*dest) - 1); i--)
        if (source[i] & ((int) i == (bytes - 1) ? 127 : 255 ))
            return 1;

    for (; i >= 0; i--)
        value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));

    if ((source[bytes - 1] & 128) && (value > 0))
        value = - value;

    *dest = value;
    return 0;
}



/**
 * ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
 * @param ch    ������16����ASCII�ַ�
 * @param intValue[OUT]   ת�����32λ����
 * @return ����ת����16�����ַ������ȡ�
*/
int hex2num(unsigned char ch)
{
  if (ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  if (ch >= '0' && ch <= '9')
    return ch-'0';
  if (ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  return -1;
}
/**
 * ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
 * @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
 *        �������ת������ʱ��ָ�롣
 * @param intValue[OUT]   ת�����32λ����
 * @return ����ת����16�����ַ������ȡ�
*/
unsigned int hex2short(char **ptr,  short *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hex2num(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

  return (numChars);
}
/**
 * ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
 * @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
 *        �������ת������ʱ��ָ�롣
 * @param intValue[OUT]   ת�����32λ����
 * @return ����ת����16�����ַ������ȡ�
*/
unsigned int hex2int(char **ptr,  int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
      hexValue = hex2num(**ptr);
      if (hexValue < 0)
        break;

      *intValue = (*intValue << 4) | hexValue;
      numChars ++;

      (*ptr)++;
    }

  return (numChars);
}


/**
 * ����: ��intת����16���Ƶ��ַ���, ���뱣֤size��С����ת������ַ�����С
 * @param ptr ����16�����ַ����Ļ�����
 * @param size   �������Ĵ�С
 * @param intValue   Ҫת��������
 * @return ת���Ĵ�С
*/
unsigned int int2hex(char *ptr,  int size, int intValue)
{
    memset(ptr, '\0', size);    //���ô�С, ��󻺳����Ĵ�С
    sprintf_s(ptr, size,  "%x", intValue);
    return (UI)strlen(ptr);
}

/********************************************************************
 * ������:   strtoi
 --------------------------------------------------------------------
 * ����:     ���ַ���ת����������
 --------------------------------------------------------------------
 * �������: nptr ---ָ���ַ�����ָ�롣
 *             base ---ת���Ľ��ơ�
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * �������: �ޡ�
 --------------------------------------------------------------------
 * ����ֵ:     ����ת�����������
 --------------------------------------------------------------------
 * ������:   ȫ��
 ********************************************************************/
int strtoi(char *nptr,  int base)
{
#define LONG_MIN_ (0x80000000)
#define LONG_MAX_ (0x7fffffff)
    register const char *s = nptr;
    register unsigned int acc;
    register int c;
    register unsigned int cutoff;
    register int neg = 0,  any,  cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0,  allow 0x for hex and 0 for octal,  else
     * assume decimal; if base is already 16,  allow 0x.
     */
    do
    {
        c = *s++;
    } while (isspace(c));
    if (c == '-') 
    {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) 
        && (c == '0') && (*s == 'x' || *s == 'X')) 
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value,  divided by the
     * base.  An input number that is greater than this value,  if
     * followed by a legal input character,  is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance,  if the range for longs is
     * [-2147483648..2147483647] and the input base is 10, 
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1),  meaning that if we have accumulated
     * a value > 214748364,  or equal but the next digit is > 7 (or 8), 
     * the number is too big,  and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? (-(unsigned int)LONG_MIN_) : LONG_MAX_;
    cutlim = cutoff % (unsigned int)base;
    cutoff /= (unsigned int)base;
    for (acc = 0,  any = 0;; c = *s++) 
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else 
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) 
    {
        acc = neg ? LONG_MIN_ : LONG_MAX_;
    } else if (neg)
        acc = -acc;
    return (acc);
}

/**
 * ����: �ر�ָ��������
 * @param fd  ���Ӿ��
 * @return : 
         TS_INVALID_LINK_HANDLE: ����fd����Ч�ľ��
         TS_SOCKET_ERROR:  �������TS�����ʱSOCEKT���������رշ��ظ�ֵ
         TS_SUCCESS:    �ɹ��رա�    
 */
DLL_FUNC_INT TClient_close(US fd)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US curDID = 0;
    int ret = 0;
    
    //�ҵ���Ӧ��wsocket����
    TMutexLocker lockerSocket(&MtxSocket);
    TMutexLocker lockerDID(&mtxDID);
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    INT2SOCKET::iterator  m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket* sktClient = NULL;
    //������Ϣ��TS����֪ͨ��ɾ���������
    if(m_ItrDID == mapLnkToDID.end())
    {
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &CLOSE_LINK,  CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd,  0,  0,  0,  buf,  (UI)(pBuf - buf));   
    if(ret <= 0)
    {
        //��Ϣδ���ͳɹ�
        return filtrateError(ret);
    }
    //���ͳɹ���ɾ�����˶�������
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = m_ItrSocket->second;
        sktClient->Close();
        mapIntToWSocket.erase(m_ItrSocket);
        delete sktClient;
        sktClient = NULL;
    }
    if(m_ItrDID != mapLnkToDID.end())
    {
        mapLnkToDID.erase(m_ItrDID);
    }
    return TS_SUCCESS;
    
}

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
                             const char* data, UI size) 
{
    if (data == NULL)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    //ͨ��AID�ҵ���Ӧ��socket����
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }

    //�԰�ͷ�����ֽ����в���
    char head[PACKET_HEADSIZE] = "\0";
    char* pBuf = head;
    __store(size, pBuf, sizeof(int));
    pBuf += sizeof(int);
    __store(des_aid, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store((short)fd, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(des_said, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(src_said, pBuf, sizeof(short));
    int reserve = 0;
    int iRet = 0;
    iRet = sktClient.Send(head,  PACKET_HEADSIZE, 0);    //���Ͱ�ͷ��Ϣ
    if (iRet == -1)    //���ӵ�socket������
    {
        return TS_SOCKET_ERROR;
    }
    iRet = sktClient.Send(data,  size,  0);                //���Ͱ�����Ϣ
    if (iRet == -1)    //���ӵ�socket������
    {
        return TS_SOCKET_ERROR;
    }
    return iRet;
}
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
                            char* data, UI dataSize) 
{
    if (src_aid == NULL || des_said == NULL || src_said == NULL || data == NULL)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    /*Modified by qingxiaohai on 2006��11��17�� 11:07:46 for BUG 1102 [Begin]*/
    /*  Modified brief:��ʼ�����ջ���*/
    memset(data,  0,  dataSize);
    /*Modified for BUG  [End]*/
    //ͨ��AID�ҵ���Ӧ��socket����
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }
    
    char recvbuf[PACKET_HEADSIZE] = "\0";
    char* pBuff = recvbuf;

    //��ȡ��ͷ
    UI size = 0;
    int rs = sktClient.Recv(recvbuf,  sizeof(recvbuf));
    if ( rs <= 0 )
    {
        return TS_SOCKET_ERROR;
    }
    //��黺�����Ĵ�С���յ����Ĵ�С
    //�Խ��յ��İ�ͷ�����ֽ������ʽ����
    __fetch(&size, recvbuf, sizeof(int));
    pBuff += sizeof(int);
    
    short des_aid = 0;
    __fetch(&des_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);    
    __fetch(src_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(des_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(src_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    int reserve = 0;
    __fetch(&reserve, pBuff, sizeof(int));
    //Ҫ���յ����ݴ��ڻ�����������
    if (size > dataSize)
    {
        return TS_REC_BUF_LIMITED;
    }
    //��ȡ����
    rs = sktClient.Recv(data,  size,  0) ;
    if ( rs <= 0 ) 
    {
        return TS_SOCKET_ERROR;
    }

    return rs;
}
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
DLL_FUNC_INT TClient_wait(US fd, int timeout) 
{
    //ͨ��AID�ҵ���Ӧ��socket����
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket  sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }
    
    if (timeout > -1) 
    {
        int flags = 0;
        struct timeval overtime;
        //���ӳ�ʱ66����
        overtime.tv_sec = timeout;
        overtime.tv_usec = 0;
        FD_SET RSet;
        FD_SET WSet;
        FD_ZERO(&RSet);
        FD_ZERO(&WSet);
        FD_SET(sktClient.m_sock,  &RSet);
        FD_SET(sktClient.m_sock,  &WSet);

        int ret = select(0,  &RSet,  NULL,  NULL,  &overtime);
        if ( ret == 0 ) 
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;        //��ʱ
        }
        else if (ret < 0)    //socket�ر�
            return TS_SOCKET_ERROR;
    }
    adjustSocketTime(false);
    return TS_DATACOMING;
}
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
DLL_FUNC_INT TClient_open(const char* IP, US port)
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    if (IP == NULL)
        return TS_NULLPTR_ENCONTERED;
    if(!checkInvalidIP(IP, (UI)strlen(IP)))
    {
        return TS_INVALID_IP;
    }
    WSocket *sktClient = new WSocket;
    sktClient->Create(AF_INET,  SOCK_STREAM,  0);    //������������socket
    // Connect
    if(port == 0)
    {
        port = LISTEN_PORT;
    }
    if( sktClient->Connect(IP,  port) != true ) 
    {
        delete sktClient;
        return TS_SOCKET_ERROR;
    }
    //��manager�����ַ�'?', ���յ�managerΪ�˷����AID��
    /*Modified by qingxiaohai on 2006��12��14�� 15:48:19 for  TS3.2 [Begin]*/
    /*  Modified brief:���ӿ�����Ϣ�ĶԻ�ID*/
    US dialogID = 0, packContentLen = 0;
    char* pBuf = NULL;
    char buf[5] = "\0";
    pBuf = buf;
    //fill dialog ID
    __store(dialogID,  pBuf,  DIDLEN);            //���Ի�ID
    pBuf+= DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);          //���Ʒָ���
    pBuf += CONTROL_FLAG_LEN;
    //control command
    memcpy(pBuf,   &CONNECT_FLAG,  CONTROL_COMMAND_LEN);                                //�����ӵ���Ϣ��ʶ
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    packContentLen = pBuf - buf;
    /*Modified for BUG  [End]*/
        
    //�԰�ͷ�����ֽ����в���
    char head[PACKET_HEADSIZE] = "\0";
    pBuf = head;
    __store((int)(packContentLen) , pBuf, sizeof(int));    //����DIDLEN+2���ֽ�'?'
    pBuf += sizeof(int);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    int reserve = 0;

    sktClient->Send(head,  PACKET_HEADSIZE, 0);    //���Ͱ�ͷ��Ϣ
    sktClient->Send(buf,  (int)packContentLen,  0);                //���Ͱ�����Ϣ

    //���õȴ�ʱ��
    int flags = 0;
    struct timeval overtime;
    //���ӳ�ʱ5 ��
    overtime.tv_sec = TIMEOUT_GETPKT;
    overtime.tv_usec = 0;
    FD_SET RSet;
    FD_SET WSet;
    FD_ZERO(&RSet);
    FD_ZERO(&WSet);
    FD_SET(sktClient->m_sock,  &RSet);
    FD_SET(sktClient->m_sock,  &WSet);

    int ret = select(0,  &RSet,  NULL,  NULL,  &overtime);
    if ( ret <= 0 ) 
    {    //��ʱ����socket���쳣
        sktClient->Close();
        delete sktClient;
        return TS_ISNOT_TS_SERVER;
    }

    US des_aid, src_aid, des_said, src_said;
    UI size;
    //��ȡmanager���ص�����
    char recvbuf[PACKET_HEADSIZE] = "\0";
    char* pBuff = recvbuf;
    US  recvDID = 0;
    //��ȡ��Ϣ����ͷ
    int rs = sktClient->Recv(recvbuf,  sizeof(recvbuf));
    if( rs <= 0 ) 
    {
        return TS_SOCKET_ERROR;
    }
    /*���յ��İ���ʽ
    ---------------
    |MSGHEAD|DID|, |
    ---------------
    */

    if(rs != PACKET_HEADSIZE)
    {
        return TS_UNKNOW_DATA;
    }
    //�Խ��յ��İ�ͷ�����ֽ������ʽ����
    __fetch(&size, pBuff, sizeof(UI));
    pBuff += sizeof(UI);

    __fetch(&des_aid, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&src_aid, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&des_said, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&src_said, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&reserve, pBuff, sizeof(int));
    pBuff += sizeof(int);

    if (size == DIDLEN+CONTROL_FLAG_LEN) 
    {
        //���հ���
        rs = sktClient->Recv(recvbuf,  DIDLEN+CONTROL_FLAG_LEN);
        if( rs <= 0 ) 
        {
            return TS_SOCKET_ERROR;
        }
        pBuff = recvbuf;
        __fetch(&recvDID,  pBuff,  DIDLEN);
        if(recvDID != dialogID)
        {
            return TS_UNKNOW_DATA;
        }
        //���յ����ӵ���Ϣ
        MtxSocket.lock();
        mtxDID.lock();
        mapIntToWSocket.insert(INT2SOCKET::value_type(des_aid, sktClient));
        mapLnkToDID.insert(LNK2DID::value_type(des_aid,  dialogID));
        MtxSocket.unlock();
        mtxDID.unlock();
        return des_aid;
    }
    else 
    {
        sktClient->Close();
        delete sktClient;
        return TS_UNKNOW_DATA;
    }
    
}

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
DLL_FUNC_INT TClient_getServerID(US fd, char* name) 
{
    //���з������ֵ����
    if(name == NULL)
        return TS_NULLPTR_ENCONTERED;

    if (!strlen(name) || strlen(name) > (NAME_SIZE-1))
        return TS_INVALID_SERVICE_NAME;
    int nameSize = (int)strlen(name);
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuff = buf;
    int ret = 0;
    US  packDID = 0,  curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
     curDID = m_ItrDID->second +1;
     m_ItrDID->second = curDID;
     mtxDID.unlock();
    //fill dialog ID
    __store(curDID,  pBuff,  DIDLEN);
    pBuff += DIDLEN;
    memcpy(pBuff,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuff += CONTROL_FLAG_LEN;
    //fill the command
    memcpy(pBuff,  &GETID,  CONTROL_COMMAND_LEN);                                //getID�Ŀ��ư���־��
    pBuff += CONTROL_COMMAND_LEN;
    memcpy(pBuff, name, nameSize);                            //����
    pBuff += nameSize;
    memcpy(pBuff,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuff += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuff-buf));        //��manager���������
    if (ret < 0 )
        return filtrateError(ret);
    
    //��ȡmanager���ص�����
    US des_aid, des_said=0, src_said;
    int bufSize = sizeof(buf);
    US aid = 0;
    char* pStr = NULL;
    while(true)
    {
        if (TClient_wait(fd,  TIMEOUT_GETPKT) == TS_DATACOMING)   //�ȴ�5��, ��5������������������ݵĽ���
        {
            //��̬������ʱʱ��
            adjustSocketTime(false);
            //��ȡmanager���ص�����, ��ʹ���ص���Ϣ���ᳬ��1�����ݰ��Ĵ�С, ����С���ܳ���PACKET_DATASIZE
            ret = TClient_getpkt(fd,  &des_aid,  &des_said,  &src_said,  buf,  sizeof(buf));
            if (ret < 0 )
                return filtrateError(ret);
            //�����հ��Ĵ�С�ж�
            /**
            ��ȷ���ĸ�ʽΪ
            -------------------
            |DID|, |OK|, |serverID|,|
            -------------------
            */
            pBuff = buf;
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            __fetch(&packDID,  pBuff,  DIDLEN);
            pBuff +=  DIDLEN + CONTROL_FLAG_LEN;
            if(packDID == curDID)
            {
                //�������ݰ�
                ret = returnErrValueFromErrMsg(pBuff);
                if(ret != TS_SUCCESS)
                {
                    return ret;
                }
                
                pStr = strstr(pBuff, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                pStr += CONTROL_FLAG_LEN;
                hex2short(&pStr, (short*)&aid);
                if(!IsInvalidAID(aid))
                {
                    return TS_SYSTEM_ERROR;
                }
                return aid;
            }        
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }            
            
        }
        else
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
     
    };

    return TS_SYSTEM_ERROR;
}


/**
 * ����: �������ݰ�������TS���ص����ݰ�����ȷ��������ֵ��Ϣ.
                   ��һ�����ڽ���ֻ�в���״̬��Ϣ,��û�в������
                   ���ݵĿ��ư��Ľ���,��ʽ��: |DID,|OK/Exx,|
 * @param fd   ���Ӿ��
 * @return 
        TS_SUCCESS:   �����ɹ�
        TS_INVALID_LINK_HANDLE: ��Ч�ľ����
        TS_UNKNOW_DATA: ��Ч������,һ�������ݳ���С����������С����
        TS_SYSTEM_ERROR: ϵͳ��������
        TS_TIME_OUT: ��ָ��ʱ����û���յ�����
        �Լ�����TS���صľ������ֵ
*/
int getAnswerPacketState(US fd) 
{
    US  packDID = 0,  curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = m_ItrDID->second;
    mtxDID.unlock();
    US des_aid, des_said=0, src_said;
    char recBuf[PACKET_DATASIZE] = "\0";
    int bufSize = sizeof(recBuf);
    int ret = 0;
    char *pBuf = NULL; 
     while(true)
     {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)   //�ȴ�5��, ��5������������������ݵĽ���
        {
            //��̬������ʱʱ��
            adjustSocketTime(false);
            //��ȡmanager���ص�����, ��ʹ���ص���Ϣ���ᳬ��1�����ݰ��Ĵ�С, ����С���ܳ���PACKET_DATASIZE
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if (ret < 0 )
                return filtrateError(ret);
            //�����ݰ��������ж�
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            __fetch(&packDID,  recBuf,  DIDLEN);
            pBuf = recBuf + DIDLEN + CONTROL_FLAG_LEN;
            if(packDID == curDID)
            {
                //�������ݰ�
                return returnErrValueFromErrMsg(pBuf);
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }            
                
        }
        else
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
         
     };
     return TS_SYSTEM_ERROR;
}
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
DLL_FUNC_INT TClient_createServerWithDll(US fd, ServerCfg* pServerConfig) 
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    if (pServerConfig == NULL || pServerConfig->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;

    ServerCfg* m_pServerConfig =pServerConfig;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    US  dialogID = 0;             //��ǰ�Ի�ID
    US  packDID = 0;            //�յ��ظ����еĶԻ�ID
    US des_aid, des_said=0, src_said;
    int bufSize = PACKET_DATASIZE;
    US aid = 0;
    char *pStr = NULL;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    dialogID = m_ItrDID->second +1;
    //������ǰDID����ӳ�����
    m_ItrDID->second = dialogID;
    mtxDID.unlock();
    //������Ϣ
    __store(dialogID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    //���ӿ����ַ�
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf,  &CREATE_SERVERWITHDLL,  CONTROL_COMMAND_LEN);                             //createServerWithDll�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;

    // the server name
    memcpy(pBuf, m_pServerConfig->name, strlen(m_pServerConfig->name));       //����
    pBuf += strlen(m_pServerConfig->name);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    // extInfo
    memcpy(pBuf, m_pServerConfig->extInfo, sizeof(m_pServerConfig->extInfo));
    pBuf += sizeof(m_pServerConfig->extInfo);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //��̬������
    memcpy(pBuf, m_pServerConfig->pComConfig->DLL.dllName, strlen(m_pServerConfig->pComConfig->DLL.dllName));    
    pBuf += strlen(m_pServerConfig->pComConfig->DLL.dllName);
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    //��־
    pBuf += int2hex(pBuf, 10, m_pServerConfig->isLog);        //��־���
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    //�ж���װ�ַ�����С�Ƿ񳬹���������С
    if (pBuf-buf+DLL_BUF > PACKET_DATASIZE)
        return TS_SERVICE_CONFIG_ERR;
    memcpy(pBuf,  m_pServerConfig->pComConfig->DLL.buf,  DLL_BUF);
    pBuf += DLL_BUF;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));           //��manager���������
    if (ret < 0 )
        return filtrateError(ret);
    //ѭ��������Ϣ��ֱ�����յ����εĻظ���ϢΪֹ
    
    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {
            //��̬������ʱʱ��
            adjustSocketTime(false);
            pBuf = buf;
            bufSize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, sizeof(buf));
            if(bufSize < 0)
            {
                return filtrateError(bufSize);
            }
            //������Ϣ���ȶ���Ϣ�������ж�
            if(!CheckValidPackBySize(bufSize))
            {
                return TS_UNKNOW_DATA;
            }
            //ȡ����Ϣ�е�DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //��pbufָ�������ݿ�ʼ����, ������DID����
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //��ָ��ָ����Ϣ�Ŀ�ʼ����,  ��OK, xx
            if(packDID == dialogID)
            {
                //�յ���ȷ�İ������а��Ľ���
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //ȡ�����е�aid
                pStr = strstr(pBuf,  CONTROL_FLAG);
                if(pStr != NULL)
                {
                    pStr += CONTROL_FLAG_LEN;
                    hex2short(&pStr,  (short*)&aid);
                    if(!IsInvalidAID(aid))
                    {
                        return TS_SYSTEM_ERROR;
                    }
                    return aid;
                }
                else
                {
                    //�������ݰ����󣬻ظ�����ʽ����
                    return TS_UNKNOW_DATA_FORMAT;
                }
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  dialogID)
            }
        }
        else
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;

}
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

���͸�ʽ: |DID|, Csa����,extInfo,  ��־,ͨ����, +����wayѡ�����������Ϣ(�Էָ��)
���ո�ʽ:      |DID|, |OK|, |aid|


 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: ��TClient_createServer���������ֶ�����(short)��Ϊ����(int)*/
DLL_FUNC_INT TClient_createServer(US fd, ServerCfg* pServerCfg) 
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    int ret = 0, size = 0;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    int ipSize = 0;
    US curDID = 0,  packDID = 0;
    
    if (pServerCfg == NULL || pServerCfg->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;
    if (!strlen(pServerCfg->name))
        return TS_INVALID_SERVICE_NAME;
    if((pServerCfg->pComConfig->way != SERVER_WAY_TCP) 
        && (pServerCfg->pComConfig->way != SERVER_WAY_UDP )
        && (pServerCfg->pComConfig->way != SERVER_WAY_USB )
        && (pServerCfg->pComConfig->way != SERVER_WAY_UART))
    {
        return TS_SERVICE_CONFIG_ERR;
    }
    //�����Ϣ
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = m_ItrDID->second + 1;
    m_ItrDID->second = curDID;
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //���з������ֵ����
    
    memcpy(pBuf,  &CREATE_SERVER,  CONTROL_COMMAND_LEN);                                //createServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    //name
    size = (int)strlen(pServerCfg->name);    //�õ����ֻ������Ĵ�С
    memcpy(pBuf,  pServerCfg->name,  strlen(pServerCfg->name));                            //����
    pBuf += size;

    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //arch info 
    memcpy(pBuf, pServerCfg->extInfo, sizeof(pServerCfg->extInfo));
    pBuf += sizeof(pServerCfg->extInfo);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);   
    pBuf += CONTROL_FLAG_LEN;
    //log
    pBuf += int2hex(pBuf, 10, pServerCfg->isLog);        //��־���
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, pServerCfg->channelCount);        //ͨ����
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    COMCONFIG* pCommConfig = pServerCfg->pComConfig;;
    for (UI i=0;i<pServerCfg->channelCount;++i) 
    {
        if(NULL == pCommConfig)
        {
            return TS_SERVICE_CONFIG_ERR;
        }
        pBuf += int2hex(pBuf, 10, pCommConfig->protocolType);        //Э�鷽ʽ
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;

        pBuf += int2hex(pBuf, 10, pCommConfig->way);                    //ͨ�ŷ�ʽ
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;

        switch (pCommConfig->way)
        {
            case SERVER_WAY_TCP:    //TCP
            case SERVER_WAY_UDP:    //UDP
                
                ipSize = (int)min(strlen(pCommConfig->NET.ip), IP_BUF_LEN);
                /*Modified by qingxiaohai on 2006��11��17�� 11:25:13 for BUG 1102 [Begin]*/
                /*  Modified brief:��ЧIP���*/
                if(!checkInvalidIP(pCommConfig->NET.ip,  (UI)strlen(pCommConfig->NET.ip)))
                {
                    return TS_INVALID_IP;
                }
                /*Modified for BUG  [End]*/
                memcpy(pBuf, pCommConfig->NET.ip, ipSize);        //IP��ַ
                pBuf += ipSize;

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                if(false == CheckInvalidPort(pCommConfig->NET.port))
                {
                    return TS_INVALID_IP_PORT;
                }
                pBuf += int2hex(pBuf, 10, pCommConfig->NET.port);    //�˿�

                memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;

                pBuf += int2hex(pBuf, 10, pCommConfig->NET.timeout);    //��ʱ

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;

                pBuf += int2hex(pBuf, 10, pCommConfig->NET.retry);    //���Դ���

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;                
                break;
                
            case SERVER_WAY_UART:
                
                ipSize = (int)min(strlen(pCommConfig->UART.channel), UART_NAME_BUF_LEN);
                //���ڶ˿�
                memcpy(pBuf,  pCommConfig->UART.channel,  ipSize);        
                pBuf += ipSize;
                memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                //������
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.baud);
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                 //��ʱ
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.timeout);   
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                //���Դ���
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.retry);        
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                
                break;
            case SERVER_WAY_USB:
                //USB device GUID
                memcpy(pBuf, pCommConfig->USB.GUID, strlen(pCommConfig->USB.GUID));
                pBuf += strlen(pCommConfig->USB.GUID);
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                pBuf += CONTROL_FLAG_LEN;
                break;
                
            case SERVER_WAY_DLL:  
            default:
                return TS_SYSTEM_ERROR;                
        }
        //��һ������
        pCommConfig = pCommConfig->next;
    }
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0 )
        return filtrateError(ret);
    //��ȡmanager���ص�����
    
    US des_aid,  des_said=0,  src_said,  aid = 0;
    char recBuf[20] = "\0";
    char *pdest = NULL,  *pStr = NULL;
    /**
     ���ص����ݰ���ʽ
     ---------------
     |DID|, |OK|, |aid|
     ---------------
    */
    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {
            //��̬������ʱʱ��
            adjustSocketTime(false);
            pBuf = recBuf;
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if(ret < 0)
            {
                return filtrateError(ret);
            }
            //������Ϣ���ȶ���Ϣ�������ж�
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            //ȡ����Ϣ�е�DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //��pbufָ�������ݿ�ʼ����, ������DID����
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //��ָ��ָ����Ϣ�Ŀ�ʼ����,  ��OK, xx
            if(packDID == curDID)
            {
                //�յ���ȷ�İ������а��Ľ���
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //ȡ�����е�aid
                pStr = strstr(pBuf,  CONTROL_FLAG);
                if(pStr != NULL)
                {
                    pStr += CONTROL_FLAG_LEN;
                    hex2short(&pStr,  (short*)&aid);
                    if(!IsInvalidAID(aid))
                    {
                        return TS_SYSTEM_ERROR;
                    }
                    return aid;
                }
                else
                {
                    //�������ݰ����󣬻ظ�����ʽ����
                    return TS_UNKNOW_DATA_FORMAT;
                }
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;
}

/**
 * ����: �ر�ָ����TS
 * @param fd   ���Ӿ��
 * @return 
        TS_SUCCESS�����ɹ�
        TS_INVALID_LINK_HANDLE:  ��Ч�����Ӿ����
        TS_SOCKET_ERROR:   �������ݰ�ʱSOCKET��������
 */
DLL_FUNC_INT TClient_closeTM(US fd) 
{
    //���з������ֵ����
    char buf[100] = "\0";
    char *pBuf = buf;
    US curDID = 0,  packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID  = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &CLOSE_TS,  CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd,  0,  0,  0,  buf,  (UI)(pBuf - buf));   
    if(ret <= 0)
    {
        //��Ϣδ���ͳɹ�
        return filtrateError(ret);
    }
    //����Զ˳ɹ��յ���������Ϣ��
    //�򷵻�OK,  ��߾�ɾ����Ӧ�����Ӿ��
    ret =  getAnswerPacketState(fd);
    if(ret != TS_SUCCESS)
    {
        return ret;
    }
    MtxSocket.lock();
    mtxDID.lock();
    INT2SOCKET::iterator m_ItrSocket = mapIntToWSocket.find(fd);
    m_ItrDID = mapLnkToDID.find(fd);
    WSocket* sktClient = NULL;
    if(m_ItrSocket != mapIntToWSocket.end())
    {
        sktClient = m_ItrSocket->second;
        sktClient->Close();
        mapIntToWSocket.erase(m_ItrSocket);
        delete sktClient;
        sktClient = NULL;

    }
    if(m_ItrDID != mapLnkToDID.end())
    {
        mapLnkToDID.erase(m_ItrDID);
    }
    mtxDID.unlock();
    MtxSocket.unlock();
    return TS_SUCCESS;

}

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
DLL_FUNC_INT TClient_changeChannelType(US fd,  US aid, UC channelType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0,  packDID = 0 ;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf,  &CHANGE_CHANNEL_TYPE,  CONTROL_COMMAND_LEN);                                //deleteServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, channelType);            //����
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);

}
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
DLL_FUNC_INT TClient_deleteServer(US fd, US aid) 
{
    char buf[100] = "\0";
    char* pBuf = buf;
    US curDID = 0,  packDID = 0;
    int ret = 0;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &DELETE_SERVER,  CONTROL_COMMAND_LEN);    //deleteServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}
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
 |DID|,|Maid|,|name|,|extInfo|,|isLog|,|channelCount|,|curChannel|,|channelConfig..............|
 -----------------------------------------------------------------------------
 DLL��ʽ
  -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|extInfo|,|isLog|,|way|,|DllBuf|
 -----------------------------------------------------------------------------
���ո�ʽ: DID,OK/Exx,
*/
DLL_FUNC_INT TClient_modifyServer(US fd, ServerCfg* pServerConfig) 
{
    if (pServerConfig == NULL || pServerConfig->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;
    ServerCfg* pServerCfg = pServerConfig;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    int dllBufSize = 0,  curBufSize= 0;
    int ipSize = 0,  ret = 0;
    US curDID = 0,  packDID = 0;

    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &MODIFY_SERVER,  CONTROL_COMMAND_LEN);                                        //modifyServerState�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(pServerCfg->aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, pServerCfg->aid);                //AID��
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //name
    memcpy(pBuf, pServerCfg->name, strlen(pServerCfg->name));    //����
    pBuf += strlen(pServerCfg->name);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //extInfo
    memcpy(pBuf, pServerCfg->extInfo, sizeof(pServerCfg->extInfo));
    pBuf += sizeof(pServerCfg->extInfo);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);           
    pBuf += CONTROL_FLAG_LEN;
    //log
    pBuf += int2hex(pBuf,  10,  pServerCfg->isLog);                //��־��¼
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //��̬������
    if (pServerCfg->pComConfig->way == SERVER_WAY_DLL) 
    {
        pBuf += int2hex(pBuf, 10, pServerCfg->pComConfig->way);                //ͨ�ŷ�ʽ
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;

        curBufSize = (UI)(pBuf-buf);
        if (curBufSize+DLL_BUF> PACKET_DATASIZE -1)
        {
            return TS_SERVICE_CONFIG_ERR;
        }            
        memcpy(pBuf, pServerCfg->pComConfig->DLL.buf, DLL_BUF);        //
        pBuf += DLL_BUF;
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;
    }
    else 
    {
        pBuf += int2hex(pBuf, 10, pServerCfg->channelCount);        //ͨ����
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;

        pBuf += int2hex(pBuf, 10, pServerCfg->currentChannel);    //��ǰͨ��
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
        pBuf += CONTROL_FLAG_LEN;

        COMCONFIG* pCommConfig = pServerCfg->pComConfig;;
        for (UI i=0;i<pServerCfg->channelCount;++i) 
        {
            if(NULL == pCommConfig)
            {
                if(i != 0)
                {
                    break;               //�ǵ�һ��ͨ��,��ֻ��д�޸ĵ�һ��ͨ����Ϣ
                }
                return TS_SERVICE_CONFIG_ERR;
            }
            pBuf += int2hex(pBuf, 10, pCommConfig->protocolType);//Э������
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //Э������
            pBuf += CONTROL_FLAG_LEN;

            pBuf += int2hex(pBuf, 10, pCommConfig->way);            //ͨ�ŷ�ʽ
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
            pBuf += CONTROL_FLAG_LEN;
            switch (pCommConfig->way) 
            {
                case SERVER_WAY_TCP:    //TCP
                case SERVER_WAY_UDP:    //UDP
                    //IP��ַ
                    ipSize = (int)min(strlen(pCommConfig->NET.ip), IP_BUF_LEN);
                    memcpy(pBuf, pCommConfig->NET.ip, ipSize);    
                    pBuf += ipSize;

                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                    //�˿ں�
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.port);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                    //��ʱ
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.timeout);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                     //���Դ���
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.retry);   
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;            
                    break;
                    
                case SERVER_WAY_UART:

                    ipSize = (int)min(UART_NAME_BUF_LEN, strlen(pCommConfig->UART.channel));
                    //���ڶ˿�
                    memcpy(pBuf, pCommConfig->UART.channel, ipSize);    
                    pBuf += ipSize;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                     //������
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.baud);   
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                    //��ʱ
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.timeout);
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;
                    //���Դ���
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.retry);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;          
                    break;
                case SERVER_WAY_USB:
                    memcpy(pBuf, pCommConfig->USB.GUID, strlen(pCommConfig->USB.GUID));
                    pBuf += strlen(pCommConfig->USB.GUID);
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ӿ��Ʒ���
                    pBuf += CONTROL_FLAG_LEN;          
                    break;
                    
                default:
                    return TS_UNKNOW_COMM_WAY;
            }

            //��һ��ͨ������
            pCommConfig = pCommConfig->next;
        }
    }
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}


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
OK, name, extInfo, state, log, chcount, curch, config of each ch..
DLL
OK, name, extInfo, state, log, chcount, curch, protocol, way, dllname, outbuf, inbuf, 
*/
DLL_FUNC_INT TClient_queryServer(US fd, ServerCfg* pServerCfg) 
{
    if (NULL == pServerCfg || NULL == pServerCfg->pComConfig)
        return TS_NULLPTR_ENCONTERED;
    
    char recBuf[PACKET_DATASIZE] = "\0";
    char* pBuf = recBuf;
    int ret = 0, bufSize = sizeof(recBuf), currentChannel = 0;
    size_t dllBufsize = 0;
    US des_aid, des_said=0, src_said;
    char *pdest = NULL, *pStr = NULL;
    COMCONFIG* pCommConfig = NULL;
    US curDID = 0, packDID = 0, idx = 0;
    
    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the contorl command
    
    memcpy(pBuf, &QUERY_SERVER_STATE, CONTROL_COMMAND_LEN);                                //queryServerState�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(pServerCfg->aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, pServerCfg->aid);                                //����AID��
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                //���ư���־��
    pBuf += CONTROL_FLAG_LEN;
    ret = TClient_putpkt(fd, 0, 0, 0, recBuf, (UI)(pBuf-recBuf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);


    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING) 
        {
            //��̬������ʱʱ��
            adjustSocketTime(false);
            pBuf = recBuf;
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if(ret < 0)
            {
                return filtrateError(ret);
            }
            //�������ݰ���С�����ݺϷ��ĳ����ж�
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            //ȡ����Ϣ�е�DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //��pbufָ�������ݿ�ʼ����, ������DID����
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //��ָ��ָ����Ϣ�Ŀ�ʼ����,  ��OK, xx
            if(packDID == curDID)
            {
                //�յ���ȷ�İ������а��Ľ���
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //��ȷ�ظ�����Ϣ���ĸ�ʽΪ
                /**
                TCP/UDP/UART
                OK, name, extInfo, state, log, chcount, CurChannel, config of each ch..
                DLL
                OK, name, extInfo, state, log, chcount, CurChannel, protocol, way, dllname, outbuf, inbuf, 
                */
                //OK,
                pdest = strstr(pBuf, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pdest == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                pdest += CONTROL_FLAG_LEN;
                //name,
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                //��������������
                if (pStr-pdest > NAME_SIZE)
                    return TS_INVALID_SERVICE_NAME;
                
                memcpy(pServerCfg->name, pdest, pStr-pdest);            //����
                pServerCfg->name[pStr-pdest] = '\0';                //���ַ�����β��
                pdest = pStr + CONTROL_FLAG_LEN;
                //extInfo
                if(bufSize < pdest - recBuf + EXT_INFO_LEN + CONTROL_FLAG_LEN)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                memcpy(pServerCfg->extInfo, pdest, EXT_INFO_LEN);
                pServerCfg->extInfo[EXT_INFO_LEN-1] = '\0';
                pdest += EXT_INFO_LEN + CONTROL_FLAG_LEN;
                
                //state,
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->state);        //״̬
                pdest = pStr + CONTROL_FLAG_LEN;

                //isLog,
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                int logflag = 0;
                hex2int(&pdest, &logflag);
                pServerCfg->isLog = ((logflag == 0)?0:1);        //��־��¼
                pdest = pStr + CONTROL_FLAG_LEN;
                //chCount,
                pStr = strstr(pdest, CONTROL_FLAG);        //ͨ����
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->channelCount);        //ͨ����
                
                pdest = pStr + CONTROL_FLAG_LEN;
                if (pServerCfg->channelCount > MAX_CHANNEL_COUNT)
                    return TS_INVALID_CH_COUNT;
                //CurChannel,
                pStr = strstr(pdest, CONTROL_FLAG);        //��ǰ�����ͨ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->currentChannel);
                pdest = pStr + CONTROL_FLAG_LEN;

                //special config of each channel
                //������do-while������for����ΪDLL������û��ͨ�����ģ�
                //����ͨ��1��������Ч��
                idx = pServerCfg->channelCount;
                pCommConfig = pServerCfg->pComConfig;
                do
                {
                    if(NULL == pCommConfig)
                    {
                        return TS_REC_BUF_LIMITED;
                    }
                    //��ǰ��ͨ������
                    pStr = strstr(pdest, CONTROL_FLAG);        
                    if (pStr == NULL )
                        return TS_UNKNOW_DATA_FORMAT;
                    hex2int(&pdest, (int*)&pCommConfig->protocolType);
                    pdest = pStr + CONTROL_FLAG_LEN;

                    //�õ���ǰͨ�����÷�ʽ
                    pStr = strstr(pdest, CONTROL_FLAG);        
                    if (pStr == NULL )
                        return TS_UNKNOW_DATA_FORMAT;
                    hex2int(&pdest, (int*)&pCommConfig->way); 
                    pdest = pStr + CONTROL_FLAG_LEN;

                    switch(pCommConfig->way)
                    {
                        case SERVER_WAY_TCP:
                        case SERVER_WAY_UDP:
                            //find control flag after IP
                            pStr = strstr(pdest, CONTROL_FLAG);       
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            if (pStr-pdest > IP_BUF_LEN)
                                return TS_INVALID_IP;
                            memcpy(pCommConfig->NET.ip, pdest, pStr-pdest);               
                            pCommConfig->NET.ip[pStr-pdest] = '\0';    
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            //find control flag after port
                            pStr = strstr(pdest, CONTROL_FLAG);        
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.port = strtoi(pdest, 16);    //�˿�
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after timout
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.timeout = strtoi(pdest, 16);    //��ʱ
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after retry
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.retry = strtoi(pdest, 16);    //���Դ���
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            break;
                            
                        case SERVER_WAY_UART:
                            //find the control flag after UART-channel
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                             if (pStr-pdest > UART_NAME_BUF_LEN)
                                return TS_INVALID_UART_PORT;
                             //COMM port
                            memcpy(pCommConfig->UART.channel, pdest, pStr-pdest);                                  
                            pCommConfig->UART.channel[pStr-pdest] = '\0';    //���ַ�����β��
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find the control flag after baud
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.baud = strtoi(pdest, 16);    //������
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after timout
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.timeout = strtoi(pdest, 16);    //��ʱ
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after retry
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.retry = strtoi(pdest, 16);    //���Դ���
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            break;
                            
                        case SERVER_WAY_DLL:
                            //find the control flag after DllName
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
        
                            memset(pCommConfig->DLL.dllName, 0, NAME_SIZE);  
                            if (pStr-pdest > NAME_SIZE)
                                return TS_INVALID_DLL_NAME;
                            memcpy(pCommConfig->DLL.dllName, pdest, pStr-pdest);             
                            pdest = pStr + CONTROL_FLAG_LEN;   
                            //find the control flag after outbuf
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��[outbuf]
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pStr = strstr(pdest+CONTROL_FLAG_LEN,  CONTROL_FLAG);        //�Ƿ��зָ��[inbuf]
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            dllBufsize = (size_t)(pStr - pdest+2*CONTROL_FLAG_LEN);
                            memcpy(pCommConfig->DLL.buf, pdest, dllBufsize);    //��С
                            pdest = pStr+CONTROL_FLAG_LEN;

                            break;

                            /*Modified bu tangxp on 2007.9.26 [begin]*/
                            /*  Modified brief:���Ӳ�ѯUSBͨ�ŷ�ʽ��Ŀ�������*/
                        case SERVER_WAY_USB:
                            //finde the control flag after GUID
                            pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            //GUID
                            memset(pCommConfig->USB.GUID, 0, GUID_LEN);
                            if (pStr-pdest > GUID_LEN)
                                return TS_INVALID_GUID;
                            memcpy(pCommConfig->USB.GUID, pdest, pStr-pdest); 
                            break;
                            /*Modified bu tangxp on 2007.9.26 [end] */
                            
                        default:
                            
                            return TS_UNKNOW_COMM_WAY;
                    }                   
                    
                    pCommConfig = pCommConfig->next;
                        
                }while(--idx > 0);
                
                return TS_SUCCESS;
                
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;
}
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
DLL_FUNC_INT TClient_activeServer(US fd, US aid)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command     
    memcpy(pBuf, &ACTIVE_SERVER, CONTROL_COMMAND_LEN);                                //activeServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}

/***************************************************************************
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
 *   ��           �ߣ� qingxh
 *   ����ʱ�䣺 2006��10��20�� 15:00:11
 *   �޸���ʷ��1��modified on 2006-12-19 for TS3.2
 *   
***************************************************************************/
DLL_FUNC_INT TClient_activeServerWithDll(US fd,  US aid,  char* answerInfo, int infolen)
{
    if(NULL_PTR == answerInfo)
    {
        return TS_NULLPTR_ENCONTERED;        //��Ч��AID�Ż��߿�ָ��
    }
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret =0;
    US curDID = 0, packDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command    
    memcpy(pBuf, &ACTIVE_SERVER, CONTROL_COMMAND_LEN);                                //activeServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);

    //��ȡmanager���صļ�������Ĳ���op_result_para
    /*
    *       Ts���ص�֡��ʽΪ��
    *       _________________________________________
    *       |op_flag(E01/OK)| ,  |Aid| ,  |len|op_result_para|
    *       |______________|__|__|__|__|_____________|
    */
    US des_aid, des_said=0, src_said;
    char recBuf[1000] = "\0";
    int bufSize = PACKET_DATASIZE;        
    int retbuflen = 0;   //������Ϣ�ĳ���
    char *pdest = NULL;
    while(true)
    {
        //�ȴ�5��, ��5������������������ݵĽ���
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {      
            //��̬������ʱʱ��
            adjustSocketTime(false);
            //��ȡmanager���ص�����, ��ʹ���ص���Ϣ���ᳬ��1�����ݰ��Ĵ�С, ����С���ܳ���PACKET_DATASIZE
            
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if (ret < 0 )
                return filtrateError(ret);
            //������Ϣ���ĳ��������ݺϷ��ĳ����ж�
            if(!CheckValidPackBySize(ret) )
            {
                return TS_UNKNOW_DATA;
            }
            //ȡ��DID,��һ���ж��Ƿ��ǵ�ǰ�Ļظ�����
            pBuf = recBuf;
            __fetch(&packDID, pBuf, DIDLEN);
            //��pbufָ�������ݿ�ʼ����, ������DID����
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //��ָ��ָ����Ϣ�Ŀ�ʼ����,  ��OK, xx
            if(packDID == curDID)
            {
                //�յ���ȷ�İ������а��Ľ���
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //����OK,
                pBuf += ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
                //ȡ�����е�aid
                pdest = strstr(pBuf,  CONTROL_FLAG);
                if(NULL == pdest)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                //ָ��len����
                pBuf = pdest +CONTROL_FLAG_LEN;
                pdest = strstr(pBuf, CONTROL_FLAG);
                if(NULL == pdest)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                hex2int(&pBuf,  &retbuflen);
                pdest += CONTROL_FLAG_LEN;
                if(retbuflen > infolen - 1)
                {
                    memcpy(answerInfo, pdest,  infolen-1);
                    *(answerInfo + infolen)= '\0';
                    return TS_REC_BUF_LIMITED;
                }
                memcpy(answerInfo, pdest,  retbuflen);
                *(answerInfo + retbuflen)= '\0';
                return retbuflen;              
                
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };

    return TS_SYSTEM_ERROR;
    
}

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
DLL_FUNC_INT TClient_inActiveServer(US fd, US aid) 
{
    char buf[100] = "\0";
    char* pBuf = buf;
    US curDID = 0;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }

    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &REST_SERVER, CONTROL_COMMAND_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    int ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}

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
DLL_FUNC_INT TClient_launchTM(const char* path) 
{
    WIN32_FIND_DATA d;
    if (NULL_PTR == path)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if (strlen(path) > NAME_SIZE)
        return TS_INVALID_FILE_NAME;

    char tsName[NAME_SIZE] = "\0";
    char* pName = tsName;
    memcpy(pName, path, strlen(path));
    pName += strlen(path);

    *pName = '\0';

    HANDLE hHandle = FindFirstFile(tsName,  &d);
    if ( hHandle == INVALID_HANDLE_VALUE )
    {
        return TS_FILE_NOT_EXIST;
    }
    else 
    {
        FindClose( hHandle );
        STARTUPINFO startInfo={0};
        startInfo.cb = sizeof(startInfo);
        PROCESS_INFORMATION ProcInfo;
        BOOL bCreatSucces = CreateProcess(tsName,  tsName, NULL, NULL, FALSE,  0,  NULL,  NULL,  &startInfo,  &ProcInfo);
        if (bCreatSucces)
            return TS_SUCCESS;
    }
    
    return TS_LAUNCH_TS_FAILURE;
}


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
DLL_FUNC_INT TClient_getSAReport(US fd, ServerCfg *pArrayServerCfg, UI timeout) 
{
    if (pArrayServerCfg == NULL)
        return TS_NULLPTR_ENCONTERED;
    US des_aid, des_said=0, src_said;
    char recvbuf[PACKET_DATASIZE] = "\0";
    int bufSize = 0, packDID = 0;
    int count = 0, size = 0;
    COMCONFIG* pCommConfig = NULL;

    if(timeout != 0)
    {
        if(TClient_wait(fd, timeout) != TS_DATACOMING)
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        //��̬������ʱʱ��
        adjustSocketTime(false);
    }
    bufSize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recvbuf, sizeof(recvbuf));
    if (bufSize < 0 )
            return filtrateError(bufSize);
    char* pdest = recvbuf;
    //��Ϣ���ĳ�������Ӧ��Ϊ�Ի�ID������ռ�ֽ�
    if((UI)bufSize < (DIDLEN + CONTROL_FLAG_LEN))
    {
        return TS_UNKNOW_DATA;
    }
    /**
    �ϱ����ĸ�ʽ��(��ֵ���Ͷ�����ʮ�����ƴ洢��)
    ----------------------------------------------------------------------------
    |DID,|aid,|name,|extInfo|,|reportType,|sate,|islog,|channelCount,|currentChannel,|config of each channel,|
    ----------------------------------------------------------------------------
    */

    //ȡDID����
    __fetch(&packDID, pdest, DIDLEN);
    pdest += DIDLEN + CONTROL_FLAG_LEN;
    
    char* pStr = NULL;
    ServerCfg *pServerCfg = pArrayServerCfg;
    
    //AID
    pStr = strstr(pdest, CONTROL_FLAG);            //aid
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2short(&pdest, (short*)&pServerCfg->aid);            //aid
    pdest = pStr + CONTROL_FLAG_LEN;
    //Name
    pStr = strstr(pdest, CONTROL_FLAG);            //�Ƿ��зָ��
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    if (pStr-pdest > NAME_SIZE)
        return TS_INVALID_SERVICE_NAME;
    memcpy(pServerCfg->name, pdest, pStr-pdest);    //����
    pServerCfg->name[pStr-pdest] = '\0';        //���ַ�����β��
    pdest = pStr + CONTROL_FLAG_LEN;
    //extInfo
    if(bufSize < pdest - recvbuf + EXT_INFO_LEN + CONTROL_FLAG_LEN)
    {
        return TS_UNKNOW_DATA_FORMAT;
    }
    memcpy(pServerCfg->extInfo, pdest, EXT_INFO_LEN);
    pServerCfg->extInfo[EXT_INFO_LEN-1] = '\0';
    pdest += EXT_INFO_LEN + CONTROL_FLAG_LEN;
    //reportType
    pStr = strstr(pdest, CONTROL_FLAG);            //�ϱ�����
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->reportType);
    pdest = pStr + CONTROL_FLAG_LEN;
    //state
    pStr = strstr(pdest, CONTROL_FLAG);            //״̬
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->state);
    pdest = pStr + CONTROL_FLAG_LEN;
    //isLog
    pStr = strstr(pdest, CONTROL_FLAG);            //��־
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    int logflag;
    hex2int(&pdest, &logflag);
    pServerCfg->isLog = ((logflag == 0)?0:1);        //��־��¼
    pdest = pStr + CONTROL_FLAG_LEN;
    //ChannnelCount
    pStr = strstr(pdest, CONTROL_FLAG);            //ͨ����
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->channelCount);
    pdest = pStr + CONTROL_FLAG_LEN;
    //currentChannel
    pStr = strstr(pdest, CONTROL_FLAG);            //��ǰͨ��
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->currentChannel);
    pdest = pStr + CONTROL_FLAG_LEN;
    //ͨ����������
    pCommConfig = pServerCfg->pComConfig;
    for (UI i=0;i<pServerCfg->channelCount; ++i) 
    {
        if(NULL == pCommConfig)
        {
            return TS_REC_BUF_LIMITED;
        }
        pStr = strstr(pdest, CONTROL_FLAG);        //��ǰ��ͨ��Э������
        if (pStr == NULL )
            return TS_UNKNOW_DATA_FORMAT;
        hex2int(&pdest, (int*)&pCommConfig->protocolType);        //��ǰ��ͨ��Э������
        pdest = pStr + CONTROL_FLAG_LEN;


        pStr = strstr(pdest, CONTROL_FLAG);        //�õ���ǰͨ����ͨ�ŷ�ʽ
        if (pStr == NULL )
            return TS_UNKNOW_DATA_FORMAT;
        hex2int(&pdest, (int*)&pCommConfig->way);        //�õ���ǰͨ����ͨ�ŷ�ʽ
        pdest = pStr + CONTROL_FLAG_LEN;

        switch (pCommConfig->way)
        {
            case SERVER_WAY_TCP:                                        //TCP, UDP
            case SERVER_WAY_UDP:                                        //TCP, UDP
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                
                if (pStr-pdest > IP_BUF_LEN)
                    return TS_INVALID_IP;
                memcpy(pCommConfig->NET.ip, pdest, pStr-pdest);                //IP��ַ
                pCommConfig->NET.ip[pStr-pdest] = '\0';    //���ַ�����β��
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.port);        
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.timeout);        
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.retry);        
                pdest = pStr + CONTROL_FLAG_LEN;
                break;
                
            case SERVER_WAY_UART:
                //�������ڶ˿�
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                if (pStr-pdest > UART_NAME_BUF_LEN)
                    return TS_INVALID_UART_PORT;
                memcpy(pCommConfig->UART.channel, pdest, pStr-pdest);                
                pCommConfig->UART.channel[pStr-pdest] = '\0';    //���ַ�����β��
                pdest = pStr + CONTROL_FLAG_LEN;
                //����������
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.baud); 
                pdest = pStr + CONTROL_FLAG_LEN;
                //������ʱʱ��
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.timeout);     
                pdest = pStr + CONTROL_FLAG_LEN;
                //�������Դ���
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.retry);      
                pdest = pStr + CONTROL_FLAG_LEN;
                
                break;
                
            case SERVER_WAY_DLL:    //��ʾ�Զ�̬��ͨ�ŵ�sa
                //����DLL�ļ���
                pStr = strstr(pdest, CONTROL_FLAG);        //�Ƿ��зָ��
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                if (pStr-pdest > NAME_SIZE)
                    return TS_INVALID_FILE_NAME;
                memcpy(pCommConfig->DLL.dllName, pdest, pStr - pdest);
                pdest = pStr + CONTROL_FLAG_LEN;
                //����DLL���壬����Ӧ�������һ�Σ�
                //����ֱ���ƶ�����Ϣ�Ľ�β��
                pStr = recvbuf + bufSize;
                if(pStr - pdest > DLL_BUF)
                    return TS_UNKNOW_DATA_FORMAT;
                size = (int)min(DLL_BUF, pStr - pdest);
                memcpy(pCommConfig->DLL.buf, pdest, size);
                pdest = pStr;
                break;
                
            case SERVER_WAY_USB://USB
                break;
            default:
                break;
        }
        
        pCommConfig = pCommConfig->next;           
        
    }
    if(pCommConfig != NULL)
        pCommConfig->next = NULL;    //�ڵ��������

    return TS_SUCCESS;
}

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
DLL_FUNC_INT TClient_register(US fd, int regType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(regType))
    {
        return TS_INVALID_REG_TYPE;
    }

    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }

    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &REGISTER_SERVER, CONTROL_COMMAND_LEN);                                //register�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, regType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    //return getAnswerPacketState(fd);
    return TS_SUCCESS;
    
}
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
DLL_FUNC_INT TClient_unregister(US fd, int regType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(regType))
    {
        return TS_INVALID_REG_TYPE;
    }
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the contorl command    
    memcpy(pBuf, &UNREGISTER_SERVER, CONTROL_COMMAND_LEN);                                //unregister�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;

    pBuf += int2hex(pBuf, 10, regType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //���ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    //return getAnswerPacketState(fd);
    return TS_SUCCESS;
    
}

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
DLL_FUNC_INT TClient_setRegisterType(US fd, US aid, int registerType) 
{
    char buf[100]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(registerType))
    {
        return TS_INVALID_REG_TYPE;
    }

    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_SERVER_TYPE, CONTROL_COMMAND_LEN);                                //activeServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, registerType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    
    return getAnswerPacketState(fd);
}
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
DLL_FUNC_INT TClient_clearRegisterType(US fd, US aid,  int registerType)
{
    char buf[100]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(registerType))
    {
        return TS_INVALID_REG_TYPE;
    }
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &CLEAR_SERVER_TYPE, CONTROL_COMMAND_LEN);                                //activeServer�Ŀ��ư���־��
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, registerType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);

}

/***********************TS3.2����API**************************************/
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

 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��20�� 9:48:39
 *   
**/
DLL_FUNC_INT Tclient_queryHostSerialPort(US fd, char *recvBuf, US size)
{
    if(NULL == recvBuf)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    //�����������10��
    char buf[1000]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0, packDID = 0;
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &QUERY_HOST_COMM_CONFIG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //��manager���������
    if (ret < 0)
        return filtrateError(ret);

    //���ջظ���Ϣ
    US des_aid, des_said=0, src_said;
    int bufsize = sizeof(buf);        
    char *pdest = NULL, *pStr = NULL;
    
    while(true)
    {
        ret = TClient_wait(fd, TIMEOUT_GETPKT);
        if(ret != TS_DATACOMING)
        {
            //��̬������ʱʱ��
            adjustSocketTime(true);
            return ret;
        }
        //��̬������ʱʱ��
        adjustSocketTime(false);
        pdest = buf;
        bufsize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, sizeof(buf));
        if(bufsize <= 0)
        {
            return filtrateError(bufsize);
        }
        //�������ݰ���С�����ݺϷ��Գ����ж�
        if(!CheckValidPackBySize(bufsize))
        {
            return TS_UNKNOW_DATA;
        }
        /**
        �ظ������� ----------------------------------
                                         |DID|,|OK|,|COM1|,|COM2|,|COM3|,|
                                        ---------------------------------
        �������޴�����ظ���  "DID,OK,"                               
        */
        //ȡDID
        __fetch(&packDID, pdest, DIDLEN);
        //��ָ��ָ����Ϣ������ʼ����
        pdest += DIDLEN + CONTROL_FLAG_LEN;
        if(packDID == curDID)
        {
            //�Ǳ��εĻظ����������ݵĽ���
            ret = returnErrValueFromErrMsg(pdest);
            if(ret != TS_SUCCESS)
            {
                return ret;
            }
            //����OK,����
            pdest += ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
            //����޴�������
            if(strstr(pdest, CONTROL_FLAG) == NULL)
            {
                memset(recvBuf, 0, size);
                return 0;
            }
            //��DID���ֿ�ʼ���п���
            pdest = buf + DIDLEN + CONTROL_FLAG_LEN + ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
            //COM�������ݵĳ���
            ret = bufsize - (DIDLEN + CONTROL_FLAG_LEN + 
                                        ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN);
            if(ret > size)
            {
                memcpy(recvBuf, pdest, size);
                return TS_REC_BUF_LIMITED;
            }
            memcpy(recvBuf, pdest, ret);
            return ret;
        }
        else
        {
            //���Ƿ�Ҫ����������Ϣ
            AssertPackDIDAndCurDID(packDID, curDID);
        }
    };
    return TS_UNKNOW_DATA;
}


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
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��20�� 10:46:31
 *   
**/
DLL_FUNC_INT TClient_setTargetChannelSwitchState(US fd, US aid, _BOOL flag)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_TARGET_SWITCH_FLAG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    //aid
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;
    //switch flag
    pBuf += int2hex(pBuf, 10, flag);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }

    return getAnswerPacketState(fd);
}

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
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��20�� 11:20:21
 *   
**/
DLL_FUNC_INT  TClient_setSystemLogLevel(US fd, LogLevel level)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_TS_SYSTEM_LOGOUT_LEVEL, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, level);
    //control char 
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }
    return TS_SUCCESS;

}

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
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��20�� 11:32:21
 *   
**/
DLL_FUNC_INT  TClient_SetServiceAgentLog(US fd, US aid, _BOOL logflag)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_SERVICE_PACK_LOG_FLAG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    //aid
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;
    //PackLog flag
    pBuf += int2hex(pBuf, 10, logflag);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer�Ŀ��ư���־��
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }
    return getAnswerPacketState(fd);
}

/**
 * @Funcname:  TClient_UpLoadFile
 * @brief        :  ����ָ���ļ���TS������
 * @para1      : fd: ��TS�����������Ӿ��
 * @para2      : filename��Ҫ���͵��ļ���
 * @para3      : isReplace�Ƿ���Ҫ����ͬ���ļ�
 * @para4      : savedname, ������TS�ϵ��ļ���
 * @return      : 
                        TS_SUCCESS���ͳɹ�
                        TS_FILE_ALREADY_EXIT�Ѿ�����ͬ���ļ�
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��2�� 14:51:36
 *   
 * ͨ�Ÿ�ʽ��"DID, Ffilename, aid, isReplace, isComplete, curLen, content"
**/
DLL_FUNC_INT TClient_UpLoadFile(US fd, const char* filename, const char *savedname, int isReplace)
{
    char buf[PACKET_DATASIZE] = "\0";
    char *pBuf = buf, *fileBuf = NULL;
    US  curDID = 0, packDID = 0;
    int ret = 0, contentidx = 0, cursize = 0; //��ǰ��ȡ�ļ����ݵĴ�С
    FILE *fp = NULL;
    long curLocation = 0;     //�ļ���ǰλ��
    if(NULL == filename)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if((strlen(filename)<=0) || (strlen(filename)>MAX_PATH))
    {
        return TS_INVALID_FILE_NAME;
    }
        
    //open the file
    fp = fopen(filename, "rb");
    if(NULL == fp)
    {
        return TS_FILE_NOT_EXIST;
    }
    //alloc buf for file
    fileBuf = (char*) malloc(FILEBUFSIZE);
    if(NULL == fileBuf)
    {
        fclose(fp);
        return TS_SYSTEM_ERROR;
    }
    //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //Ffilename
    memcpy(pBuf, &UPLOAD_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, savedname, strlen(savedname));
    pBuf += strlen(savedname);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //isReplace
    pBuf += int2hex(pBuf, 10, isReplace);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //save the last content idx in buf.
    contentidx = (int)(pBuf - buf);    
    int icount = 0;
    
    while(!feof(fp))
    {
        memset(fileBuf, 0, FILEBUFSIZE);
        fseek(fp, curLocation, SEEK_SET);
        cursize = fread(fileBuf, sizeof(char), FILEBUFSIZE, fp);
        curLocation = ftell(fp);
        if(cursize <= 0)
        {
            icount++;
            continue;
        }
        pBuf = buf + contentidx;
        //iscomplete
        if(!feof(fp))
        {
            if(icount == 0)
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_BEGIN);
            }
            else
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_CONTINUE);
            }
        }
        else
        {
            if(icount == 0)
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_BEGIN_END);
            }
            else
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_END);
            }
        }
         memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
         pBuf += CONTROL_FLAG_LEN;
        //store the cursize to packet.
        pBuf += int2hex(pBuf, 10, cursize);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //copy the data
        memcpy(pBuf, fileBuf, cursize);
        pBuf += cursize;
        
        //send the packet
        ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
        if(ret <= 0)
        {
            free(fileBuf);
            fclose(fp);
            return filtrateError(ret);
        }
        if(feof(fp))
        {
            ret = getAnswerPacketState(fd);
        }
        icount++;
    }
    
    free(fileBuf);
    fclose(fp);
    return ret;
    
}

/**
 * @Funcname:  TClient_DownLoadFile
 * @brief        :  ��TS�����������ļ������߶�
 * @para1      : fd: ��TS�����������Ӿ��
 * @para2      :  filename��Ҫ���ص��ļ���
 * @para3      : savefilename�����ļ�����Ҫ������ļ���
                        (��ȫ·��,���򽫱�����TS���еĵ�ǰĿ¼)
 * @return      : 
                         TS_SUCCESS���ͳɹ�
                         TS_FILE_�Ѿ�����ͬ���ļ�
                         TS_FILE_NOT_EXISTָ�����ļ�������
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��2�� 15:00:31
 *   
**/
DLL_FUNC_INT TClient_DownLoadFile(US fd, const char*filename, const char*savefilename)
{
    char buf[PACKET_DATASIZE] = "\0";
    char recvFn[MAX_PATH] = "\0";                  //���յ����ļ���
    char *pBuf = buf, *fileBuf = NULL, *pStr = NULL;
    US  curDID = 0, packDID = 0;
    int ret = 0, contentidx = 0, cursize = 0; //��ǰ��ȡ�ļ����ݵĴ�С
    FILE *fp = NULL;
    int iCount = 0, recvAid = 0, isComplete = 0, curLen = 0;     
    US des_aid = 0, des_said = 0, src_said = 0;
    long curLocation = 0;       //��ǰ�ļ�λ��
    
    if((NULL == filename) ||(NULL == savefilename))
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if((strlen(filename)<=0) || (strlen(filename)>MAX_PATH) 
        ||(strlen(savefilename)<=0) ||(strlen(savefilename)>MAX_PATH))
    {
        return TS_INVALID_FILE_NAME;
    }
    //���������ļ�����
     //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //f filename
    memcpy(pBuf, &DOWNLOAD_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, filename, strlen(filename));
    pBuf += strlen(filename);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //���ļ�
    fp = fopen(savefilename, "wb");
    if(NULL == fp)
    {
        return TS_SYSTEM_ERROR;
    }
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        /*Modifide by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
        /*  Modified brief: �ر��ļ������˳�*/
        fclose(fp);
        return filtrateError(ret);
    }
    
    while(true)
    {
        ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, PACKET_DATASIZE);
        if(ret <= 0)
        {
            fclose(fp);
            return filtrateError(ret);
        }
        if(ret < DIDLEN + CONTROL_COMMAND_LEN + 2* CONTROL_FLAG_LEN)
        {
            fclose(fp);
            return TS_UNKNOW_DATA;
        }
        //������Ϣ���Ľ���
        __fetch(&packDID, buf, DIDLEN);
        pBuf = buf + DIDLEN + CONTROL_FLAG_LEN;
        if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
        {
            fclose(fp);
            return ret;
        }
        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [begin]*/
        /*  Modified brief: �������ݰ�����OK�ַ�*/
        //����OK
        pBuf = pBuf + ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [end]*/
        //filename
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            return TS_UNKNOW_DATA_FORMAT;
        }
        memcpy(recvFn, pBuf, (UI)(pStr - pBuf));
        //proccess flag
        pBuf = pStr + CONTROL_FLAG_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            return TS_UNKNOW_DATA_FORMAT;
        }
        hex2int(&pBuf, &isComplete);
        //curLen
        pBuf = pStr + CONTROL_FLAG_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            /*Modifide by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
            return TS_UNKNOW_DATA_FORMAT;
        }
        hex2int(&pBuf, &curLen);
        //�����ļ�д��
        pBuf = pStr + CONTROL_FLAG_LEN;
        fwrite(pBuf, sizeof(char), curLen, fp);
        fflush(fp);
        if((isComplete == TS_FILE_BEGIN_END) || (isComplete == TS_FILE_END))
        {
            fclose(fp);
            return TS_SUCCESS;
        }
    }
    //�����հ�
}


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
DLL_FUNC_INT TClient_DeleteFile(US fd, const char*filename)
{
    char sendBuf[100] = "\0";
    char *pBuf = sendBuf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    if(NULL == filename)
    {
        return TS_NULLPTR_ENCONTERED;
    }
     //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //f filename
    memcpy(pBuf, &DELETE_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, filename, strlen(filename));
    pBuf += strlen(filename);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //send the pack
    ret = TClient_putpkt(fd, 0, 0, 0, sendBuf, (UI)(pBuf-sendBuf));        //��manager���������
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}


/**
 * @Funcname:  filtrateError
 * @brief        :  ���˿���������ڲ��������
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��4�� 14:43:43
 *   
**/
int filtrateError(int errValue)
{
    if((errValue != TS_SOCKET_ERROR) 
        &&(errValue != TS_INVALID_LINK_HANDLE))
    {
        return TS_SYSTEM_ERROR;
    }
    return errValue;
}

/***************************************************************************
 * @Funcname:  bool checkInvalidIP
 * @brief        :  ���IP�Ƿ�Ƿ�
 * @para1      : char *ip : IP
 * @para2      : iplen: IP�ĳ���
 * @return      :  �Ϸ�����true, �Ƿ�����false
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��11��17�� 11:32:52
 *   
***************************************************************************/
bool checkInvalidIP(const char *ip,  size_t iplen)
{
    /*Modified by tangxp for BUG NO.3081 on 2008��4��14�� [begin]*/
    /*  Modified brief: �������IP��ַ*/
    char tmpIP[IP_BUF_LEN] = "\0";
    char *pcur = NULL;
    int intIp = 0;
    memcpy(tmpIP, ip, min(iplen, sizeof(tmpIP)));
    pcur = tmpIP;
    
    for(int i = 0; i < 4; ++i)
    {
        intIp = atoi(pcur);
        if((intIp < 0) || (intIp > 255) 
            || (((0 == intIp) || (255 == intIp)) && (0 == i)))
        {
            //ÿ����0��255֮��,�ҵ�һ�β��ܵ���0��255
            return false;  //�Ƿ�IP
        }

        if(i < 3)
        {
            //IPv4ֻ������"."
            pcur = strstr(pcur, ".");
            if(!pcur)
            {
                return false;  //�Ƿ�IP
            }
            ++pcur;
        }
    }
    
    return true;
    /*Modified by tangxp for BUG NO.3081 on 2008��4��14�� [end]*/
}

/***
 *   �� ��    ���� returnErrValueFromErrMsg
 *   ����˵���� �ڷ��ص���Ϣ���в����Ƿ��Ǵ�����Ϣ����
 *              ����ǽ���ת���ɶ�Ӧ�Ĵ���ֵ����
 *   ��      ��1   ��buf��Ϣ����
 *
 *   ��  ��   ֵ����Ӧ�Ĵ���ֵ��NO_ERR:��������Ӧ�������Ǵ����
 *   ��           �ߣ� qingxh
 *   ����ʱ�䣺 2006��10��31�� 9:50:13
 *   �޸���ʷ��
 *   
**/

short returnErrValueFromErrMsg(const char *buf)
{
    /*Modified by tangxp for BUG NO. 0002549 on 2008.1.16 [begin]*/
    /*  Modified brief: ����ƥ����Ϣ�������㷨*/
    const char *strBuf = buf;
    char errmsg[ERR_MSG_CHAR_NUM+1];
    
    memset(errmsg, 0, sizeof(errmsg));    
    if(strlen(buf) == 0)
    {
        return TS_RECV_DATA_ERROR;
    }
    //��strBuf�������ĸE,�ҵ��ͱ�ʾ�Ǵ�����Ϣ
    if(strncmp(strBuf, ERRHEAD, 1) == 0)
    {
        strncpy(errmsg, strBuf, ERR_MSG_CHAR_NUM);
        for(UI i = 0; i < MAX_ERR_NUM, ERR_MAP[i].ErrValue != 0; i++)
        {
            if(strcmp(errmsg, ERR_MAP[i].ErrData) == 0)
            {
                return ERR_MAP[i].ErrValue;
            }
        }
        return TS_UNKNOW_ERROR;
    }
    else if(strncmp(strBuf, ANSWER_SUCEESS, 2) == 0)
    {
        //����ĸOK��������Ծͱ�ʾ����ȷ�ظ���Ϣ
        return TS_SUCCESS;
    }
    else
    {
        //������Exx��ͷ�Ĵ���ظ���Ϣ,Ҳ����OK��ͷ����ȷ�ظ���Ϣ
        return TS_UNKNOW_DATA;
    }
    /*Modified by tangxp for BUG NO. 0002549 on 2008.1.16 [end]*/
}


/**
 * @Funcname:  adjustSocketTime
 * @brief        :  ��̬����SOCKET�ĳ�ʱ�ȴ�ʱ�䣬
 *                     ����SOCKET�ȴ���ʱ����øú������ж�̬����
 * @param    : flag:  �ȴ���ʱ��������ʱ��Ϊ1������Ϊ0
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��4�� 14:53:59
 *   
**/
void adjustSocketTime(bool timeoutFlag)
{
    static int ifailCount = 0;
    //�����ʱ�����ӳ�ʱͳ��
    if(timeoutFlag)
    {
        ifailCount++;
    }
    else
    {
        if(ifailCount > 0)
        {
            ifailCount--;
        }
    }
    //�����������ϳ�ʱ�򽫳�ʱʱ������һ��
    if(ifailCount >= 2)
    {
        if(TIMEOUT_GETPKT < MAX_TIME_OUT)
        {
            TIMEOUT_GETPKT++;
        }        
    }
    else if(ifailCount < 2)
    {
        if(TIMEOUT_GETPKT > MIN_TIME_OUT)
        {
            TIMEOUT_GETPKT--;
        }     
    }
    return;
}

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
DLL_FUNC_INT guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen)
{
    char *fmtStr = NULL;
    if((NULL == strGuid) || (buflen < sizeof(GUID)+1))
    {
        return -1;
    }
    memset(strGuid, 0, buflen);
    if(upcase)
    {
        fmtStr = "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
    }
    else
    {
        fmtStr = "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    }
    sprintf_s(strGuid, buflen, fmtStr, guidData.Data1, 
                                                         guidData.Data2,
                                                         guidData.Data3, 
                                                         guidData.Data4[0],
                                                         guidData.Data4[1],
                                                         guidData.Data4[2],
                                                         guidData.Data4[3],
                                                         guidData.Data4[4],
                                                         guidData.Data4[5],
                                                         guidData.Data4[6],
                                                         guidData.Data4[7]);
    strGuid[buflen-1] = '\0';
    return 0;
}/**
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
DLL_FUNC_INT stringToGUID(char *strGuid, unsigned short len, GUID *guidData)
{
    if((NULL == strGuid) || (len < GUID_LEN -1))
    {
        return -1;
    }
    /*GUID�ַ�����ʽ
            00873FDF-61A8-11D1-AA5E-00C04FB1728B
    */
    int tmpvalue[11] = {0};
    sscanf(strGuid, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
                                                                    &tmpvalue[0], 
                                                                    &tmpvalue[1],
                                                                    &tmpvalue[2],
                                                                    &tmpvalue[3],
                                                                    &tmpvalue[4],
                                                                    &tmpvalue[5],
                                                                    &tmpvalue[6],
                                                                    &tmpvalue[7],
                                                                    &tmpvalue[8],
                                                                    &tmpvalue[9],
                                                                    &tmpvalue[10]);
    guidData->Data1 = tmpvalue[0];
    guidData->Data2 = tmpvalue[1];
    guidData->Data3 = tmpvalue[2];
    for(int i = 0; i < 8; i++)
    {
        guidData->Data4[i] = tmpvalue[i + 3];
    }
    
    return 0;
}


