/*
* ������ʷ��
* 2008-09-23  zhangxu  ���������������޹�˾
*                           �������ļ���
*/

/**
* @file  tsSymManComm.cpp
* @brief
*       ���ܣ�
*       <li>tssymman�Ĺ����궨�弰��������ʵ��</li>
*/



/************************ͷ�ļ�********************************/
#include <afx.h>
#include "windows.h"
#include "Shlwapi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tsSymManComm.h"


/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @return ����16�����ַ���Ӧ��ASCII��
*/
int hex2num(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'f')
    {
        return ch-'a'+10;
    }

    if (ch >= '0' && ch <= '9')
    {
        return ch-'0';
    }

    if (ch >= 'A' && ch <= 'F')
    {
        return ch-'A'+10;
    }

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
        {
            break;
        }

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
    return (unsigned int)strlen(ptr);
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
 * @Funcname: DeleteFolderAllFile
 * @brief        : ɾ���ļ����������ļ�
 * @para[IN]   : strPathĿ¼��
 * @return      : ɾ���Ƿ�ɹ�
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *
**/
int DeleteFolderAllFile(char *strPath)
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
                DeleteFolderAllFile(tempDir);
            }

            else
            {

                sprintf(tempFileName,"%s\\%s",strPath,foundFileName);
                DeleteFile(tempFileName);
            }
        }
    }

    fileFind.Close();
    return TRUE;
}

