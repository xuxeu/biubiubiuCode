/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsWorker.cpp
* @brief  
*       ���ܣ�
*       <li>ʵ��Worker��ʵ���ļ������Ĵ���</li>
*/
/************************ͷ�ļ�********************************/
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

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/**
 * @brief
 *    ���캯�������Worker��ʼ������������TS
 * @param size[in]: Worker������д�С
 */
TSFS_Worker::TSFS_Worker(unsigned short size):threadWithQueue(size)
{    
    //����TS
    m_lnkId = (US)TClient_open(pTSFS_Server->getTsIPAddr());

    //����Worker����ʼ����
    start();
}

/*
 * @brief
 *   ���ݴ������󣬲��ظ����
 * @param pack[in]: Ŀ����Ĵ�������
 */
void TSFS_Worker::process(Packet *pack)
{
    if(NULL == pack)
    {
        //��Worker�������Worker����
        pTSFS_Server->recycleWorker(this);

        sysLoger(LWARNING, "TSFS_Worker::process: pack is invalid" );
    }

    char *pBuf = pack->getData();

    m_errno = TSFS_PROCESS_SUCC;

    sysLoger(LDEBUG, "TSFS_Worker::process: worker get a packet, [worker threadID = %d], data:[%c]",
                      this->getThreadID(), pack->getData()[0]);

    //��������
    char *pstr = strstr(pBuf, TSFS_COMM_CONTROL_FLAG);
    if( !( pstr && *(++pstr) ) )
    {
        //��Ч�ĸ�ʽ
        m_errno = EINVAL;

        sysLoger(LWARNING, "TSFS_Worker::process: argument:invalid command" );        
    }
    else
    {
        //ִ������
        switch(pBuf[0])
        {
            case TSFS_OPEN_FILE:        //���ļ�
                tsfsOpenFile(pstr, pack);
                break;

            case TSFS_CLOSE_FILE:       //�ر��ļ�
                tsfsCloseFile(pstr, pack);
                break;

            case TSFS_READ_FILE:        //���ļ�
                tsfsReadFile(pstr, pack);
                break;

            case TSFS_WRITE_FILE:       //д�ļ�
                tsfsWriteFile(pstr, pack);
                break;

            case TSFS_CREATE_FILE:      //�����ļ�
                tsfsCreateFile(pstr, pack);
                break;

            case TSFS_DELETE_FILE:      //ɾ���ļ�
                tsfsDeleteFile(pstr, pack);
                break;

            case TSFS_IO_CONTROL:       //�ļ����Բ���
                tsfsIOControl(pstr, pack);
                break; 

            default:
                //��֧�ֵĽӿ�����
                m_errno = EINVAL;

                sysLoger(LWARNING, "TSFS_Worker::process: unknown command" );                
        }
    }    

    //������ʧ��,�����ʧ����Ϣ
    if( TSFS_PROCESS_SUCC != m_errno )
    {
        pack->setData("",0);

        //��Windows������ת��ΪPixOS������
        winErr2posixErr(m_errno, &m_errno);

        //���ظ���Ϣ
        int size = sprintf(pack->getData(), "E%x;", m_errno);

        //���ûظ���Ϣ��С
        pack->setSize((US)size);
    }

    //����Դ��Ŀ��ID
    pack->setDes_aid( pack->getSrc_aid() );
    pack->setDes_said( pack->getSrc_said() );
    pack->setSrc_said( 0 );

    //�ظ����
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

    //��Packet�����Packet����
    pTSFS_Server->recyclePacket(pack);    

    //��Worker�������Worker����
    pTSFS_Server->recycleWorker(this);    
}

/*
 * @brief
 *    ִ�д��ļ�����������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]: ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<filename>;flag;[mode;]
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

    //��������

    //filename
    pBegin  = strstr(pData,TSFS_FILE_NAME_BEGIN);
    pEnd = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:filename is invalid");
        return FALSE;
    }

    //����ļ���
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //�ļ���̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:filename is too long");
        return FALSE;
    }    

    //��ȡfilename
    memcpy(pFileName, ++pBegin, (pEnd-pBegin));
    pData = pEnd+2;

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse filename success, [%s]",
                        fileName);

    //���ļ�·��ת��Ϊ����·��
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid");
        return FALSE;
    }

    //��ȡflag
    if(0 == hexToInt(&pData,&flag))         
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse flag success, [flag:0x%x]",
                        flag);

    //ת��flag
    if(TSFS_FAIL == taParam2tsParam(flag, &flag))
    {
        //��������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:flag is invalid, [flag:0x%x]",
                            flag);
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: change to windows flag, [flag:0x%x]",
                        flag);

    //mode ��ѡ����
    if(NULL != *(++pData))
    {
        if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
        {
            //������������,�ظ���Ϣ
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:mode is invalid");
            return FALSE;
        }
        
        //��ȡmode
        if(0 == hexToInt(&pData,&mode))     
        {
            //������������,�ظ���Ϣ
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: argument:mode is invalid");
            return FALSE;
        }       

        sysLoger(LDEBUG, "TSFS_Worker::tsfsOpenFile: parse mode success, [mode:0x%x]",
                            mode);
    }

    //���mode
    if( (flag & _O_CREAT) == _O_CREAT )
    {
        /*  ǿ�������ļ�Ȩ��Ϊ�ɶ�д  */
        mode = _S_IREAD | _S_IWRITE;
    }


    //���ļ�,ǿ�����Ӷ����ƴ򿪷�ʽ
    int fp = _open(fullDir, flag | _O_BINARY, mode );

    if( -1 == fp )
    {   
        //���ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsOpenFile: open file failed, [%s]",
                            fullDir);
        return FALSE;
    }  

    //���ļ��ɹ�,��ȡ�ļ����
    pTSFS_Server->insertFileHandle(fp);

    sysLoger(LINFO, "TSFS_Worker::tsfsOpenFile: open file successful, [%s], [fp:%d]",
                    fullDir, fp);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fp,                         //fp
                    TSFS_CONTROL_FLAG);         //;  

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ִ�йر��ļ�����������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� fp;
*/

