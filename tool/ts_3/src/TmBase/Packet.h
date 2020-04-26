
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
* @file     packet.h
* @brief     TS���ݰ��Ĺ���Ͱ��Ľṹ
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/


#ifndef PACK_INCLUDE
#define PACK_INCLUDE

#include <list>
#include "mutex.h"
#include "tmBaseInclude.h"

#define PACKET_DATASIZE         (1008+3*1024)                    //�����ݴ�С
#define PACKET_HEADSIZE         16                                //��ͷ��С

class Packet {
private:
    int  size;                        //���ĳ���
    short des_aid;                    //Ŀ��AID
    short src_aid;                    //ԴAID
    short des_said;                    //Ŀ��SAID
    short src_said;                    //ԴSAID
    int      serialNum;                //���к�
    char data[PACKET_DATASIZE] ;    //����������
public:


    /**
    * ����:���ð�ԴAID
    * @param des_said  ����ԴAID
    */
    void setSrc_aid(short src_aid) 
    {
        this->src_aid = src_aid;
    }

    /**
    * ����:�õ���ԴAID
    * @return des_said  ����ԴAID
    */
    short getSrc_aid() 
    {
        return src_aid;
    }

    /**
    * ����:���ð�ԴSAID
    * @param des_said  ����ԴSAID
    */
    void setSrc_said(short src_said) 
    {
        this->src_said = src_said;
    }

    /**
    * ����:�õ���ԴSAID
    * @return des_said  ����ԴSAID
    */
    short getSrc_said()
    {
        return src_said;
    }


    /**
    * ����:���ð�Ŀ��AID
    * @param des_said  ����Ŀ��AID
    */
    void setDes_aid(short des_aid) 
    {
        this->des_aid = des_aid;
    }

    /**
    * ����:�õ���Ŀ��AID
    * @return short  ����Ŀ��AID
    */
    short getDes_aid() 
    {
        return des_aid;
    }

    /**
    * ����:���ð�Ŀ��SAID
    * @param des_said  ����Ŀ��SAID
    */
    void setDes_said(short des_said)
    {
        this->des_said = des_said;
    }


    /**
    * ����:�õ���Ŀ��SAID
    * @return short  ����Ŀ��SAID
    */
    short getDes_said() 
    {
        return des_said;
    }
        /**
        *���ܣ�������ݻ���
        */
        void clearData()
        {
            memset(data, 0, sizeof(data));
            return;
        }
    /**
    * ����:�õ���������
    * @return char*  ������
    */
    char* getData() 
    {
        return data;
    }

    /**
    * ����:�õ����Ĵ�С
    * @return size ���ð���С
    */
    int getSize() 
    {
        return size;
    }
    /**
    * ����:���ð��Ĵ�С
    * @param size ���ð���С
    */
    void setSize(int size)
    {
        this->size = size;
    }
    /**
    * ����:�õ����к�
    * @return ���к�
    */
    int getSerialNum() 
    {
        return serialNum;
    }
    /**
    * ����:�������к�
    * @param serialNum ���к�
    */
    void setSerialNum(int serialNum)
    {
        this->serialNum = serialNum;
    }
    /**
    * ����:�����ṩ�Ĵ�����Ϣ����ʧ�ܻظ���
    * @param 
    */
    int setErrorPacket(char *errStr, US strsize, int dialogID = -1)
    {
        char *pBuf = data;
        //���dialogIDΪ-1����[0,65535]��������־���Ϊ�ǲ�����DID����
        if((dialogID != -1) && (dialogID >= 0) && (dialogID <= 65535))
        {
            __store(dialogID, pBuf, DIDLEN);
            pBuf += DIDLEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
        }
        if(sizeof(data) < strsize)
        {
            return 0;
        }
        memcpy(pBuf, errStr, strsize);
        pBuf += strsize;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        return (int)(pBuf - data);
    }
    
};

class PacketManager 
{
private:
    static PacketManager* pm;
    const static int LOW_LEVEL = 256;     //���ˮλ��־
    const static int HIGH_LEVEL = 512;    //���λ��־
    static list<Packet*> freeList;        //������а�����
    static int waterLevel;                //��ǰˮλֵ 
    static TS_Mutex mutex;                        //�������
    TS_WaitCondition bufferNotFull;        //��������,�����ж��Ƿ�ﵽ��ˮλ�ж�
public:
    ~PacketManager();
    static PacketManager* getInstance() 
    {
        if (pm == NULL) 
        {    //���ؼ��һ��
            TS_MutexLocker lock(&mutex);
            if (pm == NULL)
                pm = new PacketManager;
        }

        return pm;
    }
    //�Է���İ�ָ���ɵ����߸����ͷ�,Ҳ����ͨ������PackFree�������ﵽ�ͷŵ�Ŀ��
    Packet* alloc() ;
    /**
    * ����:�ͷ�һ�������
    * @param pack һ��Packet����
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    bool free(Packet* pack);

    //�ͷ����а�
    void freeAllPacket();
};
#endif // !defined(PACKET_INCLUDE)
