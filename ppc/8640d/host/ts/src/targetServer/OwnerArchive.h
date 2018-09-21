/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  ownerarchive.cpp
* @brief
*       ���ܣ�
*       <li>TS���ݰ��Ĺ���Ͱ��Ľṹ,�������л�����</li>
*/


#ifndef OWNERARCHIVE__INCLUDED_
#define OWNERARCHIVE__INCLUDED_

/************************ͷ�ļ�********************************/
#include <QDataStream>  //QT���Ӧ�����л����� 

/************************�궨��********************************/
/************************���Ͷ���******************************/

class OwnerArchive
{
private:
public:
    QDataStream *pDataStream;       //��Ӧ������
    //��2��״̬,load�ӻ�������ȡ����.storing����������д����
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
    * �������ݶ���
    * @param socketDescriptor socket��
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
/************************�ӿ�����******************************/

#endif // !defined(OWNERARCHIVE__INCLUDED_)
