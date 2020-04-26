/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
* @file 	gdbServer.cpp
* @brief 	
* @author 	郭建川
* @date 	2005-02-6
* @version  1.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/
#include "stdafx.h"
#include <stdlib.h>
#include "gdbServer.h"

T_MODULE T_CONST T_WORD INT2HEX_SIZE = 10;
//T_MODULE T_CONST T_CHAR CONTROL_FLAG[] = ",";
T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";


GdbServer::GdbServer(short id)
{
	this->selfId = id;
	sClient = NULL;
	bufSemaphore = NULL;
	m_hExitEvent = NULL;
	pOutBuf = NULL;
	m_enableSet = TRUE;
	SetMop(FALSE);
#ifdef TRA_DEBUG
       SetLog(TRUE);
#endif
	this->SetIsProcessingGdb(FALSE);
	

	m_resetFlag = TRUE;
}

GdbServer::~GdbServer(void)
{
	//退出前清空内存
	RefreshMemory();
}

/**
* 功能：关闭客户端的连接,
*/
void GdbServer::closeClient() {
	TMutexLocker lock(&clientMutex);
	
	if( NULL != sClient)
	{
		sClient->Close();
		delete sClient;
		sClient = NULL;
	}
}

/*
* @brief
*    将一个整型根据宽度转为16进制字符串\n
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] wNum    调用者提供的整型
* @param[in] wWidth  调用者提供的字符串宽度
* @return 宽度
*/
T_MODULE T_WORD MemInt2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth)
{
	T_WORD i;

	//buf[width] = '\0';

	for (i = wWidth - 1; i >= 0; i--)
	{
		cpBuf[i] = "0123456789abcdef"[(wNum & 0xf)];
		wNum >>= 4;
	}

  return wWidth;
}

const int MAX_SEMAPHORE_COUNT = 1000;	//计数器最大值
int GdbServer::open(int lport)
{
	//根据参数确定是否打开内存优化算法
	if(des_said == 1)
	{
		//SetMop(TRUE);  //内存优化算法一般只为TA 串口调试打开
	}
	else
	{
		SetMop(FALSE);
	}

	m_runFlag = false;
	if(lport < 0 || lport > 65536)
	{
		return FALSE;
	}

	this->port = lport;

	if(serverSocket.m_sock == INVALID_SOCKET)
	{
		if(serverSocket.createServerSocket(port,1))	//地址重用
		{
			bufSemaphore = CreateSemaphore(NULL,0,MAX_SEMAPHORE_COUNT,NULL);
			if (bufSemaphore == NULL)
				return FAIL;
			DWORD dwThreadID;
			if (NULL == (m_hReadThread = CreateThread (NULL, 0 , handlerThread, (LPVOID)this, 0, 
				&dwThreadID)))
			{
				CloseHandle(bufSemaphore);
				return FAIL;
			}
			//创建一个事件对象,用来退出动态库
			m_hExitEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);

			m_runFlag = true;
			if (lport == 0)
				serverSocket.GetLocalPort();

			if (pOutBuf != NULL)
				Int2hex(pOutBuf,10,serverSocket.GetLocalPort());		//监听端口号

			#ifdef TRA_DEBUG
				QString format = "dd.hh";
				QString time = QDateTime::currentDateTime().toString(format);
				QString head = "./log/tra";
				QString tail = ".log";
				QString strPort = QString("%1").arg(serverSocket.GetLocalPort());
				QString name = head.append(strPort).append(".").append(time).append(tail);
				m_logger.open(name.data().ascii());
			#endif

			return SUCC;
		}
		else
		{
			return FAIL;
		}
	}

//	return serverSocket.GetLocalPort();
	return SUCC;
}

