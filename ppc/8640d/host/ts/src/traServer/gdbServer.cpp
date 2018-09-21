/*
* ������ʷ��
* 2005-02-6 ������  ���������������޹�˾
*                   �������ļ���
*/

/**
*
* @file     gdbServer.cpp
* @brief
*      ���ܣ�
*      <li>����ת��GDB��TA����������</li>
*
*/

/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include <stdlib.h>
#include "gdbServer.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/

T_MODULE T_CONST T_WORD INT2HEX_SIZE = 10;
T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";

const int MAX_SEMAPHORE_COUNT = 1000;   //���������ֵ
const char hexchars[]="0123456789abcdef";
/************************�ⲿ����******************************/
/************************ǰ������******************************/


/************************ʵ��*********************************/


GdbServer::GdbServer(short id)
{
    this->selfId = id;
    sClient = NULL;
    bufSemaphore = NULL;
    m_hExitEvent = NULL;
    pOutBuf = NULL;
    m_enableSet = TRUE;
    SetMop(FALSE);
#ifdef TRA_DEBUG
    SetLog(TRUE);
#endif
    this->SetIsProcessingGdb(FALSE);

    m_resetFlag = TRUE;
}

GdbServer::~GdbServer(void)
{
    //�˳�ǰ����ڴ�
    RefreshMemory();
}

/**
* ���ܣ��رտͻ��˵�����,
*/
void GdbServer::closeClient()
{
    TMutexLocker lock(&clientMutex);

    if( NULL != sClient)
    {
        sClient->Close();
        delete sClient;
        sClient = NULL;
    }
}

/*
* @brief
*    ��һ�����͸��ݿ��תΪ16�����ַ���\n
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] wNum    �������ṩ������
* @param[in] wWidth  �������ṩ���ַ������
* @return ���
*/
T_MODULE T_WORD MemInt2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth)
{
    T_WORD i;

    for (i = wWidth - 1; i >= 0; i--)
    {
        cpBuf[i] = "0123456789abcdef"[(wNum & 0xf)];
        wNum >>= 4;
    }

    return wWidth;
}



int GdbServer::open(int lport)
{
    //���ݲ���ȷ���Ƿ���ڴ��Ż��㷨
    if(des_said == 1)
    {
        //SetMop(TRUE);  //�ڴ��Ż��㷨һ��ֻΪTA ���ڵ��Դ�
    }
    else
    {
        SetMop(FALSE);
    }

    m_runFlag = false;

    if(lport < 0 || lport > 65536)
    {
        return FALSE;
    }

    this->port = lport;

    if(serverSocket.m_sock == INVALID_SOCKET)
    {
        if(serverSocket.createServerSocket(port,1)) //��ַ����
        {
            bufSemaphore = CreateSemaphore(NULL,0,MAX_SEMAPHORE_COUNT,NULL);

            if (bufSemaphore == NULL)
            {
                return FAIL;
            }

            DWORD dwThreadID;

            if (NULL == (m_hReadThread = CreateThread (NULL, 0 , handlerThread, (LPVOID)this,
                                         CREATE_SUSPENDED, &dwThreadID)))
            {
                CloseHandle(bufSemaphore);
                return FAIL;
            }

            //����һ���¼�����,�����˳���̬��
            m_hExitEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);

            m_runFlag = true;

            ResumeThread(m_hReadThread);

            if (lport == 0)
            {
                serverSocket.GetLocalPort();
            }

            if (pOutBuf != NULL)
            {
                Int2hex(pOutBuf,10,serverSocket.GetLocalPort());    //�����˿ں�
            }

#ifdef TRA_DEBUG
            QString format = "dd.hh";
            QString time = QDateTime::currentDateTime().toString(format);
            QString head = "./log/tra";
            QString tail = ".log";
            QString strPort = QString("%1").arg(serverSocket.GetLocalPort());
            QString name = head.append(strPort).append(".").append(time).append(tail);
            m_logger.open(name.data().ascii());
#endif

            return SUCC;
        }

        else
        {
            return FAIL;
        }
    }

    return SUCC;
}