int TSFS_Worker::tsfsCloseFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp = 0;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: argument:file pointer is invalid");
        return FALSE;
    }

    //��ȡfp
    hexToInt(&pData,&fp);               
    
    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: didn't find the file pointer, [fp:%d]"
                            , fp);
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCloseFile: parse file pointer success, [fp:%d]",
                        fp);

    //�ر��ļ���ɾ���ļ��ṹ��ӳ���
    if(0!=_close(fp))
    {
        //�ر��ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsCloseFile: close the file failed, [fp:%d]"
                            , fp);
        return FALSE;
    }

    //�ر��ļ��ɹ�
    sysLoger(LINFO, "TSFS_Worker::tsfsCloseFile: close the file successful, [fp:%d]"
                    , fp);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                TSFS_SUCCESS,               //ok
                TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    //ɾ���ļ����
    pTSFS_Server->removeFileHandle(fp);

    return TRUE;
}


/*
 * @brief
 *    ִ�ж�ȡ�ļ�����������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� fp;size;
*/
int TSFS_Worker::tsfsReadFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp      = 0;
    int r_size  = 0;
    char *pData = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);   

    sysLoger(LDEBUG, "TSFS_Worker::tsfsReadFile: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //size
    if( !(strstr(++pData, TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:size is invalid");
        return FALSE;
    }

    //��ȡsize,7Ϊ"OK;000;"����
    if(0 == hexToInt(&pData, &r_size) || (r_size > (TSFS_MAX_DATA_SIZE - 7)))
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: argument:size is invalid, [size:%d], max read size:[%d]",
                            r_size, (sendPack->getDataSize() - 7));
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsReadFile: parse size success, [size:%d]",
                        r_size);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    int size = sprintf(sendPack->getData(), "%s%s",
                            TSFS_SUCCESS,               //ok
                            TSFS_CONTROL_FLAG);         //;

    char *pLen = sendPack->getData() + size;

    size += sprintf(pLen, "%x%s",
                            TSFS_DATA_SIZE,             //����ȡ����
                            TSFS_CONTROL_FLAG);         //;

    //��ȡ�ļ�
    int readLen = _read(fp, sendPack->getData() + size, r_size);
    if( -1 == readLen || readLen > r_size )
    {
        //��ȡ����
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsReadFile: read file failed, [fp:%d], [size:%d]",
                            fp, r_size);
        return FALSE;
    }

    //��ȡ�ļ��ɹ�    
    sysLoger(LINFO, "TSFS_Worker::tsfsReadFile: read file successful, [size:%d]",
                        readLen);

    //���readLen,3Ϊ����ȡ���ȵ�λ��
    sprintf(pLen, "%03x", readLen);

    //�����readLen�Ὣ';'��Ϊ\0,������Ҫ�������';',3Ϊ����ȡ���ȵ�λ��
    memcpy(pLen + 3, TSFS_CONTROL_FLAG, strlen(TSFS_CONTROL_FLAG));

    size += readLen;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ִ��д�ļ�����������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� fp;size;data
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

    //��������
    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);  

    sysLoger(LDEBUG, "TSFS_Worker::tsfsWriteFile: parse file pointer success, [fp:%d]", fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //size
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is invalid");
        return FALSE;
    }
    
    //��ȡsize
    if(0 == hexToInt(&pData,&w_size))       
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsWriteFile: parse size success, [size:%d]",
                        w_size);

    //data,'x:'Ϊ����
    if( sendPack->getSize() < ((strlen("x:") + (unsigned)(++pData - pRecvData) + w_size)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: argument:size is bigger than recv data size, [size:%d], [recv data size:%d]",
                            w_size, (sendPack->getSize() - strlen("x:") - (unsigned)(pData - pRecvData)));
        return FALSE;
    }

    //д�ļ�
    int writeLen = _write(fp, pData, w_size);
    if(-1 == writeLen)
    {
        //д�ļ�����
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsWriteFile: write data failed, [fp:%d], [size:%d]",
                            fp, w_size);
        return FALSE;
    }
    
    //д�ļ��ɹ�
    sysLoger(LINFO, "TSFS_Worker::tsfsWriteFile: write data successful, [fp:%d], [size:%d]",
                            fp, writeLen);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    writeLen,                   //writeLen
                    TSFS_CONTROL_FLAG);         //

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;  
}

