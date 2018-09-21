/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  TClientAPI.h
* @brief
*       功能：
*       <li>对TClient对外接口</li>
*/


#ifndef TCLIENT_INCLUDE
#define TCLIENT_INCLUDE

/************************头文件********************************/

/************************宏定义********************************/
#define UC      unsigned char
#define US      unsigned short
#define UI       unsigned int

//函数动态库声明
#define DLL_FUNC_INT extern "C" __declspec(dllexport) int
#define DLL_FUNC_SHORT extern "C" __declspec(dllexport) short
#define DLL_FUNC_VOID extern "C" __declspec(dllexport) void

const int LISTEN_PORT = 8000;    //ts监听TClient的端口
#define SERVER_WAY_TCP          0                                //tcp
#define SERVER_WAY_UDP            1                                //udp
#define SERVER_WAY_UART            2                                //串口
#define SERVER_WAY_USB          3                                //usb
#define SERVER_WAY_DLL          4                                //动态库

#define REPORT_LOCATION_SIZE    16                                //包状态大小
#define REPORT_MSG_SIZE         128                                //包消息大小
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
/*  Modified brief: 因串口号上限256，将串口名称长度改为7位*/
#define UART_NAME_BUF_LEN        7                               //串口名称长度
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/
#define IP_BUF_LEN              16                              //IP地址
#define DLL_BUF                 800                                //传递给动态库的字符串,以分割符号','来分割的
/*Modified by tangxp for BUG No.0002259 on 2007.9.25 [begin]*/
/*  Modified brief:将路径长度改为WINDOWS路径长度上限MAX_PATH(260)*/
#define NAME_SIZE               MAX_PATH                                //名字的大小
/*Modified by tangxp for BUG No.0002259 on 2007.9.25 [end]*/
#define EXT_INFO_LEN         128                                //srvcfg扩展字段，用于记录Arch等
#define GUID_LEN                 39
#define MAX_CHANNEL_COUNT        10                                //通道最大数

#define MAX_REG_TYPE            254                             //最大允许的注册类型值 255认为是无效
#define PACKET_DATASIZE         (1008+3*1024)                    //包数据大小
#define PACKET_HEADSIZE         16                                //包头大小

//返回值定义
#define TS_SUCCESS                              0                   //调用成功
#define TS_DATACOMING                           0                  //有数据到来

#define TS_SOCKET_ERROR                         -1                 //socket错误
#define TS_UNKNOW_ERROR                         -2                 //一般错误
#define TS_SYSTEM_ERROR                         -3                 /*可能是对话管理映射表错误，也可能是多线程调用API引起的错误*/
#define TS_LAUNCH_TS_FAILURE               -4                 //加载TS失败