int GdbServer::close()
{
	m_runFlag = false;

	if (bufSemaphore != NULL) {
		ReleaseSemaphore(bufSemaphore,MAX_SEMAPHORE_COUNT,NULL);	//唤醒所有的信号量
	}
	//主要是保证别的线程得到切换,这种方式只是为了解决问题,要彻底解决应当加上同步对象
	
	if (m_hExitEvent != NULL) {
		DWORD stat = WaitForSingleObject(m_hExitEvent, 2000);
		if(stat == WAIT_OBJECT_0)
		{

		}
		else if(stat == WAIT_TIMEOUT)
		{

		}

		CloseHandle( m_hExitEvent );
		m_hExitEvent = NULL;
	}

    if(sClient != NULL)
    {
	sClient->Close();	//关闭客户端的连接
    }
	serverSocket.Close();

	//退出线程
	if (m_hReadThread != NULL) {
		WaitForSingleObject(m_hReadThread,5000);
		CloseHandle(m_hReadThread);
		m_hReadThread = NULL;
	}

	if (bufSemaphore != NULL) {
		CloseHandle(bufSemaphore);	//唤醒所有的信号量
		bufSemaphore = NULL;
	}
	//删除所有的链表对象
	clearList();
#ifdef TRA_DEBUG
	m_logger.close();
#endif
	return SUCC;
}

const char hexchars[]="0123456789abcdef";

T_VOID GdbServer::HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize)
{
	//检查参数输入合法性
	if(wSize < 4 || cpBuf[0] != 'r' || cpBuf[1] != 's' || cpBuf[2] != 'p' || cpBuf[3] != '-')
	{
		SendIDEResult("error:input unkown cmd!", strlen("error:input unkown cmd!"), hwSrcAid, hwSrcSaid);
		return;
	}

	switch(cpBuf[4])
	{
    case 's':
        {
            //切换TRA-DLL对应的调试二级代理的ID号
            char *ptr = &cpBuf[6];
            int subID = 0;
            if(Hex2int(&ptr, &subID))
            {
                des_said = subID;
                SendIDEResult("OK",strlen("OK"),hwSrcAid, hwSrcSaid);
                
            }
            else
            {
                SendIDEResult("E01",strlen("E01"),hwSrcAid, hwSrcSaid);
                
            }

            break;
        }
	case 'l':
		{
			//日志选项设置，默认关闭
			if(wSize < 10 || cpBuf[4] != 'l' || cpBuf[5] != 'o' || cpBuf[6] != 'g' || cpBuf[7] != '-')
			{
				SendIDEResult("错误：未知的日志操作命令！\n", strlen("错误：未知的日志操作命令！\n"), hwSrcAid, hwSrcSaid);
				break;
			}
			
			if(cpBuf[9] == 'n')
			{
				//打开日志选项
                //SetLog(TRUE);
				SendIDEResult("日志打开成功！\n", strlen("日志打开成功！\n"), hwSrcAid, hwSrcSaid);
			}
			else if(cpBuf[9] == 'f')
			{
				//关闭日志选项
                //SetLog(FALSE);
				SendIDEResult("日志关闭成功！\n", strlen("日志关闭成功！\n"), hwSrcAid, hwSrcSaid);
			}
			else
			{
				SendIDEResult("错误：未知的日志操作命令！\n", strlen("错误：未知的日志操作命令！\n"), hwSrcAid, hwSrcSaid);
			}

			break;
		}
	case 'm':
		{
			//内存优化选项算法设置，默认打开
			if(wSize < 10 || cpBuf[4] != 'm' || cpBuf[5] != 'o' || cpBuf[6] != 'a' || cpBuf[7] != '-')
			{
				SendIDEResult("错误：未知的内存优化设置命令！\n", strlen("错误：未知的内存优化设置命令！\n"), hwSrcAid, hwSrcSaid);
				break;
			}
			
			if(cpBuf[9] == 'n')
			{
				//打开内存优化算法
				T_WORD ret = SetMop(TRUE);
				if(SUCC == ret)
				{
					SendIDEResult("内存优化算法打开成功！\n", strlen("内存优化算法打开成功！\n"), hwSrcAid, hwSrcSaid);
				}
				else
				{
					SendIDEResult("内存优化算法打开失败！\n", strlen("内存优化算法打开失败！\n"), hwSrcAid, hwSrcSaid);
				}

			}
			else if(cpBuf[9] == 'f')
			{
				//关闭内存优化算法
				T_WORD ret = SetMop(FALSE);
				if(SUCC == ret)
				{
					SendIDEResult("内存优化算法关闭成功！\n", strlen("内存优化算法关闭成功！\n"), hwSrcAid, hwSrcSaid);
				}
				else
				{
					SendIDEResult("内存优化算法关闭失败！\n", strlen("内存优化算法关闭失败！\n"), hwSrcAid, hwSrcSaid);
				}
			}
			else
			{
				SendIDEResult("错误：未知的内存优化设置命令！\n", strlen("错误：未知的内存优化设置命令！\n"), hwSrcAid, hwSrcSaid);
			}

			break;
		}
	default:
		{
			SendIDEResult("错误：未知的操作命令！\n", strlen("错误：未知的操作命令！\n"), hwSrcAid, hwSrcSaid);
			break;
		}
	}

}

