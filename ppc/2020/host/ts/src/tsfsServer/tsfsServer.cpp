/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsServer.cpp
* @brief  
*       ���ܣ�
*       <li>ʵ��Server�������ļ����������հ�������������worker����</li>
*/

/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include <io.h>
#include "tsfscommon.h"
#include "tsfsServer.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/**
 * @brief
 *    ���캯�������TSFS_Server����ĳ�ʼ�����������������Server�����
 * @param linkId[in]: TSFS Server����������TS�����Ӿ��
 * @param tsIp[in]:  TS IP��ַ
 * @param path[in]: �û�����Ŀ¼
 */
TSFS_Server::TSFS_Server(US linkId, char *tsIp, char *path)
{
    memset(m_workerThreadPool, 0, sizeof(m_workerThreadPool));
    memset(m_packetPool, 0, sizeof(m_packetPool));
    memset(m_userDir, 0, sizeof(m_userDir));
    memset(m_tsIpAddr, 0, sizeof(m_tsIpAddr));

    memcpy(m_userDir, path, sizeof(m_userDir));     //�����û�����Ŀ¼
    memcpy(m_tsIpAddr, tsIp, sizeof(m_tsIpAddr));   //����TS IP��ַ
    m_lnkId             = linkId;                   //�������Ӿ��
    m_runFlag           = true;                     //����Server����
    m_curWorkerNum      = 0;                        //��δ����Worker 
    m_curPacketNum      = 0;                        //��δ����Packet
    
    //����Packet�ź���
    m_pPacketSemaphore  = new waitcondition(TSFS_PACKET_MAX_NUM);

    //����Worker�ź���
    m_pWorkerSemaphore  = new waitcondition(TSFS_WORKER_MAX_NUM);
}

/**
 * @brief
 *    �����������ͷ�Worker�̳߳ء��������������ռ�õ��ڴ�
 */
TSFS_Server::~TSFS_Server()
{
    //�ͷ�Worker�̳߳�
    for(int i=0; i < m_curWorkerNum; ++i)
    {
        delete m_workerThreadPool[i];
        m_workerThreadPool[i] = NULL;
    }

    //���Worker����
    m_freeWorkerList.clear();

    //�ͷ�Packet
    for(int i=0; i< m_curPacketNum; ++i)
    {
        delete m_packetPool[i];
        m_packetPool[i] = NULL;
    }

    //���Packet����
    m_emptyPacketList.clear();

    //����ļ��������
    list <int>::iterator iter;
    for ( iter = m_fileHandle.begin( ); iter != m_fileHandle.end( ); iter++ )
    {
        //�ر�δ�رյ��ļ�
        _close((*iter)); 
    }
    m_fileHandle.clear();

    //ɾ��Worker�ź���
    delete m_pWorkerSemaphore;

    //ɾ��Packet�ź���
    delete m_pPacketSemaphore;
}

/**
 * @brief
 *   �����������ݣ������ͷ���Worker��ת����Worker����
 */
void TSFS_Server::main()
{
    int ret = 0;
    Packet *packet = NULL;
    TSFS_Worker *worker = NULL;

    while(bRuning())
    {
        //��������
        ret = recvPacket(&packet);

        if(TSFS_INVALID_PACKET == ret)
        {
            //���ݰ������쳣,�����հ�
            continue;
        }
        else if( TSFS_LINK_ClOSED == ret)
        {
            //���ӶϿ�,�˳�
            break;
        }        

        //��ȡWorker
        worker = getFreeWorker();

        //��������
        processData(packet, worker);
    }
}

/*
 * @brief
 *   �������մ�������
 * @param packet[out]: ������������ݰ�
 * @return���յ����ݰ����򷵻ؽ��յ����ݰ��ֽڴ�С
 */
