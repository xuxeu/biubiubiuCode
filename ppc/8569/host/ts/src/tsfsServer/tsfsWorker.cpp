/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsWorker.cpp
* @brief  
*       功能：
*       <li>实现Worker，实现文件操作的处理</li>
*/
/************************头文件********************************/
#include "stdafx.h"
#include <fcntl.h>
#include <SYS\Stat.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <sys/utime.h>
#include <errno.h>
#include "tsfsCommon.h"
#include "tsfsWorker.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

/**
 * @brief
 *    构造函数，完成Worker初始化工作，连接TS
 * @param size[in]: Worker处理队列大小
 */
TSFS_Worker::TSFS_Worker(unsigned short size):threadWithQueue(size)
{    
    //连接TS
    m_lnkId = (US)TClient_open(pTSFS_Server->getTsIPAddr());

    //启动Worker，开始运行
    start();
}

/*
 * @brief
 *   根据处理请求，并回复结果
 * @param pack[in]: 目标机的处理请求
 */
void TSFS_Worker::process(Packet *pack)
{
    if(NULL == pack)
    {
        //将Worker放入空闲Worker链表
        pTSFS_Server->recycleWorker(this);

        sysLoger(LWARNING, "TSFS_Worker::process: pack is invalid" );
    }

    char *pBuf = pack->getData();

    m_errno = TSFS_PROCESS_SUCC;

    sysLoger(LDEBUG, "TSFS_Worker::process: worker get a packet, [worker threadID = %d], data:[%c]",
                      this->getThreadID(), pack->getData()[0]);

    //解析命令
    char *pstr = strstr(pBuf, TSFS_COMM_CONTROL_FLAG);
    if( !( pstr && *(++pstr) ) )
    {
        //无效的格式
        m_errno = EINVAL;

        sysLoger(LWARNING, "TSFS_Worker::process: argument:invalid command" );        
    }
    else
    {
        //执行命令
        switch(pBuf[0])
        {
            case TSFS_OPEN_FILE:        //打开文件
                tsfsOpenFile(pstr, pack);
                break;

            case TSFS_CLOSE_FILE:       //关闭文件
                tsfsCloseFile(pstr, pack);
                break;

            case TSFS_READ_FILE:        //读文件
                tsfsReadFile(pstr, pack);
                break;

            case TSFS_WRITE_FILE:       //写文件
                tsfsWriteFile(pstr, pack);
                break;

            case TSFS_CREATE_FILE:      //创建文件
                tsfsCreateFile(pstr, pack);
                break;

            case TSFS_DELETE_FILE:      //删除文件
                tsfsDeleteFile(pstr, pack);
                break;

            case TSFS_IO_CONTROL:       //文件属性操作
                tsfsIOControl(pstr, pack);
                break; 

            default:
                //不支持的接口命令
                m_errno = EINVAL;

                sysLoger(LWARNING, "TSFS_Worker::process: unknown command" );                
        }
    }    

    //若处理失败,则填充失败信息
    if( TSFS_PROCESS_SUCC != m_errno )
    {
        pack->setData("",0);

        //将Windows错误码转换为PixOS错误码
        winErr2posixErr(m_errno, &m_errno);

        //填充回复信息
        int size = sprintf(pack->getData(), "E%x;", m_errno);

        //设置回复信息大小
        pack->setSize((US)size);
    }

    //交换源、目的ID
    pack->setDes_aid( pack->getSrc_aid() );
    pack->setDes_said( pack->getSrc_said() );
    pack->setSrc_said( 0 );

    //回复结果
    TClient_putpkt( m_lnkId, 
                    pack->getDes_aid(), 
                    pack->getDes_said(), 
                    pack->getSrc_said(), 
                    pack->getData(), 
                    pack->getSize());
   
    sysLoger(LINFO, "TSFS_Worker::process: send data, [aid:%d], [said:%d], [size:%d]",
                       pack->getDes_aid(), pack->getDes_said(), pack->getSize() );

    sysLoger(LINFO, "TSFS_Worker::process: send data, data:[%c%c%c]",
                        pack->getData()[0],pack->getData()[1],pack->getData()[2]  );

#ifdef DEBUG
        printf("send data :%s\n",
            pack->getData());
#endif

    //将Packet放入空Packet链表
    pTSFS_Server->recyclePacket(pack);    

    //将Worker放入空闲Worker链表
    pTSFS_Server->recycleWorker(this);    
}

