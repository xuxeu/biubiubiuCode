/*
* ������ʷ��
* 2008-09-23 zhagnxu  ���������������޹�˾
*                        �������ļ���
*/

/**
* @file  tsSymManServer.cpp
* @brief
*       ���ܣ�
*       <li>tssymman������, tsSymMan���ʵ��, �����ļ����ء����ء���ѯ��ɾ����</li>
*/




/************************ͷ�ļ�********************************/

#include "afx.h"
#include <direct.h>
#include "windows.h"
#include <fcntl.h>
#include <SYS\Stat.h>
#include <io.h>
#include "common.h"
#include "tsSymManComm.h"
#include "tsSymManServer.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
T_ERR_TBL errTbl[20] =
{
    {SUCC, "OK;"},
    {FAIL, ESTR_SYS},
    {ERR_INVALID_COMMAND, ESTR_INVALID_COMMAND},
    {ERR_FILE_EXIST, ESTR_FILE_EXIST},
    {ERR_DELETE_FOLDER, ESTR_DELETE_FOLDER},
    {ERR_OPEN_FILE, ESTR_OPEN_FILE},
    {ERR_FOLDER_INEXIST, ESTR_FOLDER_INEXIST},
    {ERR_FILE_INEXIST, ESTR_FILE_INEXIST},
    {ERR_READ_FILE, ESTR_READ_FILE},
    {ERR_DELETE_FILE, ESTR_DELETE_FILE},
    {ERR_FOLDER_EMPTY, ESTR_FOLDER_EMPTY},
	{ERR_WRITE_FILE, ESTR_WRITE_FILE},
    {1, ""}
};

T_MODE_TBL modeTbl[4] =
{
    {TSSYMMAN_PLACE_FLAG_BEGIN, "wb"},
    {TSSYMMAN_PLACE_FLAG_BEGIN_END, "wb"},
    {TSSYMMAN_PLACE_FLAG_CONTINUE, "ab"},
    {TSSYMMAN_PLACE_FLAG_END, "ab"}
};

/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

/**
 * @Funcname: getAckStr
 * @brief        : ͨ������ֵ��ȡ��Ӧ�Ļظ��ַ���
 * @para[IN]   : errno, ������
 * @return      : ���ش������Ӧ�Ļظ��ַ���
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
 *
**/
char* getAckStr(int eno)
{
    int i = 0;

    for(i = 0; (errTbl[i].erNo) != 1; i++)
    {
        if((errTbl[i].erNo) == eno)
        {
            return (errTbl[i].erStr);
        }
    }

    return NULL;
}

/**
 * @Funcname:  tsSymMan
 * @brief        :  tsSymMan��Ĺ��캯��
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:53:24
 *
**/
tsSymMan::tsSymMan(char* pstrPath)
{
    strcpy(m_symFilePath, pstrPath);
}

/**
 * @Funcname:  putpkt
 * @brief        :  tsSymMan��Ľ���ts������������
 * @param[IN]: srcAid, ��Ϣ����Դ��AID
 * @param[IN]: srcSaid, ��Ϣ����Դ�Ķ���ID��
 * @param[IN]: data, ��Ϣ������
 * @param[IN]: len, ��Ϣ����
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *
**/
int tsSymMan::putpkt(T_UHWORD srcAid, T_UHWORD srcSaid, char * data, int len)
{
    int iRet = 0;
    char *pAck = NULL;
    //    char ackData[MAX_MESSAGE_LEN];

    if((NULL == data) || (len < 2))
    {
        return FAIL;
    }

    m_curSrcAid = srcAid;
    m_curSrcSaid = srcSaid;

    sysLoger(LDEBUG, "putpkt: recv cmd:%c", data[0]);

    switch(data[0])
    {
        /*�����ļ�*/
    case TSSYMMAN_UPLOAD:
        iRet = uploadFile(&data[2], len-2);
        break;

        /*�����ļ�*/
    case TSSYMMAN_DOWNLOAD:
    {
        iRet = downloadFile(&data[2], len-2);

        if(SUCC == iRet)
        {
            return SUCC;
        }

        break;
    }

    /*��ѯ������Щ�ļ�*/
    case TSSYMMAN_QUERY:
    {
        iRet = queryFile(&data[2], len-2);

        if(SUCC == iRet)
        {
            return SUCC;
        }

        break;
    }

    /*ɾ���ļ�*/
    case TSSYMMAN_DELETE:
        iRet = deleteFile(&data[2], len-2);
        break;

    default:
        iRet = ERR_INVALID_COMMAND;
        break;
    }

    pAck = getAckStr(iRet);
    sendAck(pAck, (int )strlen(pAck));

    return SUCC;
}