int GdbServer::close()
{
    m_runFlag = false;

    if (bufSemaphore != NULL)
    {
        ReleaseSemaphore(bufSemaphore,MAX_SEMAPHORE_COUNT,NULL);    //�������е��ź���
    }

    //��Ҫ�Ǳ�֤����̵߳õ��л�,���ַ�ʽֻ��Ϊ�˽������,Ҫ���׽��Ӧ������ͬ������

    if (m_hExitEvent != NULL)
    {
        DWORD stat = WaitForSingleObject(m_hExitEvent, 2000);

        if(stat == WAIT_OBJECT_0)
        {

        }
        else if(stat == WAIT_TIMEOUT)
        {

        }

        CloseHandle( m_hExitEvent );
        m_hExitEvent = NULL;
    }

    clientMutex.lock();

    if(sClient != NULL)
    {
        sClient->Close();   //�رտͻ��˵�����
        /* FIXME */
        delete sClient;
        sClient = NULL;
    }

    clientMutex.unlock();
    serverSocket.Close();

    //�˳��߳�
    if (m_hReadThread != NULL)
    {
        WaitForSingleObject(m_hReadThread,5000);
        CloseHandle(m_hReadThread);
        m_hReadThread = NULL;
    }

    if (bufSemaphore != NULL)
    {
        CloseHandle(bufSemaphore);  //�������е��ź���
        bufSemaphore = NULL;
    }

    //ɾ�����е��������
    clearList();
#ifdef TRA_DEBUG
    m_logger.close();
#endif
    return SUCC;
}



T_VOID GdbServer::HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize)
{
    //����������Ϸ���
    if(wSize < 4 || cpBuf[0] != 'r' || cpBuf[1] != 's' || cpBuf[2] != 'p' || cpBuf[3] != '-')
    {
        SendIDEResult("error:input unkown cmd!", strlen("error:input unkown cmd!"), hwSrcAid, hwSrcSaid);
        return;
    }

    switch(cpBuf[4])
    {
    case 's':
    {
        //�л�TRA-DLL��Ӧ�ĵ��Զ��������ID��
        char *ptr = &cpBuf[6];
        int subID = 0;

        if(Hex2int(&ptr, &subID))
        {
            m_aidMutex.lock();
            des_said = subID;
            m_aidMutex.unlock();
            SendIDEResult("OK",strlen("OK"),hwSrcAid, hwSrcSaid);
        }

        else
        {
            SendIDEResult("E01",strlen("E01"),hwSrcAid, hwSrcSaid);
        }

        break;
    }
    case 'a':
    {
        //�л�TRA-DLL��Ӧ�ĵ���һ�������ID��
        char *ptr = &cpBuf[6];
        int aId = 0;

        if(Hex2int(&ptr, &aId))
        {
            m_aidMutex.lock();
            des_aid = aId;
            m_aidMutex.unlock();
            SendIDEResult("OK",strlen("OK"),hwSrcAid, hwSrcSaid);
        }

        else
        {
            SendIDEResult("E01",strlen("E01"),hwSrcAid, hwSrcSaid);

        }

        break;
    }
    case 'l':
    {
        //��־ѡ�����ã�Ĭ�Ϲر�
        if(wSize < 10 || cpBuf[4] != 'l' || cpBuf[5] != 'o' || cpBuf[6] != 'g' || cpBuf[7] != '-')
        {
            SendIDEResult("����δ֪����־�������\n", strlen("����δ֪����־�������\n"), hwSrcAid, hwSrcSaid);
            break;
        }

        if(cpBuf[9] == 'n')
        {
            SendIDEResult("��־�򿪳ɹ���\n", strlen("��־�򿪳ɹ���\n"), hwSrcAid, hwSrcSaid);
        }

        else if(cpBuf[9] == 'f')
        {
            SendIDEResult("��־�رճɹ���\n", strlen("��־�رճɹ���\n"), hwSrcAid, hwSrcSaid);
        }

        else
        {
            SendIDEResult("����δ֪����־�������\n", strlen("����δ֪����־�������\n"), hwSrcAid, hwSrcSaid);
        }

        break;
    }
    case 'm':
    {
        //�ڴ��Ż�ѡ���㷨���ã�Ĭ�ϴ�
        if(wSize < 10 || cpBuf[4] != 'm' || cpBuf[5] != 'o' || cpBuf[6] != 'a' || cpBuf[7] != '-')
        {
            SendIDEResult("����δ֪���ڴ��Ż��������\n", strlen("����δ֪���ڴ��Ż��������\n"), hwSrcAid, hwSrcSaid);
            break;
        }

        if(cpBuf[9] == 'n')
        {
            //���ڴ��Ż��㷨
            T_WORD ret = SetMop(TRUE);

            if(SUCC == ret)
            {
                SendIDEResult("�ڴ��Ż��㷨�򿪳ɹ���\n", strlen("�ڴ��Ż��㷨�򿪳ɹ���\n"), hwSrcAid, hwSrcSaid);
            }

            else
            {
                SendIDEResult("�ڴ��Ż��㷨��ʧ�ܣ�\n", strlen("�ڴ��Ż��㷨��ʧ�ܣ�\n"), hwSrcAid, hwSrcSaid);
            }

        }

        else if(cpBuf[9] == 'f')
        {
            //�ر��ڴ��Ż��㷨
            T_WORD ret = SetMop(FALSE);

            if(SUCC == ret)
            {
                SendIDEResult("�ڴ��Ż��㷨�رճɹ���\n", strlen("�ڴ��Ż��㷨�رճɹ���\n"), hwSrcAid, hwSrcSaid);
            }

            else
            {
                SendIDEResult("�ڴ��Ż��㷨�ر�ʧ�ܣ�\n", strlen("�ڴ��Ż��㷨�ر�ʧ�ܣ�\n"), hwSrcAid, hwSrcSaid);
            }
        }

        else
        {
            SendIDEResult("����δ֪���ڴ��Ż��������\n", strlen("����δ֪���ڴ��Ż��������\n"), hwSrcAid, hwSrcSaid);
        }

        break;
    }
    default:
    {
        SendIDEResult("����δ֪�Ĳ������\n", strlen("����δ֪�Ĳ������\n"), hwSrcAid, hwSrcSaid);
        break;
    }
    }

}

