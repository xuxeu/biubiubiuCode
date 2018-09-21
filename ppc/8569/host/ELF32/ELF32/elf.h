/**********************************************************************************
*                     �����������ɼ������޹�˾ ��Ȩ����
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*�޸���ʷ:
*2010-9-15 9:55:08        ��˼Ω      �����������ɼ������޹�˾
*�޸ļ�����:�������ļ�
*/

/**
 *@file   elf.h
 *@brief
 *<li>elf�ࡣʵ�ֶ�elf�ļ��Ľ�������ȡelf�ļ����ݵĹ��ܡ�
 *</li>
 *@Note
 */

#ifndef _INCLUDE_ELF_H
#define _INCLUDE_ELF_H
/************************ͷ �� ��******************************/
#include "sysTypes.h"

#if 0

#include <vector>
#include <string>
using namespace std;

#else

#include "LoaderString.h"
#include "Vector.h"
#define vector Vector
#define string String
using namespace com_coretek_tools_stl;

#endif
/************************��    ��******************************/
/************************�� �� ��******************************/
/************************���Ͷ���******************************/


#ifdef ELF32_EXPORTS
#define ELF32_API __declspec(dllexport)
#else
#define ELF32_API __declspec(dllimport)
#endif


#define FAIL -1
#define SUCCESSFUL 0


/* object�и������ݳ���*/
enum basic_type
{
	EHDR_LEN = 52,
	PHDR_LEN = 32,
	SHDR_LEN = 40,
	SYM_LEN = 16,
	NAME_LEN = 256
};

/* object���� */
enum elf_type
{
	ET_NONE = 0,		/*No file type*/
	ET_REL = 1,			/*Relocatable file*/
	ET_EXEC = 2,		/*Executable file*/
	ET_DYN = 3,			/*Shared object file*/
	ET_CORE = 4,		/*Core file*/
	ET_LOPROC = 0xff00 ,/*Processor-specific*/
	ET_HIPROC = 0xffff  /*Processor-specific*/
};

/* object��ϵ�ṹ */
enum elf_Machine
{
	EM_NONE = 0x0,		/*No machine*/
	EM_M32 =  0x1,		/*AT&T WE 32100*/
	EM_SPARC = 0x2,		/*SPARC*/
	EM_386 = 0x3,		/*Intel 80386*/
	EM_68K = 0x4,		/*Motorola 68000*/
	EM_88K = 0x5,		/*Motorola 88000*/
	EM_860 =  0x7,		/*Intel 80860*/
	EM_MIPS = 0x8,		/*MIPS RS3000*/
	EM_POWERPC = 0x14   /*PowerPC*/
};

/* object ident����*/
enum elf_identNum
{
	EI_MAG0 = 0,	/*File identification*/
	EI_MAG1 = 1,	/*File identification*/
	EI_MAG2 = 2,	/*File identification*/
	EI_MAG3 = 3,	/*File identification*/
	EI_CLASS = 4,	/*File class*/
	EI_DATA = 5,	/*Data encoding*/
	EI_VERSION = 6,	/*File version*/
	EI_PAD = 7,		/*Start of padding bytes*/
	EI_NIDENT = 16	/*Size of e_ident[]*/
};

/* object���� */
enum elf_class
{
	ELFCLASSNONE = 0,	/*nvalid class*/
	ELFCLASS32 = 1,		/*32-bit objects*/
	ELFCLASS64 = 2		/*64-bit objects*/
};

/* object�����ʽ*/
enum elf_Code
{
	ELFDATANONE = 0,	/*Invalid data encoding*/
	ELFDATA2LSB = 1,	/*little endian*/
	ELFDATA2MSB = 2		/*big endian*/
};

enum elf_specialSecIndex
{
	SHN_UNDEF = 0,
	SHN_LORESERVE = 0xff00,
	SHN_LOPROC = 0xff00,
	SHN_HIPROC = 0xff1f,
	SHN_ABS = 0xfff1,
	SHN_COMMON = 0xfff2,
	SHN_HIRESERVE = 0xffff
};

enum section_flags
{
	SHF_WRITE = 0x1,
	SHF_ALLOC = 0x2,
	SHF_EXECINSTR = 0x4,
	SHF_MASKPROC = 0xf0000000
};