/*
 * @brief
 *    ִ�д����ļ�������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� <fileName>;mode;
*/
int TSFS_Worker::tsfsCreateFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:fileName is invalid");
        return FALSE;
    }
    
    //����ļ���
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //�ļ���̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:fileName is too long");
        return FALSE;
    }

    //��ȡfilename
    memcpy(pFileName,++pBegin,(pEnd-pBegin));
    pData = pEnd+2;

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCreateFile: parse fileName success, [%s]",
                        fileName);

    //���ļ�·��ת��Ϊ����·��
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:mode is invalid");
        return FALSE;
    }

    //��ȡmode
    if(0 == hexToInt(&pData,&mode))         
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: argument:mode is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsCreateFile: parse mode success, [mode:0x%x]",
                        mode);

    /*  ǿ�������ļ�Ȩ��Ϊ�ɶ�д  */    
    mode = _S_IREAD | _S_IWRITE;
    
    //�����ļ�
    int fp = _creat(fullDir, mode);
    if(-1 == fp)
    {
        //�����ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsCreateFile: create file failed,[%s]",
                            fullDir);
        return FALSE;
    }

    //�����ļ��ɹ�,��ȡ�ļ����
    pTSFS_Server->insertFileHandle(fp);

    sysLoger(LINFO, "TSFS_Worker::tsfsCreateFile: create file successful,[%s]",
                            fullDir);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    fp,                         //fp
                    TSFS_CONTROL_FLAG);         //;      

    //���ûָ������С
    sendPack->setSize((US)size);
    
    return TRUE;
}

/*
 * @brief
 *    ִ��ɾ���ļ�������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� <fileName>;
*/
int TSFS_Worker::tsfsDeleteFile(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: argument:fileName is invalid");
        return FALSE;
    }
    
    //����ļ���
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //�ļ���̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: argument:fileName is too long");
        return FALSE;
    }
    
    //��ȡfilename
    memcpy(pFileName,++pBegin,(pEnd-pBegin)); 

    sysLoger(LDEBUG, "TSFS_Worker::tsfsDeleteFile: parse fileName success, [%s]",
                        fileName);

    //���ļ�·��ת��Ϊ����·��
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsDeleteFile: full fileName, [%s]",
                        fullDir);

    //ɾ���ļ�
    int ret = remove(fullDir);
    if(-1 == ret)
    {
        //ɾ���ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::tsfsDeleteFile: delete file fialed,[%s]",
                            fullDir);
        return FALSE;
    }

    sysLoger(LINFO, "TSFS_Worker::tsfsDeleteFile: delete file successful,[%s]",
                            fullDir);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    �����ļ����Բ������ͣ����ö�Ӧ���ܺ�������
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ�� opt;optArgs;
*/
int TSFS_Worker::tsfsIOControl(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int opt         = 0;
    char *pData     = pRecvData;

    //opt
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: argument:opt is invalid");
        return FALSE;
    }
    
    //��ȡopt
    if(0 == hexToInt(&(pData),&opt))            
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: argument:opt is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::tsfsIOControl: parse optArgs success, [opt:%d]",
                        opt);
    
    //�����ѽ����Ĳ���opt,ָ��optArgs
    ++pData;                        

    //ƥ���������
    switch(opt)
    {
        case TSFS_FIONREAD:         //��ȡ�ļ���δ������
            fioNotRead(pData, sendPack);
            break;

        case TSFS_FIOSEEK:          //�����ļ���дλ��
            fioSeek(pData, sendPack);
            break;

        case TSFS_FIOWHERE:         //��ȡ�ļ���дλ��
            fioWhere(pData, sendPack);
            break;

        case TSFS_FIORENAME:        //�������ļ�
            fioRename(pData, sendPack);
            break;

        case TSFS_FIOMKDIR:         //����Ŀ¼
            fioMkDir(pData, sendPack);
            break;

        case TSFS_FIORMDIR:         //ɾ��Ŀ¼
            fioRmDir(pData, sendPack);
            break;

        case TSFS_FIOREADDIR:       //��ȡĿ¼��
            fioReadDir(pData, sendPack);
            break;

        case TSFS_FIOFSTATGETBYFD:  //��ȡ�ļ�״̬(by�ļ�������)
            fioFstatGetByFd(pData, sendPack);
            break;

        case TSFS_FIOFSTATGETBYNAME: //��ȡ�ļ�״̬(by�ļ���)
            fioFstatGetByName(pData, sendPack);
            break;

        case TSFS_FIOTRUNC:         //��ָ�����ȴ��ض��ļ�
            fioTrunc(pData, sendPack);
            break;

        case TSFS_FIOTIMESET:       //�����ļ�ʱ��
            fioTimeSet(pData, sendPack);
            break;

        case TSFS_FIOCHECKROOT:     //����Ƿ�Ϊ��Ŀ¼
            fioCheckRoot(pData, sendPack);
            break;

        //case TSFS_FIOFSTATFSGET:    //��ȡ�ļ�ϵͳ��Ϣ
        //    fioFstatFsget(pData, sendPack);
        //    break;
            
        default:
            //��֧�ָ����Ͳ���
            m_errno = EINVAL;
            sysLoger(LWARNING, "TSFS_Worker::tsfsIOControl: unknown opt, [opt:%d]",
                                    opt);
            return FALSE;
    }
    return TRUE;
}