/**
 * @Funcname:  getpkt
 * @brief        :  tsSymMan��ķ��ش���������
 * @param[OUT]: desAid, ��Ϣ��Ŀ�ĵ�AID
 * @param[OUT]: desSaid, ��Ϣ��Ŀ�ĵض���ID��
 * @param[OUT]: buf, �û�������ݵĻ���
 * @param[IN]: size, �û������С
 * @return      : �ɹ����ؽ��յ������ݵĴ�С,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:57:35
 *
**/
int tsSymMan::getpkt(T_UHWORD * desAid, T_UHWORD * desSaid, char * buf, int size)
{
    int len = 0;

    m_wtData.wait();

    if(0 == m_tmpDataLen)
    {
        return 0;
    }

    m_mtxBuf.lock();
    *desAid = m_curSrcAid;
    *desSaid = m_curSrcSaid;
    len = min(size, m_tmpDataLen);
    ZeroMemory(buf, size);
    memcpy(buf, m_tmpData, len);
    ZeroMemory(m_tmpData, sizeof(m_tmpData));
    m_mtxBuf.unlock();
    return len;
}

/**
 * @Funcname:  tsSymMan
 * @brief        :  tsSymMan��ֹͣ����ʱ,�������������̵߳��ź���
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *
**/
void tsSymMan::stop()
{
    m_tmpDataLen = 0;
    m_wtData.wakeup();
}

/**
 * @Funcname:  sendAck
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:59:31
 *
**/
int tsSymMan::sendAck(char *ack, int len)
{
    if (NULL == ack)
    {
        return FAIL;
    }

    m_mtxBuf.lock();
    ZeroMemory(m_tmpData, sizeof(m_tmpData));
    m_tmpDataLen = min(len, sizeof(m_tmpData));
    memcpy(m_tmpData, ack, m_tmpDataLen);
    m_wtData.wakeup();
    m_mtxBuf.unlock();

    return SUCC;
}