#define TS_UNKNOW_DATA                          -6                 //收到未知消息，不是这次期望的消息，可能是上次回复的消息等。
#define TS_NULLPTR_ENCONTERED                   -7                 //参数出现空指针
#define TS_FILE_NOT_EXIST                       -8                 //ts, 或者DLL 等文件不存在
#define TS_INVALID_LINK_HANDLE                  -9                 //无效的连接句柄
#define TS_INVALID_SERVICE_ID                   -10                //无效的服务ID号
#define TS_TIME_OUT                             -11                //超时
#define TS_INVALID_FILE_NAME                    -12                //无效文件名  :如超时定义的最大支持100字符的文件名
#define TS_DEVICE_CANNOT_USE                    -13                //设备目前不可用。
#define TS_REC_BUF_LIMITED                      -14                 //接收到的数据大于接收缓冲区大小
#define TS_RECV_DATA_ERROR                         -15                 //接收数据时发生错误
#define TS_INVALID_SERVICE_NAME                 -16                 //非法的服务名字
#define TS_UNKNOW_DATA_FORMAT                   -17                 //收到的数据包是非法格式
#define TS_INVALID_CH_COUNT                     -18                 //非法的通道总数
#define TS_INVALID_CH_ID                        -19                 //非法的通道号
#define TS_INVALID_IP                           -20                 //非法的IP
#define TS_INVALID_UART_PORT                    -21                 //非法的串口
#define TS_SERVICE_NOT_EXIST                    -22                 //指定的服务不存在,
#define TS_SERVICE_NAME_REPEATED                -23                 //创建重名,      
#define TS_UNKNOW_COMM_WAY                      -24                 //未知的通信方式
#define TS_UNKNOW_PROTOCOL                      -25                 //未知的通信协议         
#define TS_FILE_ALREADY_EXITS                      -26                //文件已经存在(文件上传)
#define TS_SRV_CFG_NOT_ENOUGH                   -27                 //服务配置不全
/*Modifide by tangxp on 2007.9.26 [begin]*/
/*  Modified brief:增加GUID错误类型 */
#define TS_INVALID_GUID                         -28                 //GUID错误
/*Modifide by tangxp on 2007.9.26 [end] */
#define TS_ISNOT_TS_SERVER                      -29                 //对端不是TS服务器                                                                   
#define TS_SRV_NUM_EXPIRE                       -30                 //创建的服务已经达到最大数量       
#define TS_DLL_BUF_LOST                         -31                 //DLL服务的配置缓冲为空
#define TS_CH_SW_FUNC_LOCKED_BY_OHTER    -32                 //通道切换功能被其他锁定了
#define TS_SERVICE_CONFIG_ERR               -33               //服务配置错误,如DLL服务配置的缓冲溢出等
#define TS_INVALID_IP_PORT                      -34              //无效的IP端口
#define TS_INVALID_DLL_NAME                   -35             //无效的DLL文件名                   
#define TS_SEND_DATA_FAILURE                 -36             //发送消息失败
#define TS_CHANGE_CHANNEL_FAILURE     -37             //通道切换失败
#define TS_SRV_CHANNEL_SWITCH_DISABLE     -38           //通道切换不允许
#define TS_INVALID_REG_TYPE                    -39              //无效的注册类型

//文件传送过程状态
#define TS_FILE_BEGIN                              1           //文件传送开始
#define TS_FILE_CONTINUE                       2           //文件传送中
#define TS_FILE_END                                  3          //文件传送完
#define TS_FILE_BEGIN_END                      4         //文件比较小，第一个包就可以传送完

/************************类型定义******************************/

//定义BOOL类型
//WINDOWS.h里定义了FALSE/TRUE为宏，这里消除重定义
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
enum __BOOL
{
    FALSE,
    TRUE
};
typedef __BOOL _BOOL;

//通信方式及协议方式的类型
enum _PROTOCOL
{
    TCP,
    UDP,
    UART,
    USB,
    DLL,
    BUTT = 255
};
typedef _PROTOCOL PROTOCOL;

//TS系统日志的输出级别
#ifndef LOGLEVE
#define LOGLEVE
enum _LogLevel
{
    LDEBUG,
    LINFO,
    LWARNING,
    LERROR,
    LFATAL,
    LBUTT = 255
};
typedef _LogLevel LogLevel;
#endif