/*
 * @brief
 *    执行打开文件操作，并向目标机回复执行结果
 * @param *pRecvData[in]: 待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<filename>;flag;[mode;]
*/
int TSFS_Worker::tsfsOpenFile(char *pRecvData, Packet *sendPack)
{  
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    char fileName[TSFS_NAME_SIZE];
    char fullDir[TSFS_NAME_SIZE];
    memset(fileName,0, sizeof(fileName));    
    memset(fullDir, 0, sizeof(fullDir));
    int flag        = 0;
    int mode        = 0;
    char *pBegin    = NULL;
    char *pEnd      = NULL;
    char *pFileName = fileName;
    char *pData     = pRecvData;

    //解析参数

    //filename
    pBegin  = strstr(pData,TSFS_FILE_NAME_BEGIN);
    pEnd = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:filename is invalid");
        return FALSE;
    }

    //检查文件名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //文件名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:filename is too long");
        return FALSE;
    }    

    //获取filename
    memcpy(pFileName, ++pBegin, (pEnd-pBegin));
    pData = pEnd+2;

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse filename success, [%s]",
                        fileName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: full filename, [%s]",
                        fullDir);

    //flag
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid");
        return FALSE;
    }

    //获取flag
    if(0 == hexToInt(&pData,&flag))         
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse flag success, [flag:0x%x]",
                        flag);

    //转换flag
    if(TSFS_FAIL == taParam2tsParam(flag, &flag))
    {
        //参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid, [flag:0x%x]",
                            flag);
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: change to windows flag, [flag:0x%x]",
                        flag);

    //mode 可选参数
    if(NULL != *(++pData))
    {
        if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
        {
            //解析参数错误,回复信息
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:mode is invalid");
            return FALSE;
        }
        
        //获取mode
        if(0 == hexToInt(&pData,&mode))     
        {
            //解析参数错误,回复信息
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:mode is invalid");
            return FALSE;
        }       

        sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse mode success, [mode:0x%x]",
                            mode);
    }

    //检查mode
    if( (flag & _O_CREAT) == _O_CREAT )
    {
        /*  强制设置文件权限为可读写  */
        mode = _S_IREAD | _S_IWRITE;
    }


    //打开文件,强制增加二进制打开方式
    int fp = _open(fullDir, flag | _O_BINARY, mode );

    if( -1 == fp )
    {   
        //打开文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: open file failed, [%s]",
                            fullDir);
        return FALSE;
    }  

    //打开文件成功,获取文件句柄
    pTSFS_Server->insertFileHandle(fp);

    sysLoger(LINFO, "TSFS_Worker::tsfsOpenFile: open file successful, [%s], [fp:%d]",
                    fullDir, fp);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fp,                         //fp
                    TSFS_CONTROL_FLAG);         //;  

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    执行关闭文件操作，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： fp;
*/

int TSFS_Worker::tsfsCloseFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp = 0;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: argument:file pointer is invalid");
        return FALSE;
    }

    //获取fp
    hexToInt(&pData,&fp);               
    
    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: didn't find the file pointer, [fp:%d]"
                            , fp);
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCloseFile: parse file pointer success, [fp:%d]",
                        fp);

    //关闭文件，删除文件结构和映射点
    if(0!=_close(fp))
    {
        //关闭文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: close the file failed, [fp:%d]"
                            , fp);
        return FALSE;
    }

    //关闭文件成功
    sysLoger(LINFO, "TSFS_Worker::tsfsCloseFile: close the file successful, [fp:%d]"
                    , fp);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                TSFS_SUCCESS,               //ok
                TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    //删除文件句柄
    pTSFS_Server->removeFileHandle(fp);

    return TRUE;
}


/*
 * @brief
 *    执行读取文件操作，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： fp;size;
*/
int TSFS_Worker::tsfsReadFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp      = 0;
    int r_size  = 0;
    char *pData = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);   

    sysLoger(LDEBUG, "TSFS_Worker::tsfsReadFile: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //size
    if( !(strstr(++pData, TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:size is invalid");
        return FALSE;
    }

    //获取size,7为"OK;000;"长度
    if(0 == hexToInt(&pData, &r_size) || (r_size > (TSFS_MAX_DATA_SIZE - 7)))
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:size is invalid, [size:%d], max read size:[%d]",
                            r_size, (sendPack->getDataSize() - 7));
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsReadFile: parse size success, [size:%d]",
                        r_size);

    //清空回复数据包
    sendPack->setData("",0);

    int size = sprintf(sendPack->getData(), "%s%s",
                            TSFS_SUCCESS,               //ok
                            TSFS_CONTROL_FLAG);         //;

    char *pLen = sendPack->getData() + size;

    size += sprintf(pLen, "%x%s",
                            TSFS_DATA_SIZE,             //最大读取长度
                            TSFS_CONTROL_FLAG);         //;

    //读取文件
    int readLen = _read(fp, sendPack->getData() + size, r_size);
    if( -1 == readLen || readLen > r_size )
    {
        //读取出错
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: read file failed, [fp:%d], [size:%d]",
                            fp, r_size);
        return FALSE;
    }

    //读取文件成功    
    sysLoger(LINFO, "TSFS_Worker::tsfsReadFile: read file successful, [size:%d]",
                        readLen);

    //填充readLen,3为最大读取长度的位数
    sprintf(pLen, "%03x", readLen);

    //因填充readLen会将';'置为\0,所以需要重新填充';',3为最大读取长度的位数
    memcpy(pLen + 3, TSFS_CONTROL_FLAG, strlen(TSFS_CONTROL_FLAG));

    size += readLen;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    执行写文件操作，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： fp;size;data
