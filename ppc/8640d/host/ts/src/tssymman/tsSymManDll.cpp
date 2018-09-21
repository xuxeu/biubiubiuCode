/*
* ������ʷ��
* 2008-09-23 zhangxu  ���������������޹�˾
*                       �������ļ���
*/

/**
* @file  tsSymManDll.cpp
* @brief
*       ���ܣ�
*       <li>tssymman DLL�ӿڲ�</li>
*/

/************************ͷ�ļ�********************************/
#include "afx.h"
#include <direct.h>
#include <windows.h>
#include "string.h"
#include "tsSymManComm.h"
#include "tsSymManServer.h"

using namespace std;

/************************�궨��********************************/
#define DLL_FUNC_INT extern "C" __declspec(dllexport) int
#define DLL_FUNC_VOID extern "C" __declspec(dllexport) void

/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
static T_UWORD tsSymManID = 0;

static bool isInit = false;

tsSymMan *gSymManServer = NULL;

/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

/**��̬���ʼ�����
BOOL APIENTRY DllMain( HANDLE hModule,
                                           DWORD  ul_reason_for_call,
                                           LPVOID lpReserved)
{
    return TRUE;
}
*/

/**
 * @Funcname:  open
 * @brief        :  ��ts�ṩ�ļ��̬��ӿ�
 * @para[IN]  : hwId, ��̬����TS�е�ID��
 * @para[IN]  : cpBuf, �������,����ֻ���Ƿ��¼��־
 * @para[OUT]: cpOutBuf, �ݲ�ʹ��
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 14:57:55
 *
**/
DLL_FUNC_INT  open(T_UHWORD hwId, char* cpBuf, char* cpOutBuf)
{
    char *pBuf = cpBuf;
    char *pTmp = NULL;
    char pstrPath[MAX_PATH_LEN] = "\0";
    int ret = 0;
    int isLog = LDEBUG;

    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if(isInit)
    {
        return SUCC;
    }

    /*��¼����ID��*/
    tsSymManID = hwId;

    hex2int(&pBuf, &isLog);

    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tssymman.log"));

#ifndef SYMDLL_DEBUG
    /*��ȡ��ǰϵͳ·��*/
    ret = GetModuleFileName(NULL, pstrPath, MAX_PATH_LEN);

    if(0 == ret)
    {
        return FAIL;
    }

#else
    strcpy(pstrPath, "e:\\LambdaAE_1216\\host\\pub\\ts.exe");
#endif
    pTmp = strstr(pstrPath, "ts.exe");

    if(NULL == pTmp)
    {
        return FAIL;
    }

    *pTmp = '\0';
    strcat(pstrPath, "symbolFiles");

    ret = CheckFolderExist(pstrPath);

    if (FALSE == ret)
    {
        _mkdir(pstrPath);
    }

    /*ʵ����tsSymMan����*/
    gSymManServer = new tsSymMan(pstrPath);

    if(NULL == gSymManServer)
    {
        sysLoger(LWARNING, "create tsSymMan fail");
        return FAIL;
    }

    isInit = true;

    sysLoger(LINFO, "init tsSymMan succ, aid:%d", tsSymManID);
    return SUCC;
}

/**
 * @Funcname:  close
 * @brief        :  ��ts�ṩ�Ĺرն�̬��Ľӿ�
 * @para[IN]  : hwId, ��̬����TS�е�ID��
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:34:30
 *
**/

DLL_FUNC_INT close(T_UHWORD hwId)
{

    if((isInit) && (tsSymManID == hwId))
    {
        gSymManServer->stop();
        Sleep(10);           //��ʱ10����,ȷ��getpktִ�����
        delete gSymManServer;
        gSymManServer = NULL;
        sysLoger(LINFO, "close tsSymMan succ");
        return SUCC;
    }

    return FAIL;
}

/**
 * @Funcname:  putpkt
 * @brief       :  ��ts�ṩ�ķ����ӿ�,TS���øýӿڽ����ݷ��͵�tssymman
 * @para[IN]  : vwServerId, ��̬����TS�е�ID��
 * @para[IN]  : vhwDesSaid, ��̬���еĶ�������,��ʱ����
 * @para[IN]  : vhwSrcAid, ԴID��
 * @para[IN]  : vwSrcSaid, Դ����ID��
 * @para[IN]  : cpBuf, ����
 * @para[IN]  : vwSize, ���ݴ�С
 * @return      : �ɹ�����SUCC,ʧ�ܷ���FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:38:45
 *
**/
DLL_FUNC_INT putpkt(T_UHWORD vwServerId, T_UHWORD vhwDesSaid,
                    T_UHWORD vhwSrcAid, T_UHWORD vwSrcSaid,
                    char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsSymManID) || (false == isInit))
    {
        return FAIL;
    }

    return gSymManServer->putpkt(vhwSrcAid, vwSrcSaid, cpBuf, vwSize);
}

/**
 * @Funcname:  getpkt
 * @brief       :  ��ts�ṩ���հ��ӿ�, TS���øýӿڴ�tssymman��������
 * @para[IN]  : vwServerId, ��̬����TS�е�ID��
 * @para[IN]  : vwpDesAid, Ŀ��ID��
 * @para[IN]  : vwpDesSaid, Ŀ�Ķ���ID��
 * @para[IN]  : vwpSrcSaid, Դ����ID��
 * @para[IN]  : cpBuf, ���ݻ���
 * @para[IN]  : vwSize, ���ݻ����С
 * @return    : �ɹ����ؽ��յ������ݵĴ�С,ʧ�ܷ���FAIL
 * @Author    :  zhangxu
 *
 * @History: 1.  Created this function on 2008��9��23�� 15:43:03
 *
**/
DLL_FUNC_INT getpkt(T_UHWORD vwServerId, T_UHWORD *vwpDesAid,
                    T_UHWORD *vwpDesSaid, T_UHWORD *vwpSrcSaid,
                    char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsSymManID) || (false == isInit))
    {
        return FAIL;
    }

    *vwpSrcSaid = 0;

    return gSymManServer->getpkt(vwpDesAid, vwpDesSaid, cpBuf, vwSize);

}