typedef struct COMCONFIG
{
    PROTOCOL way;                                    //通信方式的选择,0表示tcp方式,1表示udp方式,2表示串口,4标识动态库方式
    PROTOCOL protocolType;                           //协议类型

    struct
    {
        //网络参数配置(目标机端)
        char ip[IP_BUF_LEN];                    //IP地址
        UI port;                                //端口号
        UI timeout;                            //超时
        UI retry;                                //重试次数
    } NET;

    struct
    {
        //串口参数配置(主机端)
        char channel[UART_NAME_BUF_LEN];        //串口
        UI baud;                                //波特率
        UI timeout;                            //超时
        UI retry;                                //重试次数
    } UART;

    struct
    {
        char dllName[NAME_SIZE];                //动态库名字,可以带路径
        //动态库方式通信
        char buf[DLL_BUF];                        /*动态库的缓冲区
                                                  填写格式：[aid,name,isSupportPrivateProtocol,configfile,]
                                                  说明如下：
                                                  aid:  跟ICE通信的通信代理SA 的aid
                                                  name: 跟ICE通信的通信代理SA 的name
                                                  isSupportPrivateProtocol: 是否支持扩展协议，这里填1
                                                  configfile: 配置文件名, 如果在ts的loadfile目录下，即使用tclient_uploadfile传上来的文件可以直接写文件名，否则写带路径的文件名*/
    } DLL;

    struct
    {
        char GUID[GUID_LEN];              /*USB设备的GUID号，
                                                            用sprintf将GUID号格式化成16进制字符串
                                                            的形式到BUF中, 如"873fdf-61a8-......."*/
    } USB;
    COMCONFIG* next;                            //通信配置信息
} comConfig ;

typedef struct server_config
{
    char            isLog;                            //是否需要日志0,不记录,1:记录
    char        name[NAME_SIZE];                //名字
    char        extInfo[EXT_INFO_LEN];          //体系结构等，若目标机类型为TA，填0*;若目标机类型为ICE，填1*
    US        aid;                            //aid号
    UI            state;                            //状态，由TS上报，分别为1(ACTIVE)，0(INACTIVE)，2(DELETED)
    UI            reportType;                        //上报类型，由TS上报，为注册类型
    UI            currentChannel;                    //当前通道号
    UI            channelCount;                    //通道数
    COMCONFIG*    pComConfig;                        //下一链表节点，若目标机为多通道，则指向下一个通道的通道配置
} ServerCfg;


/************************接口声明******************************/

/**
 * 功能: 关闭指定的连接
 * @param fd  连接句柄
 * @return :
         TS_INVALID_LINK_HANDLE: 参数fd是无效的句柄
         TS_SOCKET_ERROR:  如果发给TS命令包时SOCEKT发生错误或关闭返回该值
         TS_SUCCESS:    成功关闭。
 */
DLL_FUNC_INT TClient_close(US fd) ;

/**
 * 功能: 将数据发送到指定的目标服务的应用代理处
 * @param fd  连接句柄
 * @param des_aid  目标aid号
 * @param des_said 目标said号
 * @param src_said 源said号
 * @param data  数据包
 * @param size  数据包大小
 * @return
        如果发送数据成功返回实际发送的数据的字节大小
        TS_NULLPTR_ENCONTERED: 包内容的缓冲为空指针
        TS_INVALID_LINK_HANDLE: 传入的连接句柄号为空
        TS_SOCKET_ERROR:  发包时SOCKET发生错误
 */
DLL_FUNC_INT TClient_putpkt(US fd, US des_aid, US des_said, US src_said,
                            const char* data, UI size) ;

/**
 * 功能: 接收目标服务的数据
 * @param fd  连接句柄
 * @param des_aid  目标aid号
 * @param des_said 目标said号
 * @param src_said 源said号
 * @param data  数据包
 * @param dataSize  缓冲区的大小
 * @return
        如果成功便返回实际得到数据包的字节大小
        TS_NULLPTR_ENCONTERED:接收缓冲的指针为空.
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_SOCKET_ERROR:收包时发生SOCKET错误.
        TS_REC_BUF_LIMITED:  接收缓冲的大小小于实际接收到的数据的字节大小
 */
DLL_FUNC_INT TClient_getpkt(US fd, US* src_aid, US* des_said, US* src_said,
                            char* data, UI dataSize) ;

/**
 * 功能: 提供非阻塞通信方式, 用户可以设定一定时间的timeout
 * @param fd  连接句柄
 * @param timeout  等待时间,单位:秒
 * @return
        TS_DATACOMING:在指定的时间内有数据到来
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_TIME_OUT: 指定的时间内没有数据到来
        TS_SOCKET_ERROR:在查询SOCKET缓冲时SOCKET错误,可能是对端关闭连接.
 */