*/
int TSFS_Worker::tsfsWriteFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    int fp          = 0;
    int w_size      = 0;
    char *pData     = pRecvData;

    //解析参数
    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);  

    sysLoger(LDEBUG, "TSFS_Worker::tsfsWriteFile: parse file pointer success, [fp:%d]", fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //size
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is invalid");
        return FALSE;
    }
    
    //获取size
    if(0 == hexToInt(&pData,&w_size))       
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsWriteFile: parse size success, [size:%d]",
                        w_size);

    //data,'x:'为命令
    if( sendPack->getSize() < ((strlen("x:") + (unsigned)(++pData - pRecvData) + w_size)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is bigger than recv data size, [size:%d], [recv data size:%d]",
                            w_size, (sendPack->getSize() - strlen("x:") - (unsigned)(pData - pRecvData)));
        return FALSE;
    }

    //写文件
    int writeLen = _write(fp, pData, w_size);
    if(-1 == writeLen)
    {
        //写文件出错
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: write data failed, [fp:%d], [size:%d]",
                            fp, w_size);
        return FALSE;
    }
    
    //写文件成功
    sysLoger(LINFO, "TSFS_Worker::tsfsWriteFile: write data successful, [fp:%d], [size:%d]",
                            fp, writeLen);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    writeLen,                   //writeLen
                    TSFS_CONTROL_FLAG);         //

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;  
}

/*
 * @brief
 *    执行创建文件，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： <fileName>;mode;
*/
int TSFS_Worker::tsfsCreateFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int mode = 0;
    char fileName[TSFS_NAME_SIZE];
    char fullDir[TSFS_NAME_SIZE];
    memset(fileName,0, sizeof(fileName));
    memset(fullDir, 0, sizeof(fullDir));

    char *pBegin    = NULL;
    char *pEnd      = NULL;
    char *pFileName = fileName;
    char *pData     = pRecvData;

    //fileName
    pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:fileName is invalid");
        return FALSE;
    }
    
    //检查文件名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //文件名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:fileName is too long");
        return FALSE;
    }

    //获取filename
    memcpy(pFileName,++pBegin,(pEnd-pBegin));
    pData = pEnd+2;

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCreateFile: parse fileName success, [%s]",
                        fileName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCreateFile: full fileName, [%s]",
                    fullDir);

    //mode
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:mode is invalid");
        return FALSE;
    }

    //获取mode
    if(0 == hexToInt(&pData,&mode))         
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:mode is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCreateFile: parse mode success, [mode:0x%x]",
                        mode);

    /*  强制设置文件权限为可读写  */    
    mode = _S_IREAD | _S_IWRITE;
    
    //创建文件
    int fp = _creat(fullDir, mode);
    if(-1 == fp)
    {
        //创建文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: create file failed,[%s]",
                            fullDir);
        return FALSE;
    }

    //创建文件成功,获取文件句柄
    pTSFS_Server->insertFileHandle(fp);

    sysLoger(LINFO, "TSFS_Worker::tsfsCreateFile: create file successful,[%s]",
                            fullDir);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fp,                         //fp
                    TSFS_CONTROL_FLAG);         //;      

    //设置恢复缓冲大小
    sendPack->setSize((US)size);
    
    return TRUE;
}

/*
 * @brief
 *    执行删除文件，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： <fileName>;
*/
int TSFS_Worker::tsfsDeleteFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    char fileName[TSFS_NAME_SIZE];
    char fullDir[TSFS_NAME_SIZE];
    memset(fileName,0, sizeof(fileName));
    memset(fullDir, 0, sizeof(fullDir));

    char *pBegin    = NULL;
    char *pEnd      = NULL;
    char *pFileName = fileName;
    char *pData     = pRecvData;

    //fileName
    pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: argument:fileName is invalid");
        return FALSE;
    }
    
    //检查文件名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //文件名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: argument:fileName is too long");
        return FALSE;
    }
    
    //获取filename
    memcpy(pFileName,++pBegin,(pEnd-pBegin)); 

    sysLoger(LDEBUG, "TSFS_Worker::tsfsDeleteFile: parse fileName success, [%s]",
                        fileName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsDeleteFile: full fileName, [%s]",
                        fullDir);

    //删除文件
    int ret = remove(fullDir);
    if(-1 == ret)
    {
        //删除文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: delete file fialed,[%s]",
                            fullDir);
        return FALSE;
    }

    sysLoger(LINFO, "TSFS_Worker::tsfsDeleteFile: delete file successful,[%s]",
                            fullDir);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    根据文件属性操作类型，调用对应功能函数处理
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式： opt;optArgs;
*/
int TSFS_Worker::tsfsIOControl(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int opt         = 0;
    char *pData     = pRecvData;

    //opt
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: argument:opt is invalid");
        return FALSE;
    }
    
    //获取opt
    if(0 == hexToInt(&(pData),&opt))            
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: argument:opt is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsIOControl: parse optArgs success, [opt:%d]",
                        opt);
    
    //跳过已解析的参数opt,指向optArgs
    ++pData;                        

    //匹配操作类型
    switch(opt)
    {
        case TSFS_FIONREAD:         //获取文件中未读长度
            fioNotRead(pData, sendPack);
            break;

        case TSFS_FIOSEEK:          //设置文件读写位置
            fioSeek(pData, sendPack);
            break;

        case TSFS_FIOWHERE:         //获取文件读写位置
            fioWhere(pData, sendPack);
            break;

        case TSFS_FIORENAME:        //重命名文件
            fioRename(pData, sendPack);
            break;

        case TSFS_FIOMKDIR:         //创建目录
            fioMkDir(pData, sendPack);
            break;

        case TSFS_FIORMDIR:         //删除目录
            fioRmDir(pData, sendPack);
            break;

        case TSFS_FIOREADDIR:       //读取目录项
            fioReadDir(pData, sendPack);
            break;

        case TSFS_FIOFSTATGETBYFD:  //获取文件状态(by文件描述符)
            fioFstatGetByFd(pData, sendPack);
            break;

        case TSFS_FIOFSTATGETBYNAME: //获取文件状态(by文件名)
            fioFstatGetByName(pData, sendPack);
            break;

        case TSFS_FIOTRUNC:         //在指定长度处截断文件
            fioTrunc(pData, sendPack);
            break;

        case TSFS_FIOTIMESET:       //设置文件时间
            fioTimeSet(pData, sendPack);
            break;

        case TSFS_FIOCHECKROOT:     //检查是否为根目录
            fioCheckRoot(pData, sendPack);
            break;

        //case TSFS_FIOFSTATFSGET:    //获取文件系统信息
        //    fioFstatFsget(pData, sendPack);
        //    break;
            
        default:
            //不支持该类型操作
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: unknown opt, [opt:%d]",
                                    opt);
            return FALSE;
    }
    return TRUE;
}


