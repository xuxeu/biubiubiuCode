/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tsComm.cpp
 * @Version        :  1.0.0
 * @Brief           :  TS�еĴ���ֵ����,�����ִ���
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��1��19�� 11:51:37
 * @History        : 
 *   
 */




/************************���ò���*****************************/
#include "tsInclude.h"
#include "stdlib.h"
#include "stdio.h"
/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/


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
    {"", 0}
    
};

/**
 * @Funcname:  getErrStrByErrValue
 * @brief        :  �Ӵ���ֵ�õ�������Ϣ�ַ���
 * @para1      : int errValue :����ֵ
 * @return      : char * ����ô���ֵ�Ĵ����ַ���
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��19�� 10:25:13
 *   
**/
char* getErrStrByErrValue(int errValue )
{
    for(int i = 0; i < MAX_ERR_NUM, ERR_MAP[i].ErrValue != 0; i++)
    {
        if(errValue == ERR_MAP[i].ErrValue)
        {
            return ERR_MAP[i].ErrData;
        }
    }
    return ERR_SYSTEM_ERR;
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
bool guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen)
{
    char *fmtStr = NULL;
    if((NULL == strGuid) || (buflen < sizeof(GUID)+1))
    {
        return false;
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
    return true;
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
bool stringToGUID(char *strGuid, unsigned short len, GUID *guidData)
{
    if((NULL == strGuid) || (len < GUID_LEN -1))
    {
        return false;
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
    
    return true;
}

WINVERSION getWindowsVersion()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    WINVERSION ver = UNKNOW;
    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return ver;
   }

   switch (osvi.dwPlatformId)
   {
      // Test for the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:

          // Test for the specific product.
          if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )         
             ver = WIN2003;
             
          if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
             ver = WINXP;

          if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
             ver = WIN2K;

          if ( osvi.dwMajorVersion <= 4 )
             ver = WINNT;
                   
          break;

      // Test for the Windows Me/98/95.
      case VER_PLATFORM_WIN32_WINDOWS:

          if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
          {
              ver = WIN95;
          } 

          if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
          {
              ver = WIN98;
          } 

          if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
          {
              ver = WINME;
          } 
          break;

      default:

          ver = UNKNOW;
      
      break;
   }
   return ver; 
}