DLL_FUNC_INT TClient_wait(US fd, int timeout) ;

/**
 * 功能: 与指定IP地址和端口号的TS建立连接
 * @param IP  TS所在的IP地址
 * @param port  端口号, 0标识采用默认端口port, 非0表示指定端口
 * @return
        操作成功返回大于0表示得到的aid号
        TS_NULLPTR_ENCONTERED :传入的IP缓冲为空指针
        TS_INVALID_IP: IP为无效IP,如0.0.0.0/255.255.255.255等.
        TS_SOCKET_ERROR: 试图和对端建立连接的时候SOCKET发生错误
        TS_ISNOT_TS_SERVER:  对端没有启动TS服务.
        TS_UNKNOW_DATA  :对端返回的消息是未知消息包,或格式不可识别
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_open返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_open(const char* IP, US port = 0) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */

/**
 * 功能:  根据目标服务的名字查找对应的ID号
 * @param fd  连接句柄
 * @param name  目标名字, 一个以0结尾的字符串
 * @return
    操作成功返回服务对应的目标服务的ID号
    TS_NULLPTR_ENCONTERED: name为空指针
    TS_INVALID_LINK_HANDLE : 无效的连接句柄
    TS_INVALID_SERVICE_NAME: name字符串长度大小系统规定的名字长度
    TS_UNKNOW_DATA: 未知的数据格式或数据包
    TS_SYSTEM_ERROR:  系统处理发生错误,有可能是收到的数据包的对话
                                    ID号与期待的不一致,也有可能返回的是无效的AID号
    TS_TIME_OUT:  在指定的收包时间内没有收到数据包


发送格式：
                            -------------------
                            |DID|, |I name|, |
                            -------------------
接收格式：
                          回复数据包格式
                            -------------------
                            |DID|, |OK|, |serverID|,|
                            -------------------
*/
DLL_FUNC_INT TClient_getServerID(US fd, char* name) ;


/**
 * 功能: 在TM中创建一个DLL形式的目标服务配置.
 * @param fd  连接句柄
 * @param pServerConfig: 服务代理的配置结构指针
 * @return
        如果操作成功返回AID号
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_UNKNOW_DATA: 无效的数据,一般是数据长度小于期望的最小长度
        TS_UNKNOW_DATA_FORMAT:  无法识别的数据格式
        TS_SYSTEM_ERROR: 系统发生错误
        TS_TIME_OUT: 在指定时间内没有收到数据
        以及其它TS返回的具体错误值

发送格式: "DID, TserviceName,  dllName,  isLog,  DllBuf,"
接收格式: "DID, OK, aid,"
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_createServerWithDll返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_createServerWithDll(US fd, ServerCfg* pServerConfig) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end]*/


/**
 * 功能: 在TM中创建一个目标服务配置.
 * @param fd  连接句柄
 * @param pServerCfg  目标机配置信息
 * @return
        如果操作成功返回AID号
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_UNKNOW_DATA: 无效的数据,一般是数据长度小于期望的最小长度
        TS_UNKNOW_DATA_FORMAT:  无法识别的数据格式
        TS_SYSTEM_ERROR: 系统发生错误
        TS_TIME_OUT: 在指定时间内没有收到数据
        TS_NULLPTR_ENCONTERED: pServerCfg或pServerCfg->pComConfig指针为空
        TS_INVALID_SERVICE_NAME:  没有指定服务代理的名字
        TS_INVALID_IP: 配置了无效的IP
        以及其它TS返回的具体错误值

发送格式: |DID|, Csa名字,  日志,通道数, +根据way选择各类配置信息(以分割符)
接收格式:      |DID|, |OK|, |aid|
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_createServer返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_createServer(US fd, ServerCfg* pServerCfg) ;
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end]*/


/**
 * 功能: 关闭指定的TS
 * @param fd   连接句柄
 * @return
        TS_SUCCESS操作成功
        TS_INVALID_LINK_HANDLE:  无效的连接句柄号
        TS_SOCKET_ERROR:   发送数据包时SOCKET发生错误
 */
