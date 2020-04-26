/************************************************************************
*                北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: lambdaIce.cpp,v $
 * Revision 1.4  2008/03/19 10:25:11  guojc
 * Bug 编号  	0002430
 * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
 *
 * 编码：唐兴培
 * 检视：卿孝海,郭建川
 *
 * Revision 1.2  2008/03/19 09:03:00  guojc
 * Bug 编号  	0002430
 * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
 *
 * 编码：唐兴培
 * 检视：卿孝海,郭建川
 *
 * Revision 1.1  2008/02/04 05:33:49  zhangxc
 * 设计报告名称：《ACoreOS CVS代码仓库管理》
 * 编写人员：张晓超
 * 检视人员：张宇旻
 * 其它：从现有最新ACoreOS仓库中取出最新的源代码，整理后提交到新的仓库目录结构中管理。
 * 第一次提交src模块。
 *
 * Revision 1.3  2007/04/20 07:10:50  guojc
 * 增加了对ICE DCC功能的支持
 * 增加了对目标板复位功能的支持
 * 增加了对内存宽度可配置的支持
 * 增加了对华邦模拟器调试兼容的支持
 *
 * Revision 1.2  2007/04/02 02:56:53  guojc
 * 提交最新的IS代码，支持脱离于IDE使用，支持配置文件解析
 *
 * Revision 1.1  2006/11/13 03:40:55  guojc
 * 提交ts3.1工程源码,在VC8下编译
 *
 * Revision 1.2  2006/07/14 09:19:30  tanjw
 * 1.修改release版问题
 * 2.加入ice升级功能
 *
 * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
 * no message
 *
 * Revision 1.2  2006/04/14 02:02:27  guojc
 * 增加了对CP15协处理器寄存器修改的支持
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS代码整理
 *
 * Revision 1.1  2006/03/04 09:25:09  guojc
 * 首次提交ICE Server动态库的代码
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * 创建文件；实现RSP_lambdaIce类的基本接口。
 */

/**
 * @file     lambdaIce.cpp
 * @brief
 *    <li>功能：封装了和ICE交互的协议操作接口 </li>
 * @author     郭建川
 * @date     2006-02-20
 * <p>部门：系统部
 */

/**************************** 引用部分 *********************************/
#include "lambdaIce.h"
#include "Packet.h"
#include "rspTcpServer.h"
#include "IceServerComm.h"
/*************************** 前向声明部分 ******************************/
using namespace std;
/**************************** 定义部分 *********************************/
T_MODULE T_CONST T_WORD INT2HEX_SIZE = 10;
/**************************** 实现部分 *********************************/


/**
* @brief 
*     功能: 设置服务器对象
* @param[in] tpServer 服务器对象指针
*/
T_VOID RSP_LambdaIce::SetServer(RSP_TcpServer *tpServer)
{
    m_server = tpServer;
}

/**
* @brief 
*     功能:查询ICE大小端
* @param[in] cpBuf 缓冲区,需要发送给ICE的一些信息
*/
T_VOID RSP_LambdaIce::QueryTargetEndian(int IceID)
{
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    if(pack == NULL)
    {
        return;
    }
    T_CHAR *pBuf = pack->GetData();
    //组装查询大小端命令进行发送
    memcpy(pBuf, "i -e", strlen("i -e"));
    pBuf += strlen("i -e");
    pack->SetSize(pBuf - pack->GetData());
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}

/**
* @brief 
*     功能:删除与ICE的会话
* @param[in] ICEID
*/
T_VOID RSP_LambdaIce::DeleteSeesion(int IceID)
{
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    memcpy(pBuf, "d*", strlen("d*"));
    pack->SetSize(strlen("d*"));
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}
/**
* @brief 
*     功能:建立与ICE的会话
* @param[in] cpBuf 缓冲区,需要发送给ICE的一些信息
* @param[in] cpBufSize 缓冲区大小
*/
T_VOID RSP_LambdaIce::CreateSeesion(T_CHAR *cpBuf, T_WORD cpBufSize, int IceID)
{
    if(cpBuf == NULL)
    {
        return;
    }
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    memcpy(pBuf, cpBuf, cpBufSize);
    pack->SetSize(cpBufSize);
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}