/*
 * @brief
 *    获取文件中未读长度，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;
*/
int TSFS_Worker::fioNotRead(char *pRecvData, Packet *sendPack)
{    
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析函数
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);  

    sysLoger(LDEBUG, "TSFS_Worker::fioNotRead: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: didn't find the file pointer, [fp:%d]",
                            fp);
        return FALSE;
    }

    //查看当前读取位置
    long curOffset = _tell(fp);
    if(-1 == curOffset)
    {
        //获取文件读写位置失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: get file offset failed, [fp:%d]",
                            fp);

        return FALSE;
    }

    //获取文件末尾位置
    long fileLen = _filelength(fp);
    if(-1 == fileLen)
    {
        //获取文件末尾位置失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: get file length failed, [fp:%d]",
                            fp);
        return FALSE;
    }      

    sysLoger(LINFO, "TSFS_Worker::fioNotRead: get file no read length successful, [fp:%d], [length:%d]",
                            fp, (fileLen-curOffset));

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    (fileLen-curOffset),        //no read len
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    设置文件读写位置，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;offset;(针对文件头的偏移)
*/
int TSFS_Worker::fioSeek(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp          = -1;
    int offset      = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);    

    sysLoger(LDEBUG, "TSFS_Worker::fioSeek: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //offset,针对文件头的偏移
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:offset is invalid");
        return FALSE;
    }
    if((0 == hexToInt(&pData,&offset)) || (offset < 0) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:offset is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioSeek: parse offset success, [offset:%d]",
                        offset);

    //设置文件读写位置
    long newOffset = _lseek(fp, offset, SEEK_SET);
    if(-1 == newOffset)
    {
        //定位文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioSeek: set file offset failed,[fp:%d],[offset:%d]",
                            fp, offset);
        return FALSE;
    }

    //定位文件成功
    sysLoger(LINFO, "TSFS_Worker::fioSeek: set file offset successful,[fp:%d],[offset:%d]",
                            fp, newOffset);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    newOffset,                  //newOffset
                    TSFS_CONTROL_FLAG);         //;    

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    获取文件读写位置，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;
*/
int TSFS_Worker::fioWhere(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);    

    sysLoger(LDEBUG, "TSFS_Worker::fioWhere: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF;  
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }
    
    //获取文件读写位置
    long curOffset = _tell(fp);
    if(-1 == curOffset)
    {
        //获取文件读写位置失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioWhere: get file offset failed,[fp:%d]",
                            fp);
        return FALSE;
    }

    //获取文件读写位置成功
    sysLoger(LINFO, "TSFS_Worker::fioWhere: get file offset successful,[fp:%d],[offset:%d]",
                            fp,curOffset);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    curOffset,                  //curOffset
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    重命名文件，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<old_name>;<new_name>;
*/
int TSFS_Worker::fioRename(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    char oldName[NAME_SIZE];
    char newName[NAME_SIZE];
    char fullDirOldName[NAME_SIZE];
    char fullDirNewName[NAME_SIZE];
    char *pName     = NULL;
    char *pData     = pRecvData;

    memset(newName,0,sizeof(newName));
    memset(oldName,0,sizeof(oldName));
    memset(fullDirOldName,0,sizeof(fullDirOldName));
    memset(fullDirNewName,0,sizeof(fullDirNewName));

    //old_name
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:oldName is invalid");
        return FALSE;
    }
    
    //检查文件名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //文件名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:oldName is too long");
        return FALSE;
    }

    //获取old_name
    pName = oldName;
    memcpy(pName,++pBegin,(pEnd-pBegin));         
    pData = ++pEnd;

    sysLoger(LDEBUG, "TSFS_Worker::fioRename: parse old name success, [%s]",
                        oldName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(oldName, fullDirOldName))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioRename: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioRename: full old name, [%s]",
                        fullDirOldName);

    //new_name
    pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:newName is invalid");
        return FALSE;
    }
    
    //检查文件名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //文件名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:newName is too long");
        return FALSE;
    }

    //获取new_name
    pName = newName;
    memcpy(pName,++pBegin,(pEnd-pBegin));  

    sysLoger(LDEBUG, "TSFS_Worker::fioRename: parse new name success, [%s]",
                        newName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(newName, fullDirNewName))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioRename: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }
    
    sysLoger(LDEBUG, "TSFS_Worker::fioRename: full new name, [%s]",
                        fullDirNewName);
    
    //重命名文件
    if( -1 == rename(fullDirOldName, fullDirNewName) )
    {
        //重命名文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::rename failed,old name:[%s],new name:[%s]",
                            fullDirOldName, fullDirNewName);
        return FALSE;
    } 

    //重命名成功
    sysLoger(LINFO, "TSFS_Worker::rename successful,old name:[%s],new name:[%s]",
                            fullDirOldName, fullDirNewName);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    创建目录，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<dirName>;
