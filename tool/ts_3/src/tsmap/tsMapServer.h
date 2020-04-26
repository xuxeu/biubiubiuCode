/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsMapServer.h
 * @Version        :  1.0.0
 * @brief           :  *   <li> ����</li> tsmap����,����������ݵĴ洢�Ͳ�ѯ�Ȳ���
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  qingxiaohai
 * @create date:  2007��11��2�� 15:56:35
 * @History        : 
 *   
 */


#ifndef _TSMAP_SERVER_H
#define _TSMAP_SERVER_H

#include "common.h"

#include <string>
#include <list>
#include <map>


using namespace std;
/*
mpName2Tbl      ӳ���洢                T_MapTbl,   ���ݱ��Ƿ�����                      T_MapData    ��ϸ��������
                           ���ݱ���                                        �洢��������
                          �����ݱ�                                          �������ݵĵ�ַ
                         ��ַ��ӳ��
|------------------------------|       |-----------------------------------|    |-------------------------------------------------------------|
| tblName    |     tblAdrr     |    /  | exclusive  |   tblData            |   /|regaid |regsaid | qryAid | qrySaid | qryStr | datalen | data |
|------------------------------|   /   |-----------------------------------|  / |------------------------------------------------------------ |
|tsfs        |    0x00003456   |  /    |    0       |    0x00005678        | /  |  3    |  0     |  4     |    0    |  c     |  0      |      |  
|------------------------------|       |-----------------------------------|    |-------------------------------------------------------------|

*/

#ifndef _TSMAP_BASE_TYPE_
#define _TSMAP_BASE_TYPE_
/*��������ݱ�*/
typedef struct
{
    int         regAid;                 //ע�᷽ID
    int         regSaid;                //ע�᷽SAID
    int         qryAid;                 //��ѯ��ID
    int         qrySaid;                //��ѯ��SAID
    string    qryStr;                   //��ѯ����
    int         dataLen;                //�û����ݳ���
    char*    data;                      //�洢�û�����
}T_MapData;


typedef list<T_MapData*>  T_DATATBL;
typedef map<char *, list<T_MapData*>*> T_NM2ADDR;

typedef map<int*, int>    T_ADDR2PRO;

typedef pair<int*, int> PAIR_ADDR2PRO;
typedef pair<char*, list<T_MapData*>*> PAIR_NM2ADDR;

#endif

class tsMap
{

private:
    T_NM2ADDR                      mpName2Tbl;                   //���ݱ�,ע���������ݱ�֮���ӳ���
    T_ADDR2PRO                    mpTAddr2Pro;                   //���Ա�,�����ݱ�ĵ�ַ��Ϊ������Ƿ�����ע�����������Ϊ������
    char                                  tmpData[2048];                //�ظ�����
    int                                     tmpDataLen;                   //�ظ����ݵĳ���
    TMutex                             mtxBuf;                            //�����ظ����ݵĻ���
    waitCondition                   wtData;                            //�ȴ��ظ����ݵ��ź���                    
    int                                    curSrcAid;                       //��ǰ�������ԴAID��
    int                                    curSrcSaid;                     //��ǰ�������Դsaid��


    int registerData(char *data, int len);

    int unRegisterData(char *data, int len);

    int query(char *data, int len, char*ackData, int size);

    T_DATATBL* getTblAddress(char *tblName);

    T_DATATBL* addNewTable(char *tblName);

    int sendAck(char *ack, int len);

    int getTableProperty(int *tblAddr);
    
public:

    ~tsMap();

    /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
    /*  Modified brief: ����ֹͣTSMAP*/
    void stop();
    /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/

    int putpkt(US srcAid, US srcSaid, char *data, int len);

    int getpkt(US *desAid, US *desSaid, char *buf, int size);
    
};

#endif