T_VOID GdbServer::SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid)
{
	//申请分配一个新的缓冲包
	RecvPacket *pack = new RecvPacket();

	T_CHAR *pBuf = pack->data;

	memcpy(pBuf, cpBuf, wSize);
	pBuf += wSize;


	pack->size = pBuf - pack->data;

	pack->m_desAid = hwSrcAid;
	pack->m_desSaid = hwSrcSaid;
	pack->m_srcSaid = 1;

	//组包完成，插入到队列中去
	insertList(pack);
}

int GdbServer::putpkt(short des_said, short src_aid, short src_said, char* pBuf, int size)
{

	if(size < 0 || !m_runFlag)
	{
		return FAIL;
	}
	if(sClient == NULL || pBuf == NULL)
	{
		return FAIL;
	}

	if(des_said == 1)
	{
		HandleIDECmd(src_aid, src_said, pBuf, size);
		
		return SUCC;
	}	

	if(GetReset() == TRUE)
	{
		//忽略掉对'r'命令的回复结果
		SetReset(FALSE);
		return TRUE;
	}

	//发送命令前先检查TA的回复是不是针对读内存命令的回复
	if(m_lastCmd.getValid() == TRUE)
	{
		UpdateMemory(pBuf, size);
		T_WORD ret = GetLocalMemory(m_lastCmd.gdbAddr, m_lastCmd.gdbSize, pBuf);
		if(ret > 0)
		{
			
			m_lastCmd.setValid(FALSE);
			PutData(pBuf, ret);
		}

		
	}
	else
	{
		//是对其它命令的回复，直接发给GDB
		PutData(pBuf, size);
	}
	
	return SUCC;
}


//更新内存缓冲区
T_VOID GdbServer::UpdateMemory(T_CHAR *cpBuf, T_WORD size)
{
	//检查回复是否是更新内存的内容
	if(m_lastCmd.traSize == size)
	{
		//更新缓冲
	
		//更新内存
		assert(NULL != cpBuf);

	//	if(cpBuf[0] == 'E' && cpBuf[1] == '0' && cpBuf[2] == '3' && size == 3)
	//	{
	//		PutData("E03", strlen("E03"));
	//		return;
	//	}
		//FIXME:没有做长度校验

		//计算内存块的个数
		T_WORD blockNum = m_lastCmd.traSize / RSP_Memory::MEMORY_BLOCK_SIZE;
		T_WORD addr = m_lastCmd.traAddr;

		T_CHAR *ptr = &cpBuf[0];
		

		while(blockNum)
		{
			//如果已经有该内存块，则不用更新
			if(m_addr2mem.contains(addr))
			{
				blockNum--;
				continue;
			}

			RSP_Memory *memory = memMan.Alloc();
			if(memory == NULL)
			{
				//释放所有内存
				RefreshMemory();
				memory = memMan.Alloc();
			}

			memory->SetBaseAddr(addr);
			memcpy(memory->GetValue(), ptr, RSP_Memory::MEMORY_BLOCK_SIZE);

			ptr += RSP_Memory::MEMORY_BLOCK_SIZE;
			m_addr2mem.insert(addr, memory);
			blockNum--;
			addr += RSP_Memory::MEMORY_BLOCK_SIZE;
		}
	
	}
	
}