T_VOID GdbServer::SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid)
{
    //�������һ���µĻ����
    RecvPacket *pack = new RecvPacket();

    if(pack == NULL)
    {
        return ;
    }

    T_CHAR *pBuf = pack->data;

    memcpy(pBuf, cpBuf, wSize);
    pBuf += wSize;


    pack->size = pBuf - pack->data;

    pack->m_desAid = hwSrcAid;
    pack->m_desSaid = hwSrcSaid;
    pack->m_srcSaid = 1;

    //�����ɣ����뵽������ȥ
    insertList(pack);
}

int GdbServer::putpkt(short des_said, short src_aid, short src_said, char* pBuf, int size)
{

    if(size < 0 || !m_runFlag)
    {
        return FAIL;
    }

    if(sClient == NULL || pBuf == NULL)
    {
        return FAIL;
    }

    if(des_said == 1)
    {
        HandleIDECmd(src_aid, src_said, pBuf, size);

        return SUCC;
    }

    if(GetReset() == TRUE)
    {
        //���Ե���'r'����Ļظ����
        SetReset(FALSE);
        return TRUE;
    }

    //��������ǰ�ȼ��TA�Ļظ��ǲ�����Զ��ڴ�����Ļظ�
    if(m_lastCmd.getValid() == TRUE)
    {
        UpdateMemory(pBuf, size);
        T_WORD ret = GetLocalMemory(m_lastCmd.gdbAddr, m_lastCmd.gdbSize, pBuf);

        if(ret > 0)
        {

            m_lastCmd.setValid(FALSE);
            PutData(pBuf, ret);
        }


    }

    else
    {
        //�Ƕ���������Ļظ���ֱ�ӷ���GDB
        PutData(pBuf, size);
    }

    return SUCC;
}


