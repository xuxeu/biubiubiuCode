/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsMapServer.cpp
 * @Version        :  1.0.0
 * @brief           :  *   <li> ����</li>tsmap������, tsMap���ʵ��,����ע��,��ѯ,���ݴ洢��
                                <li>���˼��</li> 
                                <p> n     ע�����n</p> 
 *   
 * @author        :  qingxiaohai
 * @create date:  2007��11��2�� 16:25:46
 * @History        : 
 *   
 */


/************************���ò���*****************************/

#include "tsmapComm.h"
#include "tsmapServer.h"
#include "common.h"
#include <list>
#include <vector>

using namespace std;

/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/


/**
 * @Funcname:  tsMap
 * @brief        :  tsmap�����������
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *   
**/
tsMap::~tsMap()
{
    T_NM2ADDR::iterator m_itrMp;
    T_DATATBL::iterator m_itrList;
    T_DATATBL* dataList = NULL;
    T_MapData* mydata = NULL;

    /*������ӳ�����ȡ�����ַ���ٴӱ���ɾ������*/
    m_itrMp = mpName2Tbl.begin();
    while(m_itrMp != mpName2Tbl.end())
    {
        
        dataList = m_itrMp->second;
        mpName2Tbl.erase(m_itrMp);
        /*ɾ�����е�����*/
        m_itrList = dataList->begin();
        while(m_itrList != dataList->end())
        {
            
            mydata = *m_itrList;
            if(mydata->data != NULL)
            {
                free(mydata->data);
                mydata->data = NULL;
            }
            delete mydata;
            dataList->erase(m_itrList);
            m_itrList++;
        }
        delete (m_itrMp->first);
        delete (m_itrMp->second);
        m_itrMp++;
    }
    /*������Ա�*/
    mpTAddr2Pro.clear();
}

/*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
/*  Modified brief: ����ֹͣTSMAP*/
/**
 * @Funcname:  tsMap
 * @brief        :  tsmap��ֹͣ����ʱ,�������������̵߳��ź���
 * @Author     :  tangxp
 *  
 * @History: 1.  Created this function on 2008��2��1�� 16:30:24
 *   
**/
void tsMap::stop()
{
    tmpDataLen = 0;
    wtData.wakeup();    
}
/*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/

/**
 * @Funcname:  tsMap
 * @brief        :  tsmap��Ľ���ts������������
 * @param[IN]: srcAid, ��Ϣ����Դ��AID
 * @param[IN]: srcSaid, ��Ϣ����Դ�Ķ���ID��
 * @param[IN]: data, ��Ϣ������
 * @param[IN]: len, ��Ϣ����
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *   
**/
int tsMap::putpkt(US srcAid, US srcSaid, char * data, int len)
{
    int iRet = 0;
    char *pAck = NULL;
    
    if((NULL == data) || (len < 2))
    {
        return FAIL;
    }

    /*��־����*/
    sysLoger(LDEBUG, "tsMap::putpkt: recieved cmd %c", data[0]);        
    
    curSrcAid = srcAid;
    curSrcSaid = srcSaid;
    
    switch(data[0])
    {
        case TSMAP_REGISTER:
            iRet = registerData(&data[2], len-2);
            break;
            
        case TSMAP_UNREGISTER:
            iRet = unRegisterData(&data[2], len-2);
            break;
            
        case TSMAP_QUERY:
        {
            char ackData[2048];
            
            iRet = query(&data[2], len-2, ackData, sizeof(ackData));
            if(iRet < 0)
            {
                pAck = getAckStr(iRet);
                sendAck(pAck, strlen(pAck));
                return SUCC;
            }
            /*���ͻظ�����*/
            sendAck(ackData, iRet);
            return SUCC;
        }
        
        default:
            iRet = EINVCMD;
            sysLoger(LDEBUG, "tsMap::putpkt: recieved unknow cmd %c", data[0]);        
            break;
    }

    pAck = getAckStr(iRet);
    sendAck(pAck, strlen(pAck));

    return SUCC;
}