int GdbServer::getpkt(short *des_aid, short *des_said, short *src_said, char *pBuf, int size)
{
	if (bufSemaphore != NULL && m_runFlag) {
		DWORD dw = WaitForSingleObject(bufSemaphore,INFINITE);	//等待信号量,即收到gdb数据包
		
	//	TMutexLocker lock(&recvListMutex);
		RecvPacket* pRecvPacket = getPacketFromList();
		if (m_runFlag  && pRecvPacket != NULL) {
			//list<RecvPacket*>::iterator iter = recvBufList.begin();
			
			//recvBufList.pop_front();
			int recSize = size;	

			if (size > PACKET_SIZE)
				recSize = PACKET_SIZE;
			
			if(pRecvPacket->m_desAid == -1)
			{
				*des_aid = this->des_aid;
				*des_said = this->des_said;
				*src_said = this->src_said;
			}
			else
			{
				*des_aid = pRecvPacket->m_desAid;
				*des_said = pRecvPacket->m_desSaid;
				*src_said = pRecvPacket->m_srcSaid;
			
			}


			int revcSize = pRecvPacket->size;
			if (revcSize > size)
				revcSize = size;

			memcpy(pBuf, pRecvPacket->data, revcSize);	//拷贝缓冲区
			delete pRecvPacket;		//清除分配空间
			pRecvPacket = NULL;
			return revcSize;
		}
		else {
			if (pRecvPacket != NULL) {
				delete pRecvPacket;		//清除分配空间
				pRecvPacket = NULL;
			}

			if (m_hExitEvent != NULL)
				SetEvent (m_hExitEvent);
			return -1;
		}
	}
	else
		return -1;
}

/*
* @brief
*    将回复数据发送给GDB\n
* @param     cpBuf    调用者要发送的数据缓冲
* @param[in] size    调用者要发送数据的大小
* @return  实际发送的数据的大小
*/
T_WORD GdbServer::PutData(T_CHAR *cpBuf, T_WORD wSize)
{
	TMutexLocker lock(&clientMutex);	//对客户端进行同步互斥
	if(wSize == 64)
	{
		int a = 0;
	}

	T_WORD i = 0;
	T_UBYTE *sendBuf;
	T_UBYTE checkSum = 0;
	T_WORD	count = wSize;
	T_WORD result = 0;

	if(sClient == NULL || cpBuf == NULL)
	{
		return FAIL;
	}

	//日志
#ifdef TRA_DEBUG
	if(IsLog() == TRUE)
		m_logger.write(SEND_PACK, cpBuf, wSize);
#endif

	//分配发送缓冲区
	sendBuf = new T_UBYTE[wSize+10];

	if(sendBuf == NULL)
	{
		return FAIL;
	}

	memset(sendBuf, 0, wSize + 10);
	//构造包头
	sendBuf[0] = '$';

	//求校验和
   	while(count--)
	{
		checkSum += static_cast<int>(cpBuf[i]);
		sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
		i++;
	}
	//构造包尾
	sendBuf[wSize+1] = '#';
	sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
	sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
	//发送数据
    result = sClient->Send(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);

	delete []sendBuf;
	return result;

}

