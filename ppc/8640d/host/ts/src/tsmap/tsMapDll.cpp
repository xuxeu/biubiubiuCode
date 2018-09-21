/*
* ������ʷ��
* 2007-11-02 qingxiaohai  ���������������޹�˾
*                           �������ļ���
*/

/**
* @file  tsMapDll.cpp
* @brief
*       ���ܣ�
*       <li>TSMAP DLL�ӿڲ�</li>
*/



/************************ͷ�ļ�********************************/
#include <iostream>
#include "tsmapComm.h"
#include "tsMapServer.h"
#include "common.h"

using namespace std;

/************************�궨��********************************/
#define FUNC_INT extern "C" __declspec(dllexport) int
#define FUNC_VOID extern "C" __declspec(dllexport) void

/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
static UI tsmapID = 0;

static bool isInit = false;

tsMap *gMapServer = NULL;


/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

/*
*��̬���ʼ�����
*/
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
    return TRUE;
}

/**
 * @Funcname:  open
 * @brief        :  ��ts�ṩ�ļ��̬��ӿ�
 * @para[IN]  : hwId, ��̬����TS�е�ID��
 * @para[IN]  : cpBuf, �������,����ֻ���Ƿ��¼��־
 * @para[OUT]: cpOutBuf, �ݲ�ʹ��
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��11��2�� 15:11:43
 *
**/
FUNC_INT  open(US hwId, char* cpBuf, char* cpOutBuf)
{
    char *pBuf = cpBuf;
    int isLog = LBUTT;

    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if(isInit)
    {
        return SUCC;
    }

    /*��¼����ID��*/
    tsmapID = hwId;

    hex2int(&pBuf, &isLog);

    /*����ϵͳ��־*/
    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tsmap.log"));

    gMapServer = new tsMap();

    if(NULL == gMapServer)
    {
        return FAIL;
    }

    isInit = true;
    return SUCC;

}

/**
 * @Funcname:  close
 * @brief        :  ��ts�ṩ�Ĺرն�̬��Ľӿ�
 * @para[IN]  : hwId, ��̬����TS�е�ID��
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��11��2�� 15:14:43
 *
**/

FUNC_INT close(US hwId)
{

    if((isInit) && (tsmapID == hwId))
    {
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
        /*  Modified brief: ����ֹͣTSMAP*/
        gMapServer->stop();
        Sleep(10);           //��ʱ10����,ȷ��getpktִ�����
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/
        delete gMapServer;
        gMapServer = NULL;
        return SUCC;
    }

    return FAIL;
}

/**
 * @Funcname:  putpkt
 * @brief        :  ��ts�ṩ�ķ����ӿ�,TS���øýӿڽ����ݷ��͵�tsmap
 * @para[IN]  : vwServerId, ��̬����TS�е�ID��
 * @para[IN]  : vhwDesSaid, ��̬���еĶ�������,��ʱ����
 * @para[IN]  : vhwSrcAid, ԴID��
 * @para[IN]  : vwSrcSaid, Դ����ID��
 * @para[IN]  : cpBuf, ����
 * @para[IN]  : vwSize, ���ݴ�С
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��11��2�� 15:14:43
 *
**/
FUNC_INT putpkt(US vwServerId, US vhwDesSaid,
                US vhwSrcAid, US vwSrcSaid,
                char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsmapID) || (false == isInit))
    {
        return FAIL;
    }

    return gMapServer->putpkt(vhwSrcAid, vwSrcSaid, cpBuf, vwSize);
}

/**
 * @Funcname:  getpkt
 * @brief        :  ��ts�ṩ���հ��ӿ�, TS���øýӿڴ�tsmap��������
 * @para[IN]  : vwServerId, ��̬����TS�е�ID��
 * @para[IN]  : vwpDesAid, Ŀ��ID��
 * @para[IN]  : vwpDesSaid, Ŀ�Ķ���ID��
 * @para[IN]  : vwpSrcSaid, Դ����ID��
 * @para[IN]  : cpBuf, ���ݻ���
 * @para[IN]  : vwSize, ���ݻ����С
 * @return      : �ɹ����ؽ��յ������ݵĴ�С,ʧ�ܷ���FAIL
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��11��2�� 15:14:43
 *
**/
FUNC_INT getpkt(US vwServerId, US *vwpDesAid,
                US *vwpDesSaid, US *vwpSrcSaid,
                char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsmapID) || (false == isInit))
    {
        return FAIL;
    }

    *vwpSrcSaid = 0;

    return gMapServer->getpkt(vwpDesAid, vwpDesSaid, cpBuf, vwSize);

}



