/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsComm.cpp
 * @Version       :  1.0.0
 * @brief           :  *   <li> ����</li>tssymman�Ĺ����궨�弰��������ʵ��
                                <li>���˼��</li>
                                <p> n     ע�����n</p> 
 *   
 * @author        :  zhangxu
 * @create date :  2008��9��23�� 16:32:08
 * @History        : 
 *   
 */


/************************���ò���*****************************/
#include <afx.h>
#include "windows.h"
#include "Shlwapi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tssComm.h"

/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @return ����16�����ַ���Ӧ��ASCII��
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
unsigned int hex2int(char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;
    if((NULL == ptr) || (NULL == intValue))
    {
        return 0;
    }
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
* ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
* @param ptr ����16�����ַ����Ļ�����
* @param size   �������Ĵ�С
* @param intValue   Ҫת��������
* @return ת���Ĵ�С
*/
unsigned int int2hex(char *ptr, int size,int intValue)
{
    if(NULL == ptr)
    {
        return 0;
    }
    memset(ptr, '\0',size);    //���ô�С,��󻺳����Ĵ�С
    sprintf(ptr,"%x",intValue);
    return strlen(ptr);
}

/**
 * @Funcname: CheckFolderExist
 * @brief        : ���Ŀ¼�Ƿ����
 * @para[IN]   : strPathĿ¼��
 * @return      : Ŀ¼�������
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *   
**/
int  CheckFolderExist(char* strPath)
{
    WIN32_FIND_DATA  wfd;
    HANDLE hFind;
    bool ret = FALSE;
    
    hFind = FindFirstFile(strPath, &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        ret = TRUE;   
        FindClose(hFind);
    }
    
    return ret;
}

/**
 * @Funcname: CheckFileExist
 * @brief        : ����ļ��Ƿ����
 * @para[IN]   : strFile�ļ���
 * @return      : �ļ��������
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *   
**/
int CheckFileExist(char *strFile)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool ret = FALSE;
    
    hFind = FindFirstFile(strFile, &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE) 
    {
        ret = TRUE;
        FindClose(hFind);
    }

    return ret;
}

/**
 * @Funcname: DeleteDirectory
 * @brief        : ɾ��Ŀ¼(������Ŀ¼�ͷǿ�Ŀ¼)
 * @para[IN]   : strPathĿ¼��
 * @return      : ɾ���Ƿ�ɹ�
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *   
**/
int DeleteDirectory(char *strPath)
{
    CFileFind fileFind;
    char fileFindName[260] = "\0";
    char foundFileName[260] = "\0";
    char tempDir[260] = "\0";
    char tempFileName[260] = "\0";
    int isFound;
        
    sprintf(fileFindName,"%s\\*.*",strPath);
    isFound=fileFind.FindFile(fileFindName);
    while(isFound)
    {
        isFound=fileFind.FindNextFile();
        if(!fileFind.IsDots())
        {
            
            strcpy(foundFileName,(char*)fileFind.GetFileName().GetBuffer(260));
            if(fileFind.IsDirectory())
            {
                
                sprintf(tempDir,"%s\\%s",strPath,foundFileName);
                DeleteDirectory(tempDir);
            }
            else
            {
                
                sprintf(tempFileName,"%s\\%s",strPath,foundFileName);
                DeleteFile(tempFileName);
            }
        }
    }
    fileFind.Close();
    if(!RemoveDirectory(strPath))//��ʱӦ��ֻ��һ����Ŀ¼��
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * @Funcname: ExtendESC
 * @brief        : ��չ�ļ�ȫ·�����е�\\�ַ�����һ��\��չΪ\\
 * @para[IN|OUT]   : pString�ļ�ȫ·����
 * @para[IN]   : ������峤��
 * @return      :��չ�Ƿ�ɹ�
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *   
**/
int ExtendESC(char* pString, int len)
{
    char *pPos = NULL, *pTmp = NULL;
    int origLen;

    origLen = strlen(pString);
    pTmp = (char*)malloc(len*sizeof(char));
    
    pPos = pString;
    while(pPos = strstr(pPos, "\\"))
    {
        strcpy(pTmp, pPos);
        pString[pPos-pString] = '\0';
        if(++origLen > len)
            return -1;
        strcat(pString, "\\");
        strcat(pString, pTmp);
        pPos += 2;      
    }
    
    free(pTmp);
    pTmp = NULL;
    
    return 0;
}

/**
 * @Funcname: ResumeBlank
 * @brief        : ���ַ����е�Э��ASC��ָ�Ϊ�ո�
 * @para[IN|OUT]   : pString�ַ���
 * @return      :�ָ��Ƿ�ɹ�
 * @Author     : tangxp
 *  
 * @History: 1.  Created this function on 2009��2��12�� 13:34:11
 *   
**/
int ResumeBlank(char* pString)
{
    char *pStr = NULL;
    
    if (NULL == pString)
    {
        return FAIL;
    }

    while ((pStr = strchr(pString, 6)) != NULL)
    {
        *pStr = ' ';
    }

    return SUCC;
}