/*
 * @brief
 *    ��ȡ�ļ���δ�����ȣ�����Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;
*/
int TSFS_Worker::fioNotRead(char *pRecvData, Packet *sendPack)
{    
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);  

    sysLoger(LDEBUG, "TSFS_Worker::fioNotRead: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: didn't find the file pointer, [fp:%d]",
                            fp);
        return FALSE;
    }

    //�鿴��ǰ��ȡλ��
    long curOffset = _tell(fp);
    if(-1 == curOffset)
    {
        //��ȡ�ļ���дλ��ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: get file offset failed, [fp:%d]",
                            fp);

        return FALSE;
    }

    //��ȡ�ļ�ĩβλ��
    long fileLen = _filelength(fp);
    if(-1 == fileLen)
    {
        //��ȡ�ļ�ĩβλ��ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioNotRead: get file length failed, [fp:%d]",
                            fp);
        return FALSE;
    }      

    sysLoger(LINFO, "TSFS_Worker::fioNotRead: get file no read length successful, [fp:%d], [length:%d]",
                            fp, (fileLen-curOffset));

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    (fileLen-curOffset),        //no read len
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    �����ļ���дλ�ã�����Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;offset;(����ļ�ͷ��ƫ��)
*/
int TSFS_Worker::fioSeek(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    int offset      = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);    

    sysLoger(LDEBUG, "TSFS_Worker::fioSeek: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //offset,����ļ�ͷ��ƫ��
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:offset is invalid");
        return FALSE;
    }
    if((0 == hexToInt(&pData,&offset)) || (offset < 0) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioSeek: argument:offset is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioSeek: parse offset success, [offset:%d]",
                        offset);

    //�����ļ���дλ��
    long newOffset = _lseek(fp, offset, SEEK_SET);
    if(-1 == newOffset)
    {
        //��λ�ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioSeek: set file offset failed,[fp:%d],[offset:%d]",
                            fp, offset);
        return FALSE;
    }

    //��λ�ļ��ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioSeek: set file offset successful,[fp:%d],[offset:%d]",
                            fp, newOffset);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    newOffset,                  //newOffset
                    TSFS_CONTROL_FLAG);         //;    

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ��ȡ�ļ���дλ�ã�����Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;
*/
int TSFS_Worker::fioWhere(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);    

    sysLoger(LDEBUG, "TSFS_Worker::fioWhere: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF;  
        sysLoger(LWARNING, "TSFS_Worker::fioWhere: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }
    
    //��ȡ�ļ���дλ��
    long curOffset = _tell(fp);
    if(-1 == curOffset)
    {
        //��ȡ�ļ���дλ��ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioWhere: get file offset failed,[fp:%d]",
                            fp);
        return FALSE;
    }

    //��ȡ�ļ���дλ�óɹ�
    sysLoger(LINFO, "TSFS_Worker::fioWhere: get file offset successful,[fp:%d],[offset:%d]",
                            fp,curOffset);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    curOffset,                  //curOffset
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    �������ļ�������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<old_name>;<new_name>;
*/
int TSFS_Worker::fioRename(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:oldName is invalid");
        return FALSE;
    }
    
    //����ļ���
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //�ļ���̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:oldName is too long");
        return FALSE;
    }

    //��ȡold_name
    pName = oldName;
    memcpy(pName,++pBegin,(pEnd-pBegin));         
    pData = ++pEnd;

    sysLoger(LDEBUG, "TSFS_Worker::fioRename: parse old name success, [%s]",
                        oldName);

    //���ļ�·��ת��Ϊ����·��
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:newName is invalid");
        return FALSE;
    }
    
    //����ļ���
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //�ļ���̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRename: argument:newName is too long");
        return FALSE;
    }

    //��ȡnew_name
    pName = newName;
    memcpy(pName,++pBegin,(pEnd-pBegin));  

    sysLoger(LDEBUG, "TSFS_Worker::fioRename: parse new name success, [%s]",
                        newName);

    //���ļ�·��ת��Ϊ����·��
    if(TSFS_FAIL == tempPath2fullPach(newName, fullDirNewName))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioRename: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }
    
    sysLoger(LDEBUG, "TSFS_Worker::fioRename: full new name, [%s]",
                        fullDirNewName);
    
    //�������ļ�
    if( -1 == rename(fullDirOldName, fullDirNewName) )
    {
        //�������ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::rename failed,old name:[%s],new name:[%s]",
                            fullDirOldName, fullDirNewName);
        return FALSE;
    } 

    //�������ɹ�
    sysLoger(LINFO, "TSFS_Worker::rename successful,old name:[%s],new name:[%s]",
                            fullDirOldName, fullDirNewName);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ����Ŀ¼������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<dirName>;
