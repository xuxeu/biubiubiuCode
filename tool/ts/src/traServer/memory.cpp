/*
* ������ʷ��
* 2006-02-20 ������  ���������������޹�˾
*                    �������ļ���
* 2006-04-06 ����F  ���������������޹�˾
*                    TS��������
* 2006-06-22 tanjw   ���������������޹�˾
*                    ���tsɾ���Ựʱ���ܵ����쳣�˳���bug��
*                    ����iceserver���ice����ֵ��ȷ�ԡ�
* 2006-10-30 ����F  ���������������޹�˾
*                    ��������TA���Ե��ڴ��Ż��㷨��
* 2008-03-19 ������  ���������������޹�˾
*                    ���ts���������Windows���������̣��޷���Ӧ��ݼ���bug��
*/

/**
 * @file    memory.cpp
 * @brief
 *  <li>���ܣ���ʼ���ڴ������� </li>
*
 */


/**************************** ���ò��� *********************************/
#include "memory.h"

/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/

QList<RSP_Memory*> RSP_MemoryManager::m_freeList;       //������а�����
T_WORD RSP_MemoryManager::m_waterLevel = NULL;              //��ǰˮλֵ
TMutex RSP_MemoryManager::m_mutex;                      //�������

/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

RSP_Memory::RSP_Memory()
{
}

RSP_Memory::~RSP_Memory()
{
    //��ʱû��ʲô��Դ�����ͷ�
}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<memory manager>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

RSP_MemoryManager::RSP_MemoryManager()
{

}

RSP_MemoryManager::~RSP_MemoryManager()
{
    //�ͷ�������Դ
    while (!m_freeList.isEmpty())
    {
        RSP_Memory* pPacket = m_freeList.first();
        m_freeList.removeFirst();

        if(pPacket != NULL)
        {
            delete pPacket;
            pPacket = NULL;
        }
    }
}

//�õ����������ܹ�����Ļ������Ŀ��С
T_WORD RSP_MemoryManager::LeftPackets()
{
    TMutexLocker locker(&m_mutex);
    T_WORD leftCount = 0;

    if (m_freeList.size() > 0)
    {
        leftCount += m_freeList.size();
    }

    if(m_waterLevel < HIGH_LEVEL)
    {
        leftCount += HIGH_LEVEL - m_waterLevel - 1;
    }

    return leftCount;
}

/**
* ����:����һ�����е��ڴ��
* @return �ɹ����ظ��ڴ�飬ʧ�ܷ���NULL
*/
RSP_Memory* RSP_MemoryManager::Alloc()
{
    TMutexLocker locker(&m_mutex);

    while(true)
    {
        if (m_freeList.size() > 0)
        {
            //����������������ˮλһ���������
            RSP_Memory* pPacket = m_freeList.first();
            m_freeList.removeFirst();
            return pPacket;
        }

        if(m_waterLevel < HIGH_LEVEL)
        {
            //���ڸ�ˮλ�����ռ�
            RSP_Memory* pack = new RSP_Memory();

            if(pack == NULL)
            {
                return NULL;
            }

            ++m_waterLevel;
            return pack;
        }

        if (m_waterLevel >= HIGH_LEVEL)
        {
            return NULL;
            //m_bufferNotFull.wait(locker.mutex()); //�ȴ�
        }
    }

    return NULL;
}

/**
* ����:�ͷ�һ���ڴ��
* @param[in] tpMem Ҫ�ͷŵ��ڴ��
* @return �ɹ�����true��ʧ�ܷ���false
*/
T_BOOL RSP_MemoryManager::Free(RSP_Memory* tpPack)
{
    TMutexLocker locker(&m_mutex);

    if(m_freeList.isEmpty())
    {
        //�յ�
        if (m_waterLevel >= LOW_LEVEL)
        {
            //������ڵ�ˮλ
            if(m_waterLevel >= HIGH_LEVEL)
            {
                //���ڸ�ˮλ
                if (LOW_LEVEL != HIGH_LEVEL)
                {
                    //�ߵ�ˮλ�����
                    --m_waterLevel; //����ˮλ
                    delete tpPack;
                    tpPack = NULL;
                }

                else//���ߵ�ˮλ��ȵ�ʱ��,�����а���ɾ��,ֱ�ӷ����������
                {
                    m_freeList.append(tpPack);
                }

                //  m_bufferNotFull.wakeOne();  //������������
            }

            else
            {
                //ɾ���߳���ˮλ�����ݰ�
                --m_waterLevel; //����ˮλ
                delete tpPack;
                tpPack = NULL;
            }
        }

        else
        {
            m_freeList.append(tpPack);  //��ӵ���������
        }
    }

    else
    {
        m_freeList.append(tpPack);  //��ӵ���������
    }

    return TRUE;

}