enum section_types
{
	SHT_NULL = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB = 2,
	SHT_STRTAB = 3,
	SHT_RELA = 4,
	SHT_HASH = 5,
	SHT_DYNAMIC = 6,
	SHT_NOTE = 7,
	SHT_NOBITS = 8,
	SHT_REL = 9,
	SHT_SHLIB = 10,
	SHT_DYNSYM = 11,
	SHT_LOPROC = 0x70000000,
	SHT_HIPROC = 0x7fffffff,
	SHT_LOUSER = 0x80000000,
	SHT_HIUSER = 0xffffffff
};

enum segment_types
{
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_LOPROC = 0x70000000,
	PT_HIPROC = 0x7fffffff
};


class ELF32_API Symbol
{
public:
	Symbol(void);
	~Symbol(void);
	friend class ELF32_Parse;
	friend class SymbolTable;

	/* ��ȡ��������	*/
	BOOL GetName(char* buf);

	/* ��ȡ����ֵ */
	INT32 GetValue();

	/* ��ȡ���Ŵ�С	*/
	UINT32 GetSize();

	/* ��ȡ�������� */
	INT8 GetInfo();

	/* ��ȡ������ķ��� */
	INT32 GetAssociatedSection();

private:
	INT32	st_name;		/* Symbol name*/
	string  nameStr;
	INT32	st_value;		/* Value of the symbol */
	INT32	st_size;		/* Associated symbol size */
	UINT8	st_info;		/* Type and binding attributes */
	UINT8	st_other;		/* No defined meaning, 0 */
	UINT16 st_shndx;		/* Associated section */

};


class ELF32_API Section
{
public:
	Section(void);
	~Section(void);
	friend class ELF32_Parse;

	/* ��ȡ�������� */
	UINT32 GetType();

	/* ��ȡ�������� */
	BOOL GetContents(INT8* buf, UINT32 len);

	/* ��ȡ�������� */
	BOOL GetName(char* nameBuf);

	/* ��ȡ����flags*/
	UINT32 GetFlags();

	/* ��ȡ�������ڴ��еĵ�ַ */
	UINT32 GetAddr();

	/* ��ȡ�������ļ��е�ƫ���� */
	UINT32 GetOffset();

	/* ��ȡ������С*/
	UINT32 GetSize();

	/* ��ȡ��������������� */
	UINT32 GetLink();

	/* ��ȡ����������Ϣ */
	UINT32 GetInfo();

	/* ��ȡ������������ */
	UINT32 GetAlign();

	/* ��ȡ������ŵı�Ĺ̶���С */
	UINT32 GetEntSize();

private:

	UINT32 sh_name;
	string nameStr; 
	UINT32 sh_type;
	UINT32 sh_flags;
	UINT32 sh_addr;
	UINT32 sh_offset;
	INT32 sh_size;
	UINT32 sh_link;
	UINT32 sh_info;
	UINT32 sh_addralign;
	UINT32 sh_entsize;

	FILE *m_fp;
};

class ELF32_API SymbolTable :
	public Section
{
public:
	SymbolTable(void);
	~SymbolTable(void);
	friend class ELF32_Parse;
	friend class ELF32;

	/**
	* @brief
	*	 ��ȡ���Ÿ���
	* @param	��
	* @return	���Ÿ�����
	*/
	UINT32 GetSymCount();

	/**
	* @brief
	*	 ��ȡ�������
	* @param	symName ��������
	* @return  ������š�
	*/
	INT32 GetSymIndex(string symName);

	/**
	* @brief
	*	 ��ȡ�������
	* @param	symValue ����ֵ
	* @return  ������š�
	*/
	INT32 GetSymIndex(UINT32 symValue);

	/**
	* @brief
	*	 ��ȡ����
	* @param	symValue ����ֵ
	* @return  ������š�
	*/
	Symbol* GetSymbol(UINT32 index);

private:
	vector<Symbol*> m_symTbl;

};

class ELF32_API Segment
{
public:
	Segment(void);
	~Segment(void);
	friend class ELF32_Parse;

	/* ��ȡ������ */
	INT32 GetType();

	/*��ȡ����m_elf�ļ��е�ƫ��λ��*/
	UINT32 GetOffset();

	/*��ȡ�ö����ڴ��е����ֽڵ�ַ*/
	UINT32 GetVirtualAddr();

	/*��ȡ�ε������ַ��λ������еĻ���*/
	UINT32 GetPhysicalAddr();