//�����ڴ滺����
T_VOID GdbServer::UpdateMemory(T_CHAR *cpBuf, T_WORD size)
{
    //���ظ��Ƿ��Ǹ����ڴ������
    if(m_lastCmd.traSize == size)
    {
        //���»���

        //�����ڴ�
        assert(NULL != cpBuf);

        //FIXME:û��������У��

        //�����ڴ��ĸ���
        T_WORD blockNum = m_lastCmd.traSize / RSP_Memory::MEMORY_BLOCK_SIZE;
        T_WORD addr = m_lastCmd.traAddr;

        T_CHAR *ptr = &cpBuf[0];


        while(blockNum)
        {
            //����Ѿ��и��ڴ�飬���ø���
            if(m_addr2mem.contains(addr))
            {
                blockNum--;
                continue;
            }

            RSP_Memory *memory = memMan.Alloc();

            if(memory == NULL)
            {
                //�ͷ������ڴ�
                RefreshMemory();
                memory = memMan.Alloc();
            }

            memory->SetBaseAddr(addr);
            memcpy(memory->GetValue(), ptr, RSP_Memory::MEMORY_BLOCK_SIZE);

            ptr += RSP_Memory::MEMORY_BLOCK_SIZE;
            m_addr2mem.insert(addr, memory);
            blockNum--;
            addr += RSP_Memory::MEMORY_BLOCK_SIZE;
        }

    }

}

int GdbServer::getpkt(short *des_aid, short *des_said, short *src_said, char *pBuf, int size)
{
    if (bufSemaphore != NULL && m_runFlag)
    {
        DWORD dw = WaitForSingleObject(bufSemaphore,INFINITE);  //�ȴ��ź���,���յ�gdb���ݰ�

        RecvPacket* pRecvPacket = getPacketFromList();

        if (m_runFlag  && pRecvPacket != NULL)
        {

            int recSize = size;

            if (size > PACKET_SIZE)
            {
                recSize = PACKET_SIZE;
            }

            if(pRecvPacket->m_desAid == -1)
            {
                m_aidMutex.lock();
                *des_aid = this->des_aid;
                *des_said = this->des_said;
                *src_said = this->src_said;
                m_aidMutex.unlock();
            }

            else
            {
                *des_aid = pRecvPacket->m_desAid;
                *des_said = pRecvPacket->m_desSaid;
                *src_said = pRecvPacket->m_srcSaid;

            }

            int revcSize = pRecvPacket->size;

            if (revcSize > size)
            {
                revcSize = size;
            }

            memcpy(pBuf, pRecvPacket->data, revcSize);  //����������
            delete pRecvPacket;     //�������ռ�
            pRecvPacket = NULL;
            return revcSize;
        }

        else
        {
            if (pRecvPacket != NULL)
            {
                delete pRecvPacket;     //�������ռ�
                pRecvPacket = NULL;
            }

            if (m_hExitEvent != NULL)
            {
                SetEvent (m_hExitEvent);
            }

            return -1;
        }
    }

    else
    {
        return -1;
    }
}

/*
* @brief
*    ���ظ����ݷ��͸�GDB\n
* @param     cpBuf    ������Ҫ���͵����ݻ���
* @param[in] size    ������Ҫ�������ݵĴ�С
* @return  ʵ�ʷ��͵����ݵĴ�С
*/
T_WORD GdbServer::PutData(T_CHAR *cpBuf, T_WORD wSize)
{
    TMutexLocker lock(&clientMutex);    //�Կͻ��˽���ͬ������

    if(wSize == 64)
    {
        int a = 0;
    }

    T_WORD i = 0;
    T_UBYTE *sendBuf;
    T_UBYTE checkSum = 0;
    T_WORD  count = wSize;
    T_WORD result = 0;

    if(sClient == NULL || cpBuf == NULL)
    {
        return FAIL;
    }

    //��־
#ifdef TRA_DEBUG

    if(IsLog() == TRUE)
    {
        m_logger.write(SEND_PACK, cpBuf, wSize);
    }

#endif

    //���䷢�ͻ�����
    sendBuf = new T_UBYTE[wSize+10];

    if(sendBuf == NULL)
    {
        return FAIL;
    }

    memset(sendBuf, 0, wSize + 10);
    //�����ͷ
    sendBuf[0] = '$';

    //��У���
    while(count--)
    {
        checkSum += static_cast<int>(cpBuf[i]);
        sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
        i++;
    }

    //�����β
    sendBuf[wSize+1] = '#';
    sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
    sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
    //��������
    result = sClient->Send(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);

    delete []sendBuf;
    sendBuf = NULL;
    return result;

}

