/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  ownerarchive.cpp
* @brief
*       功能：
*       <li>TS数据包的管理和包的结构,用于序列话操作</li>
*/


#ifndef OWNERARCHIVE__INCLUDED_
#define OWNERARCHIVE__INCLUDED_

/************************头文件********************************/
#include <QDataStream>  //QT库对应的序列化对象 

/************************宏定义********************************/
/************************类型定义******************************/

class OwnerArchive
{
private:
public:
    QDataStream *pDataStream;       //对应的数据
    //有2种状态,load从缓冲区读取数据.storing是往缓冲区写数据
    enum ARCHIVESTATUS
    { 
        load = 0,
        storing
     };
    ARCHIVESTATUS status;

    OwnerArchive(ARCHIVESTATUS c)
    {
        status = c;
    }
    ~OwnerArchive()
    {
    }
    /**
    * 设置数据对象
    * @param socketDescriptor socket号
    */
    void setDataStream(QDataStream*  pDataStream)
    {
        this->pDataStream = pDataStream;
    }
    void changeMode(ARCHIVESTATUS c)
    {
        status = c;
    }
    bool IsStoring()
    {
        return status == storing;
    };
};
/************************接口声明******************************/

#endif // !defined(OWNERARCHIVE__INCLUDED_)
