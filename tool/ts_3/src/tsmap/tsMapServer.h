/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsMapServer.h
 * @Version        :  1.0.0
 * @brief           :  *   <li> 功能</li> tsmap主体,处理关联数据的存储和查询等操作
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  qingxiaohai
 * @create date:  2007年11月2日 15:56:35
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
mpName2Tbl      映射表存储                T_MapTbl,   数据表是否允许                      T_MapData    详细的数据项
                           数据表名                                        存储多项数据
                          到数据表                                          具体数据的地址
                         地址的映射
|------------------------------|       |-----------------------------------|    |-------------------------------------------------------------|
| tblName    |     tblAdrr     |    /  | exclusive  |   tblData            |   /|regaid |regsaid | qryAid | qrySaid | qryStr | datalen | data |
|------------------------------|   /   |-----------------------------------|  / |------------------------------------------------------------ |
|tsfs        |    0x00003456   |  /    |    0       |    0x00005678        | /  |  3    |  0     |  4     |    0    |  c     |  0      |      |  
|------------------------------|       |-----------------------------------|    |-------------------------------------------------------------|

*/

#ifndef _TSMAP_BASE_TYPE_
#define _TSMAP_BASE_TYPE_
/*具体的数据表*/
typedef struct
{
    int         regAid;                 //注册方ID
    int         regSaid;                //注册方SAID
    int         qryAid;                 //查询方ID
    int         qrySaid;                //查询方SAID
    string    qryStr;                   //查询条件
    int         dataLen;                //用户数据长度
    char*    data;                      //存储用户数据
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
    T_NM2ADDR                      mpName2Tbl;                   //数据表,注册名到数据表之间的映射表
    T_ADDR2PRO                    mpTAddr2Pro;                   //属性表,用数据表的地址作为索引项，是否允许注册多条数据作为数据项
    char                                  tmpData[2048];                //回复数据
    int                                     tmpDataLen;                   //回复数据的长度
    TMutex                             mtxBuf;                            //操作回复数据的互斥
    waitCondition                   wtData;                            //等待回复数据的信号量                    
    int                                    curSrcAid;                       //当前命令包的源AID号
    int                                    curSrcSaid;                     //当前命令包的源said号


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
    /*  Modified brief: 增加停止TSMAP*/
    void stop();
    /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/

    int putpkt(US srcAid, US srcSaid, char *data, int len);

    int getpkt(US *desAid, US *desSaid, char *buf, int size);
    
};

#endif