/*
* @brief
*    �����ڷ���������GDB�����ݣ��յ����ݺ�\n
*    �����ݿ������������ṩ�Ļ������󷵻�\n
* @param     cpBuf    �������ṩ�����ݻ���
* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
* @return  ʵ�ʽ��յ����ݵĴ�С������Ͽ�����-1
*/
T_WORD GdbServer::GetData(T_CHAR *cpBuf, T_WORD wSize)
{
    //���SOCKET�Ϸ���
    if (NULL == sClient || NULL == cpBuf)
    {
        return FAIL;
    }

    //��������GDB����
    T_CHAR head;
    T_WORD offset = 0;

    while(TRUE)
    {
        offset = 0;
        T_WORD ret = sClient->Recv(&head, 1);

        //�������Ͽ����˳�ѭ��
        if(ret < 0)
        {
            return FAIL;
        }

        //������յ�ͷ�ַ�����$�����������
        if('$'!= head)
        {
            continue;
        }

        //��������ֱ��#
        ret = sClient->Recv(&head, 1);

        while((offset < wSize) && (FAIL != ret))
        {
            if('#' == head)
            {
                break;
            }

            cpBuf[offset++] = head;
            ret = sClient->Recv(&head, 1);
        }

        if(ret < 0)
        {
            return FAIL;
        }

        if(offset > wSize)
        {
            continue;
        }

        //Ӧ����Ϊ��TCP���գ���û�н���У��
        sClient->Send("+", strlen("+"));

        return offset;
    }
}

T_WORD GdbServer::SetReset(T_BOOL isReset)
{
    m_resetMutex.lock();

    this->m_resetFlag = isReset;

    m_resetMutex.unlock();

    return SUCC;
}

T_BOOL GdbServer::GetReset()
{
    TMutexLocker lock(&m_resetMutex);

    return this->m_resetFlag;
}


