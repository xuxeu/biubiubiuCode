/**********************************************************************************
*                     �����������ɼ������޹�˾ ��Ȩ����
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*�޸���ʷ:
*2010-9-15 9:57:23       ��˼Ω     �����������ɼ������޹�˾
*�޸ļ�����:�������ļ�
*/

/**
 *@file   ELF32_Parse.cpp
 *@brief
 *<li>ʵ��elf�ļ��Ľ���</li>
 *@Note
 */
/************************ͷ �� ��******************************/
#include "StdAfx.h"
#include "elf.h"
#include "Windows.h"
/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
FILE* ELF32_Parse::m_fp = NULL;

/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ʵ   ��*******************************/
/**
* @brief      elf�����๹�캯��
* @param[in]	��
* @return      ��
**/
ELF32_Parse::ELF32_Parse(VOID)
{

}

/**
* @brief      elf��������������
* @param[in]	��
* @return      ��
**/
ELF32_Parse::~ELF32_Parse(VOID)
{
}

/**
* @brief	����ӿڡ���ָ��·����elf�ļ���������
*			�������ݰ�����
*				�α�ͷ
*				�ڱ�ͷ
*				���ű�
*				�ַ�����
* @param	elfpath :elf�ļ��������ơ�
* @return	elf���ݽṹָ�롣
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

	/*����elfͷ*/
	if (_ElfParse(elf) != TRUE)
	{
		close(elf);
		printf("����ELF�ļ�ͷ����\n");
		return NULL;
	}

	/*����segmentͷ*/
	if (elf->e_phnum != 0)
	{
		ret = _SegmentParse(elf) ;

		if (FALSE == ret)
		{
			close(elf);
			printf("������ͷ����\n");
			return NULL;
		}
	}

	/*����sectionͷ*/
	if (elf->e_shnum != 0)
	{
		ret = _SectionParse(elf) ;

		if (FALSE == ret)
		{
			close(elf);
			printf("������ͷ����\n");
			return NULL;
		}
	}

	/*�����ַ�����*/
	if (elf->e_shstrndx != SHN_UNDEF)
	{
		ret = _StrTblParse(elf);
		if (FALSE == ret)
		{
			close(elf);
			printf("�����ַ������\n");
			return NULL;
		}
	}

	elf->parseDone = TRUE;

	return elf;
}