/*
* @brief
*    阻塞在服务器接收GDB的数据，收到数据后\n
*    将数据拷贝到调用者提供的缓冲区后返回\n
* @param     cpBuf    调用者提供的数据缓冲
* @param[in] wSize    调用者提供的缓冲区的大小
* @return  实际接收的数据的大小，网络断开返回-1
*/
T_WORD GdbServer::GetData(T_CHAR *cpBuf, T_WORD wSize)
{
	//检查SOCKET合法性
	if (NULL == sClient || NULL == cpBuf)
	{
		return FAIL;
	}

	//阻塞接收GDB命令
	T_CHAR head;
	T_WORD offset = 0;

	while(TRUE)
	{
		offset = 0;
        T_WORD ret = sClient->Recv(&head, 1);

		//如果网络断开，退出循环
		if(ret < 0)
		{
			return FAIL;
		}

		//如果接收的头字符不是$，则继续接收
		if('$'!= head)
		{
			continue;
		}

		//接收数据直到#
		ret = sClient->Recv(&head, 1);
		while((offset < wSize) && (FAIL != ret))
		{
			if('#' == head)
			{
				break;
			}
			cpBuf[offset++] = head;
			ret = sClient->Recv(&head, 1);
		}

		if(ret < 0)
		{
			return FAIL;
		}

		if(offset > wSize)
		{
			continue;
		}

		//应答，因为是TCP接收，故没有进行校验
		sClient->Send("+", strlen("+"));

		return offset;
	}
}

T_WORD GdbServer::SetReset(T_BOOL isReset)
{
	m_resetMutex.lock();

	this->m_resetFlag = isReset;

	m_resetMutex.unlock();

	return SUCC;
}

T_BOOL GdbServer::GetReset()
{
	TMutexLocker lock(&m_resetMutex);
	
	return this->m_resetFlag;
}


DWORD GdbServer::handlerThread(LPVOID lpvoid)
{
	GdbServer * pGdbServer = reinterpret_cast<GdbServer* >(lpvoid);



	while (pGdbServer->m_runFlag)
	{
		//清空缓存
		pGdbServer->RefreshMemory();

		pGdbServer->sClient = new WSocket;

		if(pGdbServer->sClient == NULL )
			return 0;

		//监听端口
		if(!pGdbServer->serverSocket.acceptSocket(*pGdbServer->sClient)) 
		{
			pGdbServer->closeClient();
			return 0;
		}

//-------------------------------------------------------------------------
//2006.06.29 by guojc 在GDB连接上TRA服务器后，首先向目标机的DA发送复位命令
			//RESET m_resetFlag
			pGdbServer->SetReset(TRUE);

			//通知DA进行初始化操作
			RecvPacket *pa = new RecvPacket();
			pa->size = strlen("r");
			memcpy(pa->data, "r", strlen("r"));
			pGdbServer->MessageHandler(pa);
//-------------------------------------------------------------------------

		while(pGdbServer->m_runFlag)
		{
			pGdbServer->m_enableSet = FALSE;
			//连接成功
			//开始接收数据
			RecvPacket *pack = new RecvPacket();
			memset(pack->data, 0, 20);

			pGdbServer->SetIsProcessingGdb(FALSE);

			T_WORD ret = pGdbServer->GetData(pack->data, PACKET_SIZE);

			pGdbServer->SetIsProcessingGdb(TRUE);

			if(ret == FAIL)
			{
				//网络断开
				pGdbServer->closeClient();
				pGdbServer->m_enableSet = TRUE;

                            /*Modified by tangxp for BUG NO. on 2008.2.28 [begin]*/
                            /*  Modified brief: 退出时释放内存*/
                            delete pack;
                            pack = NULL;
                            /*Modified by tangxp for BUG NO. on 2008.2.28 [end]*/
				break;
			}

			#ifdef TRA_DEBUG
			if(pGdbServer->IsLog() == TRUE)
				pGdbServer->m_logger.write(RECV_PACK, pack->data, ret);
			#endif

			if(0 == ret)
			{
				//收到空包
				/*Modified by tangxp for BUG NO. on 2008.2.28 [begin]*/
                            /*  Modified brief: 退出时释放内存*/
                            delete pack;
                            pack = NULL;
                            /*Modified by tangxp for BUG NO. on 2008.2.28 [end]*/
				continue;
			}

			pack->size = ret;

			//处理数据包
			pGdbServer->MessageHandler(pack);
		}


	}


	return 1;
}