DWORD GdbServer::handlerThread(LPVOID lpvoid)
{
    GdbServer * pGdbServer = reinterpret_cast<GdbServer* >(lpvoid);

    if (NULL == pGdbServer)
    {
        return 0;
    }


    while (pGdbServer->m_runFlag)
    {
        //��ջ���
        pGdbServer->RefreshMemory();

        pGdbServer->sClient = new WSocket;

        if(pGdbServer->sClient == NULL )
        {
            return 0;
        }

        //�����˿�
        if(!pGdbServer->serverSocket.acceptSocket(*pGdbServer->sClient))
        {
            pGdbServer->closeClient();
            return 0;
        }

        //-------------------------------------------------------------------------
        //2006.06.29 by guojc ��GDB������TRA��������������Ŀ�����DA���͸�λ����
        //RESET m_resetFlag
        pGdbServer->SetReset(TRUE);

        //֪ͨDA���г�ʼ������
        RecvPacket *pa = new RecvPacket();

        if(NULL == pa )
        {
            return 0;
        }

        pa->size = strlen("r");
        memcpy(pa->data, "r", strlen("r"));
        pGdbServer->MessageHandler(pa);
        //-------------------------------------------------------------------------

        while(pGdbServer->m_runFlag)
        {
            pGdbServer->m_enableSet = FALSE;
            //���ӳɹ�
            //��ʼ��������
            RecvPacket *pack = new RecvPacket();

            if(NULL == pack )
            {
                return 0;
            }

            memset(pack->data, 0, 20);

            pGdbServer->SetIsProcessingGdb(FALSE);

            T_WORD ret = pGdbServer->GetData(pack->data, PACKET_SIZE);

            pGdbServer->SetIsProcessingGdb(TRUE);

            if(ret == FAIL)
            {
                //����Ͽ�
                pGdbServer->closeClient();
                pGdbServer->m_enableSet = TRUE;

                /*Modified by tangxp for BUG NO. on 2008.2.28 [begin]*/
                /*  Modified brief: �˳�ʱ�ͷ��ڴ�*/
                delete pack;
                pack = NULL;

                /*Modified by tangxp for BUG NO. on 2008.2.28 [end]*/
                break;
            }

#ifdef TRA_DEBUG

            if(pGdbServer->IsLog() == TRUE)
            {
                pGdbServer->m_logger.write(RECV_PACK, pack->data, ret);
            }

#endif

            if(0 == ret)
            {
                //�յ��հ�
                /*Modified by tangxp for BUG NO. on 2008.2.28 [begin]*/
                /*  Modified brief: �˳�ʱ�ͷ��ڴ�*/
                delete pack;
                pack = NULL;
                /*Modified by tangxp for BUG NO. on 2008.2.28 [end]*/
                continue;
            }

            pack->size = ret;

            //�������ݰ�
            pGdbServer->MessageHandler(pack);
        }

        pGdbServer->clientMutex.lock();

        if (NULL != pGdbServer->sClient)
        {
            delete pGdbServer->sClient;
            pGdbServer->sClient = NULL;
        }

        pGdbServer->clientMutex.unlock();

    }


    return 1;
}

/*
* @brief
*    ������GDB�����ݽ���Э�������װ����������\n
* @param[in] cpBuf    �������ṩ�����ݻ���
* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
* @return
*/
T_WORD GdbServer::MessageHandler(RecvPacket *tPack)
{

    //���巢�ͻ���
    // T_CHAR sendBuf[PACKET_SIZE];
    //ֱ�ӽ����ݰ����͵�DA
    insertList(tPack);
    return 0;
}