*/
int TSFS_Worker::fioMkDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    char dirName[NAME_SIZE];
    char fullDir[NAME_SIZE];
    char *pDirName  = dirName;
    char *pData     = pRecvData;
    memset(fullDir, 0, sizeof(fullDir));
    memset(dirName, 0, sizeof(dirName));

    //dirName
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //检查dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //目录名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: argument:dirName is too long");
        return FALSE;
    }
    
    //获取dirName
    memcpy(pDirName,++pBegin,(pEnd-pBegin));  

    sysLoger(LDEBUG, "TSFS_Worker::fioMkDir: parse dir name success, [%s]",
                        dirName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pDirName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioMkDir: full dir name, [%s]", fullDir);

    //创建目录
    if(-1 == _mkdir(fullDir) )
    {
        //创建目录失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: make directory failed,[%s]",
                            fullDir);
        return FALSE;
    }

    //创建目录成功
    sysLoger(LINFO, "TSFS_Worker::fioMkDir: make directory successful,[%s]",
                            fullDir);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    删除目录，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<dirName>;
*/
int TSFS_Worker::fioRmDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    char dirName[NAME_SIZE];
    char fullDir[NAME_SIZE];
    char *pDirName  = dirName;
    char *pData     = pRecvData;
    memset(dirName, 0, sizeof(dirName));
    memset(fullDir, 0, sizeof(fullDir));

    //dirName
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //检查dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //目录名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: argument:dirName is too long");
        return FALSE;
    }

    //获取dirName
    memcpy(pDirName,++pBegin,(pEnd-pBegin));

    sysLoger(LDEBUG, "TSFS_Worker::fioRmDir: parse dir name success, [%s]",
                        dirName);
    
    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pDirName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioRmDir: full dir name, [%s]",
                        fullDir);

    //删除目录
    int ret = _rmdir(fullDir);
    if(-1 == ret)
    {
        //删除目录失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: remove directory fialed,[%s]",
                            fullDir);
        return FALSE;
    }

    //删除目录成功
    sysLoger(LINFO, "TSFS_Worker::fioRmDir: remove directory successful,[%s]",
                            fullDir);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    读取目录项，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<dir>;idx;
*/
int TSFS_Worker::fioReadDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    char dirName[NAME_SIZE];
    char fullDir[NAME_SIZE];
    memset(dirName, 0, sizeof(dirName));
    memset(fullDir, 0, sizeof(fullDir));
    
    int idx         = 0;        //文件索引
    char *pDirName  = dirName;
    char *pData     = pRecvData;

    //目录
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //检查目录名
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //目录名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:dirName is too long");
        return FALSE;
    }

    //获取dir
    memcpy(pDirName,++pBegin,(pEnd-pBegin));   

    sysLoger(LDEBUG, "TSFS_Worker::fioReadDir: parse dir name success, [%s]",
                        dirName);

    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pDirName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    pData = pEnd+2;

    sysLoger(LDEBUG, "TSFS_Worker::fioReadDir: full dir name, [%s]", fullDir);

    //idx
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:idx is invalid");
        return FALSE;
    }

    //获取idx
    if((0 == hexToInt(&pData,&idx)) || (idx < 0) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:idx is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioReadDir: parse idx success, [idx:%d]",
                        idx);

    //切换至读取目录
    if(-1 == _chdir(fullDir))  
    {
        //目录不存在,回复信息
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: can't change to the directory, [%s]",
                            fullDir);
        return FALSE;
    }

    intptr_t hFile;         //搜索句柄
    _finddata_t fileInfo;   //文件信息
    struct _stat idxFile;   //第idx个文件信息
    
    //读取目录
    hFile = _findfirst("*.*", &fileInfo);
    if (-1 == hFile ) 
    { 
        //遍历目录失败
        _findclose(hFile);
        _chdir(pTSFS_Server->getUserDir());
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: find file failed, [%s]",
                            fullDir);
        _get_errno( &m_errno );
        return FALSE;
    }

    //读取目录下文件,遍历到第idx个文件
    int count = idx;
    do
    { 
        //除根目录外,文件目录下第1,2文件为"."和".."目录,需跳过  
        if( strcmp(fileInfo.name, ".") == NULL 
         || strcmp(fileInfo.name, "..") == NULL)
        {
            ++count;
        }

        if(count > 0)
        {
            //查找下一个文件
            if(-1 == _findnext(hFile, &fileInfo))   
            {
                //查找文件失败
                _findclose(hFile);
                _chdir(pTSFS_Server->getUserDir());
                sysLoger(LWARNING, "TSFS_Worker::fioReadDir: didn't find file,[idx:%d]",
                                idx);
                _get_errno( &m_errno );                
                return FALSE;
            }
        }
    }while(count-- > 0);

    //关闭搜索句柄
    _findclose(hFile);    

    //获取第idx个文件信息
    if( -1 == _stat( fileInfo.name, &idxFile) )
    {
        //获取第idx个文件信息失败
        _get_errno( &m_errno );
        
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: get file info failed, [idx:%d], [%s]",
                            idx, fileInfo.name);
        return FALSE;
    }

    //返回逻辑根目录
    _chdir(pTSFS_Server->getUserDir());

    //获取第idx个文件信息成功
    sysLoger(LINFO, "TSFS_Worker::fioReadDir: get file info successful,[idx:%d], [%s]",
                    idx, fileInfo.name); 

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%s%s%s%s%x%s",
            TSFS_SUCCESS,                       //ok
            TSFS_CONTROL_FLAG,                  //;
            TSFS_FILE_NAME_BEGIN,               //<
            fileInfo.name,                      //fileName
            TSFS_FILE_NAME_END,                 //>
            TSFS_CONTROL_FLAG,                  //;
            idxFile.st_mode,                    //st_modes
            TSFS_CONTROL_FLAG);                 //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE; 
}