/**
* @brief 
*     功能: 封装写CP15寄存器的控制包
* @param[in] wNum CP15寄存器号
* @param[in] wValue 寄存器值
*/
T_VOID RSP_LambdaIce::WriteCp15Reg(T_WORD wNum, T_WORD wValue)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式 Dnn,XXXX
    memcpy(pBuf, RSP_SET_CPP_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wNum);    //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);
    //转换成四四字节的整形存储
    m_server->Int2Char(wValue, pBuf, m_server->GetTargetEndian());
    pBuf += 4;
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装读CP15寄存器的控制包
* @param[in] wNum CP15寄存器号
* @param[in] wValue 寄存器值
*/
T_VOID RSP_LambdaIce::ReadCp15Reg(T_WORD wNum)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式 Dnn,XXXX
    memcpy(pBuf, RSP_GET_CPP_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wNum);    //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}


/**
* @brief 
*     功能: 封装写内存控制包
* @param[in] wAddr 内存地址
* @param[in] cpBuf 缓冲区
* @param[in] wOffset 偏移地址
* @param[in] wSize 要写的内存大小
*/
T_VOID RSP_LambdaIce::WriteMemory(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    
    //组装协议缓冲，格式 Xaddr,size:data
    memcpy(pBuf, RSP_WRITE_MEMORY, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr);    //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize);    //大小
    memcpy(pBuf,COLON_CHAR, COLON_CHAR_LEN);    //增加:
    pBuf += COLON_CHAR_LEN;

    T_CHAR *pContent = cpBuf + wOffset;           //内存内容
    memcpy(pBuf, pContent, wSize);
    pBuf += wSize;


    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装写内存控制包
* @param[in] wAddr 内存地址
* @param[in] cpBuf 缓冲区
* @param[in] wOffset 偏移地址
* @param[in] wSize 要写的内存大小
* @param[in] wDelay 延时时间长度
*/
T_VOID RSP_LambdaIce::WriteMemoryWithDelay(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize, T_WORD wDelay)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    
    //组装协议缓冲，格式 Maddr,size,data
    memcpy(pBuf, "M", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr);    //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize);    //大小
    memcpy(pBuf,"#",strlen("#"));    //增加#号
    pBuf += strlen("#");

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wDelay);    //大小
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);


    T_CHAR *pContent = cpBuf + wOffset;           //内存内容
    memcpy(pBuf, pContent, wSize);
    pBuf += wSize;


    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装读内存控制包
* @param[in] wAddr 内存地址
* @param[in] wSize 要读的内存大小
*/
T_VOID RSP_LambdaIce::ReadMemory(T_WORD wAddr, T_WORD wSize)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式maddr,size
    memcpy(pBuf, RSP_READ_MEMORY, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize); //大小
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);

}

/**
* @brief 
*     功能: 封装写所有寄存器控制包
* @param[in] waRegs 寄存器数组
* @param[in] waRegs 寄存器数组长度
*/
T_VOID RSP_LambdaIce::WriteAllRegisters(T_WORD *waRegs, T_WORD wNum)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式Gxxxx
    memcpy(pBuf, RSP_SET_ALL_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    //寄存器数据以大端方式发送
    for(T_WORD i = 0; i < wNum; i++)
    {
        m_server->Int2Char(waRegs[i], pBuf, TRUE);//TRUE);
        pBuf += 4; //每个寄存器的值占4字节
    }

    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);

}

/**
* @brief 
*     功能: 封装读所有寄存器控制包
*/
T_VOID RSP_LambdaIce::ReadAllRegisters()
{    
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式g
    memcpy(pBuf, RSP_GET_ALL_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);
}


/**
* @brief 
*     功能: 封装继续运行目标机控制包
*/
T_VOID RSP_LambdaIce::ContinueTarget()
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式c
    memcpy(pBuf, RSP_RUN_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装停止目标机控制包
*/
T_VOID RSP_LambdaIce::StopTarget()
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式!
    memcpy(pBuf, RSP_STOP_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);

}