/*
* @brief
*    �ӱ����ڴ滺���л�ȡ��Ҫ���ڴ���Ϣ�����û�У��÷�����ѹ���������д�Ŀ����ж�ȡ��Ӧ���ڴ��\n
* @param[in] wAddr   �ڴ��ַ
* @param[in] wSize   �ڴ��С
* @param[in] cpBuf   �����߸����Ļ�����
* @return  ���������������ڴ����Ĵ�С��0Ϊ�ޱ��ػ���, -1ΪDA���ڻظ������У������GDB���ظ�����
*/
T_WORD GdbServer::GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf)
{
    //�������ַ,��ӳ����в��ұ��ػ����Ƿ���ڸ��ڴ��
    T_WORD baseAddr = wAddr & RSP_Memory::MEMORY_BLOCK_MASK;
    T_WORD offset = wAddr - baseAddr;
    T_WORD leftSize = wSize;
    T_CHAR *pLocalBuf = cpBuf;



    //��������Ҫ�ڴ�����Ŀ
    T_WORD computeSize = (wSize + RSP_Memory::MEMORY_BLOCK_SIZE - 1 + offset) / RSP_Memory::MEMORY_BLOCK_SIZE;
    T_WORD count = 0;

    count = computeSize;
    //��鱾���Ƿ����㹻���ڴ��
    T_WORD validAddr = baseAddr;  //�ڴ��ַ
    T_WORD i = count;

    while(i != 0)
    {
        if(!m_addr2mem.contains(validAddr))
        {
            break;
        }

        validAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
        i--;
    }

    //����������㹻���ڴ滺��,���ڴ���Ϣ��������������
    if(i == 0)
    {
        T_WORD index = offset; //�����ڴ���������
        T_WORD localSize = 0;

        while(count)
        {
            localSize = wSize < RSP_Memory::MEMORY_BLOCK_SIZE ? wSize : RSP_Memory::MEMORY_BLOCK_SIZE;

            if((wSize + index) < RSP_Memory::MEMORY_BLOCK_SIZE)
            {
                localSize = wSize;
            }

            else
            {
                localSize = RSP_Memory::MEMORY_BLOCK_SIZE - index;
            }

            //ȡ��һ���ڴ��
            RSP_Memory *pMem = m_addr2mem.value(baseAddr);

            for(T_WORD j = index; j < (localSize + index); j++)
            {
                //gdb��ʽ�ڴ�
                pLocalBuf += MemInt2Hex(pLocalBuf, static_cast<int>(*(pMem->GetValue() + j)), 2);

            }

            baseAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
            index = 0;

            wSize -= localSize;
            count--;
        }



        return pLocalBuf - cpBuf;//���ط������ݵĳ���
    }

    else
    {
        //-------------------------------------------------------------------------------------------
        //2006.10.30 �ڴ��Ż��㷨����һ�������⣬��Ҫ����Ϊ��û���㹻�Ļ�����ʱ�򣬻����refreshMemory����
        //�п��ܻ��ͷŵ��Ѿ����ڵģ������ڴ˴ε�����������Ҫ���ڴ�飬�������޷����ظ�GDB�㹻���ڴ����ݶ�ʧ��
        //����������Ϊ�ڷ�������֮ǰ�ȼ���Ƿ����㹻���ڴ�飬���û�У����ͷź�������֯�·�����
        if(i > memMan.LeftPackets())
        {
            //�����Ҫ����Ļ�����������ڿ��Է���ĸ����������㻺����
            this->RefreshMemory();
            //��Ϊˢ�£�����validAddr����ӻ���ַ��ʼ����
            validAddr = baseAddr;
            i = count;
        }

        T_WORD memSize = i * RSP_Memory::MEMORY_BLOCK_SIZE;

        //����û�л��棬����Ƿ����һ�������ظ�
        if(m_lastCmd.getValid() == TRUE && m_lastCmd.gdbAddr == wAddr && m_lastCmd.gdbSize == wSize)
        {
            return -1;
        }


        //��¼���ĵ�ַ�������յ��ظ�ʱ���±��ػ���
        m_lastCmd.traAddr = validAddr;
        m_lastCmd.traSize = memSize;

        //��DA���Ͷ�ȡ�ڴ�����
        ReadMemory(validAddr, memSize);

        return SUCC;
    }




}

void GdbServer::ReadMemory(T_WORD wAddr, T_WORD wSize)
{
    //��װ���ڴ滺�����ݰ�
    //�������һ���µĻ����
    RecvPacket *pack = new RecvPacket();

    if(pack == NULL)
    {
        return ;
    }

    T_CHAR *pBuf = pack->data;

    //��װЭ�黺�壬��ʽmaddr,size
    memcpy(pBuf, "x", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,",",strlen(","));   //���ӿ��Ʒ���
    pBuf += strlen(",");

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize); //��С

    pack->size = pBuf - pack->data;
    //�����ɣ����뵽������ȥ
    insertList(pack);
}

/*
* @brief
*    ��ձ����ڴ滺��,
*/
T_VOID GdbServer::RefreshMemory()
{
    TMutex refreshMutex;
    TMutexLocker lock(&refreshMutex);


    T_WORD size = m_addr2mem.size();

    if(size == 0)
    {
        return;
    }

    QList<T_WORD> keys;

    QMapIterator<T_WORD, RSP_Memory*> i(m_addr2mem);

    while (i.hasNext())
    {
        i.next();
        keys.push_back(i.key());
    }

    for(int j = 0; j < size; j++)
    {
        RSP_Memory *mem = m_addr2mem.take(keys.value(j));

        if(NULL != mem)
        {
            memMan.Free(mem);
        }
    }
}
/*
* ���� �����������������
*/
void GdbServer::clearList()
{
    TMutexLocker lock(&recvListMutex);
    RecvPacket* pRecvPacket = NULL;

    while(recvBufList.size()>0)
    {
        list<RecvPacket*>::iterator iter = recvBufList.begin();
        pRecvPacket = *iter;
        recvBufList.pop_front();

        if(pRecvPacket != NULL)
        {
            delete pRecvPacket;     //�������ռ�
            pRecvPacket = NULL;
        }
    }

    recvBufList.resize(0);
}