/*
 * @brief
 *    根据文件描述符,获取文件状态，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;
*/
int TSFS_Worker::fioFstatGetByFd(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: argument:file pointer is invalid");
        return FALSE;
    }

    //获取fp
    hexToInt(&pData,&fp);

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByFd: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //获取文件状态
    struct stat fileInfo;
    if( -1 == fstat(fp,&fileInfo) )
    {
        //获取文件状态失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: get the file information failed,[fp:%d]",
                            fp);
        return FALSE;
    }    

    //填充结果缓冲
    char fileCtime[10];
    memset(fileCtime, 0, sizeof(fileCtime));
    sprintf(fileCtime,"%x",fileInfo.st_ctime);

    char fileMtime[10];
    memset(fileMtime, 0, sizeof(fileMtime));
    sprintf(fileMtime,"%x",fileInfo.st_mtime);

    char fileAtime[10];
    memset(fileAtime, 0, sizeof(fileAtime));
    sprintf(fileAtime,"%x",fileInfo.st_atime);

    //清空回复数据包
    sendPack->setData("",0);
    
    int size = sprintf(sendPack->getData(),"%s%s%x%s%x%s%x%s%x%s%x%s%x%s%x%s%x%s%s%s%s%s%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_dev,            //st_dev    
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_ino,            //st_ino
                    TSFS_CONTROL_FLAG,          //;   
                    fileInfo.st_mode,           //st_mode
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_nlink,          //st_nlink
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_uid,            //st_uid
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_gid,            //st_gid
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_rdev,           //st_rdev
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_size,           //st_size
                    TSFS_CONTROL_FLAG,          //;
                    fileAtime,                  //st_atime
                    TSFS_CONTROL_FLAG,          //;
                    fileMtime,                  //st_mtime
                    TSFS_CONTROL_FLAG,          //;
                    fileCtime,                  //st_ctime
                    TSFS_CONTROL_FLAG);         //;   

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    //获取文件状态成功，
    sysLoger(LINFO, "TSFS_Worker::fioFstatGetByFd: get the file information successful,[fp:%d]",
                            fp);
    return TRUE; 
}

/*
 * @brief
 *    根据文件名字,获取文件状态，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<name>;
 */
int TSFS_Worker::fioFstatGetByName(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: pRecvData OR sendPack is invalid");
        return FALSE;
    }
    //解析参数
    char fileName[TSFS_NAME_SIZE];
    char fullDir[TSFS_NAME_SIZE];
    char *pFileName  = fileName;
    char *pData     = pRecvData;
    memset(fileName, 0, sizeof(fileName));
    memset(fullDir, 0, sizeof(fullDir));

    //dirName
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: argument:dirName is invalid");
        return FALSE;
    }
    
    //检查dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //目录名太长,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: argument:dirName is too long");
        return FALSE;
    }

    //获取dirName
    memcpy(pFileName,++pBegin,(pEnd-pBegin));

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: parse file name success, [%s]",
                        fileName);
    
    //将文件路径转换为绝对路径
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: full file name, [%s]", fullDir);

    //获取文件状态
    struct stat fileInfo;
    if( -1 == stat(fullDir, &fileInfo) )
    {
        //获取文件状态失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: get the file information failed,[%s]",
                            fullDir);
        return FALSE;
    }       

#if 0
    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: st_ctime:%s", ctime(&(fileInfo.st_ctime)));
    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: st_mtime:%s", ctime(&(fileInfo.st_mtime)));
    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: st_atime:%s", ctime(&(fileInfo.st_atime)));