/**
 * @Funcname:  tsMap
 * @brief        :  tsmap��ķ��ش���������
 * @param[OUT]: desAid, ��Ϣ��Ŀ�ĵ�AID
 * @param[OUT]: desSaid, ��Ϣ��Ŀ�ĵض���ID��
 * @param[OUT]: buf, �û�������ݵĻ���
 * @param[IN]: size, �û������С
 * @return      : �ɹ����ؽ��յ������ݵĴ�С,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *   
**/
int tsMap::getpkt(US * desAid, US * desSaid, char * buf, int size)
{
    int len = 0;
    
    wtData.wait();
    /*Modified by tangxp for BUG NO.0002823 on 2008.3.4 [begin]*/
    /*  Modified brief: ���tmpDataLenΪ0,��ʾ�˳�,����������*/
    if(0 == tmpDataLen)
    {
        return 0;
    }
    /*Modified by tangxp for BUG NO.0002823 on 2008.3.4 [end]*/
    mtxBuf.lock();
    *desAid = curSrcAid;
    *desSaid = curSrcSaid;
    len = min(size, tmpDataLen);
    ZeroMemory(buf, size);
    memcpy(buf, tmpData, len);
    ZeroMemory(tmpData, sizeof(tmpData));
    mtxBuf.unlock();
    return len;
}

/**
 * @Funcname:  sendAck
 * @brief        :  ��������ͽ������
 * @param[IN]: ack, �������
 * @param[IN]: len, ������ݳ���
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��5�� 16:22:31
 *   
**/
int tsMap::sendAck(char *ack, int len)
{
    mtxBuf.lock();
    ZeroMemory(tmpData, sizeof(tmpData));
    tmpDataLen = min(len, sizeof(tmpData));
    memcpy(tmpData, ack, tmpDataLen);
    wtData.wakeup();
    mtxBuf.unlock();
    return SUCC;
}

/**
 * @Funcname:  tsMap
 * @brief        :  tsmap���ע�ắ��
 * @param[IN]: data, ��Ϣ������
 * @param[IN]: len, ��Ϣ����
 * @return      : �ɹ�����SUCC,ʧ�ܷ��ؾ������ֵ
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *   ע��ĸ�ʽ:
 *  r:name;exclusive;regAid;regSaid;queryAid;querySaid;qrlen;queryStr;[datalen;data;]
 *   ����data��"name"�ֶο�ʼ
**/
int tsMap::registerData(char * data, int len)
{
    char * qryStr = NULL;                     //ע�������еĲ�ѯ�ַ���
    char * usrData = NULL;                  //ע�������е��û�����
    char *pData= NULL;
    char *pStr = NULL;
    char qryBuf[255] = "\0";
    int iLen = 0;
    int iDataLen = 0;
    int iRet = 0;
    int args[6] = {0};
    int idx = 0;
    char             *tblName = NULL;                         //���ݱ���,��ע����
    T_DATATBL* tblAddr = NULL;                //���ݱ��ַ
    T_MapData *tblData = NULL;
    
    if(NULL == data)
    {
        sysLoger(LWARNING, "tsMap::registerData  paragram data is null pointer!");
        return FAIL;
    }
    
    pData = data;
    
    /*����*/
    pStr = strstr(pData, TSMAP_SPLIT_STR);
    if(NULL == pStr)
    {
        sysLoger(LWARNING, "tsMap::registerData  parsed registerData name error!");
        return EINVCMD;
    }
    tblName = new char[(pStr- pData +1)];
    if(NULL == tblName)
    {
        sysLoger(LWARNING, "tsMap::registerData  Alloc buffer for  name error!");
        return EISTFAIL;
    }
    ZeroMemory(tblName, (pStr- pData +1));
    memcpy(tblName, pData, pStr- pData);
    
    idx = 0;
    /*����ע�����ݵĸ������ֲ���*/
    /*������exclusive,regid,regsaid,qryid,qrysaid,qrylen*/
    while(idx < 6)
    {
        pData = pStr + TSMAP_SPLIT_STR_LEN;
        pStr = strstr(pData, TSMAP_SPLIT_STR);
        if(NULL == pStr)
        {
            sysLoger(LWARNING, "tsMap::registerData  parse ags error!");
            return EINVCMD;
        }
        hex2int(&pData, &args[idx++]);
    }

    /*�������ֶ�Ӧ�ı��Ƿ����*/
    tblAddr = getTblAddress(tblName);
    /*�������򴴽������ֶ�Ӧ�ı�,�����뵽mpName2Tblӳ�����*/
    if(NULL == tblAddr)
    {
        tblAddr = addNewTable(tblName);
        if(NULL == tblAddr)
        {
            sysLoger(LWARNING, "tsMap::registerData  create tabel error!");
            return EISTFAIL;
        }
        /*���ñ���Ƿ�����������ݵ����������Ա��еǼ�*/
        mpTAddr2Pro[(int*)tblAddr] = args[0];
    }    
    
    /*�Ƿ�������ڶ������������������Ѵ���*/
    /*1��ʾ���������*/
    if((1 == getTableProperty((int *)tblAddr)) && (!(tblAddr->empty())))
    {
        sysLoger(LINFO, "tsMap::registerData  can't registerData repeat for %s!", tblName);
        return ENOTPMT;
    }

    *(pData+TSMAP_SPLIT_STR_LEN+args[5]) = '\0';
    
    /*��ѯ�Ƿ�����ͬ��ѯ���������ݴ���*/
    iLen = sprintf(qryBuf, "%s;%x;%x;%x;%s;", tblName,
                           args[3], args[4], args[5], pData+TSMAP_SPLIT_STR_LEN);
    iLen = query(qryBuf, iLen, qryBuf, sizeof(qryBuf));
    if(iLen > 0)
    {
        /*�Ѿ�������ͬ��ѯ����������*/
        sysLoger(LWARNING, "tsMap::registerData  existed the same data!");
        return ENOTPMT;
    }
    
    /*����������*/
    tblData = new T_MapData;
    if(NULL == tblData)
    {
        sysLoger(LWARNING, "tsMap::registerData  alloc data set for registered data error");
        return EISTFAIL;
    }

    tblData->data = NULL;
    tblData->regAid = args[1];
    tblData->regSaid = args[2];
    tblData->qryAid = args[3];
    tblData->qrySaid = args[4];    
    tblData->qryStr = string(strlwr(pData+TSMAP_SPLIT_STR_LEN), args[5]);
    pData +=  2 * TSMAP_SPLIT_STR_LEN + args[5];
    /*��������û����ݲ���*/
    if(data + len > pData)
    {
        /*�û����ݳ���*/

        pStr = strstr(pData, TSMAP_SPLIT_STR);
        if(NULL == pStr)
        {
            sysLoger(LWARNING, "tsMap::registerData  parse user data len error");
            return EINVCMD;
        }
        hex2int(&pData, &iLen);
        tblData->dataLen = iLen;
        /*�û�����*/
        tblData->data = (char*)malloc(tblData->dataLen);
        if(NULL == tblData->data)
        {
            sysLoger(LWARNING, "tsMap::registerData  alloc buffer for user data error");
            return EISTFAIL;
        }
        memset(tblData->data, 0, tblData->dataLen);
        memcpy((tblData->data), (pStr + TSMAP_SPLIT_STR_LEN), (tblData->dataLen));
    }

    /*���������ݲ����û�����*/
    tblAddr->push_back(tblData);
    sysLoger(LDEBUG, "tsMap::registerData  registerData succ!");
    
    /*����*/
    return SUCC;    
}