*/
int TSFS_Worker::fioMkDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //���dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //Ŀ¼��̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: argument:dirName is too long");
        return FALSE;
    }
    
    //��ȡdirName
    memcpy(pDirName,++pBegin,(pEnd-pBegin));  

    sysLoger(LDEBUG, "TSFS_Worker::fioMkDir: parse dir name success, [%s]",
                        dirName);

    //���ļ�·��ת��Ϊ����·��
    if(TSFS_FAIL == tempPath2fullPach(pDirName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioMkDir: full dir name, [%s]", fullDir);

    //����Ŀ¼
    if(-1 == _mkdir(fullDir) )
    {
        //����Ŀ¼ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioMkDir: make directory failed,[%s]",
                            fullDir);
        return FALSE;
    }

    //����Ŀ¼�ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioMkDir: make directory successful,[%s]",
                            fullDir);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ɾ��Ŀ¼������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<dirName>;
*/
int TSFS_Worker::fioRmDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //���dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //Ŀ¼��̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: argument:dirName is too long");
        return FALSE;
    }

    //��ȡdirName
    memcpy(pDirName,++pBegin,(pEnd-pBegin));

    sysLoger(LDEBUG, "TSFS_Worker::fioRmDir: parse dir name success, [%s]",
                        dirName);
    
    //���ļ�·��ת��Ϊ����·��
    if(TSFS_FAIL == tempPath2fullPach(pDirName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioRmDir: full dir name, [%s]",
                        fullDir);

    //ɾ��Ŀ¼
    int ret = _rmdir(fullDir);
    if(-1 == ret)
    {
        //ɾ��Ŀ¼ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioRmDir: remove directory fialed,[%s]",
                            fullDir);
        return FALSE;
    }

    //ɾ��Ŀ¼�ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioRmDir: remove directory successful,[%s]",
                            fullDir);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ��ȡĿ¼�����Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<dir>;idx;
