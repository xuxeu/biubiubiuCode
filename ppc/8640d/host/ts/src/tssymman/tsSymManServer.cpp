/*
* 更改历史：
* 2008-09-23 zhagnxu  北京科银技术有限公司
*                        创建该文件。
*/

/**
* @file  tsSymManServer.cpp
* @brief
*       功能：
*       <li>tssymman的主体, tsSymMan类的实现, 处理文件上载、下载、查询、删除等</li>
*/




/************************头文件********************************/

#include "afx.h"
#include <direct.h>
#include "windows.h"
#include <fcntl.h>
#include <SYS\Stat.h>
#include <io.h>
#include "common.h"
#include "tsSymManComm.h"
#include "tsSymManServer.h"

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
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

/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/

/**
 * @Funcname: getAckStr
 * @brief        : 通过返回值获取对应的回复字符串
 * @para[IN]   : errno, 错误码
 * @return      : 返回错误码对应的回复字符串
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
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
 * @brief        :  tsSymMan类的构造函数
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:53:24
 *
**/
tsSymMan::tsSymMan(char* pstrPath)
{
    strcpy(m_symFilePath, pstrPath);
}

/**
 * @Funcname:  putpkt
 * @brief        :  tsSymMan类的接收ts发过来的数据
 * @param[IN]: srcAid, 消息包来源的AID
 * @param[IN]: srcSaid, 消息包来源的二级ID号
 * @param[IN]: data, 消息包内容
 * @param[IN]: len, 消息长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
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
        /*上载文件*/
    case TSSYMMAN_UPLOAD:
        iRet = uploadFile(&data[2], len-2);
        break;

        /*下载文件*/
    case TSSYMMAN_DOWNLOAD:
    {
        iRet = downloadFile(&data[2], len-2);

        if(SUCC == iRet)
        {
            return SUCC;
        }

        break;
    }

    /*查询存在哪些文件*/
    case TSSYMMAN_QUERY:
    {
        iRet = queryFile(&data[2], len-2);

        if(SUCC == iRet)
        {
            return SUCC;
        }

        break;
    }

    /*删除文件*/
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
 * @brief        :  tsSymMan类的返回处理结果数据
 * @param[OUT]: desAid, 消息包目的地AID
 * @param[OUT]: desSaid, 消息包目的地二级ID号
 * @param[OUT]: buf, 用户存放数据的缓冲
 * @param[IN]: size, 用户缓冲大小
 * @return      : 成功返回接收到的数据的大小,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:57:35
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
 * @brief        :  tsSymMan类停止运行时,唤醒阻塞接收线程的信号量
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *
**/
void tsSymMan::stop()
{
    m_tmpDataLen = 0;
    m_wtData.wakeup();
}

/**
 * @Funcname:  sendAck
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:59:31
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
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:59:31
 *
**/
/*收到的数据格式:
U:targetName;fileType;isAllowReplace;placeFlag;fileNameLen;fileName;data
(targetName:目标机名字,
fileNameLen:文件名长度
fileName:文件名,
fileType:文件类型,
isReplace:是否覆盖,
placeFlag:文件开始/开始结束/继续/结束标志,
data:文件内容)
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

    /*如果目录存在且上载的符号文件为只能在目标机上
    唯一存在则先删除该目录*/
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

    /*合成上载文件的全路径名*/
    strncpy(fileName, pData, fileNameLen);
    sprintf(fileFullName, "%s\\%s", folderName, fileName);
    pData += ++fileNameLen;

    sysLoger(LDEBUG, "uploadFile:fileName:%s, placeFlag:%d, fileType:%d", fileFullName,
             placeFlag, fileType);

    /*检查文件是否已存在*/
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

    /*打开并将数据写入文件,如果没有收到文件结束标记我
    们先将文件保存到原文件名加".tmp"作为后缀的文件中，
    待接收到文件结束标记后并全部接收完文件数据后在重
    命名回原文件名*/
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
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:59:31
 *
**/
/*收到的数据格式:
L:targetName;fileNameLen;fileName;
(targetName:目标机名字, fileNameLen:文件名长度,fileName:文件名)
回复数据格式:
OK:placeFlag;data
(placeFlag:文件开始/开始结束/继续/结束, data:文件内容)*/
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

    /*合成下载文件的全路径名*/
    strcpy(fileFullName, folderName);
    strcat(fileFullName, "\\");
    strncpy(fileName, pData, fileNameLen);
    strcat(fileFullName, fileName);

    /*检查文件是否存在*/
    ret = CheckFileExist(fileFullName);

    if(FALSE == ret)
    {
        sysLoger(LWARNING, "downloadFile:file not exist, file:%s", fileFullName);
        return ERR_FILE_INEXIST;
    }

    /*打开并将数据写入文件*/
    fp = fopen(fileFullName, "rb");

    if(NULL == fp)
    {
        sysLoger(LWARNING, "downloadFile:open file fail, file:%s", fileFullName);
        return ERR_OPEN_FILE;
    }

    isBegin = TRUE;

    /*开始下载文件*/
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
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:59:31
 *
**/
/*收到的数据格式:
D:targetName;
(targettargetName:目标机名字)
回复数据格式:
OK:placeFlag;data
(placeFlag:文件名开始/开始结束/继续/结束,
data:文件名信息以"|"分隔)*/
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

        /*如果找到的是目录，则不做处理*/
        if((fileFind.IsDots()) || (fileFind.IsDirectory()))
        {
            continue;
        }

        //将文件名填入缓冲
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

        /*如果超出则先发送回复消息*/
        if((!isFound) || ackLen + MAX_PATH_LEN >= MAX_MESSAGE_LEN)
        {
            if(isBegin)
            {
                // 3是回复数据格式OK:placeFlag;data中placeFlag的位置
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
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:59:31
 *
**/
/*收到的数据格式:
D:targetName;fileNameLen;fileName;
(targetName:目标机名字, fileNameLen:文件名长度,fileName:文件名)*/
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

    /*合成删除文件的全路径名*/
    pData[fileNameLen] = '\0';
    sprintf(fileFullName, "%s\\%s", folderName, pData);

    /*检查文件是否存在*/
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