/**
 * @Funcname:  tsMap
 * @brief        :  tsmap���ע������
 * @param[IN]: data, ��Ϣ������
 * @param[IN]: len, ��Ϣ����
 * @return      : �ɹ�����SUCC,ʧ�ܷ��ؾ������ֵ
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *   ע���������ʽ��
 * u:name; regAid;regSaid;queryAid;querySaid;qrlen;queryStr;
**/
int tsMap::unRegisterData(char * data, int len)
{
    char *pData= data;
    char *pStr = NULL;
    int idx = 0;
    int args[5] = {0};
    string qryStr;
    char             tblName[MAX_TBLNM_LEN] = "\0";
    T_DATATBL    *tblAddr = NULL;
    T_MapData *tblData = NULL;
    bool isFind = false;
    
    if(NULL == data)
    {
        sysLoger(LWARNING, "tsMap::unRegisterData  paragram data is null pointer!");
        return FAIL;
    }

    /*��������*/
    pStr = strstr(pData, TSMAP_SPLIT_STR);
    if(NULL == pStr)
    {
        sysLoger(LWARNING, "tsMap::unRegisterData  parse name error!");
        return EINVCMD;
    }
    memcpy(tblName, pData, pStr- pData);
    
    idx = 0;
    /*����ע�����ݵĸ������ֲ���*/
    /*������regid,regsaid,qryid,qrysaid,qrylen*/
    while(idx < 5)
    {
        pData = pStr + TSMAP_SPLIT_STR_LEN;
        pStr = strstr(pData, TSMAP_SPLIT_STR);
        if(NULL == pStr)
        {
            sysLoger(LWARNING, "tsMap::unRegisterData  parse args error!");
            return EINVCMD;
        }
        hex2int(&pData, &args[idx++]);
    }    
    /*��ѯ�ֶ�qryStr*/
    qryStr = string(strlwr(pData+TSMAP_SPLIT_STR_LEN), args[4]);
    
    /*�������ֶ�Ӧ�ı��Ƿ����,�����ڷ��ش���*/
    tblAddr = getTblAddress(tblName);
    if((NULL == tblAddr) || (tblAddr->empty()))
    {
        sysLoger(LINFO, "tsMap::unRegisterData  the table specified by name %s!is not exist", tblName);
        return ENEXIT;
    }

    sysLoger(LINFO, "tsMap::unRegisterData  unregister %s  %s ....", tblName, qryStr.data());
    
    /*������ַ���ڣ������*/
    T_DATATBL::iterator m_itr = tblAddr->begin();
    while(m_itr != tblAddr->end())
    {
        if(((*m_itr)->regAid == args[0])
            && ((*m_itr)->regSaid == args[1])
            && ((*m_itr)->qryAid == args[2])
            && ((*m_itr)->qrySaid == args[3])
            && ((*m_itr)->qryStr.length() == args[4])
            && ((*m_itr)->qryStr.compare(qryStr) == 0))
        {
            isFind = true;
            break;
        }
        m_itr++;
    }

    if(isFind)
    {
        if((*m_itr)->data != NULL)
        {
            free((*m_itr)->data);
            (*m_itr)->data = NULL;
        }        
        delete (*m_itr);
        *m_itr = NULL;
        tblAddr->erase(m_itr);
        /*�������ݱ���û�����ݣ���ɾ�����Ҵ�ӳ���mpName2Tbl
        ��ɾ��*/
        if(tblAddr->empty())
        {           
            /*��ӳ���mpName2Tbl���ҵ�ƥ���ɾ��*/
            map<char *, T_DATATBL*>::iterator m_N2TItr = mpName2Tbl.begin();
            while(m_N2TItr != mpName2Tbl.end())
            {
                if(m_N2TItr->second == tblAddr)
                {
                    
                    delete (m_N2TItr->first);                      //ɾ�����ֿռ�
                    delete (m_N2TItr->second);                //ɾ����ռ�
                    mpName2Tbl.erase(m_N2TItr);
                    break;
                }
                m_N2TItr++;
            }
            /*�����Ա���ɾ�����¼*/
            map<int*, int>::iterator m_Adr2PrItr = mpTAddr2Pro.find((int*)tblAddr);
            if(m_Adr2PrItr != mpTAddr2Pro.end())
            {
                mpTAddr2Pro.erase(m_Adr2PrItr);
            }
            
        }
        sysLoger(LWARNING, "tsMap::unRegisterData  unregister succ");
        return SUCC;
    }

    return ENEXIT;
        
}