/**
 * @Funcname:  uploadFile
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:59:31
 *
**/
/*�յ������ݸ�ʽ:
U:targetName;fileType;isAllowReplace;placeFlag;fileNameLen;fileName;data
(targetName:Ŀ�������,
fileNameLen:�ļ�������
fileName:�ļ���,
fileType:�ļ�����,
isReplace:�Ƿ񸲸�,
placeFlag:�ļ���ʼ/��ʼ����/����/������־,
data:�ļ�����)
*/
int tsSymMan::uploadFile(char *data, int len)
{
    T_WORD fileType;
    T_WORD isAllowReplace;
    T_WORD placeFlag;
    T_WORD fileNameLen;
    T_WORD ret = FALSE;
    T_WORD fileLen = 0;
    T_CHAR fileFullName[MAX_PATH_LEN] = "\0";
    T_CHAR fileFullNameTmp[MAX_PATH_LEN] = "\0";
    T_CHAR fileName[MAX_PATH_LEN] = "\0";
    T_CHAR folderName[MAX_PATH_LEN] = "\0";
    T_CHAR* pData = data;
    T_CHAR* pTmp = NULL;
    int fp = 0;

    pTmp = strstr(pData, ";");

    if (NULL == pTmp)
    {
        sysLoger(LWARNING, "uploadFile:invalid parameter, cmd:%s", data);
        return FAIL;
    }


    *pTmp = '\0';
    sprintf(folderName, "%s\\%s", m_symFilePath, pData);
    pData = pTmp + 1;

    if(!hex2int(&pData, &fileType) ||
            *(pData++) != ';' ||
            !hex2int(&pData, &isAllowReplace) ||
            *(pData++) != ';' ||
            !hex2int(&pData, &placeFlag) ||
            *(pData++) != ';' ||
            !hex2int(&pData, &fileNameLen) ||
            *(pData++) != ';')
    {
        *pTmp = ';';
        sysLoger(LWARNING, "uploadFile:invalid parameter, cmd:%s", data);
        return FAIL;
    }

    ret = CheckFolderExist(folderName);

    /*���Ŀ¼���������صķ����ļ�Ϊֻ����Ŀ�����
    Ψһ��������ɾ����Ŀ¼*/
    if(TRUE == ret)
    {
        if((TSSYMMAN_FILE_TYPE_ONLY == fileType)
                && ((TSSYMMAN_PLACE_FLAG_BEGIN == placeFlag)
                    ||(TSSYMMAN_PLACE_FLAG_BEGIN_END== placeFlag)))
        {
            ret = DeleteFolderAllFile(folderName);

            if(FALSE == ret)
            {
                return ERR_DELETE_FILE;
            }

           _mkdir(folderName);        
        }
    }

    else
    {
        sysLoger(LDEBUG, "uploadFile:made directory dir:%s", folderName);
        _mkdir(folderName);        
    }

    /*�ϳ������ļ���ȫ·����*/
    strncpy(fileName, pData, fileNameLen);
    sprintf(fileFullName, "%s\\%s", folderName, fileName);
    pData += ++fileNameLen;

    sysLoger(LDEBUG, "uploadFile:fileName:%s, placeFlag:%d, fileType:%d", fileFullName,
             placeFlag, fileType);

    /*����ļ��Ƿ��Ѵ���*/
    ret = CheckFileExist(fileFullName);

    if(TRUE == ret)
    {
        if(FALSE == isAllowReplace)
        {
            sysLoger(LWARNING, "uploadFile:file already exist, disallow replace, file:%s", fileFullName);
            return ERR_FILE_EXIST;
        }

        else
        {
            sysLoger(LDEBUG, "uploadFile:remove file:%s", fileFullName);
            remove(fileFullName);
        }
    }

    /*�򿪲�������д���ļ�,���û���յ��ļ����������
    ���Ƚ��ļ����浽ԭ�ļ�����".tmp"��Ϊ��׺���ļ��У�
    �����յ��ļ�������Ǻ�ȫ���������ļ����ݺ�����
    ������ԭ�ļ���*/
    sprintf(fileFullNameTmp, "%s.tmp", fileFullName);

    fp = _open(fileFullNameTmp, _O_CREAT | _O_APPEND | _O_BINARY | _O_RDWR,
               _S_IREAD | _S_IWRITE );

    if(-1 == fp)
    {
        sysLoger(LWARNING, "uploadFile:open file fail:%s", fileFullNameTmp);
        return ERR_OPEN_FILE;
    }

    fileLen = _write(fp, pData, (unsigned int)(len - (pData - data)));

    if(fileLen < 0)
    {
		sysLoger(LWARNING, "uploadFile:write data false");
        return ERR_WRITE_FILE;
     }
    
    _close(fp);

    sysLoger(LINFO, "uploadFile:write data succ, len:%d, placeFlag:%d, , fileType:%d, file:%s",
             fileLen, placeFlag, fileType, fileFullName);

    if((TSSYMMAN_PLACE_FLAG_BEGIN_END == placeFlag )
            || (TSSYMMAN_PLACE_FLAG_END == placeFlag))
    {
        sysLoger(LINFO, "uploadFile:upload file finish, %s", fileFullName);
        rename(fileFullNameTmp, fileFullName);
    }

    return SUCC;

}

