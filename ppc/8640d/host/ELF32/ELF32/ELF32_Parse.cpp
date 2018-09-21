/**********************************************************************************
*                     北京科银京成技术有限公司 版权所有
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*修改历史:
*2010-9-15 9:57:23       文思惟     北京科银京成技术有限公司
*修改及方案:创建本文件
*/

/**
 *@file   ELF32_Parse.cpp
 *@brief
 *<li>实现elf文件的解析</li>
 *@Note
 */
/************************头 文 件******************************/
#include "StdAfx.h"
#include "elf.h"
#include "Windows.h"
/************************宏 定 义******************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
FILE* ELF32_Parse::m_fp = NULL;

/************************外部声明******************************/
/************************前向声明******************************/
/************************实   现*******************************/
/**
* @brief      elf解析类构造函数
* @param[in]	无
* @return      无
**/
ELF32_Parse::ELF32_Parse(VOID)
{

}

/**
* @brief      elf解析类析构函数
* @param[in]	无
* @return      无
**/
ELF32_Parse::~ELF32_Parse(VOID)
{
}

/**
* @brief	对外接口。打开指定路径的elf文件并解析。
*			解析内容包括：
*				段表头
*				节表头
*				符号表
*				字符串表
* @param	elfpath :elf文件完整名称。
* @return	elf数据结构指针。
**/
ELF32* ELF32_Parse::open(const char* elfPath)
{
	INT32 ret = 0;
	ELF32* elf = new ELF32();

	m_fp = fopen((const char *)elfPath, "rb");

	if (m_fp == NULL)
	{
		return NULL;
	}

	/*解析elf头*/
	if (_ElfParse(elf) != TRUE)
	{
		close(elf);
		printf("解析ELF文件头错误\n");
		return NULL;
	}

	/*解析segment头*/
	if (elf->e_phnum != 0)
	{
		ret = _SegmentParse(elf) ;

		if (FALSE == ret)
		{
			close(elf);
			printf("解析段头错误\n");
			return NULL;
		}
	}

	/*解析section头*/
	if (elf->e_shnum != 0)
	{
		ret = _SectionParse(elf) ;

		if (FALSE == ret)
		{
			close(elf);
			printf("解析节头错误\n");
			return NULL;
		}
	}

	/*解析字符串表*/
	if (elf->e_shstrndx != SHN_UNDEF)
	{
		ret = _StrTblParse(elf);
		if (FALSE == ret)
		{
			close(elf);
			printf("解析字符表错误\n");
			return NULL;
		}
	}

	elf->parseDone = TRUE;

	return elf;
}

/**
* @brief		解析ELF文件头。
* @param[in]	elf: elf 数据结构
* @return		TRUE：	解析成功
*				FALSE：	解析失败
**/
BOOL ELF32_Parse::_ElfParse(ELF32* elf)
{
	UINT32 cnt = 0;

	if (m_fp)
	{
		UINT32 rc = 0;

		/* 读取文件头 */
		INT8* ehdr = (INT8*)malloc(EHDR_LEN);

		rc = (UINT32)fread(ehdr, EHDR_LEN, 1, m_fp);
		if (rc != 1)
		{
			free(ehdr);
			return NULL;
			/* 抛出异常 */
		}

		/* 记录elf头 */
		_ehdr_get(ehdr, elf);

		/* 检查文件头 */
		if ( elf->e_ident[0] != 0x7f ||
			elf->e_ident[1] != 'E'  ||
			elf->e_ident[2] != 'L'  ||
			elf->e_ident[3] != 'F')
		{
			free(ehdr);
			return NULL;
			/* 抛出异常 */
		}

		/* 判断文件的大小端 */
		if (elf->e_ident[5] == ELFDATA2MSB)
		{
			/* 大端换成小端 */
			_big2little(2, (VOID*)(&(elf->e_type)));
			_big2little(2, (VOID*)(&(elf->e_machine)));
			_big2little(4, (VOID*)(&(elf->e_version)));
			_big2little(4, (VOID*)(&(elf->e_entry)));
			_big2little(4, (VOID*)(&(elf->e_phoff)));
			_big2little(4, (VOID*)(&(elf->e_shoff)));
			_big2little(4, (VOID*)(&(elf->e_flags)));
			_big2little(2, (VOID*)(&(elf->e_ehsize)));
			_big2little(2, (VOID*)(&(elf->e_phentsize)));
			_big2little(2, (VOID*)(&(elf->e_phnum)));
			_big2little(2, (VOID*)(&(elf->e_shentsize)));
			_big2little(2, (VOID*)(&(elf->e_shnum)));
			_big2little(2, (VOID*)(&(elf->e_shstrndx)));
		}

		free(ehdr);
		return TRUE;
	}

	return NULL;
}

