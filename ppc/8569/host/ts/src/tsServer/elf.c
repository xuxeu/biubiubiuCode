/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	elf.c
 * @brief	ELF的解析
 * @author 	张宇旻
 * @date 	2007年12月17日
 * @History
 */

#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include "Utils.h"   

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
                          )
{
    unsigned int              size, i; 
    char *buf,*temp,*strings,*segSource;
    FILE *fp;
    struct Elf32_Ehdr *Ehdr;
    struct Elf32_Shdr *Shdr,*string_Shdr;

    //得到文件的大小
    if (!(fp=fopen(imgfile,"rb")))
    {
        return FAIL;
    }

    if ((fseek(fp,0,SEEK_END)))
    {
        return FAIL;
    }

    size = ftell(fp);

    if ((fseek(fp,0,SEEK_SET)))
    {
        return FAIL;
    }

    //分配一个比imgfile文件大64个字节的buf，并把文件内容读入.关闭文件
    buf = (char *)malloc(size+64);
    fread(buf,sizeof(char),size,fp);
    buf[size] = 0;
    fclose(fp);

    //elf头
    Ehdr = (struct Elf32_Ehdr *)buf;

    //判断该文件是不是elf文件
    temp = (char *)Ehdr->e_ident;
    if((temp[0]!=0x7f)||(temp[1]!='E')||(temp[2]!='L')||(temp[3]!='F'))
    {
        return FAIL;
    }

    /*大小端转换*/
    if (Ehdr->e_ident[EI_DATA]==PISA_ELF_BIG_ENDIAN)
    {
        Ehdr->e_type = big2little_short(Ehdr->e_type);
        Ehdr->e_machine = big2little_short(Ehdr->e_machine);
        Ehdr->e_version = big2little_endian(Ehdr->e_version);
        Ehdr->e_entry = big2little_endian(Ehdr->e_entry);
        Ehdr->e_phoff = big2little_endian(Ehdr->e_phoff);
        Ehdr->e_shoff = big2little_endian(Ehdr->e_shoff);
        Ehdr->e_flags = big2little_endian(Ehdr->e_flags);
        Ehdr->e_ehsize = big2little_short(Ehdr->e_ehsize);
        Ehdr->e_phentsize = big2little_short(Ehdr->e_phentsize);
        Ehdr->e_phnum = big2little_short(Ehdr->e_phnum);
        Ehdr->e_shentsize = big2little_short(Ehdr->e_shentsize);
        Ehdr->e_shnum = big2little_short(Ehdr->e_shnum);
        Ehdr->e_shstrndx = big2little_short(Ehdr->e_shstrndx);
    };

    //判断是不是可执行的elf
    if(ET_EXEC != Ehdr->e_type)
    {
        return FAIL;
    }

    //section header table头
    Shdr = (struct Elf32_Shdr *)((Ehdr->e_shoff) + buf);

    //section header for string section
    string_Shdr =(struct Elf32_Shdr *)((int)Shdr+ (Ehdr->e_shstrndx)*sizeof(struct Elf32_Shdr));
    /*大小端转换*/
    if (Ehdr->e_ident[EI_DATA]==PISA_ELF_BIG_ENDIAN)
    {
            string_Shdr->sh_name = big2little_endian(string_Shdr->sh_name);
            string_Shdr->sh_type = big2little_endian(string_Shdr->sh_type);
            string_Shdr->sh_flags = big2little_endian(string_Shdr->sh_flags);
            string_Shdr->sh_addr = big2little_endian(string_Shdr->sh_addr);
            string_Shdr->sh_offset = big2little_endian(string_Shdr->sh_offset);
            string_Shdr->sh_size = big2little_endian(string_Shdr->sh_size);
            string_Shdr->sh_link = big2little_endian(string_Shdr->sh_link);
            string_Shdr->sh_info = big2little_endian(string_Shdr->sh_info);
            string_Shdr->sh_addralign = big2little_endian(string_Shdr->sh_addralign);
            string_Shdr->sh_entsize = big2little_endian(string_Shdr->sh_entsize);
    };

    //string section
    strings = (char *)(buf + string_Shdr->sh_offset);

    for(i =0;i<(int)(Ehdr->e_shnum);i++)
    {
        /*大小端转换*/
        if (Ehdr->e_ident[EI_DATA]==PISA_ELF_BIG_ENDIAN)
        {
            Shdr->sh_name = big2little_endian(Shdr->sh_name);
            Shdr->sh_type = big2little_endian(Shdr->sh_type);
            Shdr->sh_flags = big2little_endian(Shdr->sh_flags);
            Shdr->sh_addr = big2little_endian(Shdr->sh_addr);
            Shdr->sh_offset = big2little_endian(Shdr->sh_offset);
            Shdr->sh_size = big2little_endian(Shdr->sh_size);
            Shdr->sh_link = big2little_endian(Shdr->sh_link);
            Shdr->sh_info = big2little_endian(Shdr->sh_info);
            Shdr->sh_addralign = big2little_endian(Shdr->sh_addralign);
            Shdr->sh_entsize = big2little_endian(Shdr->sh_entsize);
        };

        temp =(char *)(strings + Shdr->sh_name);
        if(!strcmp(temp, segName))
        {
            /* 获取到段运行地址 */
            *sectionAddr = Shdr->sh_addr;
            
            break;
        }

        Shdr++;
    }
    if(i == Ehdr->e_shnum)
    {
        return FAIL;
    }

    return SUCC;
}