/*
* @brief
*    将接收GDB的数据进行协议解析，装载命令序列\n
* @param[in] cpBuf    调用者提供的数据缓冲
* @param[in] wSize    调用者提供的缓冲区的大小
* @return
*/
T_WORD GdbServer::MessageHandler(RecvPacket *tPack)
{

	//定义发送缓冲
	T_CHAR sendBuf[PACKET_SIZE];

#if 0
	//检查数据包内容是否为读内存
	if(tPack->data[0] == 'm')
	{
		if(GetMop() == FALSE)
		{//如果内存优化算法关闭，则直接把命令发送到DA
			insertList(tPack);
			return SUCC;
		}
		//读内存,首先检查本地缓冲的内存是否有效,如果有效,则将本地缓冲直接拷贝给GDB
		T_WORD addr = 0; //内存地址
		T_WORD size = 0; //内存长度
		T_CHAR *ptr = &tPack->data[1];
		T_CHAR *pSendBuf = &sendBuf[0];

		if(Hex2int(&ptr,&addr) && *(ptr++) == ',' && Hex2int(&ptr,&size))
		{
			if(size < 0 || size > PACKET_SIZE)
			{
				PutData("E03", strlen("E03"));
				//释放缓冲包
				if(tPack != NULL)
				{
					delete tPack;
					tPack = NULL;
				}
				return FAIL;
			}

			T_WORD ret = GetLocalMemory(addr, size, pSendBuf);
			if(ret > 0)
			{
				//本地有缓存，直接回复GDB
				PutData(pSendBuf, ret);
				//释放缓冲包
				if(tPack != NULL)
				{
					delete tPack;
					tPack = NULL;
				}
				return SUCC;
			}
			else if(ret == SUCC)
			{
				//记录GDB的这条命令
					m_lastCmd.gdbAddr = addr;
					m_lastCmd.gdbSize = size;
					m_lastCmd.setValid(TRUE);	
			}
		

          
		}

		//释放缓冲包
		if(tPack != NULL)
		{
			delete tPack;
			tPack = NULL;
		}
	}
	else
	{
		
		switch(tPack->data[0])
		{
		case 'X':
		case 'c':
		case 'M':
		case 's':
			{
				//清空内存缓冲区
				RefreshMemory();
				break;		
			}
		default:
			break;
		}

		//直接将数据包发送到DA
		insertList(tPack);
	}
#endif

	//直接将数据包发送到DA
	insertList(tPack);
	return 0;
}