/**
* @brief		解析section段
* @param[in]	elf: elf 数据结构
* @return		TRUE： 成功
*				FALSE: 失败
**/
BOOL ELF32_Parse::_SectionParse(ELF32* elf)
{
	INT8* shdrp;
	INT8* ptr = NULL;
	UINT32 rc = 0;
	INT32 cnt = 0;
	Section* sec = NULL;

	/* 读取节区头表内容 */
	/* 检查section描述结构的尺寸 */
	if (elf->e_shentsize != SHDR_LEN)
	{
		printf("解析节表错误：节表描述结构的长度错误");
		return FALSE;
	}

	/* 为节表分配空间 */
	shdrp = (INT8*)calloc(elf->e_shentsize, elf->e_shnum);
	if (shdrp == NULL)
	{
		printf("节表解析错误：申请空间失败");
		free(shdrp);
		return FALSE;
	}

	/* 定位文件到节表位置 */
	rc = fseek(m_fp, elf->e_shoff, SEEK_SET);
	if (rc != 0)
	{
		printf("节表解析失败");
		free(shdrp);
		return FALSE;
	}

	/* 获取所有节表头数据 */
	rc = (UINT32)fread(shdrp, elf->e_shentsize, elf->e_shnum, m_fp);
	if (rc != elf->e_shnum)
	{
		printf("节表解析失败");
		free(shdrp);
		return FALSE;
	}

	if (elf->e_ident[5] == ELFDATA2MSB)
	{
		/* 大端换成小端 */
		cnt = 0;
		ptr = shdrp;

		while (cnt < (elf->e_shentsize * elf->e_shnum))
		{
			_big2little(4,(VOID*)ptr);
			ptr += 4;
			cnt += 4;
		}
	}

	cnt = 0;
	ptr = shdrp;

	/* 解析所有节表头，并存放到elf数据结构中 */
	while (cnt < elf->e_shnum)
	{
		/* 解析section */
		sec = _shdr_get(ptr);

		elf->m_secTbl.insert(elf->m_secTbl.end(), sec);
	
		if (SHT_SYMTAB == sec->sh_type)
		{
			elf->m_symCtl = new SymbolTable();
			_SymbolParse(sec, elf);
		}

		ptr += elf->e_shentsize;
		cnt ++;
	}

	free(shdrp);
	return TRUE;
}

/**
* @brief       解析符号表
* @param		无
* @param[in]	symSec	符号数据所在的节
* @param[in]	elf     elf数据结构
* @return		TRUE： 成功
*				FALSE: 失败
**/
BOOL ELF32_Parse::_SymbolParse(Section* symSec, ELF32* elf)
{
	UINT32 rc = 0;
	UINT32 cnt = 0;
	INT8* symData = NULL;
	INT8* ptr = NULL;
	UINT32 symCnt = 0;
	Symbol *sym = NULL;

	if (symSec->sh_entsize != SYM_LEN)
	{
		printf("解析节表错误：符号描述结构的长度错误");
		return FALSE;
	}

	symData = (INT8*)malloc(symSec->sh_size);
	if (NULL == symData)
	{
		printf("申请空间失败\n");
		return FALSE;
	}

	rc = fseek(m_fp, symSec->sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("获取文件数据失败\n");
		free(symData);
		return FALSE;
	}

	rc = (UINT32)fread(symData, 1, symSec->sh_size, m_fp);
	if (rc != symSec->sh_size)
	{
		rc = GetLastError();
	}

	if (symSec->sh_size % symSec->sh_entsize)
	{
		/*该section长度不为符号长度的整数倍，错误*/
		printf("获取符号数据错误\n");
		free(symData);
		return FALSE;
	}

	symCnt = symSec->sh_size / symSec->sh_entsize;
	ptr = symData;

	while (cnt < symCnt)
	{
		sym = _sym_get(ptr, elf);
		elf->m_symCtl->m_symTbl.insert(elf->m_symCtl->m_symTbl.end(),sym);

		cnt++;
		ptr += symSec->sh_entsize;
	}

	free(symData);

	return TRUE;

}

