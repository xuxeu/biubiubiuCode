/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  LoadCommand.cpp
 * @brief
 *       <li>加载功能</li>
 */
/************************引用部分******************************/
#include "LoadCommand.h"

/************************定 义部分******************************/

/************************实现部分*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  :run
	 * @brief	  : 命令执行体
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadCommand::Exec(UINT32 said)
	{
		BOOL status = FALSE;

		/* 检查传递的参数是否正确 */
		status = IsParamOK();
		if(TRUE != status)
		{	
			status = GetErrorNumber(LOADER_ERROR_PARAMETER);
			ErrorMessageOutput(status);
			return FALSE;
		}

		/* 执行加载体 */
		return LoadFile(said);
	}

	/**
	 * @Funcname  :LoadFile
	 * @brief	  :加载文件
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadCommand::LoadFile(UINT32 said)
	{
		ELF32 *elf32 = NULL;
		char  *segmentContent = NULL;
		INT16 segmentNum = 0;
		UINT32 segmentFileSize = 0;
		UINT32 status = 0;
		UINT32 segmentAddress = 0;
		string packSize = NULL_STRING;
		string elfpath = NULL_STRING;

		/* 前面已经检查了参数的有效性，以后获取参数不用检查有效性 */

		/* 获得发送包大小 */
		packSize = GetParameterValue("-psize");
		
		/* 获得elf文件路径 */
		elfpath = GetParameterValue("-file");	  
		
		/* 打开elf文件获取文件的属性 */
		elf32 = ELF32_Parse::open(elfpath.c_str()); 
		if(NULL == elf32)
		{
			status = GetErrorNumber(LOADER_ERROR_OPENELF_FAIL);
			ErrorMessageOutput(status);
			return FALSE;
		}

		/* 获取需要下载的总大小 */
		m_LoadFileSize = GetLoadSegmentSize(elf32); 
		  
		/* 获取下载段的个数 */
		segmentNum = elf32->GetSegmentCount();	
		
		/* 针对程序头表中每一项进行处理 */
		for(INT32 i = 0; i < segmentNum; i++)
		{
			if(IsSegmentNeedLoad(elf32->GetSegment(i)))
			{		
				/* 获得段大小 */
				segmentFileSize = elf32->GetSegment(i)->GetFileSize();	
				if(0 == segmentFileSize)
				{
					continue;
				}

				/* 获得段地址 */
				segmentAddress =  elf32->GetSegment(i)->GetPhysicalAddr();
				
				/* 分配内存空间存储段数据 */
				segmentContent = new char[segmentFileSize];
				if(NULL == segmentContent)
				{
					status = GetErrorNumber(LOADER_ERROR_BAD_ALLOC);
					ErrorMessageOutput(status);
					ELF32_Parse::close(elf32);
					return FALSE;
				}
				
				/* 清空内存 */
				memset(segmentContent, 0, segmentFileSize);
				
				/* 获取段内容 */
				elf32->GetSegment(i)->GetContents((INT8*)segmentContent, segmentFileSize);
				
				/* 下载内容,写内容到内存 */
				status = LoadData(segmentAddress, segmentContent, segmentFileSize, atoi(packSize.c_str()),said);	
				if(TRUE != status)
				{	
					status = GetErrorNumber(LOADER_ERROR_LoadFile_FAIL);
					ErrorMessageOutput(status);
					delete [] segmentContent;
					ELF32_Parse::close(elf32);
					return FALSE;
				}
		
				/* 释放空间 */
				delete [] segmentContent;
				segmentContent = NULL;
			}
		}

		ELF32_Parse::close(elf32);
		return TRUE;
	}

	/**
	 * @Funcname:IsSegmentNeedLoad
	 * @brief    : 将段内容下载到目标机
	 * @para[IN] : addr:段地址 content:段内容 contentSize:段大小 packSize:每次下载数据包的大小
	 * @reture	 : 需要加载返回TRUE,不需要加载返回FALSE
	 * @Author	 : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadCommand::LoadData(UINT32 address, char *content, UINT32 contentSize, UINT32 packSize,UINT32 said)
	{
		UINT32 currentsentsize = 0; 
		UINT32 cnt  = 0;
		UINT32 snedsize = 0;
		UINT32 extra = 0;
		UINT32 index = 0;
		UINT32 status = 0;
		Target *target = NULL;
		char *src = NULL;
		char *des = NULL;
		static char outbuf[OUT_BUFFFER_SIZE] = {0};

		/* 获得目标机 */
		target = GetTarget();

		/* 设置要发送到目标机的二级代理ID */
		target->SetDesSAid(said);

		while(currentsentsize < contentSize)
		{
			/* 获得发送大小 */
			snedsize = contentSize - currentsentsize;
			snedsize =  min(snedsize, packSize);

			/*下载段，组装命令 */
			cnt = sprintf_s(outbuf, OUT_BUFFFER_SIZE, "X%x,%x:", (address + currentsentsize), snedsize);

			index = snedsize;
			src = content + currentsentsize;
			des = outbuf + cnt;
			extra = 0;

			while(index--)
			{
				if(*src == '$' || *src == '#' || *src == 0x7d)
				{
					*des++ = 0x7d;
					*des++ = *src++ ^ 0x20;
					extra++;
				}
				else
				{
					*des++ = *src++;
				}
			}
				
			/* 发送命令 */
			status = target->ExecCommand(outbuf, cnt + snedsize + extra, GetTimeOut());
			if(TARGET_OK == status)
			{
				currentsentsize	+= snedsize;
				m_CurrentLoadSize += snedsize;
				InformationProgressMessageOutput("已经加载", (m_CurrentLoadSize / (float)m_LoadFileSize) * 100, m_CurrentLoadSize, m_LoadFileSize);
			}
			else
			{	
				return FALSE;
			}
		}

		return TRUE;
	}

	/**
	 * @Funcname  :IsSegmentNeedLoad
	 * @brief	  : 判断段是否需要加载
	 * @para[IN]  : Segment:   段地址
	 * @reture	  :  需要加载返回TRUE,不需要加载返回FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadCommand::IsSegmentNeedLoad(Segment *segment)
	{
		INT32 segmentType = 0;

		/* 获得段类型*/
		segmentType = segment->GetType();

		if(PT_LOAD == segmentType)
		{
			return TRUE;        
		}
		
		return FALSE;
	}

	/**
	 * @Funcname  :GetLoadSegmentSize
	 * @brief	  :累加段的大小
	 * @para[IN]  : ELF32:   ELF结构指针
	 * @reture	  :  总大小
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	UINT32 LoadCommand::GetLoadSegmentSize(ELF32 *elf32)
	{
		UINT32 segmentCount = 0;
		Segment *segment = NULL;
		UINT32 totalSize = 0;
		UINT32 index = 0;

		/* 获得段的个数 */
		segmentCount = elf32->GetSegmentCount();

		/* 获取段的总大小 */
		for (index = 0; index < segmentCount; index++)
		{
			segment = elf32->GetSegment(index);
			if(IsSegmentNeedLoad(segment))
			{
				totalSize += segment->GetFileSize();
			}   
		}
		
		return totalSize;
	}

	/**
	 * @Funcname  :IsParamOK
	 * @brief	  : 检查命令参数是否齐全
	 * @para[IN]  : 无
	 * @reture	  : 成功返回TRUE 失败返回FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	 */
	BOOL LoadCommand::IsParamOK(void)
	{
		return Command::IsParamOK();
	}
}