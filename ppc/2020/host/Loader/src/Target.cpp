/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2012-7-24  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/**
* @file  Target.cpp
* @brief
*       功能：
*       <li>定义和目标机服务器之间的交互操作</li>
*/
/************************引用部分******************************/
#include "Target.h"

/************************定义部分******************************/

/************************实现部分******************************/

/**
 * @Funcname  : Target
 * @brief	  : 构造函数
 * @para[IN]  : tsip:ip地址 tgtname：目标机名字 tsport：端口
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012年7月24日 
 *	 
**/
Target::Target(string tsip, string tgtname, UINT32 tsport)
{
	m_tsIP = tsip;
	m_tsPort = tsport;
	m_tgtName = tgtname;
	m_tsFd = 0;
	m_desAid = 0;
	m_currentDesSaid = 0;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : 发送命令并等指定回复
 * @para[IN]  : cmd：命令 cmdCount：命令个数 timeout：超时时间
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012年7月24日  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount, UINT32 timeout)	
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;
	char buf[10] = {0};

	/* 参数输入检查 */
	if(NULL == cmd ||  0 == cmdCount)
	{
		return GetErrorNumber(INVALID_PARAMETER);
	}

    /* 发送命令 */
    ret = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, src_said, cmd, cmdCount);
    if (ret < 0)
    {    
        return GetErrorNumber(TARGET_PUTPKT_FAIL);        
    }    

    /* 等待结果 */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
        return GetErrorNumber(TARGET_OUTTIME);   
    }
	
	/* 接受回复 */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, buf, sizeof(buf));
	if (buf[0] != 'O' && buf[1] != 'K')
	{
		return GetErrorNumber(TARGET_GETPKT_FAIL);   
	}

	return TARGET_OK;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : 发送命令并等回复,将回复放到指定的buffer中
 * @para[IN]  : cmd：命令 cmdCount：命令个数 result：结果 resultCount：结果个数 timeout：超时时间
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012年7月24日  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount, char *result, UINT32 resultCount, UINT32 timeout)	
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;

	if(NULL == cmd || NULL == result || 0 == cmdCount)
	{
		return GetErrorNumber(INVALID_PARAMETER);
	}

    /* 发送命令 */
    ret = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, src_said, cmd, cmdCount) ;
    if (ret < 0)
    {    
       return GetErrorNumber(TARGET_PUTPKT_FAIL);  
    }    

    /* 等待结果 */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
       return GetErrorNumber(TARGET_OUTTIME);   
    }

	/* 接受回复 */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, result, resultCount);
	if (ret < 0)
    {    
        return GetErrorNumber(TARGET_GETPKT_FAIL);  
    }   
	
   	return TARGET_OK;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : 发送命令不等回复
 * @para[IN]  : cmd：命令 cmdCount：命令个数 
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012年7月24日  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount)	
{
	UINT32 snedSzie = 0;
	UINT16 src_said = 0;
	
	snedSzie = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, 0,  cmd, cmdCount);
	
	return (snedSzie > 0) ? TARGET_OK : GetErrorNumber(TARGET_PUTPKT_FAIL);
}

/**
 * @Funcname  : ReceiveData
 * @brief	  : 接收数据
 * @para[IN]  : result：结果 cmdCount：结果数组大小   timeout：超时时间 receSize：实际接受大小
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 Target::ReceiveData(char *result, UINT32 resultCount, UINT32 timeout, INT32 *receSize)
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;
    
	/* 等待结果 */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
       return GetErrorNumber(TARGET_OUTTIME);   
    }

	/* 接受数据 */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, result, resultCount);
	if (ret < 0)
    {    
        return GetErrorNumber(TARGET_GETPKT_FAIL);  
    }
	
	/* 设置回复数据包大小 */
	*receSize = ret;

	return TARGET_OK;
}

/**
 * @Funcname  : Connect
 * @brief	  : 连接到指定的目标机服务器
 * @para[IN]  : 无
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 Target::Connect()	
{
	/* 与指定IP地址和端口号的TS建立连接*/
    m_tsFd = TClient_open(m_tsIP.c_str(), m_tsPort);
	if(m_tsFd < 0)
	{
		return GetErrorNumber(TARGET_TCLIENT_OPEN_FAIL);
	}

	/* 获取目标机ID */
	m_desAid = TClient_getServerID(m_tsFd, const_cast<char*>(m_tgtName.c_str()));

	return (m_desAid > 0) ? TARGET_OK : GetErrorNumber(TARGET_TCLIENT_GETSERVERID_FAIL);
}

/**
 * @Funcname  : DisConnect
 * @brief	  : 断开连接
 * @para[IN]  : 无
 * @reture	  : 成功返回TARGET_OK 失败返回相应错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 Target::DisConnect()	
{
	INT32 ret = 0;
		
	ret = TClient_close(m_tsFd);
	
	return (ret == TS_SUCCESS) ? TARGET_OK : GetErrorNumber(TARGET_DISCONNECT_FAIL);
}

/**
 * @Funcname  : SetDesSAid
 * @brief	  : 设置二级代理号
 * @para[IN]  : desSAid：二级代理号
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
void Target::SetDesSAid(UINT16 desSAid)
{
	m_currentDesSaid = desSAid;
}

/**
 * @Funcname  : GetErrorNumber
 * @brief	  : 根据ErrprStr获取ErrprMum,以数字形式返回
 * @para[IN]  : string：错误字符串
 * @reture	  : 数字形式的错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 Target::GetErrorNumber(string str)
{
	return Util::GetDefault()->GetErrorNumber(str);
}