/**
* @brief 
*     功能: 封装重启目标机控制包
*   resetType＝0  CPU硬件复位，只是将目标板CPU进行硬件复位，不对目标板外围设备进行复位。
*   resetType＝1  外设硬件复位，只是将目标板的外围硬件设备进行复位，不对CPU进行复位。
*   resetType＝2  CPU外设同时硬件复位。
*/
T_VOID RSP_LambdaIce::ReSetTarget(T_UWORD resetType)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式Rtype
    memcpy(pBuf, RSP_RESET_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pBuf += Int2hex(pBuf, 10, resetType);
    
    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装查询目标机CPUID CODE
*/
T_VOID RSP_LambdaIce::QueryCPUIDCode()
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式i
    memcpy(pBuf, RSP_GET_TARGET_CPUCODE, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装查询目标机控制包
*/
T_VOID RSP_LambdaIce::QueryTarget()
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式q
    memcpy(pBuf, RSP_QUERY_TARGET_STATE, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装打开DCC功能的控制包
*/
T_VOID RSP_LambdaIce::OpenDccFunction(T_WORD dccAddr)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式Axxxx(addr)
    memcpy(pBuf, LAMBDA_ICE_START_DCC, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pBuf += Int2hex(pBuf, 10, dccAddr);
    pack->SetSize(pBuf - pack->GetData());

    //组包完成，插入到队列去
    m_server->Enqueue(pack);

    return;
}

/**
* @brief 
*     功能: 封装插入断点或观察点
* @param[in] type 断点类型
*                           0：软件断点
*                           1：硬件断点
*                           2：写观察点
*                           3：读观察点
*                           4：访问观察点
* @param[in] wAddr 断点地址
* @param[in] wWidth 断点宽度
*/
T_VOID RSP_LambdaIce::InsertBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    //组装协议缓冲，格式P
    memcpy(pBuf, RSP_SET_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    //type
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, type); //类型为0 插入断点
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //addr
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //length
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装删除断点或观察点
* @param[in] type 断点类型
*                           0：软件断点
*                           1：硬件断点
*                           2：写观察点
*                           3：读观察点
*                           4：访问观察点
* @param[in] wAddr 断点地址
* @param[in] wWidth 断点宽度
*/
T_VOID RSP_LambdaIce::DeleteBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    //组装协议缓冲，格式P
    memcpy(pBuf, RSP_DELETE_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    //type
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, type); //类型为0 插入断点
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //addr
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //length
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

#if 0
/**
* @brief 
*     功能: 封装插入硬件断点控制包
* @param[in] wAddr 断点地址
* @param[in] wWidth 断点宽度
*/
T_VOID RSP_LambdaIce::InsertBreakPoint(T_WORD wAddr, T_WORD wWidth)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式P
    memcpy(pBuf, RSP_SET_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 0); //类型为0 插入断点
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);

}

/**
* @brief 
*     功能: 封装删除硬件断点控制包
* @param[in] wAddr 断点地址
* @param[in] wWidth 断点宽度
*/
T_VOID RSP_LambdaIce::RemoveBreakPoint(T_WORD wAddr, T_WORD wWidth)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式P
    memcpy(pBuf, RSP_DELETE_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 1); //类型为1 删除断点
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}

/**
* @brief 
*     功能: 封装插入观察点控制包
* @param[in] wAddr 观察点地址
* @param[in] wWidth 观察点宽度
* @param[in] wAccess 观察点类型
*/
T_VOID RSP_LambdaIce::InsertWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式P
    memcpy(pBuf, "w", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 0); //类型为0 插入观察点
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAccess); //观察点类型
    
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);

}

/**
* @brief 
*     功能: 封装删除观察点控制包
* @param[in] wAddr 观察点地址
* @param[in] wWidth 观察点宽度
* @param[in] wAccess 观察点类型
*/
T_VOID RSP_LambdaIce::RemoveWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式P
    memcpy(pBuf, "w", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 1); //类型为1 删除观察点
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //宽度
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //增加控制符号
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAccess); //观察点类型
    
    
    pack->SetSize(pBuf - pack->GetData());
    //组包完成，插入到队列中去
    m_server->Enqueue(pack);
}
#endif

/*
* 功能: 把int转换成16进制的字符串,必须保证size大小大于转换后的字符串大小
* @param[out] cpPtr 保存16进制字符串的缓冲区
* @param[in] wSize   缓冲区的大小
* @param[in] wIntValue   要转换的整形
* @return 转换的大小
*/
T_UWORD RSP_LambdaIce::Int2hex(T_CHAR *cpPtr, T_WORD wSize,T_WORD wIntValue)
{
    memset(cpPtr,'\0',wSize);    //设置大小,最大缓冲区的大小
    sprintf(cpPtr,"%x",wIntValue);
    return strlen(cpPtr);
}