/**
* @brief		解析字符串表
* @param[in]	elf:     elf数据结构
* @return		TRUE： 成功
*				FALSE: 失败
**/
BOOL ELF32_Parse::_StrTblParse(ELF32* elf)
{
	INT32 rc = 0;
	UINT32 cnt = 0;
	UINT32 size = 0;
	INT8* m_secStrTbl = NULL;
	INT8* m_symStrTbl = NULL;
	Symbol *sym;
	Section* sec;
	UINT32 numPos;

	/* 为字符串表分配空间 */
	Section* strSec = elf->m_secTbl.at(elf->e_shstrndx);

	m_secStrTbl = (INT8*)malloc(strSec->sh_size);
	if (NULL == m_secStrTbl)
	{
		printf("解析节表字符串表:申请空间失败\n");
		free(m_secStrTbl);
		return FALSE;
	}

	/* 定位文件到字符串段 */
	rc = fseek(m_fp, strSec->sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("节表字符串表解析失败");
		free(m_secStrTbl);
		return FALSE;
	}

	/* 读取字符串表 */
	rc = (INT32)fread(m_secStrTbl, strSec->sh_size, 1, m_fp);
	if (rc != 1)
	{
		printf("节表字符串表解析失败");
		free(m_secStrTbl);
		return FALSE;
	}

	elf->m_secStrTbl = (char*)m_secStrTbl;

	if (elf->e_shnum != 0)
	{
		/* 设置Section名称 */
		while (cnt < elf->e_shnum)
		{
			sec = elf->m_secTbl.at(cnt);
			sec->nameStr = (char*)(m_secStrTbl + sec->sh_name);

			/* 符号字符串表 */
			if (sec->nameStr == ".strtab")
			{
				/* 获取符号字符串表 */
				m_symStrTbl = (INT8*)malloc(sec->sh_size);
				if (m_symStrTbl == NULL)
				{
					printf("符号字符串表解析：申请空间失败\n");
					free(m_secStrTbl);
					return FALSE;
				}

				/* 定位文件到字符串段 */
				rc = fseek(m_fp, sec->sh_offset, SEEK_SET);
				if (rc != 0)
				{
					printf("符号字符串表解析失败\n");
					free(m_secStrTbl);
					return FALSE;
				}

				rc = (INT32)fread(m_symStrTbl, sec->sh_size, 1, m_fp);
				if (rc != 1)
				{
					printf("符号字符串表解析失败\n");
					free(m_secStrTbl);
					return FALSE;
				}

				/* 记录符号字符串表 */
				elf->m_symStrTbl = (char*)m_symStrTbl;
			}
			cnt ++;
		}
	}	

	cnt = 0;

	/* 设置符号名称 */
	size = (INT32)elf->m_symCtl->m_symTbl.size();
	if (size != 0)
	{
		while (cnt < size)
		{
			sym = elf->m_symCtl->m_symTbl.at(cnt);
			numPos = sym->st_name;
			sym->nameStr = (char*)(m_symStrTbl + sym->st_name);	
			cnt++;
		}
	}

	free(m_symStrTbl);
	free(m_secStrTbl);

	return TRUE;
}