/**
 * @Funcname:  tsMap
 * @brief        :  tsmap��Ĳ�ѯ����
 * @param[IN]: data, ��Ϣ������
 * @param[IN]: len, ��Ϣ����
 * @param[OUT]: ackData, �����Ļظ���Ϣ,���û�����ռ�
 * @param[IN]: len, �ظ���Ϣ�ռ�ĳ���
 * @return      : �ɹ����ػظ����ݰ��ĳ���,ʧ�ܷ��ؾ������ֵ
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��2�� 16:30:24
 *  ��ѯ�����ʽ:
 *  q:name; queryAid;querySaid;qrlen;queryStr;
**/
int tsMap::query(char * data, int len, char *ackData, int size)
{

    char *pData= data;
    char *pStr = NULL;
    int idx = 0;
    int args[3] = {0};
    string qryStr;
    char             tblName[MAX_TBLNM_LEN] = "\0";
    T_DATATBL  *tblAddr = NULL;
    T_MapData *tblData = NULL;
    bool isFind = false;
    
    if((NULL == data) || (NULL == ackData))
    {
        sysLoger(LWARNING, "tsMap::query  paragram data or ackData is null pointer!");
        return FAIL;
    }

    /*��������*/
    pStr = strstr(pData, TSMAP_SPLIT_STR);
    if(NULL == pStr)
    {
        sysLoger(LWARNING, "tsMap::query  parse name error!");
        return EINVCMD;
    }
    memcpy(tblName, pData, pStr- pData);
    
    idx = 0;
    /*����ע�����ݵĸ������ֲ���*/
    /*������queryAid;querySaid;qrlen;*/
    while(idx < 3)
    {
        pData = pStr + TSMAP_SPLIT_STR_LEN;
        pStr = strstr(pData, TSMAP_SPLIT_STR);
        if(NULL == pStr)
        {
            sysLoger(LWARNING, "tsMap::query  parse args error!");
            return EINVCMD;
        }
        hex2int(&pData, &args[idx++]);
    }    
    /*��ѯ�ֶ�qryStr*/
    *(pData+TSMAP_SPLIT_STR_LEN+args[2]) = '\0';
    qryStr = string(strlwr(pData+TSMAP_SPLIT_STR_LEN), args[2]);
    
    /*�������ֶ�Ӧ�ı��Ƿ����,�����ڷ��ش���*/
    tblAddr = getTblAddress(tblName);
    if((NULL == tblAddr) || (tblAddr->empty()))
    {
        sysLoger(LINFO, "tsMap::query  table specified by name %s is not exist!", tblName);
        return ENEXIT;
    }

    /*������ַ���ڣ������*/
    /*�Ȳ���׼��ƥ����*/
    T_DATATBL::iterator m_itr = tblAddr->begin();
    while(m_itr != tblAddr->end())
    {
        if(((*m_itr)->qryAid == args[0])
            && ((*m_itr)->qrySaid == args[1])
            && ((*m_itr)->qryStr.length() == args[2])
            && ((*m_itr)->qryStr.compare(qryStr) == 0))
        {
            isFind = true;
            break;
        }
        m_itr++;
    }
    
    /*û���ҵ����ٲ�ͨ����id=0Ϊͨ��*/
    if(false == isFind)
    {
        m_itr = tblAddr->begin();
        while(m_itr != tblAddr->end())
        {
            if(((*m_itr)->qryAid == 0)
                && ((*m_itr)->qrySaid == 0)
                && ((*m_itr)->qryStr.length() == args[2])
                && ((*m_itr)->qryStr.compare(qryStr) == 0))
            {
                isFind = true;
                break;
            }
            m_itr++;
        }
    }

    /*�ҵ���������*/
    if(isFind)
    {
        int ilen = sprintf(ackData, "OK;%x;%x;", (*m_itr)->regAid, (*m_itr)->regSaid);
        if((*m_itr)->dataLen > 0)
        {
            ilen += sprintf(ackData+ilen, "%x;", (*m_itr)->dataLen);
            memcpy(ackData + ilen, (*m_itr)->data, min((*m_itr)->dataLen, size-ilen));
            ilen += min((*m_itr)->dataLen, size-ilen);
        }
        return ilen;
    }

    /*û���ҵ�����δ�ҵ�ƥ�������*/
    
    return ENEXIT;
}


