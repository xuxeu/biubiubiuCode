/**********************************************************************************
*                     北京科银京成技术有限公司 版权所有
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*修改历史:
*2010-9-15 9:55:08        文思惟      北京科银京成技术有限公司
*修改及方案:创建本文件
*/

/**
 *@file   elf.h
 *@brief
 *<li>elf类。实现对elf文件的解析及获取elf文件数据的功能。
 *</li>
 *@Note
 */

#ifndef _INCLUDE_ELF_H
#define _INCLUDE_ELF_H
/************************头 文 件******************************/
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
/************************引    用******************************/
/************************宏 定 义******************************/
/************************类型定义******************************/


#ifdef ELF32_EXPORTS
#define ELF32_API __declspec(dllexport)
#else
#define ELF32_API __declspec(dllimport)
#endif


#define FAIL -1
#define SUCCESSFUL 0


/* object中各种数据长度*/
enum basic_type
{
	EHDR_LEN = 52,
	PHDR_LEN = 32,
	SHDR_LEN = 40,
	SYM_LEN = 16,
	NAME_LEN = 256
};

/* object类型 */
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

/* object体系结构 */
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

/* object ident定义*/
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

/* object类型 */
enum elf_class
{
	ELFCLASSNONE = 0,	/*nvalid class*/
	ELFCLASS32 = 1,		/*32-bit objects*/
	ELFCLASS64 = 2		/*64-bit objects*/
};

/* object编码格式*/
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

	/* 获取符号名称	*/
	BOOL GetName(char* buf);

	/* 获取符号值 */
	INT32 GetValue();

	/* 获取符号大小	*/
	UINT32 GetSize();

	/* 获取符号属性 */
	INT8 GetInfo();

	/* 获取相关联的符号 */
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

	/* 获取节区类型 */
	UINT32 GetType();

	/* 获取节区内容 */
	BOOL GetContents(INT8* buf, UINT32 len);

	/* 获取节区名称 */
	BOOL GetName(char* nameBuf);

	/* 获取节区flags*/
	UINT32 GetFlags();

	/* 获取节区在内存中的地址 */
	UINT32 GetAddr();

	/* 获取节区在文件中的偏移量 */
	UINT32 GetOffset();

	/* 获取节区大小*/
	UINT32 GetSize();

	/* 获取节区表的索引连接 */
	UINT32 GetLink();

	/* 获取节区额外信息 */
	UINT32 GetInfo();

	/* 获取节区对齐属性 */
	UINT32 GetAlign();

	/* 获取节区存放的表的固定大小 */
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
	*	 获取符号个数
	* @param	无
	* @return	符号个数。
	*/
	UINT32 GetSymCount();

	/**
	* @brief
	*	 获取符号序号
	* @param	symName 符号名称
	* @return  符号序号。
	*/
	INT32 GetSymIndex(string symName);

	/**
	* @brief
	*	 获取符号序号
	* @param	symValue 符号值
	* @return  符号序号。
	*/
	INT32 GetSymIndex(UINT32 symValue);

	/**
	* @brief
	*	 获取符号
	* @param	symValue 符号值
	* @return  符号序号。
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

	/* 获取段类型 */
	INT32 GetType();

	/*获取段在m_elf文件中的偏移位置*/
	UINT32 GetOffset();

	/*获取该段在内存中的首字节地址*/
	UINT32 GetVirtualAddr();

	/*获取段的物理地址定位（如果有的话）*/
	UINT32 GetPhysicalAddr();

	/* 获取段内容 */
	BOOL GetContents(INT8* buf, UINT32 len);

	/* 获取段文件空间 */
	UINT32 GetFileSize();

	/* 获取段内存空间大小*/
	UINT32 GetMemSize();

	/* 获取段对齐字节 */
	UINT32 GetAlign();

	/* 判断该段属性 */
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
	* ELF头基本信息 
	*/
	/* 获取elf类型 */
	elf_type GetFileType();

	/* 获取elf起始地址 */
	UINT32 GetStartAddr();

	/* 获取elf大小端 */
	UINT32 GetEndian();

	/* 获取elf体系结构 */
	elf_Machine GetCpuType();

	/* 获取节表数量 */
	INT16 GetSectionCount();

	/* 获取段数量 */
	INT16 GetSegmentCount();

	/* 获取指定序号的段表 */
	Segment* GetSegment(UINT32 index);

	/* 获取指定序号的节表 */
	Section* GetSection(UINT32 index);

	/* 查找指定名称的段的序号 */
	UINT16 GetSectionIndex(char* name);

	/* 查看是否已经进行了解析 */
	INT32 IsElfParse();

	/* 获取符号表控制结构 */
	SymbolTable* GetSymTable();

private:
	INT32 parseDone;

	/* 符号表实例，记录了所有符号表数据，并提供符号操作接口 */
	SymbolTable* m_symCtl;

	/* 段头数据 */
	vector<Segment*> m_sgmtTbl;

	/* 节表头数据 */
	vector<Section*> m_secTbl;

	/* 节表名字符表 */
	string m_secStrTbl;

	/* 符号名字符表*/
	string m_symStrTbl;

	/*m_elf头数据*/
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