/**
* @brief       解析段头
* @param[in]	elf:     elf数据结构
* @return		TRUE： 成功
*				FALSE: 失败
**/
BOOL ELF32_Parse::_SegmentParse(ELF32* elf)
{
	INT8* phdrp;
	INT8* ptr = NULL;
	UINT32 rc = 0;
	INT32 cnt = 0;
	Segment* segment = NULL;

	/* 读取段头表内容 */
	/* 检查segment描述结构的尺寸 */
	if (elf->e_phentsize != PHDR_LEN)
	{
		printf("解析节表错误：段表描述结构的长度错误");
		return FALSE;
	}

	/* 为段表分配空间  */
	phdrp = (INT8*)calloc(elf->e_phentsize, elf->e_phnum);
	if (phdrp == NULL)
	{
		printf("申请空间失败\n");
		free(phdrp);
		return FALSE;
	}

	/* 定位文件到段表位置  */
	rc = fseek(m_fp, elf->e_phoff, SEEK_SET);
	if (rc != 0)
	{
		printf("从文件获取数据失败\n");
		free(phdrp);
		return FALSE;
	}

	/* 获取所有段头数据*/
	rc = (UINT32)fread(phdrp, elf->e_phentsize, elf->e_phnum, m_fp);
	if (rc != elf->e_phnum)
	{
		printf("从文件获取数据失败\n");
		free(phdrp);
		return FALSE;
	}

	if (elf->e_ident[5] == ELFDATA2MSB)
	{
		/*大端换成小端 */
		cnt = 0;
		ptr = phdrp;

		while (cnt < (elf->e_phentsize * elf->e_phnum))
		{
			_big2little(4,(VOID*)ptr);
			ptr += 4;
			cnt += 4;
		}
	}

	cnt = 0;
	ptr = phdrp;

	while (cnt < elf->e_phnum)
	{
		segment = _phdr_get(ptr);
		elf->m_sgmtTbl.insert(elf->m_sgmtTbl.end(), segment);

		ptr += elf->e_phentsize;
		cnt ++;
	}

	free(phdrp);

	return TRUE;
 }


/**
* @brief       大端转换成小端
* @param[in]	cnt  被转换对象的size
* @param[in]	ptr  指向待转换数据的指针
* @return		无
**/
VOID ELF32_Parse::_big2little(INT32 cnt, VOID *ptr)
{
	INT8* ByteData;
	INT8	TempByte;

	/* Get data pointer */
	ByteData = ( INT8 * )ptr;

	switch(cnt)
	{
	case 2:
		{
			TempByte = ByteData[0];
			ByteData[0] = ByteData[1];
			ByteData[1] = TempByte;
			break;
		}

	case 4:
		{
			TempByte = ByteData[0];
			ByteData[0] = ByteData[3];
			ByteData[3] = TempByte;
			TempByte = ByteData[1];
			ByteData[1] = ByteData[2];
			ByteData[2] = TempByte;
			break;
		}
	default:
		{
			TempByte = ByteData[0];
			ByteData[0] = ByteData[3];
			ByteData[3] = TempByte;
			TempByte = ByteData[1];
			ByteData[1] = ByteData[2];
			ByteData[2] = TempByte;	
		}
	}
}

/**
* @brief       获取单个elf头数据，并保存fp指针。
* @param[in]	ehdr	elf头数据
* @return      无
**/
VOID ELF32_Parse::_ehdr_get(INT8* ehdr, ELF32* elf)
{
	INT8 *temp = ehdr;

	elf->e_ident[EI_MAG0] = _read_Byte(&temp);
	elf->e_ident[EI_MAG1] = _read_Byte(&temp);
	elf->e_ident[EI_MAG2] = _read_Byte(&temp);
	elf->e_ident[EI_MAG3] = _read_Byte(&temp);
	elf->e_ident[EI_CLASS] = _read_Byte(&temp);
	elf->e_ident[EI_DATA] = _read_Byte(&temp);
	elf->e_ident[EI_VERSION] = _read_Byte(&temp);
	temp += 9;
	elf->e_type = (elf_type)_read_HWord(&temp);
	elf->e_machine = (elf_Machine)_read_HWord(&temp);
	elf->e_version = _read_Word(&temp);
	elf->e_entry = _read_Word(&temp);
	elf->e_phoff = _read_Word(&temp);
	elf->e_shoff = _read_Word(&temp);
	elf->e_flags = _read_Word(&temp);
	elf->e_ehsize = _read_HWord(&temp);
	elf->e_phentsize = _read_HWord(&temp);
	elf->e_phnum = _read_HWord(&temp);
	elf->e_shentsize = _read_HWord(&temp);
	elf->e_shnum = _read_HWord(&temp);
	elf->e_shstrndx = _read_HWord(&temp);
}

