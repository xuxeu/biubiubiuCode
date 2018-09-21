/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  main.cpp
* @brief  
*       功能：
*       <li>TSFS Server的主程序</li>
*/
/************************头文件********************************/
#include "stdafx.h"
#include <signal.h>
#include "tsfsCommon.h"

#pragma comment(lib, "tsapi")

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/
//全局Server对象
TSFS_Server *pTSFS_Server = NULL;

//连接句柄
US lnkId = 0; 

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/

/*
 * @brief
 *   程序入口，连接ts、向tsmap注册“TSFS”服务映射、初始化Server模块。
 * @param argc[in]: 初始化参数个数
 * @param argv[in]: 初始化参数
 * @return  正常退出,           TSFS_SERVER_EXIT
 *          初始化参数不完整,   TSFS_INVALID_INIT_ARGUMENT
 *          连接TS失败,         TSFS_CONNECT_TS_FAILED
 *          获取控制台句柄失败, TSFS_GET_HANDLE_FAILED
 *          创建Server失败,     TSFS_CREATE_SERVER_FAILED
 * 初始化参数格式:  -Name drvName -Root myroot -TS tsIP -Tgt tgtID -Log logLevel
 */
int _tmain(int argc, _TCHAR * argv[])
{
    //设置退出中断信号
    signal(SIGINT, tsfs_serverSignalExit);

    SetConsoleCtrlHandler(tsfs_serverExit,TRUE);
    
    char *pstr              = NULL;
    int paramNum            = 0;            //解析参数个数
    int ret                 = 0;            //发送接收数据包字节数
    unsigned short src_aid  = 0;            //接收数据包的源AID
    unsigned short des_said = 0;            //接收数据包的目的SAID
    unsigned short src_said = 0;            //接收数据包的源SAID
    char dataBuf[TSFS_DATA_SIZE];           //和TSMAP通信的数据包

    int targetId = 0;                       //用户配置的目标机ID
    char loagicDisk[MAX_PATH];              //用户配置的逻辑盘符名
    char userRoot[MAX_PATH];                //用户配置的根目录
    char tsIp[TSFS_IP4_LEN];                //用户配置的TSIP地址
    char paramBuf[1024];                //初始化参数
    LogLevel logLevel = LBUTT;              //日记标识

    memset(loagicDisk, 0, sizeof(loagicDisk));
    memset(userRoot, 0, sizeof(userRoot));
    memset(tsIp, 0, sizeof(tsIp));
    memset(paramBuf, 0, sizeof(paramBuf));
    memset(dataBuf, 0, sizeof(dataBuf));    

    //解析初始化参数
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
   
    //若初始化参数不完整,则退出,5:初始化参数个数
    if( paramNum < 5 )
    {
        return TSFS_INVALID_INIT_ARGUMENT;
    }

    //将用户配置的根目录中的*替换成空格
    while(pstr = strstr(userRoot, "*"))
    {
        *pstr = ' ';
    }    

    //将用户配置的根目录中'/'转换为'\'
    while( pstr = strstr((char*)userRoot, "/") )
    {
        *pstr = '\\';
    }

    //在用户配置的根目录添加':'
    if(strlen(userRoot) < (sizeof(userRoot)-2) 
        && !strstr((char*)userRoot, ":"))
    {
        strcat(userRoot, ":");
    }

    //在用户配置的根目录末尾添加'\'
    if( strlen(userRoot) < (sizeof(userRoot)-2) 
        &&  '\\' != userRoot[strlen(userRoot)-1] )
    {
        strcat(userRoot, "\\");
    } 
  
    //去掉用户配置的逻辑盘符名中的':'
    pstr = strstr(loagicDisk, ":");
    if(pstr)
    {
        *pstr = '\0';
    }

    //设置日志文件名
    char logFileName[MAX_PATH];
    memset(logFileName, 0, sizeof(logFileName));
    sprintf(logFileName, "TsfsServer_%s_%x.log" ,loagicDisk, targetId); 
    
    //打开日志
    SystemLog::getInstance(logLevel)->openLogFile(logFileName);

    sysLoger(LDEBUG, "main: initialize param Name:[%s]", loagicDisk);
    sysLoger(LDEBUG, "main: initialize param Root:[%s]", userRoot);
    sysLoger(LDEBUG, "main: initialize param TSIP:[%s]", tsIp);
    sysLoger(LDEBUG, "main: initialize param TgtID:[%d]", targetId);
    sysLoger(LDEBUG, "main: initialize param Log:[%d]", logLevel);

    //连接TS
    lnkId = (US)TClient_open(tsIp);
    if(lnkId<0)
    {
        //连接TS失败
        sysLoger(LWARNING, "main: TSFS Server connect TS failed, [%d]", lnkId);
        return TSFS_CONNECT_TS_FAILED;  
    }

    sysLoger(LINFO, "main: TSFS Server connect TS success, [id:%d]", lnkId);

#ifdef DEBUG
    printf("lnkId = %d\n",lnkId);
#endif

    //获取TSFS Server的PID
    DWORD consolePID = GetCurrentProcessId();
    if( !consolePID )
    {
        //获取TSFS Server的PID失败
        sysLoger(LWARNING, "main: TSFS Server get console PID failed");
        return TSFS_GET_PID_FAILED;
    }

    char consolePIDHex[8];
    sprintf(consolePIDHex,"%x",consolePID);
   
    //填写注册BUF，r:注册命令，tsfs::映射关系名，0:该映射名非唯一多映射
    sprintf(dataBuf, "r:tsfs;0;%x%s%x%s%x%s%x%s%x%s%s%s%x%s%s%s",   
            lnkId,                      TSFS_CONTROL_FLAG,  //TSFS Server ID
            0,                          TSFS_CONTROL_FLAG,  //TSFS Server SubID
            targetId,                   TSFS_CONTROL_FLAG,  //TA ID
            0,                          TSFS_CONTROL_FLAG,  //TA SubID
            strlen(loagicDisk),         TSFS_CONTROL_FLAG,  //逻辑盘符名长度
            loagicDisk,                 TSFS_CONTROL_FLAG,  //逻辑盘符名
            strlen(consolePIDHex),      TSFS_CONTROL_FLAG,  //TSFS Server PID长度
            consolePIDHex,              TSFS_CONTROL_FLAG); //TSFS Server PID             

    //向TSMAP注册
    ret = TClient_putpkt(lnkId, TSFS_TSMAP_ID, 0, 0, dataBuf, (UI)(strlen(dataBuf)));
    if( ret < 0 )
    {
        //发送注册数据包失败
        sysLoger(LWARNING, "main: TSFS Server send register data failed, [%d]",
                            ret);
        return TSFS_SEND_REGDATA_FAILED;   
    }

    ret = TClient_getpkt(lnkId, &src_aid, &des_said, &src_said, dataBuf, (UI)(strlen(dataBuf)));
    if ( NULL == strstr(dataBuf, TSFS_SUCCESS) )
    {
        //注册失败
        sysLoger(LWARNING, "main: TSFS Server register failed, [ans: %s], [ret: %d]",
                            dataBuf, ret);
        return TSFS_REGISTER_FAILED;
    }

    sysLoger(LINFO, "main: TSFS Server register success");

    //创建Server
    pTSFS_Server = new TSFS_Server( lnkId, tsIp, userRoot );
    if(!pTSFS_Server)
    {
        //创建Server失败
        sysLoger(LWARNING, "main: TSFS Server create server failed");
        return TSFS_CREATE_SERVER_FAILED;
    }

    sysLoger(LINFO, "main: TSFS Server initialize successful");

#ifdef DEBUG
    printf("start server\n");
#endif

    //启动Server
    pTSFS_Server->main();

    //关闭Server
    if(NULL != pTSFS_Server)
    {
        delete pTSFS_Server;
        pTSFS_Server = NULL;
    }

    //重新连接TS,向TSMAP注销
    US uLnkId = (US)TClient_open(tsIp);
    if( uLnkId < 0 )
    {
        sysLoger(LWARNING, "main: TSFS Server did not logout, [%d]", uLnkId);
        return TSFS_DID_NOT_LOGOUT;
    }

    //填充注销数据包
    memset(dataBuf, 0, sizeof(dataBuf));
    sprintf(dataBuf, "u:tsfs;%x%s%x%s%x%s%x%s%x%s%s%s",   
            lnkId,                      TSFS_CONTROL_FLAG,  //TSFS Server ID
            0,                          TSFS_CONTROL_FLAG,  //TSFS Server SubID
            targetId,                   TSFS_CONTROL_FLAG,  //TA ID
            0,                          TSFS_CONTROL_FLAG,  //TA SubID
            strlen(loagicDisk),         TSFS_CONTROL_FLAG,  //逻辑盘符名长度
            loagicDisk,                 TSFS_CONTROL_FLAG); //逻辑盘符名

    //向TSMAP注销
    ret = TClient_putpkt(uLnkId, TSFS_TSMAP_ID, 0, 0, dataBuf, (UI)(strlen(dataBuf)));
    if( ret < 0 )
    {
        //发送注销数据包失败
        sysLoger(LWARNING, "main: TSFS Server send logout data failed, [%d]",
                            ret);
        return TSFS_SEND_LOGOUT_DATA_FAILED;   
    }

    ret = TClient_getpkt(uLnkId, &src_aid, &des_said, &src_said, dataBuf, (UI)(strlen(dataBuf)));
    if( strstr(dataBuf, TSFS_SUCCESS) == NULL )
    {
        //注销失败
        sysLoger(LWARNING, "main: TSFS Server logout failed, [ans:%s]", dataBuf);
        return TSFS_LOGOUT_FAILED;             
    }
    
    sysLoger(LINFO, "main: TSFS Server logout success"); 

    //关闭连接
    TClient_close(uLnkId);

    sysLoger(LINFO, "main: TSFS Server exit");
    return TSFS_SERVER_EXIT;
}

/**
 * @brief
 *   退出TSFS Server
 * @param CtrlType[in]:    关闭信号
 */
BOOL WINAPI tsfs_serverExit( DWORD CtrlType )
{
    switch(CtrlType)
    {
        case CTRL_CLOSE_EVENT:  //停止TSFS_Server 
            tsfs_serverSignalExit( SIGINT );
            break;
    }
    Sleep(600);
    return FALSE;
}

/**
 * @brief
 *   退出TSFS Server
 * @param signal[in] :信号,未使用
 */
void tsfs_serverSignalExit( int signal )
{
    //停止TSFS_Server            
    if( NULL != pTSFS_Server)
    {
        pTSFS_Server->stop();
    }      

    //关闭TS连接
    TClient_close(lnkId);
}