*/
int TSFS_Worker::fioReadDir(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    char dirName[NAME_SIZE];
    char fullDir[NAME_SIZE];
    memset(dirName, 0, sizeof(dirName));
    memset(fullDir, 0, sizeof(fullDir));
    
    int idx         = 0;        //�ļ�����
    char *pDirName  = dirName;
    char *pData     = pRecvData;

    //Ŀ¼
    char *pBegin = strstr(pData,TSFS_FILE_NAME_BEGIN);
    char *pEnd   = strstr(pData,TSFS_FILE_NAME_END);
    if(!pBegin || !pEnd)
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:dirName is invalid");
        return FALSE;
    }
    
    //���Ŀ¼��
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //Ŀ¼��̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:dirName is too long");
        return FALSE;
    }

    //��ȡdir
    memcpy(pDirName,++pBegin,(pEnd-pBegin));   

    sysLoger(LDEBUG, "TSFS_Worker::fioReadDir: parse dir name success, [%s]",
                        dirName);

    //���ļ�·��ת��Ϊ����·��
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:idx is invalid");
        return FALSE;
    }

    //��ȡidx
    if((0 == hexToInt(&pData,&idx)) || (idx < 0) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: argument:idx is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioReadDir: parse idx success, [idx:%d]",
                        idx);

    //�л�����ȡĿ¼
    if(-1 == _chdir(fullDir))  
    {
        //Ŀ¼������,�ظ���Ϣ
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: can't change to the directory, [%s]",
                            fullDir);
        return FALSE;
    }

    intptr_t hFile;         //�������
    _finddata_t fileInfo;   //�ļ���Ϣ
    struct _stat idxFile;   //��idx���ļ���Ϣ
    
    //��ȡĿ¼
    hFile = _findfirst("*.*", &fileInfo);
    if (-1 == hFile ) 
    { 
        //����Ŀ¼ʧ��
        _findclose(hFile);
        _chdir(pTSFS_Server->getUserDir());
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: find file failed, [%s]",
                            fullDir);
        _get_errno( &m_errno );
        return FALSE;
    }

    //��ȡĿ¼���ļ�,��������idx���ļ�
    int count = idx;
    do
    { 
        //����Ŀ¼��,�ļ�Ŀ¼�µ�1,2�ļ�Ϊ"."��".."Ŀ¼,������  
        if( strcmp(fileInfo.name, ".") == NULL 
         || strcmp(fileInfo.name, "..") == NULL)
        {
            ++count;
        }

        if(count > 0)
        {
            //������һ���ļ�
            if(-1 == _findnext(hFile, &fileInfo))   
            {
                //�����ļ�ʧ��
                _findclose(hFile);
                _chdir(pTSFS_Server->getUserDir());
                sysLoger(LWARNING, "TSFS_Worker::fioReadDir: didn't find file,[idx:%d]",
                                idx);
                _get_errno( &m_errno );                
                return FALSE;
            }
        }
    }while(count-- > 0);

    //�ر��������
    _findclose(hFile);    

    //��ȡ��idx���ļ���Ϣ
    if( -1 == _stat( fileInfo.name, &idxFile) )
    {
        //��ȡ��idx���ļ���Ϣʧ��
        _get_errno( &m_errno );
        
        sysLoger(LWARNING, "TSFS_Worker::fioReadDir: get file info failed, [idx:%d], [%s]",
                            idx, fileInfo.name);
        return FALSE;
    }

    //�����߼���Ŀ¼
    _chdir(pTSFS_Server->getUserDir());

    //��ȡ��idx���ļ���Ϣ�ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioReadDir: get file info successful,[idx:%d], [%s]",
                    idx, fileInfo.name); 

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%s%s%s%s%x%s",
            TSFS_SUCCESS,                       //ok
            TSFS_CONTROL_FLAG,                  //;
            TSFS_FILE_NAME_BEGIN,               //<
            fileInfo.name,                      //fileName
            TSFS_FILE_NAME_END,                 //>
            TSFS_CONTROL_FLAG,                  //;
            idxFile.st_mode,                    //st_modes
            TSFS_CONTROL_FLAG);                 //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE; 
}

/*
 * @brief
 *    �����ļ�������,��ȡ�ļ�״̬������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;
*/
int TSFS_Worker::fioFstatGetByFd(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: argument:file pointer is invalid");
        return FALSE;
    }

    //��ȡfp
    hexToInt(&pData,&fp);

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByFd: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF; 
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //��ȡ�ļ�״̬
    struct stat fileInfo;
    if( -1 == fstat(fp,&fileInfo) )
    {
        //��ȡ�ļ�״̬ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByFd: get the file information failed,[fp:%d]",
                            fp);
        return FALSE;
    }    

    //���������
    char fileCtime[10];
    memset(fileCtime, 0, sizeof(fileCtime));
    sprintf(fileCtime,"%x",fileInfo.st_ctime);

    char fileMtime[10];
    memset(fileMtime, 0, sizeof(fileMtime));
    sprintf(fileMtime,"%x",fileInfo.st_mtime);

    char fileAtime[10];
    memset(fileAtime, 0, sizeof(fileAtime));
    sprintf(fileAtime,"%x",fileInfo.st_atime);

    //��ջظ����ݰ�
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

    //���ûָ������С
    sendPack->setSize((US)size);

    //��ȡ�ļ�״̬�ɹ���
    sysLoger(LINFO, "TSFS_Worker::fioFstatGetByFd: get the file information successful,[fp:%d]",
                            fp);
    return TRUE; 
}

/*
 * @brief
 *    �����ļ�����,��ȡ�ļ�״̬������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<name>;
 */