/*
* @brief
*    从本地内存缓冲中获取需要的内存信息，如果没有，该方法会压入命令序列从目标机中读取相应的内存块\n
* @param[in] wAddr   内存地址
* @param[in] wSize   内存大小
* @param[in] cpBuf   调用者给定的缓冲区
* @return  给缓冲区拷贝的内存区的大小，0为无本地缓存, -1为DA正在回复命令中，不理会GDB的重复命令
*/
T_WORD GdbServer::GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf)
{
	//计算基地址,从映射表中查找本地缓冲是否存在该内存块
	T_WORD baseAddr = wAddr & RSP_Memory::MEMORY_BLOCK_MASK;
	T_WORD offset = wAddr - baseAddr;
	T_WORD leftSize = wSize;
	T_CHAR *pLocalBuf = cpBuf;



	//计算所需要内存块的数目
	T_WORD computeSize = (wSize + RSP_Memory::MEMORY_BLOCK_SIZE - 1 + offset) / RSP_Memory::MEMORY_BLOCK_SIZE;
	T_WORD count = 0;

	count = computeSize;
	//检查本地是否有足够的内存块
	T_WORD validAddr = baseAddr;  //内存地址
	T_WORD i = count;
	while(i != 0)
	{
		if(!m_addr2mem.contains(validAddr))
		{
			break;
		}
		validAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
		i--;
	}

	//如果本地有足够的内存缓冲,则将内存信息拷贝到缓冲区中
	if(i == 0)
	{
		T_WORD index = offset; //本地内存数组索引
		T_WORD localSize = 0;
		while(count)
		{
			localSize = wSize < RSP_Memory::MEMORY_BLOCK_SIZE ? wSize : RSP_Memory::MEMORY_BLOCK_SIZE;
			
			if((wSize + index) < RSP_Memory::MEMORY_BLOCK_SIZE)
			{
				localSize = wSize;
			}
			else
			{
				localSize = RSP_Memory::MEMORY_BLOCK_SIZE - index;
			}

			//取出一块内存块
			RSP_Memory *pMem = m_addr2mem.value(baseAddr);
			for(T_WORD j = index; j < (localSize + index); j++)
			{
				//gdb格式内存
				pLocalBuf += MemInt2Hex(pLocalBuf, static_cast<int>(*(pMem->GetValue() + j)), 2);
				
			}
			
			baseAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
			//clear offset while count > 2
			index = 0;
		
			wSize -= localSize;
			count--;
		}

		

		return pLocalBuf - cpBuf;//返回发送数据的长度
	}
	else
	{
		//-------------------------------------------------------------------------------------------
		//2006.10.30 内存优化算法存在一定的问题，主要是因为当没有足够的缓冲块的时候，会调用refreshMemory方法
		//有可能会释放掉已经存在的，并且在此次调试命令所需要的内存块，因此造成无法返回给GDB足够的内存数据而失败
		//因此在这里改为在发送命令之前先检查是否有足够的内存块，如果没有，就释放后重新组织下发命令
		if(i > memMan.LeftPackets())
		{
			//如果需要申请的缓冲包个数大于可以分配的个数，则清零缓冲区
			this->RefreshMemory();
			//因为刷新，所以validAddr必须从基地址开始算起
			validAddr = baseAddr;
			i = count;
		}

		T_WORD memSize = i * RSP_Memory::MEMORY_BLOCK_SIZE;
		
		//本地没有缓存，检查是否和上一条命令重复
		if(m_lastCmd.getValid() == TRUE && m_lastCmd.gdbAddr == wAddr && m_lastCmd.gdbSize == wSize)
		{
			return -1;
		}

		
		//记录读的地址，便于收到回复时更新本地缓存
		m_lastCmd.traAddr = validAddr;
		m_lastCmd.traSize = memSize;
	
		//向DA发送读取内存命令
		ReadMemory(validAddr, memSize);
		
		return SUCC;
	}
	



}

void GdbServer::ReadMemory(T_WORD wAddr, T_WORD wSize)
{
	//组装读内存缓冲数据包
	//申请分配一个新的缓冲包
	RecvPacket *pack = new RecvPacket();
	T_CHAR *pBuf = pack->data;

	//组装协议缓冲，格式maddr,size
	memcpy(pBuf, "x", 1);
	pBuf += 1;

	pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //地址
	memcpy(pBuf,",",strlen(","));	//增加控制符号
	pBuf += strlen(",");

	pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize); //大小
	
	pack->size = pBuf - pack->data;
	//组包完成，插入到队列中去
	insertList(pack);
}

/*
* @brief
*    清空本地内存缓冲,
*/
T_VOID GdbServer::RefreshMemory()
{
	TMutex refreshMutex;
	TMutexLocker lock(&refreshMutex);


	T_WORD size = m_addr2mem.size();

	if(size == 0)
	{
		return;
	}

	QList<T_WORD> keys;

	QMapIterator<T_WORD, RSP_Memory*> i(m_addr2mem);
    while (i.hasNext()) {
        i.next();
       keys.push_back(i.key());
    }

	for(int j = 0; j < size; j++)
	{
		RSP_Memory *mem = m_addr2mem.take(keys.value(j));
		if(NULL != mem)
		{
			memMan.Free(mem);
		}
	}
}
/*
* 功能 清空链表中所有数据
*/
void GdbServer::clearList() {
	TMutexLocker lock(&recvListMutex);
	RecvPacket* pRecvPacket = NULL;
	while(recvBufList.size()>0) {
		list<RecvPacket*>::iterator iter = recvBufList.begin();
		recvBufList.pop_front();
		pRecvPacket = *iter;
        if(pRecvPacket != NULL)
        {
		delete pRecvPacket;		//清除分配空间
		pRecvPacket = NULL;
	}
    }

	recvBufList.resize(0);
}