/**
* @brief		����ELF�ļ�ͷ��
* @param[in]	elf: elf ���ݽṹ
* @return		TRUE��	�����ɹ�
*				FALSE��	����ʧ��
**/
BOOL ELF32_Parse::_ElfParse(ELF32* elf)
{
	UINT32 cnt = 0;

	if (m_fp)
	{
		UINT32 rc = 0;

		/* ��ȡ�ļ�ͷ */
		INT8* ehdr = (INT8*)malloc(EHDR_LEN);

		rc = (UINT32)fread(ehdr, EHDR_LEN, 1, m_fp);
		if (rc != 1)
		{
			free(ehdr);
			return NULL;
			/* �׳��쳣 */
		}

		/* ��¼elfͷ */
		_ehdr_get(ehdr, elf);

		/* ����ļ�ͷ */
		if ( elf->e_ident[0] != 0x7f ||
			elf->e_ident[1] != 'E'  ||
			elf->e_ident[2] != 'L'  ||
			elf->e_ident[3] != 'F')
		{
			free(ehdr);
			return NULL;
			/* �׳��쳣 */
		}

		/* �ж��ļ��Ĵ�С�� */
		if (elf->e_ident[5] == ELFDATA2MSB)
		{
			/* ��˻���С�� */
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
* @brief		����section��
* @param[in]	elf: elf ���ݽṹ
* @return		TRUE�� �ɹ�
*				FALSE: ʧ��
**/
BOOL ELF32_Parse::_SectionParse(ELF32* elf)
{
	INT8* shdrp;
	INT8* ptr = NULL;
	UINT32 rc = 0;
	INT32 cnt = 0;
	Section* sec = NULL;

	/* ��ȡ����ͷ������ */
	/* ���section�����ṹ�ĳߴ� */
	if (elf->e_shentsize != SHDR_LEN)
	{
		printf("�����ڱ���󣺽ڱ������ṹ�ĳ��ȴ���");
		return FALSE;
	}

	/* Ϊ�ڱ����ռ� */
	shdrp = (INT8*)calloc(elf->e_shentsize, elf->e_shnum);
	if (shdrp == NULL)
	{
		printf("�ڱ������������ռ�ʧ��");
		free(shdrp);
		return FALSE;
	}

	/* ��λ�ļ����ڱ�λ�� */
	rc = fseek(m_fp, elf->e_shoff, SEEK_SET);
	if (rc != 0)
	{
		printf("�ڱ����ʧ��");
		free(shdrp);
		return FALSE;
	}

	/* ��ȡ���нڱ�ͷ���� */
	rc = (UINT32)fread(shdrp, elf->e_shentsize, elf->e_shnum, m_fp);
	if (rc != elf->e_shnum)
	{
		printf("�ڱ����ʧ��");
		free(shdrp);
		return FALSE;
	}

	if (elf->e_ident[5] == ELFDATA2MSB)
	{
		/* ��˻���С�� */
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

	/* �������нڱ�ͷ������ŵ�elf���ݽṹ�� */
	while (cnt < elf->e_shnum)
	{
		/* ����section */
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
* @brief       �������ű�
* @param		��
* @param[in]	symSec	�����������ڵĽ�
* @param[in]	elf     elf���ݽṹ
* @return		TRUE�� �ɹ�
*				FALSE: ʧ��
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
		printf("�����ڱ���󣺷��������ṹ�ĳ��ȴ���");
		return FALSE;
	}

	symData = (INT8*)malloc(symSec->sh_size);
	if (NULL == symData)
	{
		printf("����ռ�ʧ��\n");
		return FALSE;
	}

	rc = fseek(m_fp, symSec->sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("��ȡ�ļ�����ʧ��\n");
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
		/*��section���Ȳ�Ϊ���ų��ȵ�������������*/
		printf("��ȡ�������ݴ���\n");
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
* @brief		�����ַ�����
* @param[in]	elf:     elf���ݽṹ
* @return		TRUE�� �ɹ�
*				FALSE: ʧ��
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

	/* Ϊ�ַ��������ռ� */
	Section* strSec = elf->m_secTbl.at(elf->e_shstrndx);

	m_secStrTbl = (INT8*)malloc(strSec->sh_size);
	if (NULL == m_secStrTbl)
	{
		printf("�����ڱ��ַ�����:����ռ�ʧ��\n");
		free(m_secStrTbl);
		return FALSE;
	}

	/* ��λ�ļ����ַ����� */
	rc = fseek(m_fp, strSec->sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("�ڱ��ַ��������ʧ��");
		free(m_secStrTbl);
		return FALSE;
	}

	/* ��ȡ�ַ����� */
	rc = (INT32)fread(m_secStrTbl, strSec->sh_size, 1, m_fp);
	if (rc != 1)
	{
		printf("�ڱ��ַ��������ʧ��");
		free(m_secStrTbl);
		return FALSE;
	}

	elf->m_secStrTbl = (char*)m_secStrTbl;

	if (elf->e_shnum != 0)
	{
		/* ����Section���� */
		while (cnt < elf->e_shnum)
		{
			sec = elf->m_secTbl.at(cnt);
			sec->nameStr = (char*)(m_secStrTbl + sec->sh_name);

			/* �����ַ����� */
			if (sec->nameStr == ".strtab")
			{
				/* ��ȡ�����ַ����� */
				m_symStrTbl = (INT8*)malloc(sec->sh_size);
				if (m_symStrTbl == NULL)
				{
					printf("�����ַ��������������ռ�ʧ��\n");
					free(m_secStrTbl);
					return FALSE;
				}

				/* ��λ�ļ����ַ����� */
				rc = fseek(m_fp, sec->sh_offset, SEEK_SET);
				if (rc != 0)
				{
					printf("�����ַ��������ʧ��\n");
					free(m_secStrTbl);
					return FALSE;
				}

				rc = (INT32)fread(m_symStrTbl, sec->sh_size, 1, m_fp);
				if (rc != 1)
				{
					printf("�����ַ��������ʧ��\n");
					free(m_secStrTbl);
					return FALSE;
				}

				/* ��¼�����ַ����� */
				elf->m_symStrTbl = (char*)m_symStrTbl;
			}
			cnt ++;
		}
	}	

	cnt = 0;

	/* ���÷������� */
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
* @brief       ������ͷ
* @param[in]	elf:     elf���ݽṹ
* @return		TRUE�� �ɹ�
*				FALSE: ʧ��
**/
BOOL ELF32_Parse::_SegmentParse(ELF32* elf)
{
	INT8* phdrp;
	INT8* ptr = NULL;
	UINT32 rc = 0;
	INT32 cnt = 0;
	Segment* segment = NULL;

	/* ��ȡ��ͷ������ */
	/* ���segment�����ṹ�ĳߴ� */
	if (elf->e_phentsize != PHDR_LEN)
	{
		printf("�����ڱ���󣺶α������ṹ�ĳ��ȴ���");
		return FALSE;
	}

	/* Ϊ�α����ռ�  */
	phdrp = (INT8*)calloc(elf->e_phentsize, elf->e_phnum);
	if (phdrp == NULL)
	{
		printf("����ռ�ʧ��\n");
		free(phdrp);
		return FALSE;
	}

	/* ��λ�ļ����α�λ��  */
	rc = fseek(m_fp, elf->e_phoff, SEEK_SET);
	if (rc != 0)
	{
		printf("���ļ���ȡ����ʧ��\n");
		free(phdrp);
		return FALSE;
	}

	/* ��ȡ���ж�ͷ����*/
	rc = (UINT32)fread(phdrp, elf->e_phentsize, elf->e_phnum, m_fp);
	if (rc != elf->e_phnum)
	{
		printf("���ļ���ȡ����ʧ��\n");
		free(phdrp);
		return FALSE;
	}

	if (elf->e_ident[5] == ELFDATA2MSB)
	{
		/*��˻���С�� */
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
* @brief       ���ת����С��
* @param[in]	cnt  ��ת�������size
* @param[in]	ptr  ָ���ת�����ݵ�ָ��
* @return		��
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
* @brief       ��ȡ����elfͷ���ݣ�������fpָ�롣
* @param[in]	ehdr	elfͷ����
* @return      ��
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
* @brief		��ȡ����section���ݣ�����¼elf�ļ�ָ��
* @param[in]	shdr: sectionͷ����
* @return		��
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

	/* ÿ���ڱ�ͷ���ݽṹ������elf�ļ�ָ�� */
	singleSec->m_fp = m_fp;

	return singleSec;
}

/**
* @brief		��ȡ����segment���ݣ�����¼elf�ļ�ָ��
* @param[in]	phdr	segmentͷ����
* @return		��
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

	/* ÿ����ͷ���ݽṹ������elf�ļ�ָ�� */
	singleSgmt->m_fp = m_fp;

	return singleSgmt;
}

/**
* @brief		��ȡ������������
* @param[in]	symData: �������ķ������ݴ�ŵĵ�ַ
* @param[in]	elf:     elf���ݽṹ
* @return		��
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

	/* ��С��ת�� */
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
* @brief       ��ȡ˫�ֽڵ�����
* @param[in]	ptr  ָ��ת�������ָ��
* @return      ��
**/
INT16 ELF32_Parse::_read_HWord(INT8 **ptr)
{
	UINT16* temp = (UINT16*)(*ptr);
	(*ptr)+=2;
	return *temp;
}


/**
* @brief      ��ȡ4�ֽڵ�����
* @param[in]	ptr  ָ��ת�������ָ��
* @return      ��
**/
INT32 ELF32_Parse::_read_Word(INT8 **ptr)
{
	UINT32* temp = (UINT32*)(*ptr);
	(*ptr)+=4;
	return *temp;
}

/**
* @brief       ��ȡ���ֽڵ�����
* @param[in]	ptr  ָ��ת�������ָ��
* @return      ��
**/
INT8 ELF32_Parse::_read_Byte(INT8 **ptr)
{
	UINT8* temp = (UINT8*)(*ptr);
	(*ptr)+=1;
	return *temp;
}

/**
* @brief		�ر�elf�ļ����ͷ���Դ
* @param[in]	ptr  ָ��ת�������ָ��
* @return		��
**/
VOID ELF32_Parse::close(ELF32 *elf)
{
	/*�ͷ������Դ*/
	if (NULL != elf)
	{
		fclose(m_fp);
		delete(elf);
	}
}