/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  main.cpp
* @brief  
*       ���ܣ�
*       <li>TSFS Server��������</li>
*/
/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include <signal.h>
#include "tsfsCommon.h"

#pragma comment(lib, "tsapi")

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/
//ȫ��Server����
TSFS_Server *pTSFS_Server = NULL;

//���Ӿ��
US lnkId = 0; 

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/

/*
 * @brief
 *   ������ڣ�����ts����tsmapע�ᡰTSFS������ӳ�䡢��ʼ��Serverģ�顣
 * @param argc[in]: ��ʼ����������
 * @param argv[in]: ��ʼ������
 * @return  �����˳�,           TSFS_SERVER_EXIT
 *          ��ʼ������������,   TSFS_INVALID_INIT_ARGUMENT
 *          ����TSʧ��,         TSFS_CONNECT_TS_FAILED
 *          ��ȡ����̨���ʧ��, TSFS_GET_HANDLE_FAILED
 *          ����Serverʧ��,     TSFS_CREATE_SERVER_FAILED
 * ��ʼ��������ʽ:  -Name drvName -Root myroot -TS tsIP -Tgt tgtID -Log logLevel
 */
int _tmain(int argc, _TCHAR * argv[])
{
    //�����˳��ж��ź�
    signal(SIGINT, tsfs_serverSignalExit);

    SetConsoleCtrlHandler(tsfs_serverExit,TRUE);
    
    char *pstr              = NULL;
    int paramNum            = 0;            //������������
    int ret                 = 0;            //���ͽ������ݰ��ֽ���
    unsigned short src_aid  = 0;            //�������ݰ���ԴAID
    unsigned short des_said = 0;            //�������ݰ���Ŀ��SAID
    unsigned short src_said = 0;            //�������ݰ���ԴSAID
    char dataBuf[TSFS_DATA_SIZE];           //��TSMAPͨ�ŵ����ݰ�

    int targetId = 0;                       //�û����õ�Ŀ���ID
    char loagicDisk[MAX_PATH];              //�û����õ��߼��̷���
    char userRoot[MAX_PATH];                //�û����õĸ�Ŀ¼
    char tsIp[TSFS_IP4_LEN];                //�û����õ�TSIP��ַ
    char paramBuf[1024];                //��ʼ������
    LogLevel logLevel = LBUTT;              //�ռǱ�ʶ

    memset(loagicDisk, 0, sizeof(loagicDisk));
    memset(userRoot, 0, sizeof(userRoot));
    memset(tsIp, 0, sizeof(tsIp));
    memset(paramBuf, 0, sizeof(paramBuf));
    memset(dataBuf, 0, sizeof(dataBuf));    

    //������ʼ������
    int idx = 0;
    for(int i = 1; i < argc; ++i)
    {
        idx += sprintf(paramBuf + idx, "%s ", argv[i]);
    }
    
    paramNum = sscanf(paramBuf, "-Name %s -Root %s -TS %s -Tgt %x -Log %x" ,
                                    &loagicDisk,
                                    &userRoot,
                                    &tsIp,
                                    &targetId,
                                    &logLevel);
   
    //����ʼ������������,���˳�,5:��ʼ����������
    if( paramNum < 5 )
    {
        return TSFS_INVALID_INIT_ARGUMENT;
    }

    //���û����õĸ�Ŀ¼�е�*�滻�ɿո�
    while(pstr = strstr(userRoot, "*"))
    {
        *pstr = ' ';
    }    

    //���û����õĸ�Ŀ¼��'/'ת��Ϊ'\'
    while( pstr = strstr((char*)userRoot, "/") )
    {
        *pstr = '\\';
    }

    //���û����õĸ�Ŀ¼���':'
    if(strlen(userRoot) < (sizeof(userRoot)-2) 
        && !strstr((char*)userRoot, ":"))
    {
        strcat(userRoot, ":");
    }

    //���û����õĸ�Ŀ¼ĩβ���'\'
    if( strlen(userRoot) < (sizeof(userRoot)-2) 
        &&  '\\' != userRoot[strlen(userRoot)-1] )
    {
        strcat(userRoot, "\\");
    } 
  
    //ȥ���û����õ��߼��̷����е�':'
    pstr = strstr(loagicDisk, ":");
    if(pstr)
    {
        *pstr = '\0';
    }

    //������־�ļ���
    char logFileName[MAX_PATH];
    memset(logFileName, 0, sizeof(logFileName));
    sprintf(logFileName, "TsfsServer_%s_%x.log" ,loagicDisk, targetId); 
    
    //����־
    SystemLog::getInstance(logLevel)->openLogFile(logFileName);

    sysLoger(LDEBUG, "main: initialize param Name:[%s]", loagicDisk);
    sysLoger(LDEBUG, "main: initialize param Root:[%s]", userRoot);
    sysLoger(LDEBUG, "main: initialize param TSIP:[%s]", tsIp);
    sysLoger(LDEBUG, "main: initialize param TgtID:[%d]", targetId);
    sysLoger(LDEBUG, "main: initialize param Log:[%d]", logLevel);

    //����TS
    lnkId = (US)TClient_open(tsIp);
    if(lnkId<0)
    {
        //����TSʧ��
        sysLoger(LWARNING, "main: TSFS Server connect TS failed, [%d]", lnkId);
        return TSFS_CONNECT_TS_FAILED;  
    }

    sysLoger(LINFO, "main: TSFS Server connect TS success, [id:%d]", lnkId);

#ifdef DEBUG
    printf("lnkId = %d\n",lnkId);
#endif

    //��ȡTSFS Server��PID
    DWORD consolePID = GetCurrentProcessId();
    if( !consolePID )
    {
        //��ȡTSFS Server��PIDʧ��
        sysLoger(LWARNING, "main: TSFS Server get console PID failed");
        return TSFS_GET_PID_FAILED;
    }

    char consolePIDHex[8];
    sprintf(consolePIDHex,"%x",consolePID);
   
    //��дע��BUF��r:ע�����tsfs::ӳ���ϵ����0:��ӳ������Ψһ��ӳ��
    sprintf(dataBuf, "r:tsfs;0;%x%s%x%s%x%s%x%s%x%s%s%s%x%s%s%s",   
            lnkId,                      TSFS_CONTROL_FLAG,  //TSFS Server ID
            0,                          TSFS_CONTROL_FLAG,  //TSFS Server SubID
            targetId,                   TSFS_CONTROL_FLAG,  //TA ID
            0,                          TSFS_CONTROL_FLAG,  //TA SubID
            strlen(loagicDisk),         TSFS_CONTROL_FLAG,  //�߼��̷�������
            loagicDisk,                 TSFS_CONTROL_FLAG,  //�߼��̷���
            strlen(consolePIDHex),      TSFS_CONTROL_FLAG,  //TSFS Server PID����
            consolePIDHex,              TSFS_CONTROL_FLAG); //TSFS Server PID             

    //��TSMAPע��
    ret = TClient_putpkt(lnkId, TSFS_TSMAP_ID, 0, 0, dataBuf, (UI)(strlen(dataBuf)));
    if( ret < 0 )
    {
        //����ע�����ݰ�ʧ��
        sysLoger(LWARNING, "main: TSFS Server send register data failed, [%d]",
                            ret);
        return TSFS_SEND_REGDATA_FAILED;   
    }

    ret = TClient_getpkt(lnkId, &src_aid, &des_said, &src_said, dataBuf, (UI)(strlen(dataBuf)));
    if ( NULL == strstr(dataBuf, TSFS_SUCCESS) )
    {
        //ע��ʧ��
        sysLoger(LWARNING, "main: TSFS Server register failed, [ans: %s], [ret: %d]",
                            dataBuf, ret);
        return TSFS_REGISTER_FAILED;
    }

    sysLoger(LINFO, "main: TSFS Server register success");

    //����Server
    pTSFS_Server = new TSFS_Server( lnkId, tsIp, userRoot );
    if(!pTSFS_Server)
    {
        //����Serverʧ��
        sysLoger(LWARNING, "main: TSFS Server create server failed");
        return TSFS_CREATE_SERVER_FAILED;
    }

    sysLoger(LINFO, "main: TSFS Server initialize successful");

#ifdef DEBUG
    printf("start server\n");
#endif

    //����Server
    pTSFS_Server->main();

    //�ر�Server
    if(NULL != pTSFS_Server)
    {
        delete pTSFS_Server;
        pTSFS_Server = NULL;
    }

    //��������TS,��TSMAPע��
    US uLnkId = (US)TClient_open(tsIp);
    if( uLnkId < 0 )
    {
        sysLoger(LWARNING, "main: TSFS Server did not logout, [%d]", uLnkId);
        return TSFS_DID_NOT_LOGOUT;
    }

    //���ע�����ݰ�
    memset(dataBuf, 0, sizeof(dataBuf));
    sprintf(dataBuf, "u:tsfs;%x%s%x%s%x%s%x%s%x%s%s%s",   
            lnkId,                      TSFS_CONTROL_FLAG,  //TSFS Server ID
            0,                          TSFS_CONTROL_FLAG,  //TSFS Server SubID
            targetId,                   TSFS_CONTROL_FLAG,  //TA ID
            0,                          TSFS_CONTROL_FLAG,  //TA SubID
            strlen(loagicDisk),         TSFS_CONTROL_FLAG,  //�߼��̷�������
            loagicDisk,                 TSFS_CONTROL_FLAG); //�߼��̷���

    //��TSMAPע��
    ret = TClient_putpkt(uLnkId, TSFS_TSMAP_ID, 0, 0, dataBuf, (UI)(strlen(dataBuf)));
    if( ret < 0 )
    {
        //����ע�����ݰ�ʧ��
        sysLoger(LWARNING, "main: TSFS Server send logout data failed, [%d]",
                            ret);
        return TSFS_SEND_LOGOUT_DATA_FAILED;   
    }

    ret = TClient_getpkt(uLnkId, &src_aid, &des_said, &src_said, dataBuf, (UI)(strlen(dataBuf)));
    if( strstr(dataBuf, TSFS_SUCCESS) == NULL )
    {
        //ע��ʧ��
        sysLoger(LWARNING, "main: TSFS Server logout failed, [ans:%s]", dataBuf);
        return TSFS_LOGOUT_FAILED;             
    }
    
    sysLoger(LINFO, "main: TSFS Server logout success"); 

    //�ر�����
    TClient_close(uLnkId);

    sysLoger(LINFO, "main: TSFS Server exit");
    return TSFS_SERVER_EXIT;
}

/**
 * @brief
 *   �˳�TSFS Server
 * @param CtrlType[in]:    �ر��ź�
 */
BOOL WINAPI tsfs_serverExit( DWORD CtrlType )
{
    switch(CtrlType)
    {
        case CTRL_CLOSE_EVENT:  //ֹͣTSFS_Server 
            tsfs_serverSignalExit( SIGINT );
            break;
    }
    Sleep(600);
    return FALSE;
}

/**
 * @brief
 *   �˳�TSFS Server
 * @param signal[in] :�ź�,δʹ��
 */
void tsfs_serverSignalExit( int signal )
{
    //ֹͣTSFS_Server            
    if( NULL != pTSFS_Server)
    {
        pTSFS_Server->stop();
    }      

    //�ر�TS����
    TClient_close(lnkId);
}
