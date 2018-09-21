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
* @file     memory.h
* @brief
*     <li>���ܣ� �ڴ�������</li>
*
*/

#ifndef _MEMORY_H
#define _MEMORY_H


/************************ͷ�ļ�********************************/

#include <QList>
#include "sysTypes.h"       ///�����ض���
#include "common.h"

/************************�궨��********************************/


/************************���Ͷ���******************************/

/**
* @brief
*   RSP_Memory�Ǳ��ػ���Ŀ������ڴ���Ϣ�Ļ����࣬ÿ������ӵ��һ������ַ�͸û���ַ��Ӧ���ڴ��\n
* @author   ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ��
*/
class RSP_Memory
{
public:
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_BIT = 6;    ///�ڴ���С����λ
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_SIZE = 1 << MEMORY_BLOCK_BIT;   ///ϵͳ���屾���ڴ�黺���С
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_MASK = (~0) - MEMORY_BLOCK_SIZE + 1;  //�����ڴ����ַ������

    /**
    * @brief
    *    ����: ���캯��
    */
    RSP_Memory();

    /**
    * @brief
    *    ����: ��������
    */
    ~RSP_Memory();

    /**
    * @brief
    *    ����: �����ڴ��Ļ���ַ
    * @param[in] wAddr ����ַ
    */
    T_VOID SetBaseAddr(T_WORD wAddr)
    {
        m_addr = wAddr;
    }

    /**
    * @brief
    *    ����: �õ��ڴ��Ļ���ַ
    * @return ����ַ
    */
    T_WORD GetBaseAddr()
    {
        return m_addr;
    }

    /**
    * @brief
    *    ����: �õ��ڴ�鱾�ػ��������ָ��
    * @return ����ַ
    */
    T_CHAR* GetValue()
    {
        return m_value;
    }

private:

    T_WORD m_addr;
    T_CHAR m_value[MEMORY_BLOCK_SIZE];
};

/**
* @brief
*   RSP_MemoryManager����������ڴ滺�������Ӷ�������Ƶ���ش������ͷŶ���\n
* @author   ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ��
*/
class RSP_MemoryManager
{
public:
    RSP_MemoryManager();

    ~RSP_MemoryManager();

    /**
    * ����:����һ�����е��ڴ��
    * @return �ɹ����ظ��ڴ�飬ʧ�ܷ���NULL
    */
    RSP_Memory* Alloc() ;

    /**
    * ����:�ͷ�һ���ڴ��
    * @param[in] tpMem Ҫ�ͷŵ��ڴ��
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    T_BOOL Free(RSP_Memory* tpMem) ;

    T_WORD RSP_MemoryManager::LeftPackets();
private:
    T_MODULE QList<RSP_Memory*> m_freeList;     //������а�����
    T_MODULE T_WORD m_waterLevel;               //��ǰˮλֵ
    T_MODULE TMutex m_mutex;                        //�������
    waitCondition m_bufferNotFull;      //��������,�����ж��Ƿ�ﵽ��ˮλ�ж�

    T_CONST T_MODULE T_WORD LOW_LEVEL = 32; //���ˮλ��־
    T_CONST T_MODULE T_WORD HIGH_LEVEL = 12800; //���λ��־
};
#endif