/**
 * @Funcname:  tsMap
 * @brief        : ͨ��ע����(����)�������ݱ��ַ
 * @param[IN]: tblName, ע������������
 * @return      : û���ҵ�����NULL�����򷵻ر��ַ
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��5�� 10:30:24
 *   
**/
T_DATATBL* tsMap::getTblAddress(char *tblName)
{
    T_NM2ADDR::iterator itrNm2Tbl;

    if(NULL == tblName)
    {
        return NULL;
    }

    //��ӳ����в���
    itrNm2Tbl = mpName2Tbl.begin();
    while(itrNm2Tbl != mpName2Tbl.end())
    {
        if(strcmp(itrNm2Tbl->first, tblName) == 0)
        {
            break;
        }
        itrNm2Tbl++;
    }
    
    //û�ҵ����ؿ�
    if(itrNm2Tbl == mpName2Tbl.end())
    {
        return NULL;
    }
    //�ҵ����������ݱ��ַ
    return (itrNm2Tbl->second);
}

/**
 * @Funcname:  addNewTable
 * @brief        :  �������ּ����±�ӳ�����
 * @param[IN]: tblName, ע������������
 * @return      : ���ʧ�ܷ���NULL,  ���򷵻��±�ĵ�ַ
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��5�� 11:36:01
 *   
**/
T_DATATBL* tsMap::addNewTable(char *tblName)
{
    typedef PAIR_NM2ADDR nm2TblPair;
    T_DATATBL *tblAddr = NULL;
    
    if(NULL == tblName)
    {
        return NULL;
    }

    /*�����±�*/
    tblAddr = new T_DATATBL();
    if(NULL == tblAddr)
    {
        return NULL;
    }
    /*�����±�ӳ�����*/
    mpName2Tbl[tblName] = tblAddr;
    return tblAddr;
    
}

/**
 * @Funcname:  getTableProperty
 * @brief        :  ��ȡ���ݱ��Ƿ�����ע��������ݵ�����
 * @para1      : 
 * @para2      : 
 * @para3      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��11��7�� 17:27:07
 *   
**/
int tsMap::getTableProperty(int *tblAddr)
{
    T_ADDR2PRO::iterator m_itr;
    
    if(NULL == tblAddr)
    {
        return FAIL;
    }

    m_itr = mpTAddr2Pro.find(tblAddr);
    if(m_itr == mpTAddr2Pro.end())
    {
        return FAIL;
    }

    return (m_itr->second);
}