	/* ��ȡ������ */
	BOOL GetContents(INT8* buf, UINT32 len);

	/* ��ȡ���ļ��ռ� */
	UINT32 GetFileSize();

	/* ��ȡ���ڴ�ռ��С*/
	UINT32 GetMemSize();

	/* ��ȡ�ζ����ֽ� */
	UINT32 GetAlign();

	/* �жϸö����� */
	INT32 GetFlags();

private:
	INT32 p_type;
	UINT32 p_offset;
	UINT32 p_vaddr;
	UINT32 p_paddr;
	INT32 p_filesz;
	INT32 p_memsz;
	INT32 p_flags;
	INT32 p_align;	

	FILE *m_fp;

};




class ELF32_API ELF32
{
public:
	ELF32();
	friend class ELF32_Parse;
	~ELF32(void);

	/* 
	* ELFͷ������Ϣ 
	*/
	/* ��ȡelf���� */
	elf_type GetFileType();

	/* ��ȡelf��ʼ��ַ */
	UINT32 GetStartAddr();

	/* ��ȡelf��С�� */
	UINT32 GetEndian();

	/* ��ȡelf��ϵ�ṹ */
	elf_Machine GetCpuType();

	/* ��ȡ�ڱ����� */
	INT16 GetSectionCount();

	/* ��ȡ������ */
	INT16 GetSegmentCount();

	/* ��ȡָ����ŵĶα� */
	Segment* GetSegment(UINT32 index);

	/* ��ȡָ����ŵĽڱ� */
	Section* GetSection(UINT32 index);

	/* ����ָ�����ƵĶε���� */
	UINT16 GetSectionIndex(char* name);

	/* �鿴�Ƿ��Ѿ������˽��� */
	INT32 IsElfParse();

	/* ��ȡ���ű���ƽṹ */
	SymbolTable* GetSymTable();

private:
	INT32 parseDone;

	/* ���ű�ʵ������¼�����з��ű����ݣ����ṩ���Ų����ӿ� */
	SymbolTable* m_symCtl;

	/* ��ͷ���� */
	vector<Segment*> m_sgmtTbl;

	/* �ڱ�ͷ���� */
	vector<Section*> m_secTbl;

	/* �ڱ����ַ��� */
	string m_secStrTbl;

	/* �������ַ���*/
	string m_symStrTbl;

	/*m_elfͷ����*/
	UINT8 e_ident[EI_NIDENT];	/* ELF "magic number" */
	elf_type e_type;				/* Identifies object file type */
	elf_Machine e_machine;			/* Specifies required architecture */
	UINT32 e_version;			/* Identifies object file version */
	UINT32 e_entry;				/* Entry point virtual address */
	UINT32 e_phoff;				/* Program header table file offset */
	INT32 e_shoff;				/* Sector header table file offset */
	UINT32 e_flags;				/* Processor-specific flags */
	UINT16 e_ehsize;			/* ELF header size in bytes */
	UINT16 e_phentsize;			/* Program header table entry size */
	UINT16 e_phnum;				/* Program header table entry count */
	UINT16 e_shentsize;			/* Sector header table entry size */
	UINT16 e_shnum;				/* Sector header table entry count */
	UINT16 e_shstrndx;			/* Sector header string table index */

};

class ELF32_API ELF32_Parse
{
public:
	ELF32_Parse(void);	
	~ELF32_Parse(void);

	static ELF32*  open(const char* elfPath);
	static void close(ELF32* elf);

private:
	static BOOL  _ElfParse(ELF32* elf);
	static BOOL _SectionParse(ELF32* elf);
	static BOOL _SegmentParse(ELF32* elf);
	static BOOL _SymbolParse(Section *symSec, ELF32* elf);
	static BOOL _StrTblParse(ELF32* elf);

	static INT32 _read_Word(INT8 **ptr);
	static INT16 _read_HWord(INT8 **ptr);
	static INT8 _read_Byte(INT8 **ptr);
 
	static void _elfClose();

	static void _ehdr_get(INT8 *ehdr, ELF32* elf);
	static Section* _shdr_get(INT8 *shdr);
	static Segment* _phdr_get(INT8 *phdr);
	static Symbol* _sym_get(INT8 *symData, ELF32* elf);
	static void _big2little(INT32 cnt, void *ptr);

	static FILE* m_fp;

};

#endif