/**
 * @Funcname:  downloadFile
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:59:31
 *
**/
/*�յ������ݸ�ʽ:
L:targetName;fileNameLen;fileName;
(targetName:Ŀ�������, fileNameLen:�ļ�������,fileName:�ļ���)
�ظ����ݸ�ʽ:
OK:placeFlag;data
(placeFlag:�ļ���ʼ/��ʼ����/����/����, data:�ļ�����)*/
int tsSymMan::downloadFile(char *data, int len)
{
    T_WORD fileNameLen;
    T_WORD ret = FALSE;
    T_WORD isBegin = 0;
    T_WORD numRead = 0;
    T_CHAR fileFullName[MAX_PATH_LEN] = "\0";
    T_CHAR fileName[MAX_PATH_LEN] = "\0";
    T_CHAR folderName[MAX_PATH_LEN] = "\0";
    T_CHAR readBuf[MAX_READ_BUF_LEN] = "\0";
    T_CHAR ackData[MAX_MESSAGE_LEN] = "\0";
    T_CHAR* pData = data;
    T_CHAR* pTmp = NULL;
    T_CHAR* pAckData = NULL;
    FILE* fp = NULL;

    pTmp = strstr(pData, ";");

    if (NULL == pTmp)
    {
        sysLoger(LWARNING, "uploadFile:invalid parameter, cmd:%s", data);
        return FAIL;
    }

    *pTmp = '\0';
    sprintf(folderName, "%s\\%s", m_symFilePath, pData);
    pData = pTmp + 1;

    if(!hex2int(&pData, &fileNameLen) ||
            *(pData++) != ';' )
    {
        *pTmp = ';';
        sysLoger(LWARNING, "downloadFile:invalid parameter, cmd:%s", data);
        return FAIL;
    }

    ret = CheckFolderExist(folderName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "downloadFile:folder not exist, dir:%s", folderName);
        return ERR_FOLDER_INEXIST;
    }

    /*�ϳ������ļ���ȫ·����*/
    strcpy(fileFullName, folderName);
    strcat(fileFullName, "\\");
    strncpy(fileName, pData, fileNameLen);
    strcat(fileFullName, fileName);

    /*����ļ��Ƿ����*/
    ret = CheckFileExist(fileFullName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "downloadFile:file not exist, file:%s", fileFullName);
        return ERR_FILE_INEXIST;
    }

    /*�򿪲�������д���ļ�*/
    fp = fopen(fileFullName, "rb");

    if(NULL == fp)
    {
        sysLoger(LWARNING, "downloadFile:open file fail, file:%s", fileFullName);
        return ERR_OPEN_FILE;
    }

    isBegin = TRUE;

    /*��ʼ�����ļ�*/
    while(!feof(fp))
    {
        memset(readBuf, 0, MAX_READ_BUF_LEN);
        numRead  = (T_WORD)fread(readBuf, sizeof(char), MAX_READ_BUF_LEN, fp);

        if(numRead  < 0)
        {
            fclose(fp);
            sysLoger(LWARNING, "downloadFile:read file fail, ret:%d, file:%s", numRead, fileFullName);
            return ERR_READ_FILE;
        }

        sysLoger(LDEBUG, "downloadFile:read file succ");

        memset(ackData, 0, MAX_MESSAGE_LEN);
        pAckData = ackData;
        strcpy(pAckData, "OK:");
        pAckData += strlen("OK:");

        if(isBegin)
        {
            if(!feof(fp))
            {
                sysLoger(LINFO, "downloadFile:begin rdad file and end, file:%s", fileFullName);
                pAckData += int2hex(pAckData, 10, TSSYMMAN_PLACE_FLAG_BEGIN_END);
            }

            else
            {
                sysLoger(LINFO, "downloadFile:begin rdad file, file:%s", fileFullName);
                pAckData += int2hex(pAckData, 10, TSSYMMAN_PLACE_FLAG_BEGIN);
            }

            isBegin = FALSE;
        }

        else
        {
            if(!feof(fp))
            {
                sysLoger(LDEBUG, "downloadFile:continue rdad file, file:%s", fileFullName);
                pAckData += int2hex(pAckData, 10, TSSYMMAN_PLACE_FLAG_CONTINUE);
            }

            else
            {
                sysLoger(LINFO, "downloadFile:rdad file end, file:%s", fileFullName);
                pAckData += int2hex(pAckData, 10, TSSYMMAN_PLACE_FLAG_END);
            }
        }

        memcpy(pAckData, TSSYMMAN_SPLIT_FLAG, TSSYMMAN_SPLIT_FLAG_LEN);
        pAckData += TSSYMMAN_SPLIT_FLAG_LEN;
        memcpy(pAckData, readBuf, numRead);
        pAckData += numRead;
        sendAck(ackData, (int )(pAckData - ackData));
    }

    fclose(fp);
    return SUCC;

}