int TSFS_Server::recvData(Packet *packet)
{
    /*Modified by tangxp for BUG NO.0002964 on 2008.3.19 [begin]*/
    /*  Modified brief: ������־��¼*/
    if(NULL == packet)
    {
        sysLoger(LDEBUG, "TSFS_Server::recvData: packet is invalid" );

        return TSFS_INVALID_PACKET;
    }

    int dataSize = -1;
    US src_aid = 0; 
    US des_said = 0;
    US src_said = 0;

    sysLoger(LDEBUG, "TSFS_Server::recvData: wait for data" );
    
    //ѭ�����մ�������
    while( m_runFlag )
    {  
        //�������մ�������
        dataSize = TClient_getpkt(m_lnkId,
                                    &src_aid,
                                    &des_said,
                                    &src_said,
                                    packet->getData(),
                                    packet->getDataSize());

        if( dataSize > 0 )
        {     
            //���յ��������󣬱���Ŀ���AID
            packet->setSrc_aid( src_aid );

            //����Ŀ���SAID
            packet->setSrc_said( src_said );

            //���洦���������ݴ�С
            packet->setSize((US)dataSize);

            sysLoger(LINFO, "TSFS_Server::recvData: receive a packet, [aid:%d], [said:%d], [size:%d]",
                            src_aid, src_said, dataSize);

            sysLoger(LDEBUG, "TSFS_Server::recvData: receive a packet, data:[%c]",
                            packet->getData()[0]);

            return dataSize;
        }
        else if( TS_SOCKET_ERROR == dataSize 
                 || TS_INVALID_LINK_HANDLE == dataSize )
        {
            //�����ѹر�
            sysLoger(LWARNING, "TSFS_Server::recvData: link closed, [ret:%d]",
                                dataSize);
            return TSFS_LINK_ClOSED;
        }
    }
    /*Modified by tangxp for BUG NO.0002964 on 2008.3.19 [end]*/    
}

/*
 * @brief
 *   ��ȡ����Worker��
 * @return������Worker
 */
TSFS_Worker* TSFS_Server::getFreeWorker()
{     
    TSFS_Worker *freeWorker  = NULL;

    sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: begin get free worker" );

    while(1)
    {
        //��û�п���Worker,��û�дﵽWorker��������,���½�Worker
        if( m_freeWorkerList.empty() && m_curWorkerNum < TSFS_WORKER_MAX_NUM )
        {
            //��û���½���Worker,������½��ɹ�
            do
            {
                //�½�һ��Worker
                freeWorker = new TSFS_Worker( TSFS_WORKER_QUEUE_SIZE );
                if( freeWorker )
                {
                    //�½�Worker�ɹ�
                    m_workerThreadPool[m_curWorkerNum] = freeWorker;

                    //��¼Worker����
                    ++m_curWorkerNum;

                    sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: create new worker success, [ThreadID = %d], [worker sum:%d]",
                                freeWorker->getThreadID(), m_curWorkerNum);
                    return freeWorker;
                }

                sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: create new worker failed, [worker sum:%d]",
                                m_curWorkerNum);
            }while( 0 == m_curWorkerNum );
        }

        //�ӿ���Worker������ȡWorker
        if( m_pWorkerSemaphore->wait() )
        {
            m_workerMutex.lock();

            freeWorker = m_freeWorkerList.front();

            m_freeWorkerList.pop_front();

            m_workerMutex.unlock();

            sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: get free worker success, [ThreadID = %d]",
                                freeWorker->getThreadID());
            return freeWorker;
        }

        sysLoger(LDEBUG, "TSFS_Server::getFreeWorker: wait for free worker failed");
    }    
}        

/**
 * @brief
 *   �����ļ��ľ�������������
 * @param fp[in]: �ļ����
 */
void TSFS_Server::insertFileHandle(int fp)
{
    m_fpMutex.lock();

    m_fileHandle.push_back(fp);

    m_fpMutex.unlock();
}

/**
 * @brief
 *   �ӹ���������ɾ���Ѿ��رյ��ļ����
 * @param fp[in]:   �ļ����
 */
void TSFS_Server::removeFileHandle(int fp)
{
    m_fpMutex.lock();

    m_fileHandle.remove(fp);

    m_fpMutex.unlock();
}

/**
 * @brief
 *   �ھ�����������в����ļ����
 * @param fp[in]:   ����ѯ���ļ����
 * @return�����ҳɹ����򷵻�TSFS_SUCC
 *          ����ʧ�ܣ��򷵻�TSFS_FAIL
 */
int TSFS_Server::searchFileHandle(int fp)
{
    list<int>::iterator iter;

    //�ھ�����������в����ļ����
    for(iter=m_fileHandle.begin(); iter!=m_fileHandle.end(); iter++)
    {
        //�����ҳɹ�������TSFS_SUCC
        if( (*iter) == fp )
        {
            return TSFS_SUCC;
        }
    }

    //�������������û������ѯ���ļ����������TSFS_FAIL
    return TSFS_FAIL;
}

/*
 * @brief
 *   �˳�Server
 */
void TSFS_Server::stop()
{
    //����ֹͣ����
    m_runFlag = false;
    
    //�ͷ�Worker�̳߳�
    for(int i=0;i < m_curWorkerNum ;i++)
    {
        m_workerThreadPool[i]->stopThread();
    }
}