int TSFS_Worker::fioFstatGetByName(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: pRecvData OR sendPack is invalid");
        return FALSE;
    }
    //��������
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
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: argument:dirName is invalid");
        return FALSE;
    }
    
    //���dirName
    if( TSFS_NAME_SIZE<(pEnd-pBegin) )
    {
        //Ŀ¼��̫��,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: argument:dirName is too long");
        return FALSE;
    }

    //��ȡdirName
    memcpy(pFileName,++pBegin,(pEnd-pBegin));

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: parse file name success, [%s]",
                        fileName);
    
    //���ļ�·��ת��Ϊ����·��
    if(TSFS_FAIL == tempPath2fullPach(pFileName, fullDir))
    {
        sysLoger(LWARNING, "TSFS_Worker::fioFstatGetByName: change target path to full path failed");
        m_errno = EINVAL;
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioFstatGetByName: full file name, [%s]", fullDir);

    //��ȡ�ļ�״̬
    struct stat fileInfo;
    if( -1 == stat(fullDir, &fileInfo) )
    {
        //��ȡ�ļ�״̬ʧ��
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

    //���������
    char fileCtime[10];
    memset(fileCtime, 0, sizeof(fileCtime));
    sprintf(fileCtime,"%x",fileInfo.st_ctime);

    char fileMtime[10];
    memset(fileMtime, 0, sizeof(fileMtime));
    sprintf(fileMtime,"%x",fileInfo.st_mtime);

    char fileAtime[10];
    memset(fileAtime, 0, sizeof(fileAtime));
    sprintf(fileAtime,"%x",fileInfo.st_atime);

    //��ջظ����ݰ�
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


    //���ûָ������С
    sendPack->setSize((US)size);

    //��ȡ�ļ�״̬�ɹ���
    sysLoger(LINFO, "TSFS_Worker::fioFstatGetByName: get the file information successful,[%s]",
                            fullDir);
    return TRUE; 

}

/*
 * @brief
 *    ��ָ�����ȴ��ض��ļ�������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;len;
*/
int TSFS_Worker::fioTrunc(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    int len         = 0;
    char *pData     = pRecvData;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:file pointer is invalid");
        return FALSE;
    }
    
    //��ȡfp
    hexToInt(&pData,&fp);

    sysLoger(LDEBUG, "TSFS_Worker::fioTrunc: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //len
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:len is invalid");
        return FALSE;
    }
    
    //��ȡlen
    if((0 == hexToInt(&pData,&(len))) || (len < 0))          
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: argument:len is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTrunc: parse length success, [length:%d]",
                        len);

    //��ָ�����ȴ��ض��ļ�
    if(-1 == _chsize(fp,len))
    {
        //�ض��ļ�ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioTrunc: cut off the file failed,[fp:%d], [len:%d]",
                            fp, len);
        return FALSE;
    }

    //�ض��ļ��ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioTrunc: cut off the file successful;,[fp:%d], [len:%d]",
                            fp, len);

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    �����ļ�ʱ�䣬����Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;lastAccessTime;lastModifiedTime;
*/
int TSFS_Worker::fioTimeSet(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    //��������
    int fp          = -1;
    char *pData     = pRecvData;
    time_t lastAccessTime   = 0;
    time_t lastModifiedTime = 0;

    //fp
    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:file pointer is invalid");
        return FALSE;
    }

    //��ȡfp
    hexToInt(&pData,&fp);   

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse file pointer success, [fp:%d]",
                        fp);

    //���ļ������в����ļ�
    if( !(pTSFS_Server->searchFileHandle(fp)) )        
    {
        //û���ҵ��ļ����ظ�����
        m_errno = EBADF;   
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: didn't find the file pointer,[fp:%d]",
                            fp);
        return FALSE;
    }

    //lastAccessTime
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:lastAccessTime is invalid");
        return FALSE;
    }
    
    //��ȡlastAccessTime
    if(0 == hexToInt64(&pData,&lastAccessTime)) 
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:accessTime is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse accessTime success, [actime:%d]",
                        lastAccessTime);

    //lastModifiedTime
    if( !(strstr(++pData,TSFS_CONTROL_FLAG)) )
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime is invalid");
        return FALSE;
    }
    
    //��ȡlastModifiedTime
    if(0 == hexToInt64(&pData,&lastModifiedTime))
    {
        //������������,�ظ���Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime is invalid");
        return FALSE;
    }

    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse modifiedTime success, [modtime:%d]",
                    lastModifiedTime);
          

    if( (lastModifiedTime >= 0x7933fff80) || (lastAccessTime >= 0x7933fff80) )
    {
        //�޸�ʱ�����3000��,�ظ�������Ϣ
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: argument:modifiedTime or AccessTime is invalid");
        return FALSE;
    }


    //�����ļ�ʱ��   
    struct _utimbuf newtime;
    newtime.modtime = lastModifiedTime; 
    newtime.actime = lastAccessTime;   
    
    //�����ļ��޸ġ�����ʱ��
    if( -1 == _futime(fp,&newtime) )        
    {
        //�����ļ�ʱ��ʧ��
        _get_errno( &m_errno );

        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: set file time failed, [modtime:%d] ",
                            newtime.modtime );
        sysLoger(LWARNING, "TSFS_Worker::fioTimeSet: set file time failed, [actime:%d]",
                            newtime.actime );
        return FALSE;
    }

    //�����ļ�ʱ��ɹ�
    sysLoger(LINFO, "TSFS_Worker::fioTimeSet: set file time successful");

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ����Ƿ�Ϊ��Ŀ¼
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��<>;
 */
