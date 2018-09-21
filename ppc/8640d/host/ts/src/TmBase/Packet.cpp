/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  packet.cpp
* @brief
*       ���ܣ�
*       <li> C++TS���ݰ��Ĺ���Ͱ��Ľṹ</li>
*/

/************************ͷ�ļ�********************************/
#include "Packet.h"
#include "log.h"

using namespace std;

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
PacketManager* PacketManager::pm = NULL;
int PacketManager::waterLevel = 0;
list<Packet*> PacketManager::freeList;
TS_Mutex PacketManager::mutex;


/**
* ����:�Է���İ�ָ���ɵ����߸����ͷ�,Ҳ����ͨ������PackFree�������ﵽ�ͷŵ�Ŀ��
* @param pack һ��Packet����
* @return �ɹ�����true��ʧ�ܷ���false
*/
Packet* PacketManager::alloc()
{
    TS_MutexLocker locker(&mutex);

    while(true)
    {
        if(freeList.size() > 0)
        {
            //����������������ˮλһ���������
            Packet* pPacket = freeList.front();
            freeList.pop_front();
            memset((char*)pPacket,0,sizeof(Packet));
            return pPacket;
        }

        if(waterLevel < HIGH_LEVEL)
        {
            //���ڸ�ˮλ�����ռ�
            Packet* pPacket = NULL;

            try
            {
                pPacket = new Packet();
                ++waterLevel;
                memset((char*)pPacket,0,sizeof(Packet));
            }

            catch(...)
            {
                int aaa = 0;
            }

            return pPacket;
        }

        //������ڸ�ˮλ,��ȴ�
        if (waterLevel >= HIGH_LEVEL)
        {
            bufferNotFull.wait(locker.getMutex());
        }
    }

    return NULL;
}

/**
* ����:�ͷ�һ�������
* @param pack һ��Packet����
* @return �ɹ�����true��ʧ�ܷ���false
*/
bool PacketManager::free(Packet* pack)
{
    TS_MutexLocker locker(&mutex);

    if(freeList.empty())
    {
        //��������Ϊ��,�����������ܽϴ�,���ܳ�����ˮλ
        if (waterLevel >= LOW_LEVEL)
        {
            //���ڵ�ˮλ
            if(waterLevel >= HIGH_LEVEL)
            {
                //���ڸ�ˮλ
                if (LOW_LEVEL != HIGH_LEVEL)
                {
                    //�ߵ�ˮλ�����
                    --waterLevel;    //����ˮλ
                    delete pack;
                    pack = NULL;
                }

                else
                {
                    //���ߵ�ˮλ��ȵ�ʱ��,�����а���ɾ��,ֱ�ӷ����������
                    freeList.remove(pack);
                    freeList.push_back(pack);
                }

                bufferNotFull.wakeOne();    //������������
            }

            else
            {
                //ɾ���߳���ˮλ�����ݰ�
                --waterLevel;    //����ˮλ
                delete pack;
                pack = NULL;
            }
        }

        else
        {
            //���ڵ�ˮλ,�����������
            freeList.remove(pack);
            freeList.push_back(pack);
        }
    }

    else
    {
        //��������ǿ�,��ǰ������������ڵ�ˮλ
        freeList.remove(pack);
        freeList.push_back(pack);
    }

    return true;
}

void PacketManager::freeAllPacket()
{
    TS_MutexLocker locker(&mutex);

    if(freeList.empty())
    {
        return;
    }

    list<Packet*>::iterator iter;

    for(iter = freeList.begin(); iter != freeList.end(); ++iter)
    {
        delete (*iter);
        *iter = NULL;
    }

    freeList.clear();
}

PacketManager::~PacketManager()
{
    freeAllPacket();
}




