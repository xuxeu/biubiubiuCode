/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	elf.h
 * @brief	ELF格式中相关结构体，常量的定义
 * @author 	张宇旻
 * @date 	2007年12月17日
 * @History
 */


#ifndef ELF_H
#define ELF_H

#include <stdio.h>
#include <windows.h>


#ifdef __cplusplus
extern "C" {
#endif

#define SUCC (0)
#define FAIL (-1)

#define EI_NIDENT 16


/* ELF文件类型 */
#define ET_NONE     0   //No file type
#define ET_REL      1   //Relocatable file
#define ET_EXEC    2    //Executable file
#define ET_DYN      3   //Shared object file
#define ET_CORE     4   //Core file
#define ET_LOPROC 0xff00    //Processor-specific
#define ET_HIPROC   0xffff  //Processor-specific

/* ELF标识索引 */
#define EI_MAG0     0 //File identification
#define EI_MAG1     1 //File identification
#define EI_MAG2     2 //File identification
#define EI_MAG3     3 //File identification
#define EI_CLASS    4 //File class
#define EI_DATA     5 //Data encoding
#define EI_VERSION 6 //File version
#define EI_PAD      7 //Start of padding bytes
#define EI_NIDENT 16 //Size of e_ident[] 

//elf文件的大小端
#define PISA_ELF_BIG_ENDIAN             2
#define PISA_ELF_LITTLE_ENDIAN          1

struct Elf32_Ehdr
{
    unsigned char e_ident[EI_NIDENT];
    signed short e_type;
    signed short e_machine;
    int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned int e_shoff;
    int e_flags;
    signed short e_ehsize;
    signed short e_phentsize;
    signed short e_phnum;
    signed short e_shentsize;
    signed short e_shnum;
    signed short e_shstrndx;
};

//section header
struct Elf32_Shdr
{
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned int sh_flags;
    unsigned int sh_addr;
    unsigned int sh_offset;
    unsigned int sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned int sh_addralign;
    unsigned int sh_entsize;
} ;

/**
 * @brief
 *        获取指定ELF文件中指定段的内容。
 * @param[in]    imgfile: 指定的ELF文件的路径名
 * @param[in]    segName: 指定的段名
 * @param[out]    sectionAddr: 段运行地址
 * @return        成功返回TRUE，否则返回FALSE。
 */
int fnELF_Get_Seg_Address(char *imgfile,
                          char * segName,
                          unsigned int* sectionAddr
                          );

#ifdef __cplusplus
}
#endif

#endif //ELF_H