/**
 * @Funcname:  queryFile
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:59:31
 *
**/
/*�յ������ݸ�ʽ:
D:targetName;
(targettargetName:Ŀ�������)
�ظ����ݸ�ʽ:
OK:placeFlag;data
(placeFlag:�ļ�����ʼ/��ʼ����/����/����,
data:�ļ�����Ϣ��"|"�ָ�)*/
int tsSymMan::queryFile(char *data, int len)
{
    T_WORD ackLen;
    T_WORD ret = FALSE;
    T_WORD isBegin = TRUE;
    T_WORD isFound;
    T_CHAR folderName[MAX_PATH_LEN] = "\0";
    T_CHAR fileFindName[MAX_PATH_LEN] = "\0";
    T_CHAR ackData[MAX_MESSAGE_LEN] = "\0";
    T_CHAR* pData = data;
    T_CHAR* pTmp = NULL;
    T_CHAR* pAckData = NULL;
    FILE* fp = NULL;
    CFileFind fileFind;
    bool isFirstFile = true;

    pTmp = strstr(pData, ";");

    if (NULL == pTmp)
    {
        sysLoger(LWARNING, "queryFile:invalid parameter, cmd:%s", data);
        return ERR_FOLDER_INEXIST;
    }

    *pTmp = '\0';

    sprintf(folderName, "%s\\%s", m_symFilePath, data);
    ret = CheckFolderExist(folderName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "queryFile:folder not exist , dir:%s", folderName);
        return ERR_FOLDER_INEXIST;
    }

    sprintf(fileFindName, "%s\\*.*", folderName);
    isFound = fileFind.FindFile((LPCTSTR)fileFindName);

    while(isFound)
    {
        isFound = fileFind.FindNextFile();

        /*����ҵ�����Ŀ¼����������*/
        if((fileFind.IsDots()) || (fileFind.IsDirectory()))
        {
            continue;
        }

        //���ļ������뻺��
        if (isFirstFile)
        {
            pAckData = ackData;
            memset(ackData, 0, MAX_MESSAGE_LEN);
            ackLen = sprintf(pAckData, "OK:0;%s",
                             (char*)fileFind.GetFileName().GetBuffer(MAX_PATH_LEN));
            sysLoger(LDEBUG, "queryFile:find file:%s", pAckData + 5);

        }

        else
        {
            ackLen += sprintf(pAckData, "%s%s", TSSYMMAN_FILENAME_SPLIT_FLAG,
                              (char*)fileFind.GetFileName().GetBuffer(MAX_PATH_LEN));
            sysLoger(LDEBUG, "queryFile:find file:%s", pAckData + 2);
        }

        pAckData += ackLen;
        isFirstFile = false;

        /*����������ȷ��ͻظ���Ϣ*/
        if((!isFound) || ackLen + MAX_PATH_LEN >= MAX_MESSAGE_LEN)
        {
            if(isBegin)
            {
                // 3�ǻظ����ݸ�ʽOK:placeFlag;data��placeFlag��λ��
                ackData[3] += (isFound) ? TSSYMMAN_PLACE_FLAG_BEGIN : \
                              TSSYMMAN_PLACE_FLAG_BEGIN_END;
                isBegin = FALSE;
            }

            else
            {
                ackData[3] += (isFound) ? TSSYMMAN_PLACE_FLAG_CONTINUE : \
                              TSSYMMAN_PLACE_FLAG_END;
            }

            sendAck(ackData, ackLen);
            isFirstFile = true;
        }

    }

    if (isBegin)
    {
        sysLoger(LWARNING, "queryFile:folder is empty, dir:%s", folderName);
        return ERR_FOLDER_EMPTY;
    }

    fileFind.Close();
    sysLoger(LINFO, "queryFile:find file succ");

    return SUCC;

}

/**
 * @Funcname:  deleteFile
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:59:31
 *
**/
/*�յ������ݸ�ʽ:
D:targetName;fileNameLen;fileName;
(targetName:Ŀ�������, fileNameLen:�ļ�������,fileName:�ļ���)*/
int tsSymMan::deleteFile(char *data, int len)
{
    T_WORD fileNameLen;
    T_WORD ret = FALSE;
    T_CHAR fileFullName[MAX_PATH_LEN] = "\0";
    T_CHAR fileName[MAX_PATH_LEN] = "\0";
    T_CHAR folderName[MAX_PATH_LEN] = "\0";
    T_CHAR readBuf[MAX_READ_BUF_LEN] = "\0";
    T_CHAR* pData = data;
    T_CHAR* pTmp = NULL;
    FILE* fp = NULL;

    pTmp = strstr(pData, ";");

    if (NULL == pTmp)
    {
        sysLoger(LWARNING, "deleteFile:invalid parameter, cmd:%s", data);
        return ERR_FOLDER_INEXIST;
    }

    *pTmp = '\0';

    sprintf(folderName, "%s\\%s", m_symFilePath, data);
    pData = pTmp + 1;

    if(!hex2int(&pData, &fileNameLen) ||
            *(pData++) != ';' )
    {
        *pTmp = ';';
        sysLoger(LWARNING, "deleteFile:invalid parameter, cmd:%s", data);
        return FAIL;
    }

    ret = CheckFolderExist(folderName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "deleteFile:folder not exist , dir:%s", folderName);
        return ERR_FOLDER_INEXIST;
    }

    /*�ϳ�ɾ���ļ���ȫ·����*/
    pData[fileNameLen] = '\0';
    sprintf(fileFullName, "%s\\%s", folderName, pData);

    /*����ļ��Ƿ����*/
    ret = CheckFileExist(fileFullName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "deleteFile:file not exist , file:%s", fileFullName);
        return ERR_FILE_INEXIST;
    }

    ret = remove(fileFullName);

    if(SUCC == ret)
    {
        sysLoger(LINFO, "deleteFile:delete file succ, file:%s", fileFullName);
        return SUCC;
    }

    else
    {
        sysLoger(LWARNING, "deleteFile:delete file fail, file:%s", fileFullName);
        return ERR_DELETE_FILE;
    }

}