/*
* 功能 从链表中读第一个表,并删除此链表节点
* @return  pReceiveBuf    接收到的包
*/
RecvPacket* GdbServer::getPacketFromList() {
	TMutexLocker lock(&recvListMutex);
	if (recvBufList.size() > 0) {
		list<RecvPacket*>::iterator iter = recvBufList.begin();
		RecvPacket* pRecvPacket = *iter;
		recvBufList.pop_front();
		return pRecvPacket;
	}
	else
		return NULL;
}


/*
* 功能 把收到的包插入缓冲链表中
* @param[in]     pReceiveBuf    接收到的包
*/
void GdbServer::insertList(RecvPacket* pReceiveBuf) {
	TMutexLocker lock(&recvListMutex);
	recvBufList.push_back(pReceiveBuf);
	ReleaseSemaphore(bufSemaphore,1,NULL);
}

/*
* @brief
*    将给定的16进制字符串转换为整型,需要注意的是该操作会移动给定缓冲指针的地址\n
* @param[in]     cptr    16进制字符串
* @param[out] wIntValue 转换的INT数据
* @return  指针移动的字节数
*/
T_UWORD GdbServer::Hex2int(T_CHAR **cptr, T_WORD *wIntValue)
{
  int numChars = 0;
  int hexValue;

  *wIntValue = 0;

  while (**cptr)
    {
      hexValue = Hex2num(**cptr);
      if (hexValue < 0)
      break;

      *wIntValue = (*wIntValue << 4) | hexValue;
      numChars ++;

      (*cptr)++;
    }

  return (numChars);
}

T_UWORD GdbServer::Int2hex(T_CHAR *cpPtr, T_WORD wSize,T_WORD wIntValue)
{
	memset(cpPtr,'\0',wSize);	//设置大小,最大缓冲区的大小
	sprintf(cpPtr,"%x",wIntValue);
	return strlen(cpPtr);
}



/*
* @brief
*    将单个的16进制ASCII字符转换成对应的数字\n
* @param[in]     cptr   单个的16进制ASCII字符
* @return  返回对应的数字
*/
T_WORD GdbServer::Hex2num(T_UBYTE vubCh)
{
  if (vubCh >= 'a' && vubCh <= 'f')
    return vubCh-'a'+10;
  if (vubCh >= '0' && vubCh <= '9')
    return vubCh-'0';
  if (vubCh >= 'A' && vubCh <= 'F')
    return vubCh-'A'+10;
  return -1;
}

#ifdef TRA_DEBUG
T_VOID GdbServer::SetLog(T_BOOL isLog)
{
    QMutexLocker locker(&m_logMutex);
    m_isLog = isLog;
}

T_BOOL GdbServer::IsLog()
{
    QMutexLocker locker(&m_logMutex);
    return m_isLog;
}
#endif

#if 1
T_WORD GdbServer::SetMop(T_BOOL isMop)
{
	TMutexLocker locker(&m_mopMutex);

#if 0

	if(m_enableSet == FALSE)
	{
		return FAIL;
	}
	if(this->IsProcessingGdb() || m_lastCmd.getValid())
	{
		//如果服务器正在处理GDB的命令,或者正在等待回复GDB读内存命令的结果,不允许修改内存优化算法设置,避免出现非法地址访问
		return FAIL;
	}
	if(m_mopFlag != isMop)
	{
		RefreshMemory();
	}
#endif

	m_mopFlag = isMop;
	return SUCC;
}

T_BOOL GdbServer::GetMop()
{
	TMutexLocker locker(&m_mopMutex);
	return m_mopFlag;
}

#endif

T_VOID GdbServer::SetIsProcessingGdb(T_BOOL flag)
{
	TMutexLocker locker(&m_gdbCmdMutex);
	m_isProcessingGdb = flag;
}

T_BOOL GdbServer::IsProcessingGdb()
{
	TMutexLocker locker(&m_gdbCmdMutex);
	return m_isProcessingGdb;
}