DLL_FUNC_INT TClient_closeTM(US fd) ;

/**
 * 功能: 通道切换
 * @param fd   连接句柄
 * @param AID  目标服务的AID号
 * @param channelType  要切换到的通道ID
 * @return
        TS_SUCCESS: 操作成功
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_CHANGE_CHANNEL_FAILURE  : 通道切换失败
        TS_SRV_CHANNEL_SWITCH_DISABLE: 通道切换不允许,被自己禁用
        TS_CH_SW_FUNC_LOCKED_BY_OHTER: 通道切换功能被其他锁定了
        以及其他一些TS返回值
 */
DLL_FUNC_INT TClient_changeChannelType(US fd,  US aid, UC channelType);

/**
 * 功能: 删除TM一个目标服务配置.
 * @param fd   连接句柄
 * @param AID  目标服务的AID号
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        以及其他TS返回的一些具体错误值
发送格式: DID,RID,
接收格式:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_deleteServer(US fd, US aid) ;

/**
 * 功能: 修改一个已经存在在TS中的目标服务配置
 * @param fd   连接句柄
 * @param pServerCfg   指定目标server的配置结构, 是个输出参数
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_SERVICE_CONFIG_ERR : 指定的通道配置信息错误
        TS_UNKNOW_COMM_WAY: 通道配置指定的通信方式错误
        以及其他TS返回的一些具体错误值

 发送格式:
 NET/UART方式
 -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|isLog|,|channelCount|,|curChannel|,|channelConfig..............|
 -----------------------------------------------------------------------------
 DLL方式
  -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|isLog|,|way|,|DllBuf|
 -----------------------------------------------------------------------------
接收格式: DID,OK/Exx,
*/
DLL_FUNC_INT TClient_modifyServer(US fd, ServerCfg* pServerCfg) ;

/**
 * 功能: 通过指定目标服务ID号查找该目标机的配置参数和状态信息
 * @param fd   连接句柄
 * @param pServerCfg[OUT]   指定目标server的配置结构, 是个输出参数
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_NULLPTR_ENCONTERED:  pServerCfg为空指针
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式：
 -------------
 |DID|,|Qaid|,|
 -------------
查询服务返回的格式为：
TCP/UDP/UART
OK, name, state, log, chcount, curch, config of each ch..
DLL
OK, name, state, log, chcount, curch, protocol, way, dllname, outbuf, inbuf,
*/
DLL_FUNC_INT TClient_queryServer(US fd, ServerCfg* pServerCfg) ;


/**
 * 功能: 激活指定的处于休眠状态的目标服务
 * @param fd   连接句柄
 * @param aid   目标的aid号
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式: DID, Aaid,
接收格式: DID, OK/Exx,
*/
DLL_FUNC_INT TClient_activeServer(US fd, US aid) ;

/**
 *   函 数    名： TClient_activeServerWithDll
 *   函数说明： 激活指定的处于休眠状态的DLL_SA
 *   参      数1   ：fd:连接句柄
 *   参      数2   ：aid:目标服务的Aid号
 *   参      数3   ：char* anserInfo:  TC收到的TS激操作的信息
 *   参      数4   ：infolen[in/out]  指定answerInfo的字节大小，
                                        返回它实际内容的字节大小
 *   返  回   值：
                                如果操作成功则返回接收到的回复字节数
                                TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                                TS_NULLPTR_ENCONTERED:  answerInfo为空指针
                                TS_INVALID_SERVICE_ID:  无效的AID
                                TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
                                TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
                                TS_TIME_OUT:  指定时间内没有收到数据包
                                TS_REC_BUF_LIMITED:  指定的answerInfo空间长度不足
                                以及其他TS返回的一些具体错误值
       发送格式: DID,Aaid,
 *    接收格式:DID,Exx,  // DID, OK, outbuflen, outbuf...,
 *
**/
DLL_FUNC_INT TClient_activeServerWithDll(US fd,  US aid,  char* answerInfo,  int infolen);