int TSFS_Worker::fioCheckRoot(char *pRecvData, Packet *sendPack)
{
    if( NULL == pRecvData || NULL == sendPack)
    {
        m_errno = EINVAL;
        sysLoger(LWARNING, "TSFS_Worker::fioCheckRoot: pRecvData OR sendPack is invalid");
        return FALSE;
    }

    int isRoot = 0;        //��Ŀ¼��ʶ,Ĭ��Ϊ�Ǹ�Ŀ¼

    char *pUserDir = pTSFS_Server->getUserDir();

    char *pstr = strstr(pUserDir, ":");
    if(!pstr)
    {
        pstr = pUserDir;
    }

    while( '\\' == *(++pstr))
    {
        //��ѭ��
    }

    if(NULL == *pstr)
    {
        //Ϊ��Ŀ¼
        isRoot = 1;
    }

    //����Ƿ��Ŀ¼�ɹ�
    if(isRoot)
    {
        sysLoger(LINFO, "TSFS_Worker::fioCheckRoot: check root successful, is root");
    }
    else
    {
        sysLoger(LINFO, "TSFS_Worker::fioCheckRoot: check root successful, is not root");
    }

    //��ջظ����ݰ�
    sendPack->setData("",0);

    //���������
    int size = sprintf(sendPack->getData(),"%s%s%x%s",
                    TSFS_SUCCESS,               //ok
                    TSFS_CONTROL_FLAG,          //;
                    isRoot,                     //��Ŀ¼��ʾ
                    TSFS_CONTROL_FLAG);         //;

    //���ûָ������С
    sendPack->setSize((US)size);

    return TRUE;
}

/*
 * @brief
 *    ��ȡ�ļ�ϵͳ��Ϣ������Ŀ����ظ�ִ�н��
 * @param *pRecvData[in]:    ������Ĳ�������
 * @return   �����ɹ����򷵻�TURE
             ����ʧ�ܣ��򷵻�FALSE
 * �����ʽ��fp;
*/
//int TSFS_Worker::fioFstatFsget(char *pRecvData, Packet *sendPack)
//{
//    if( NULL == pRecvData || NULL == sendPack)
//    {
//        m_errno = EINVAL;
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: pRecvData OR sendPack is invalid");
//        return FALSE;
//    }

//    //��������
//    int fp          = -1;
//    char *pData     = pRecvData;
//
//    //fp
//    if( !(strstr(pData,TSFS_CONTROL_FLAG)) )
//    {
//        //������������,�ظ���Ϣ
//        m_errno = EINVAL;
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: argument:file pointer is invalid");
//        return FALSE;
//    }
//    
//    //��ȡfp
//    hexToInt(&pData,&fp); 

//    sysLoger(LDEBUG, "TSFS_Worker::fioTimeSet: parse file pointer success, [fp:%d]", fp);
//
//    //���ļ������в����ļ�
//    if( !(pTSFS_Server->searchFileHandle(fp)) )        
//    {
//        //û���ҵ��ļ����ظ�����
//        m_errno = EBADF;   
//        sysLoger(LWARNING, "TSFS_Worker::fioFstatFsget: didn't find the file pointer,[fp:%d]",
//                            fp);
//        return FALSE;
//    }
//
//    //��ȡ�ļ�ϵͳ��Ϣ,���ܻ�δȷ��
//
//
//
//
//    //�ظ���ʽδ��
//        
//    return TRUE;
//}

/**
 * @brief
 *    �����������ر�����
 */
TSFS_Worker::~TSFS_Worker()
{ 
    //�ر�TS����
    TClient_close(m_lnkId);         
}

/*
 * @brief
 *    �����·��ת��Ϊ����·��
 * @param tempPath[in]:    ���·��
 * @param fullPath[out]:   ����·��
 * @return   �����ɹ����򷵻�TSFS_SUCC
             ����ʧ�ܣ��򷵻�TSFS_FAIL
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
        //ת��ʧ��
        sysLoger(LWARNING, "TSFS_Worker::tempPath2fullPach: change target path to full path failed, user:[%s], target:[%s]",
                        pTSFS_Server->getUserDir(), tempPath);
        return TSFS_FAIL;          
    }
    
    //ת���ɹ�
    return TSFS_SUCC; 
}

/*
 * @brief
 *   ��Windows������ת��ΪPixOS������
 * @param winErr[in]: Windows������
 * @param pixErr[out]: PixOS������
 */
void TSFS_Worker::winErr2posixErr(int winErr, int *pixErr)
{
    if(NULL == pixErr)
    {
        return;
    }

    switch(winErr)
    {
        case ENOTEMPTY:     //Ŀ¼�ǿ�
            *pixErr = 90;
            break;

        default:
            *pixErr = winErr;
    }
}

/*
 * @brief
 *   ��Ŀ������ļ���������ת��Ϊ�������ļ���������
 * @param taParam[in]: Ŀ������ļ���������
 * @param tsParam[out]: �������ļ���������
 */
int TSFS_Worker::taParam2tsParam(int taParam, int *tsParam)
{
    if(NULL == tsParam)
    {
        return TSFS_FAIL;
    }    

    //������windows flag����
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

    //Ŀ�����Acoreos flag����
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

    //ת������
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
        //�����д�
        return TSFS_FAIL;
    }

    *tsParam = tempParam;
    
    return TSFS_SUCC;
}
