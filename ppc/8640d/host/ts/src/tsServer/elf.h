/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	elf.h
 * @brief	ELF��ʽ����ؽṹ�壬�����Ķ���
 * @author 	����F
 * @date 	2007��12��17��
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


/* ELF�ļ����� */
#define ET_NONE     0   //No file type
#define ET_REL      1   //Relocatable file
#define ET_EXEC    2    //Executable file
#define ET_DYN      3   //Shared object file
#define ET_CORE     4   //Core file
#define ET_LOPROC 0xff00    //Processor-specific
#define ET_HIPROC   0xffff  //Processor-specific

/* ELF��ʶ���� */
#define EI_MAG0     0 //File identification
#define EI_MAG1     1 //File identification
#define EI_MAG2     2 //File identification
#define EI_MAG3     3 //File identification
#define EI_CLASS    4 //File class
#define EI_DATA     5 //Data encoding
#define EI_VERSION 6 //File version
#define EI_PAD      7 //Start of padding bytes
#define EI_NIDENT 16 //Size of e_ident[] 

//elf�ļ��Ĵ�С��
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
 *        ��ȡָ��ELF�ļ���ָ���ε����ݡ�
 * @param[in]    imgfile: ָ����ELF�ļ���·����
 * @param[in]    segName: ָ���Ķ���
 * @param[out]    sectionAddr: �����е�ַ
 * @return        �ɹ�����TRUE�����򷵻�FALSE��
 */
int fnELF_Get_Seg_Address(char *imgfile,
                          char * segName,
                          unsigned int* sectionAddr
                          );

#ifdef __cplusplus
}
#endif

#endif //ELF_H