#endif

    //填充结果缓冲
    char fileCtime[10];
    memset(fileCtime, 0, sizeof(fileCtime));
    sprintf(fileCtime,"%x",fileInfo.st_ctime);

    char fileMtime[10];
    memset(fileMtime, 0, sizeof(fileMtime));
    sprintf(fileMtime,"%x",fileInfo.st_mtime);

    char fileAtime[10];
    memset(fileAtime, 0, sizeof(fileAtime));
    sprintf(fileAtime,"%x",fileInfo.st_atime);

    //清空回复数据包
    sendPack->setData("",0);
    
    int size = sprintf(sendPack->getData(),"%s%s%x%s%x%s%x%s%x%s%x%s%x%s%x%s%x%s%s%s%s%s%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_dev,            //st_dev    
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_ino,            //st_ino
                    TSFS_CONTROL_FLAG,          //;   
                    fileInfo.st_mode,           //st_mode
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_nlink,          //st_nlink
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_uid,            //st_uid
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_gid,            //st_gid
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_rdev,           //st_rdev
                    TSFS_CONTROL_FLAG,          //;
                    fileInfo.st_size,           //st_size
                    TSFS_CONTROL_FLAG,          //;
                    fileAtime,                  //st_atime
                    TSFS_CONTROL_FLAG,          //;
                    fileMtime,                  //st_mtime
                    TSFS_CONTROL_FLAG,          //;
                    fileCtime,                  //st_ctime
                    TSFS_CONTROL_FLAG);         //;


    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    //获取文件状态成功，
    sysLoger(LINFO, "TSFS_Worker::fioFstatGetByName: get the file information successful,[%s]",
                            fullDir);
    return TRUE; 

}

/*
 * @brief
 *    在指定长度处截断文件，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;len;
*/
int TSFS_Worker::fioTrunc(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp          = -1;
    int len         = 0;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:file pointer is invalid");
        return FALSE;
    }
    
    //获取fp
    hexToInt(&pData,&fp);

    sysLoger(LDEBUG, "TSFS_Worker::fioTrunc: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //len
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:len is invalid");
        return FALSE;
    }
    
    //获取len
    if((0 == hexToInt(&pData,&(len))) || (len < 0))          
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:len is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTrunc: parse length success, [length:%d]",
                        len);

    //在指定长度处截断文件
    if(-1 == _chsize(fp,len))
    {
        //截断文件失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: cut off the file failed,[fp:%d], [len:%d]",
                            fp, len);
        return FALSE;
    }

    //截断文件成功
    sysLoger(LINFO, "TSFS_Worker::fioTrunc: cut off the file successful;,[fp:%d], [len:%d]",
                            fp, len);

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    设置文件时间，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;lastAccessTime;lastModifiedTime;
*/
int TSFS_Worker::fioTimeSet(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //解析参数
    int fp          = -1;
    char *pData     = pRecvData;
    time_t lastAccessTime   = 0;
    time_t lastModifiedTime = 0;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:file pointer is invalid");
        return FALSE;
    }

    //获取fp
    hexToInt(&pData,&fp);   

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse file pointer success, [fp:%d]",
                        fp);

    //在文件链表中查找文件
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //没查找到文件，回复错误
        m_errno = EBADF;   
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //lastAccessTime
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:lastAccessTime is invalid");
        return FALSE;
    }
    
    //获取lastAccessTime
    if(0 == hexToInt64(&pData,&lastAccessTime)) 
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:accessTime is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse accessTime success, [actime:%d]",
                        lastAccessTime);

    //lastModifiedTime
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime is invalid");
        return FALSE;
    }
    
    //获取lastModifiedTime
    if(0 == hexToInt64(&pData,&lastModifiedTime))
    {
        //解析参数错误,回复信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse modifiedTime success, [modtime:%d]",
                    lastModifiedTime);
          

    if( (lastModifiedTime >= 0x7933fff80) || (lastAccessTime >= 0x7933fff80) )
    {
        //修改时间大于3000年,回复错误信息
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime or AccessTime is invalid");
        return FALSE;
    }


    //设置文件时间   
    struct _utimbuf newtime;
    newtime.modtime = lastModifiedTime; 
    newtime.actime = lastAccessTime;   
    
    //设置文件修改、访问时间
    if( -1 == _futime(fp,&newtime) )        
    {
        //设置文件时间失败
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: set file time failed, [modtime:%d] ",
                            newtime.modtime );
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: set file time failed, [actime:%d]",
                            newtime.actime );
        return FALSE;
    }

    //设置文件时间成功
    sysLoger(LINFO, "TSFS_Worker::fioTimeSet: set file time successful");

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    检查是否为根目录
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：<>;
 */
