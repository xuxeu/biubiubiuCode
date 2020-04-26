/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  LoadCommand.cpp
 * @brief
 *       <li>���ع���</li>
 */
/************************���ò���******************************/
#include "LoadCommand.h"

/************************�� �岿��******************************/

/************************ʵ�ֲ���*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  :run
	 * @brief	  : ����ִ����
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL LoadCommand::Exec(UINT32 said)
	{
		BOOL status = FALSE;

		/* ��鴫�ݵĲ����Ƿ���ȷ */
		status = IsParamOK();
		if(TRUE != status)
		{	
			status = GetErrorNumber(LOADER_ERROR_PARAMETER);
			ErrorMessageOutput(status);
			return FALSE;
		}

		/* ִ�м����� */
		return LoadFile(said);
	}

	/**
	 * @Funcname  :LoadFile
	 * @brief	  :�����ļ�
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
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

		/* ǰ���Ѿ�����˲�������Ч�ԣ��Ժ��ȡ�������ü����Ч�� */

		/* ��÷��Ͱ���С */
		packSize = GetParameterValue("-psize");
		
		/* ���elf�ļ�·�� */
		elfpath = GetParameterValue("-file");	  
		
		/* ��elf�ļ���ȡ�ļ������� */
		elf32 = ELF32_Parse::open(elfpath.c_str()); 
		if(NULL == elf32)
		{
			status = GetErrorNumber(LOADER_ERROR_OPENELF_FAIL);
			ErrorMessageOutput(status);
			return FALSE;
		}

		/* ��ȡ��Ҫ���ص��ܴ�С */
		m_LoadFileSize = GetLoadSegmentSize(elf32); 
		  
		/* ��ȡ���ضεĸ��� */
		segmentNum = elf32->GetSegmentCount();	
		
		/* ��Գ���ͷ����ÿһ����д��� */
		for(INT32 i = 0; i < segmentNum; i++)
		{
			if(IsSegmentNeedLoad(elf32->GetSegment(i)))
			{		
				/* ��öδ�С */
				segmentFileSize = elf32->GetSegment(i)->GetFileSize();	
				if(0 == segmentFileSize)
				{
					continue;
				}

				/* ��öε�ַ */
				segmentAddress =  elf32->GetSegment(i)->GetPhysicalAddr();
				
				/* �����ڴ�ռ�洢������ */
				segmentContent = new char[segmentFileSize];
				if(NULL == segmentContent)
				{
					status = GetErrorNumber(LOADER_ERROR_BAD_ALLOC);
					ErrorMessageOutput(status);
					ELF32_Parse::close(elf32);
					return FALSE;
				}
				
				/* ����ڴ� */
				memset(segmentContent, 0, segmentFileSize);
				
				/* ��ȡ������ */
				elf32->GetSegment(i)->GetContents((INT8*)segmentContent, segmentFileSize);
				
				/* ��������,д���ݵ��ڴ� */
				status = LoadData(segmentAddress, segmentContent, segmentFileSize, atoi(packSize.c_str()),said);	
				if(TRUE != status)
				{	
					status = GetErrorNumber(LOADER_ERROR_LoadFile_FAIL);
					ErrorMessageOutput(status);
					delete [] segmentContent;
					ELF32_Parse::close(elf32);
					return FALSE;
				}
		
				/* �ͷſռ� */
				delete [] segmentContent;
				segmentContent = NULL;
			}
		}

		ELF32_Parse::close(elf32);
		return TRUE;
	}

	/**
	 * @Funcname:IsSegmentNeedLoad
	 * @brief    : �����������ص�Ŀ���
	 * @para[IN] : addr:�ε�ַ content:������ contentSize:�δ�С packSize:ÿ���������ݰ��Ĵ�С
	 * @reture	 : ��Ҫ���ط���TRUE,����Ҫ���ط���FALSE
	 * @Author	 : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
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

		/* ���Ŀ��� */
		target = GetTarget();

		/* ����Ҫ���͵�Ŀ����Ķ�������ID */
		target->SetDesSAid(said);

		while(currentsentsize < contentSize)
		{
			/* ��÷��ʹ�С */
			snedsize = contentSize - currentsentsize;
			snedsize =  min(snedsize, packSize);

			/*���ضΣ���װ���� */
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
				
			/* �������� */
			status = target->ExecCommand(outbuf, cnt + snedsize + extra, GetTimeOut());
			if(TARGET_OK == status)
			{
				currentsentsize	+= snedsize;
				m_CurrentLoadSize += snedsize;
				InformationProgressMessageOutput("�Ѿ�����", (m_CurrentLoadSize / (float)m_LoadFileSize) * 100, m_CurrentLoadSize, m_LoadFileSize);
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
	 * @brief	  : �ж϶��Ƿ���Ҫ����
	 * @para[IN]  : Segment:   �ε�ַ
	 * @reture	  :  ��Ҫ���ط���TRUE,����Ҫ���ط���FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL LoadCommand::IsSegmentNeedLoad(Segment *segment)
	{
		INT32 segmentType = 0;

		/* ��ö�����*/
		segmentType = segment->GetType();

		if(PT_LOAD == segmentType)
		{
			return TRUE;        
		}
		
		return FALSE;
	}

	/**
	 * @Funcname  :GetLoadSegmentSize
	 * @brief	  :�ۼӶεĴ�С
	 * @para[IN]  : ELF32:   ELF�ṹָ��
	 * @reture	  :  �ܴ�С
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	UINT32 LoadCommand::GetLoadSegmentSize(ELF32 *elf32)
	{
		UINT32 segmentCount = 0;
		Segment *segment = NULL;
		UINT32 totalSize = 0;
		UINT32 index = 0;

		/* ��öεĸ��� */
		segmentCount = elf32->GetSegmentCount();

		/* ��ȡ�ε��ܴ�С */
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
	 * @brief	  : �����������Ƿ���ȫ
	 * @para[IN]  : ��
	 * @reture	  : �ɹ�����TRUE ʧ�ܷ���FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	 */
	BOOL LoadCommand::IsParamOK(void)
	{
		return Command::IsParamOK();
	}
}