/**
 * 功能: 休眠指定的处于激活状态的目标服务
 * @param fd   连接句柄
 * @param aid   目标的aid号
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式:DID, Paid,
接收格式:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_inActiveServer(US fd, US aid) ;

/**
 * 功能: 运行TS
 * @param path   TS的带路径的文件名
 * @return
        TS_SUCCESS:   加载成功
        TS_NULLPTR_ENCONTERED: path :为空指针
        TS_INVALID_FILE_NAME: 文件名大小系统规定的文件名大小
        TS_FILE_NOT_EXIST:  文件名对应的TS文件不存在
        TS_LAUNCH_TS_FAILURE:  启动TS进程失败
 */
DLL_FUNC_INT TClient_launchTM(const char* path) ;

/**
 * 功能:工具可选用单独的线程阻塞在该方法。
 *              得到TS自动上报的信息。
 *               可能是消息, 也可能是状态
 *               内存空间由调用者来分配
 * @param fd   连接句柄
 * @param pServerCfg    上报信息头指针
 * @return
         如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 格式:sa的AID号, 名字, 上报类型, 状态, 日志, 通信方式, 根据不同的通信方式的配置信息(以分割符来分割)
 */
DLL_FUNC_INT TClient_getSAReport(US fd, ServerCfg *pServerCfg, UI timeout = 0) ;

/**
 * 功能: TS进行注册, 注册后, TS会向TC自动发送消息和状态
 * @param fd   连接句柄
 * @param regType   注册类型
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

发送格式: DID,Etype,
接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_register(US fd, int regType) ;

/**
 * 功能: 取消某种类型的注册登记
 * @param fd   连接句柄
 * @param regType   注册类型
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 发送格式：DID,UregType,
接收格式：DID,OK/Exx,
*/
DLL_FUNC_INT TClient_unregister(US fd, int regType);

/**
 * 功能: 设置sa的注册类型
 * @param fd   连接句柄
 * @param aid   要设置sa的aid号
 * @param registerType   类型
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 发包格式: DID,s aid, registerType,
 接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_setRegisterType(US fd, US aid, int registerType) ;

/**
 * 功能: 清除sa的注册类型
 * @param fd   连接句柄
 * @param aid   要设置sa的aid号
 * @param registerType   要注销类型
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 发包格式: DID,c aid, registerType,
 接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_clearRegisterType(US fd, US aid,  int registerType) ;

/**
 * @Funcname:  Tclient_queryHostSerialPort
 * @brief        :  查询TS所在主机的串口配置
 * @para1      : fd  连接句柄号
 * @para2      : buf  接收缓冲
 * @para3      : 操作成功:size  接收缓冲的字节大小
 * @return      :
                    操作成功返回接收缓冲中的实际字节大小
                    TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                    TS_TIME_OUT:  指定时间内没有收到数据包
                    TS_REC_BUF_LIMITED: 缓冲中指定的大小小于实际接收到的字节大小
                    以及其他TS返回的一些具体错误值

    发送格式: DID, p,
    接收格式: |DID|,|OK|,|COM1|,|COM2|,|COM3|,|或者DID,Exx,
 */
DLL_FUNC_INT Tclient_queryHostSerialPort(US fd,  char *recvBuf,  US size);

/**
 * @Funcname:  TClient_setTargetChannelSwitchState
 * @brief        :  设置目标机通道切换标志
 * @para1      : fd : 连接句柄
 * @para2      : aid  目标服务ID号
 * @para3      :  flag :false禁止通道切换true:允许通道切换
 * @return      :
                        TS_SUCESS 设置成功,
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_TIMEOUT: 收取回复消息包超时
                        TS_CH_SW_FUNC_LOCKED_BY_OHTER: 通道切换被其他客户端锁定
                        以及其他一些返回值

    发包格式:DID,a aid, flag
    接收格式:DID,OK,/Exx,
 *
**/
DLL_FUNC_INT TClient_setTargetChannelSwitchState(US fd,  US aid,  _BOOL flag);

