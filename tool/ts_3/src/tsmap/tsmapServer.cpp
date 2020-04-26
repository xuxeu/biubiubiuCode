/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsMapServer.cpp
 * @Version        :  1.0.0
 * @brief           :  *   <li> 功能</li>tsmap的主体, tsMap类的实现,处理注册,查询,数据存储等
                                <li>设计思想</li> 
                                <p> n     注意事项：n</p> 
 *   
 * @author        :  qingxiaohai
 * @create date:  2007年11月2日 16:25:46
 * @History        : 
 *   
 */


/************************引用部分*****************************/

#include "tsmapComm.h"
#include "tsmapServer.h"
#include "common.h"
#include <list>
#include <vector>

using namespace std;

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/


/**
 * @Funcname:  tsMap
 * @brief        :  tsmap类的析构函数
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
 *   
**/
tsMap::~tsMap()
{
    T_NM2ADDR::iterator m_itrMp;
    T_DATATBL::iterator m_itrList;
    T_DATATBL* dataList = NULL;
    T_MapData* mydata = NULL;

    /*从名字映射表中取出表地址，再从表中删除数据*/
    m_itrMp = mpName2Tbl.begin();
    while(m_itrMp != mpName2Tbl.end())
    {
        
        dataList = m_itrMp->second;
        mpName2Tbl.erase(m_itrMp);
        /*删除表中的数据*/
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
    /*清空属性表*/
    mpTAddr2Pro.clear();
}

/*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
/*  Modified brief: 增加停止TSMAP*/
/**
 * @Funcname:  tsMap
 * @brief        :  tsmap类停止运行时,唤醒阻塞接收线程的信号量
 * @Author     :  tangxp
 *  
 * @History: 1.  Created this function on 2008年2月1日 16:30:24
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
 * @brief        :  tsmap类的接收ts发过来的数据
 * @param[IN]: srcAid, 消息包来源的AID
 * @param[IN]: srcSaid, 消息包来源的二级ID号
 * @param[IN]: data, 消息包内容
 * @param[IN]: len, 消息长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
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

    /*日志处理*/
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
            /*发送回复数据*/
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
 * @brief        :  tsmap类的返回处理结果数据
 * @param[OUT]: desAid, 消息包目的地AID
 * @param[OUT]: desSaid, 消息包目的地二级ID号
 * @param[OUT]: buf, 用户存放数据的缓冲
 * @param[IN]: size, 用户缓冲大小
 * @return      : 成功返回接收到的数据的大小,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
 *   
**/
int tsMap::getpkt(US * desAid, US * desSaid, char * buf, int size)
{
    int len = 0;
    
    wtData.wait();
    /*Modified by tangxp for BUG NO.0002823 on 2008.3.4 [begin]*/
    /*  Modified brief: 如果tmpDataLen为0,表示退出,则立即返回*/
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
 * @brief        :  处理完后发送结果数据
 * @param[IN]: ack, 结果数据
 * @param[IN]: len, 结果数据长度
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月5日 16:22:31
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
 * @brief        :  tsmap类的注册函数
 * @param[IN]: data, 消息包内容
 * @param[IN]: len, 消息长度
 * @return      : 成功返回SUCC,失败返回具体错误值
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
 *   注册的格式:
 *  r:name;exclusive;regAid;regSaid;queryAid;querySaid;qrlen;queryStr;[datalen;data;]
 *   参数data从"name"字段开始
**/
int tsMap::registerData(char * data, int len)
{
    char * qryStr = NULL;                     //注册数据中的查询字符串
    char * usrData = NULL;                  //注册数据中的用户数据
    char *pData= NULL;
    char *pStr = NULL;
    char qryBuf[255] = "\0";
    int iLen = 0;
    int iDataLen = 0;
    int iRet = 0;
    int args[6] = {0};
    int idx = 0;
    char             *tblName = NULL;                         //数据表名,即注册名
    T_DATATBL* tblAddr = NULL;                //数据表地址
    T_MapData *tblData = NULL;
    
    if(NULL == data)
    {
        sysLoger(LWARNING, "tsMap::registerData  paragram data is null pointer!");
        return FAIL;
    }
    
    pData = data;
    
    /*名字*/
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
    /*解析注册数据的各项数字参数*/
    /*依次是exclusive,regid,regsaid,qryid,qrysaid,qrylen*/
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

    /*查找名字对应的表是否存在*/
    tblAddr = getTblAddress(tblName);
    /*不存在则创建该名字对应的表,并加入到mpName2Tbl映射表中*/
    if(NULL == tblAddr)
    {
        tblAddr = addNewTable(tblName);
        if(NULL == tblAddr)
        {
            sysLoger(LWARNING, "tsMap::registerData  create tabel error!");
            return EISTFAIL;
        }
        /*将该表的是否允许多项数据的属性在属性表中登记*/
        mpTAddr2Pro[(int*)tblAddr] = args[0];
    }    
    
    /*是否允许存在多项，如果不允许并且数据已存在*/
    /*1表示不允许多项*/
    if((1 == getTableProperty((int *)tblAddr)) && (!(tblAddr->empty())))
    {
        sysLoger(LINFO, "tsMap::registerData  can't registerData repeat for %s!", tblName);
        return ENOTPMT;
    }

    *(pData+TSMAP_SPLIT_STR_LEN+args[5]) = '\0';
    
    /*查询是否有相同查询条件的数据存在*/
    iLen = sprintf(qryBuf, "%s;%x;%x;%x;%s;", tblName,
                           args[3], args[4], args[5], pData+TSMAP_SPLIT_STR_LEN);
    iLen = query(qryBuf, iLen, qryBuf, sizeof(qryBuf));
    if(iLen > 0)
    {
        /*已经存在相同查询条件的数据*/
        sysLoger(LWARNING, "tsMap::registerData  existed the same data!");
        return ENOTPMT;
    }
    
    /*分配数据项*/
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
    /*如果存在用户数据部分*/
    if(data + len > pData)
    {
        /*用户数据长度*/

        pStr = strstr(pData, TSMAP_SPLIT_STR);
        if(NULL == pStr)
        {
            sysLoger(LWARNING, "tsMap::registerData  parse user data len error");
            return EINVCMD;
        }
        hex2int(&pData, &iLen);
        tblData->dataLen = iLen;
        /*用户数据*/
        tblData->data = (char*)malloc(tblData->dataLen);
        if(NULL == tblData->data)
        {
            sysLoger(LWARNING, "tsMap::registerData  alloc buffer for user data error");
            return EISTFAIL;
        }
        memset(tblData->data, 0, tblData->dataLen);
        memcpy((tblData->data), (pStr + TSMAP_SPLIT_STR_LEN), (tblData->dataLen));
    }

    /*将表项数据插入用户表中*/
    tblAddr->push_back(tblData);
    sysLoger(LDEBUG, "tsMap::registerData  registerData succ!");
    
    /*返回*/
    return SUCC;    
}


/**
 * @Funcname:  tsMap
 * @brief        :  tsmap类的注销函数
 * @param[IN]: data, 消息包内容
 * @param[IN]: len, 消息长度
 * @return      : 成功返回SUCC,失败返回具体错误值
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
 *   注销的命令格式：
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

    /*解析名字*/
    pStr = strstr(pData, TSMAP_SPLIT_STR);
    if(NULL == pStr)
    {
        sysLoger(LWARNING, "tsMap::unRegisterData  parse name error!");
        return EINVCMD;
    }
    memcpy(tblName, pData, pStr- pData);
    
    idx = 0;
    /*解析注册数据的各项数字参数*/
    /*依次是regid,regsaid,qryid,qrysaid,qrylen*/
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
    /*查询字段qryStr*/
    qryStr = string(strlwr(pData+TSMAP_SPLIT_STR_LEN), args[4]);
    
    /*查找名字对应的表是否存在,不存在返回错误*/
    tblAddr = getTblAddress(tblName);
    if((NULL == tblAddr) || (tblAddr->empty()))
    {
        sysLoger(LINFO, "tsMap::unRegisterData  the table specified by name %s!is not exist", tblName);
        return ENEXIT;
    }

    sysLoger(LINFO, "tsMap::unRegisterData  unregister %s  %s ....", tblName, qryStr.data());
    
    /*如果表地址存在，则查找*/
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
        /*数据数据表中没有数据，则删除表并且从映射表mpName2Tbl
        中删除*/
        if(tblAddr->empty())
        {           
            /*从映射表mpName2Tbl中找到匹配项并删除*/
            map<char *, T_DATATBL*>::iterator m_N2TItr = mpName2Tbl.begin();
            while(m_N2TItr != mpName2Tbl.end())
            {
                if(m_N2TItr->second == tblAddr)
                {
                    
                    delete (m_N2TItr->first);                      //删除名字空间
                    delete (m_N2TItr->second);                //删除表空间
                    mpName2Tbl.erase(m_N2TItr);
                    break;
                }
                m_N2TItr++;
            }
            /*从属性表中删除表记录*/
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
 * @brief        :  tsmap类的查询函数
 * @param[IN]: data, 消息包内容
 * @param[IN]: len, 消息长度
 * @param[OUT]: ackData, 处理后的回复消息,由用户分配空间
 * @param[IN]: len, 回复消息空间的长度
 * @return      : 成功返回回复数据包的长度,失败返回具体错误值
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 16:30:24
 *  查询命令格式:
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

    /*解析名字*/
    pStr = strstr(pData, TSMAP_SPLIT_STR);
    if(NULL == pStr)
    {
        sysLoger(LWARNING, "tsMap::query  parse name error!");
        return EINVCMD;
    }
    memcpy(tblName, pData, pStr- pData);
    
    idx = 0;
    /*解析注册数据的各项数字参数*/
    /*依次是queryAid;querySaid;qrlen;*/
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
    /*查询字段qryStr*/
    *(pData+TSMAP_SPLIT_STR_LEN+args[2]) = '\0';
    qryStr = string(strlwr(pData+TSMAP_SPLIT_STR_LEN), args[2]);
    
    /*查找名字对应的表是否存在,不存在返回错误*/
    tblAddr = getTblAddress(tblName);
    if((NULL == tblAddr) || (tblAddr->empty()))
    {
        sysLoger(LINFO, "tsMap::query  table specified by name %s is not exist!", tblName);
        return ENEXIT;
    }

    /*如果表地址存在，则查找*/
    /*先查找准备匹配项*/
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
    
    /*没有找到，再查通配项id=0为通配*/
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

    /*找到则发送数据*/
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

    /*没有找到返回未找到匹配项错误*/
    
    return ENEXIT;
}


/**
 * @Funcname:  tsMap
 * @brief        : 通过注册名(表名)查找数据表地址
 * @param[IN]: tblName, 注册名，即表名
 * @return      : 没有找到表返回NULL，否则返回表地址
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月5日 10:30:24
 *   
**/
T_DATATBL* tsMap::getTblAddress(char *tblName)
{
    T_NM2ADDR::iterator itrNm2Tbl;

    if(NULL == tblName)
    {
        return NULL;
    }

    //从映射表中查找
    itrNm2Tbl = mpName2Tbl.begin();
    while(itrNm2Tbl != mpName2Tbl.end())
    {
        if(strcmp(itrNm2Tbl->first, tblName) == 0)
        {
            break;
        }
        itrNm2Tbl++;
    }
    
    //没找到返回空
    if(itrNm2Tbl == mpName2Tbl.end())
    {
        return NULL;
    }
    //找到，返回数据表地址
    return (itrNm2Tbl->second);
}

/**
 * @Funcname:  addNewTable
 * @brief        :  给定名字加入新表到映射表中
 * @param[IN]: tblName, 注册名，即表名
 * @return      : 添加失败返回NULL,  否则返回新表的地址
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月5日 11:36:01
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

    /*分配新表*/
    tblAddr = new T_DATATBL();
    if(NULL == tblAddr)
    {
        return NULL;
    }
    /*插入新表到映射表中*/
    mpName2Tbl[tblName] = tblAddr;
    return tblAddr;
    
}

/**
 * @Funcname:  getTableProperty
 * @brief        :  获取数据表是否允许注册多项数据的属性
 * @para1      : 
 * @para2      : 
 * @para3      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月7日 17:27:07
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