/*
* ���� �������ж���һ����,��ɾ��������ڵ�
* @return  pReceiveBuf    ���յ��İ�
*/
RecvPacket* GdbServer::getPacketFromList()
{
    TMutexLocker lock(&recvListMutex);

    if (recvBufList.size() > 0)
    {
        list<RecvPacket*>::iterator iter = recvBufList.begin();
        RecvPacket* pRecvPacket = *iter;
        recvBufList.pop_front();
        return pRecvPacket;
    }

    else
    {
        return NULL;
    }
}


/*
* ���� ���յ��İ����뻺��������
* @param[in]     pReceiveBuf    ���յ��İ�
*/
void GdbServer::insertList(RecvPacket* pReceiveBuf)
{
    TMutexLocker lock(&recvListMutex);
    recvBufList.push_back(pReceiveBuf);
    ReleaseSemaphore(bufSemaphore,1,NULL);
}

/*
* @brief
*    ��������16�����ַ���ת��Ϊ����,��Ҫע����Ǹò������ƶ���������ָ��ĵ�ַ\n
* @param[in]     cptr    16�����ַ���
* @param[out] wIntValue ת����INT����
* @return  ָ���ƶ����ֽ���
*/
T_UWORD GdbServer::Hex2int(T_CHAR **cptr, T_WORD *wIntValue)
{
    int numChars = 0;
    int hexValue;

    *wIntValue = 0;

    while (**cptr)
    {
        hexValue = Hex2num(**cptr);

        if (hexValue < 0)
        {
            break;
        }

        *wIntValue = (*wIntValue << 4) | hexValue;
        numChars ++;

        (*cptr)++;
    }

    return (numChars);
}

T_UWORD GdbServer::Int2hex(T_CHAR *cpPtr, T_WORD wSize,T_WORD wIntValue)
{
    memset(cpPtr,'\0',wSize);   //���ô�С,��󻺳����Ĵ�С
    sprintf(cpPtr,"%x",wIntValue);
    return strlen(cpPtr);
}



/*
* @brief
*    ��������16����ASCII�ַ�ת���ɶ�Ӧ������\n
* @param[in]     cptr   ������16����ASCII�ַ�
* @return  ���ض�Ӧ������
*/
T_WORD GdbServer::Hex2num(T_UBYTE vubCh)
{
    if (vubCh >= 'a' && vubCh <= 'f')
    {
        return vubCh-'a'+10;
    }

    if (vubCh >= '0' && vubCh <= '9')
    {
        return vubCh-'0';
    }

    if (vubCh >= 'A' && vubCh <= 'F')
    {
        return vubCh-'A'+10;
    }

    return -1;
}

#ifdef TRA_DEBUG
T_VOID GdbServer::SetLog(T_BOOL isLog)
{
    QMutexLocker locker(&m_logMutex);
    m_isLog = isLog;
}

T_BOOL GdbServer::IsLog()
{
    QMutexLocker locker(&m_logMutex);
    return m_isLog;
}
#endif

#if 1
T_WORD GdbServer::SetMop(T_BOOL isMop)
{
    TMutexLocker locker(&m_mopMutex);
    m_mopFlag = isMop;
    return SUCC;
}

T_BOOL GdbServer::GetMop()
{
    TMutexLocker locker(&m_mopMutex);
    return m_mopFlag;
}

#endif

T_VOID GdbServer::SetIsProcessingGdb(T_BOOL flag)
{
    TMutexLocker locker(&m_gdbCmdMutex);
    m_isProcessingGdb = flag;
}

T_BOOL GdbServer::IsProcessingGdb()
{
    TMutexLocker locker(&m_gdbCmdMutex);
    return m_isProcessingGdb;
}

