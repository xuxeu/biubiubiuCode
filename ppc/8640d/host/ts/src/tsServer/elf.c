/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	elf.c
 * @brief	ELF�Ľ���
 * @author 	����F
 * @date 	2007��12��17��
 * @History
 */

#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include "Utils.h"   

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
                          )
{
    unsigned int              size, i; 
    char *buf,*temp,*strings,*segSource;
    FILE *fp;
    struct Elf32_Ehdr *Ehdr;
    struct Elf32_Shdr *Shdr,*string_Shdr;

    //�õ��ļ��Ĵ�С
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

    //����һ����imgfile�ļ���64���ֽڵ�buf�������ļ����ݶ���.�ر��ļ�
    buf = (char *)malloc(size+64);
    fread(buf,sizeof(char),size,fp);
    buf[size] = 0;
    fclose(fp);

    //elfͷ
    Ehdr = (struct Elf32_Ehdr *)buf;

    //�жϸ��ļ��ǲ���elf�ļ�
    temp = (char *)Ehdr->e_ident;
    if((temp[0]!=0x7f)||(temp[1]!='E')||(temp[2]!='L')||(temp[3]!='F'))
    {
        return FAIL;
    }

    /*��С��ת��*/
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

    //�ж��ǲ��ǿ�ִ�е�elf
    if(ET_EXEC != Ehdr->e_type)
    {
        return FAIL;
    }

    //section header tableͷ
    Shdr = (struct Elf32_Shdr *)((Ehdr->e_shoff) + buf);

    //section header for string section
    string_Shdr =(struct Elf32_Shdr *)((int)Shdr+ (Ehdr->e_shstrndx)*sizeof(struct Elf32_Shdr));
    /*��С��ת��*/
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
        /*��С��ת��*/
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
            /* ��ȡ�������е�ַ */
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

