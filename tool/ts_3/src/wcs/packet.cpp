
/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* TS���ݰ��Ĺ���Ͱ��Ľṹ
*
* @file     packet.cpp
* @brief     C++TS���ݰ��Ĺ���Ͱ��Ľṹ
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#include "Packet.h"
#include "log.h"
//////////////////////////////////////Packet
////////////////////////////////PacketManager
PacketManager* PacketManager::pm = NULL;
int PacketManager::waterLevel = 0;
QList<Packet*> PacketManager::freeList;
QMutex PacketManager::mutex;

/**
* TS���ݰ��Ĺ���Ͱ��Ľṹ
*
* @file     packet.cpp
* @brief     C++TS���ݰ��Ĺ���Ͱ��Ľṹ
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/
/**
* ����:�Է���İ�ָ���ɵ����߸����ͷ�,Ҳ����ͨ������PackFree�������ﵽ�ͷŵ�Ŀ��
* @param pack һ��Packet����
* @return �ɹ�����true��ʧ�ܷ���false
*/
Packet* PacketManager::alloc() 
{
    QMutexLocker locker(&mutex);
    
    while(true) 
    {
        if (freeList.size() > 0) 
        {    //����������������ˮλһ���������
            Packet* pPacket = freeList.first();
            freeList.removeFirst();
            int a = sizeof(Packet);
            a = sizeof Packet;
            memset((char*)pPacket,0,sizeof(Packet));
            return pPacket;
        }

        if(waterLevel < HIGH_LEVEL) 
        {    //���ڸ�ˮλ�����ռ�
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
        if (waterLevel >= HIGH_LEVEL)
            bufferNotFull.wait(locker.mutex());    //�ȴ�
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
    QMutexLocker locker(&mutex);
    if(freeList.isEmpty()) 
    {    //�յ�
        if (waterLevel >= LOW_LEVEL) 
        {    //������ڵ�ˮλ
            if(waterLevel >= HIGH_LEVEL)
            {    //���ڸ�ˮλ
                if (LOW_LEVEL != HIGH_LEVEL) 
                {//�ߵ�ˮλ�����
                    --waterLevel;    //����ˮλ
                    delete pack;
                    pack = NULL;
                }
                else 
                {//���ߵ�ˮλ��ȵ�ʱ��,�����а���ɾ��,ֱ�ӷ����������
                    if (freeList.indexOf(pack) == -1)    //û���ҵ�������
                        freeList.append(pack);    //��ӵ���������
                    else
                        int a = 0;
                }
                bufferNotFull.wakeOne();    //������������
            } 
            else 
            {    //ɾ���߳���ˮλ�����ݰ�
                --waterLevel;    //����ˮλ
                delete pack;    
                pack = NULL;
            }
        } 
        else 
        {
            if (freeList.indexOf(pack) == -1)    //û���ҵ�������
                freeList.append(pack);    //��ӵ���������
            else
                int a = 0;
        }
    }
    else 
    {    
        if (freeList.indexOf(pack) == -1)    //û���ҵ�������
            freeList.append(pack);    //��ӵ���������
        else
            int a = 0;
    }
    return true;

}