/**
 * @brief
 *   ��ȡ�û�����·��
 * @return���û�����·��

 */
char* TSFS_Server::getUserDir()
{
    return this->m_userDir;
}

/**
 * @brief
 *   ��ȡTS IP��ַ
 * @return��TS IP��ַ
 */
char* TSFS_Server::getTsIPAddr()
{
    return this->m_tsIpAddr;
}

/*
 * @brief
 *   ��ȡһ��Pakcet
 * @return��Pakcet
 */
Packet* TSFS_Server::getPacket()
{
    Packet *packet = NULL;

    sysLoger(LDEBUG, "TSFS_Server::getPacket: begin get empty packet" );

    while(1)
    {
        //��û�п�Pakcet��Packetû�дﵽ��������,���½�
        if( m_emptyPacketList.empty() && m_curPacketNum < TSFS_PACKET_MAX_NUM )
        {  
            //����û��Packet,������½��ɹ�
            do
            {
                //�½�һ��Pakcet
                packet = new Packet;
                if( packet )
                {
                    //�½�Packet�ɹ�
                    m_packetPool[m_curPacketNum] = packet;

                    //��¼Packet����
                    ++m_curPacketNum;

                    sysLoger(LDEBUG, "TSFS_Server::getPacket: create new packet success, [packet sum:%d]",
                                    m_curPacketNum);
                    return packet;
                }

                sysLoger(LDEBUG, "TSFS_Server::getPacket: create new packet failed, [packet sum:%d]",
                                    m_curPacketNum);
            }while( 0 == m_curPacketNum );
        }

        //�ӿ�Pakcet������ȡPacket
        if( m_pPacketSemaphore->wait() )
        {
            m_packMutex.lock();

            packet = m_emptyPacketList.front();

            m_emptyPacketList.pop_front();

            m_packMutex.unlock();

            sysLoger(LDEBUG, "TSFS_Server::getPacket: get empty packet success" );
            return packet;
        }

        sysLoger(LDEBUG, "TSFS_Server::getPacket: wait for empty packet failed" );
    }
}

/*
 * @brief
 *   ���մ�������
 * @param packet[out]: ������������ݰ�
 * @return�����ճɹ�,�ظ����������ֽڴ�С
            ���ӶϿ�,�ظ�TSFS_SERVER_EXIT
 */
int TSFS_Server::recvPacket(Packet** packet)
{
    //��ȡһ��Packet
    *packet = getPacket();

    //�����������ݰ�
    int ret = recvData(*packet);
    if( ret < 0 )
    {
        //�������ݳ����쳣,����Packet
        recyclePacket(*packet);
    }
    
    return ret;
}

/*
 * @brief
 *   ��Worker��������
 * @param packet[in]: �������ݰ�
 * @param worker[in]: ���������Worker
 */
void TSFS_Server::processData(Packet *packet, TSFS_Worker *worker)
{
    if(NULL != packet && NULL != worker)
    {
        //���������ݰ�����Worker����
        worker->appendMsg(packet);
    }
}


/*
 * @brief
 *   ��ȡServer����״̬��־
 * @return��Server����״̬��־
 */
bool TSFS_Server::bRuning()
{
    return this->m_runFlag;
}

/**
 * @brief
 *   ��ʹ�����Packet�����Packet����
 * @param pack[in]:   ʹ����ϵ�packet
 */
void TSFS_Server::recyclePacket(Packet *pack)
{
    if(NULL != pack)
    {
        //���Packet
        memset(pack, 0, sizeof(Packet));

        m_packMutex.lock();

        //��ʹ�����Packet�����Packet����
        m_emptyPacketList.push_back(pack);

        sysLoger(LDEBUG, "TSFS_Server::recyclePacket: recycle packet success");
    
        //�ͷ��ź���
        m_pPacketSemaphore->wakeup();
        
        m_packMutex.unlock();
    }
}

/**
 * @brief
 *   ������Worker�������Worker����
 * @param worker[in]:   ����Worker
 */
void TSFS_Server::recycleWorker(TSFS_Worker *worker)
{
    if(NULL != worker)
    {
        m_workerMutex.lock();

        //��ʹ�����Packet�����Packet����
        m_freeWorkerList.push_back(worker);

        sysLoger(LDEBUG, "TSFS_Server::recycleWorker: recycle worker success, [ThreadID = %d]",
                        worker->getThreadID() );
    
        //�ͷ��ź���
        m_pWorkerSemaphore->wakeup();
        
        m_workerMutex.unlock();
    }
}