int TSFS_Worker::fioCheckRoot(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioCheckRoot: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    int isRoot = 0;        //根目录标识,默认为非根目录

    char *pUserDir = pTSFS_Server->getUserDir();

    char *pstr = strstr(pUserDir, ":");
    if(!pstr)
    {
        pstr = pUserDir;
    }

    while( '\\' == *(++pstr))
    {
        //空循环
    }

    if(NULL == *pstr)
    {
        //为根目录
        isRoot = 1;
    }

    //检查是否根目录成功
    if(isRoot)
    {
        sysLoger(LINFO, "TSFS_Worker::fioCheckRoot: check root successful, is root");
    }
    else
    {
        sysLoger(LINFO, "TSFS_Worker::fioCheckRoot: check root successful, is not root");
    }

    //清空回复数据包
    sendPack->setData("",0);

    //填充结果缓冲
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    isRoot,                     //根目录表示
                    TSFS_CONTROL_FLAG);         //;

    //设置恢复缓冲大小
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    获取文件系统信息，并向目标机回复执行结果
 * @param *pRecvData[in]:    待处理的操作命令
 * @return   操作成功，则返回TURE
             操作失败，则返回FALSE
 * 命令格式：fp;
*/
//int TSFS_Worker::fioFstatFsget(char *pRecvData, Packet *sendPack)
//{
//    if( NULL == pRecvData || NULL == sendPack)
//    {
//        m_errno = EINVAL;
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: pRecvData OR sendPack is invalid");
//        return FALSE;
//    }

//    //解析参数
//    int fp          = -1;
//    char *pData     = pRecvData;
//
//    //fp
//    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
//    {
//        //解析参数错误,回复信息
//        m_errno = EINVAL;
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: argument:file pointer is invalid");
//        return FALSE;
//    }
//    
//    //获取fp
//    hexToInt(&pData,&fp); 

//    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse file pointer success, [fp:%d]", fp);
//
//    //在文件链表中查找文件
//    if( !(pTSFS_Server->searchFileHandle(fp)) )        
//    {
//        //没查找到文件，回复错误
//        m_errno = EBADF;   
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: didn't find the file pointer,[fp:%d]",
//                            fp);
//        return FALSE;
//    }
//
//    //获取文件系统信息,功能还未确定
//
//
//
//
//    //回复格式未定
//        
//    return TRUE;
//}

/**
 * @brief
 *    析构函数，关闭连接
 */
TSFS_Worker::~TSFS_Worker()
{ 
    //关闭TS连接
    TClient_close(m_lnkId);         
}

/*
 * @brief
 *    将相对路径转换为绝对路径
 * @param tempPath[in]:    相对路径
 * @param fullPath[out]:   绝对路径
 * @return   操作成功，则返回TSFS_SUCC
             操作失败，则返回TSFS_FAIL
 */
int TSFS_Worker::tempPath2fullPach(char *tempPath, char *fullPath)
{
    if(NULL == tempPath || NULL == fullPath)
    {
        sysLoger(LWARNING, "TSFS_Worker::tempPath2fullPach: tempPath or fullPath is invalid");
        return TSFS_FAIL;
    }     

    int fullLen = _sprintf_p(fullPath, MAX_PATH, "%s%s", 
                            pTSFS_Server->getUserDir(), tempPath);
    if(-1 == fullLen)
    {            
        //转换失败
        sysLoger(LWARNING, "TSFS_Worker::tempPath2fullPach: change target path to full path failed, user:[%s], target:[%s]",
                        pTSFS_Server->getUserDir(), tempPath);
        return TSFS_FAIL;          
    }
    
    //转换成功
    return TSFS_SUCC; 
}

/*
 * @brief
 *   将Windows错误码转换为PixOS错误码
 * @param winErr[in]: Windows错误码
 * @param pixErr[out]: PixOS错误码
 */
void TSFS_Worker::winErr2posixErr(int winErr, int *pixErr)
{
    if(NULL == pixErr)
    {
        return;
    }

    switch(winErr)
    {
        case ENOTEMPTY:     //目录非空
            *pixErr = 90;
            break;

        default:
            *pixErr = winErr;
    }
}

/*
 * @brief
 *   将目标机端文件操作参数转换为主机端文件操作参数
 * @param taParam[in]: 目标机端文件操作参数
 * @param tsParam[out]: 主机端文件操作参数
 */
int TSFS_Worker::taParam2tsParam(int taParam, int *tsParam)
{
    if(NULL == tsParam)
    {
        return TSFS_FAIL;
    }    

    //主机端windows flag参数
    static const int winFlag[] = {  _O_RDONLY,
                                    _O_WRONLY,
                                    _O_RDWR, 
                                    _O_APPEND,
                                    _O_CREAT,
                                    _O_TRUNC,
                                    _O_EXCL,
                                    _O_TEXT,
                                    _O_BINARY
                                 };

    //目标机端Acoreos flag参数
    static const int acoreFlag[] = {    O_RDONLY,
                                        O_WRONLY,
                                        O_RDWR,
                                        _FAPPEND,
                                        _FCREAT,
                                        _FTRUNC,
                                        _FEXCL,
                                        _FTEXT,
                                        _FBINARY
                                   };

    int flagNum = _countof(acoreFlag);
    int tempParam = 0;

    //转换参数
    for(int i=0; i < flagNum; i++)
    {
        if( (taParam & acoreFlag[i]) == acoreFlag[i] )
        {
            taParam = (taParam ^ acoreFlag[i] | winFlag[i]);
            tempParam = (tempParam | winFlag[i]);
        }
    }

    if(taParam != tempParam)
    {
        //参数有错
        return TSFS_FAIL;
    }

    *tsParam = tempParam;
    
    return TSFS_SUCC;
}