/**
* @brief		获取单个section数据，并记录elf文件指针
* @param[in]	shdr: section头数据
* @return		无
**/
Section* ELF32_Parse::_shdr_get(INT8* shdr)
{
	INT8 *temp = shdr;
	Section *singleSec = new Section();

	singleSec->sh_name = _read_Word(&temp);
	singleSec->sh_type = _read_Word(&temp);
	singleSec->sh_flags = _read_Word(&temp);
	singleSec->sh_addr = _read_Word(&temp);
	singleSec->sh_offset = _read_Word(&temp);
	singleSec->sh_size = _read_Word(&temp);
	singleSec->sh_link = _read_Word(&temp);
	singleSec->sh_info = _read_Word(&temp);
	singleSec->sh_addralign = _read_Word(&temp);
	singleSec->sh_entsize = _read_Word(&temp);

	/* 每个节表头数据结构都保存elf文件指针 */
	singleSec->m_fp = m_fp;

	return singleSec;
}

/**
* @brief		获取单个segment数据，并记录elf文件指针
* @param[in]	phdr	segment头数据
* @return		无
**/
Segment* ELF32_Parse::_phdr_get(INT8* phdr)
{
	INT8 *temp = phdr;
	Segment *singleSgmt = new Segment();

	singleSgmt->p_type = _read_Word(&temp);
	singleSgmt->p_offset = _read_Word(&temp);
	singleSgmt->p_vaddr = _read_Word(&temp);
	singleSgmt->p_paddr = _read_Word(&temp);
	singleSgmt->p_filesz = _read_Word(&temp);
	singleSgmt->p_memsz = _read_Word(&temp);
	singleSgmt->p_flags = _read_Word(&temp);
	singleSgmt->p_align = _read_Word(&temp);	

	/* 每个段头数据结构都保存elf文件指针 */
	singleSgmt->m_fp = m_fp;

	return singleSgmt;
}

/**
* @brief		获取单个符号数据
* @param[in]	symData: 待解析的符号数据存放的地址
* @param[in]	elf:     elf数据结构
* @return		无
**/
Symbol* ELF32_Parse::_sym_get(INT8 *symData, ELF32* elf)
{
	INT8 *temp = symData;
	Symbol *singleSym = new Symbol();

	singleSym->st_name = _read_Word(&temp);	
	singleSym->st_value = _read_Word(&temp);	
	singleSym->st_size = _read_Word(&temp);		
	singleSym->st_info = _read_Byte(&temp);		
	singleSym->st_other = _read_Byte(&temp);	
	singleSym->st_shndx = _read_HWord(&temp);	

	/* 大小端转换 */
	if (elf->e_ident[5] == ELFDATA2MSB)
	{
		_big2little(4, (VOID*)(&(singleSym->st_name)));
		_big2little(4, (VOID*)(&(singleSym->st_value)));
		_big2little(4, (VOID*)(&(singleSym->st_size)));
		_big2little(1, (VOID*)(&(singleSym->st_info)));
		_big2little(1, (VOID*)(&(singleSym->st_other)));
		_big2little(2, (VOID*)(&(singleSym->st_shndx)));
	}

	return singleSym;
}


/**
* @brief       读取双字节的数据
* @param[in]	ptr  指向转换对象的指针
* @return      无
**/
INT16 ELF32_Parse::_read_HWord(INT8 **ptr)
{
	UINT16* temp = (UINT16*)(*ptr);
	(*ptr)+=2;
	return *temp;
}


/**
* @brief      读取4字节的数据
* @param[in]	ptr  指向转换对象的指针
* @return      无
**/
INT32 ELF32_Parse::_read_Word(INT8 **ptr)
{
	UINT32* temp = (UINT32*)(*ptr);
	(*ptr)+=4;
	return *temp;
}

/**
* @brief       读取单字节的数据
* @param[in]	ptr  指向转换对象的指针
* @return      无
**/
INT8 ELF32_Parse::_read_Byte(INT8 **ptr)
{
	UINT8* temp = (UINT8*)(*ptr);
	(*ptr)+=1;
	return *temp;
}

/**
* @brief		关闭elf文件，释放资源
* @param[in]	ptr  指向转换对象的指针
* @return		无
**/
VOID ELF32_Parse::close(ELF32 *elf)
{
	/*释放相关资源*/
	if (NULL != elf)
	{
		fclose(m_fp);
		delete(elf);
	}
}