/**
 * @Funcname:  DLL_FUNC_INT  TClient_SetSystemLogLevel
 * @brief        :  设置TS系统日志的输出级别
 * @para1      : fd:  连接句柄号
 * @para2      : level 日志级别
 * @return      :
                        TS_SUCESS 设置成功,
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_SOCKET_ERROR: 发送命令包时SOCKET错误

    发包格式:DID,L level,
    无回复数据
 *
**/
DLL_FUNC_INT  TClient_setSystemLogLevel(US fd,  LogLevel level);

/**
 * @Funcname:  TClient_SetServiceAgentLog
 * @brief        :  设置指定服务的数据包日志标记
 * @para1      : fd : 连接句柄
 * @para2      : aid  目标服务ID号
 * @para3      :  logflag :0不输出指定服务的日志1:输出指定服务的消息日志
 * @return      :
                        TS_SUCESS 设置成功
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_SOCKET_ERROR: 发送命令包时SOCKET错误

    发包格式:DID,l aid,logflag,
    接收格式: DID, Ok/Exx,
 *
**/
DLL_FUNC_INT  TClient_SetServiceAgentLog(US fd, US aid, _BOOL logflag);

/**
 * @Funcname:  TClient_UpLoadFile
 * @brief        :  传送指定文件到TS服务器
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      : filename需要传送的文件名
 * @para3      : isReplace是否需要覆盖同名文件0,不覆盖;1: 覆盖
 * @para4      : savedname, 保存在TS上的文件名
 * @return      :
                        TS_SUCCESS传送成功
                        TS_FILE_ALREADY_EXIT已经存在同名文件
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月2日 14:51:36
 *
**/
DLL_FUNC_INT TClient_UpLoadFile(US fd, const char* filename, const char *savedname, int isReplace = 1);

/**
 * @Funcname:  TClient_DownLoadFile
 * @brief        :  从TS服务器下载文件到工具端
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      :  filename需要下载的文件名
 * @para3      : savefilename下载文件名需要保存的文件名
                        (带全路径,否则将保存在TS运行的当前目录)
 * @return      :
                         TS_SUCCESS传送成功
                         TS_FILE_ALREADY_EXITS已经存在同名文件
                         TS_FILE_NOT_EXIST指定的文件不存在
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月2日 15:00:31
 *
**/
DLL_FUNC_INT TClient_DownLoadFile(US fd, const char*filename, const char*savefilename);

/**
 * @Funcname:  TClient_DeleteFile
 * @brief        :  从TS服务器删除指定文件
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      :  filename需要下载的文件名
 * @return      :
                         TS_SUCCESS传送成功
                         TS_FILE_NOT_EXIST指定的文件不存在
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月11日 11:00:31
 *
**/
DLL_FUNC_INT TClient_DeleteFile(US fd, const char*filename);

/**
 * @Funcname:  guidToString
 * @brief        :  将GUID转换成字符串
 * @para1      : guidData, guid数据
 * @para2      : upcase转换成大小写形式
 * @para3      : strGuid, 转换后的字符串，缓冲由用户分配
 * @para4      : buflen, 用户分配的缓冲大小
 * @return      : 成功返回0，失败返回-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月27日 10:44:03
 *
**/
DLL_FUNC_INT guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen);
/**
 * @Funcname:  stringToGUID
 * @brief        :  将字符串形式的GUID转换成GUID结构数据
 * @para1      : strGuid, 字符串形式的GUID
 * @para2      : len 字符串的长度
 * @para3      : guidData, 转换后的结构
 * @return      : 成功返回0，失败返回-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月27日 10:44:03
 *
**/
DLL_FUNC_INT stringToGUID(char *strGuid, unsigned short len, GUID *guidData);


#endif
