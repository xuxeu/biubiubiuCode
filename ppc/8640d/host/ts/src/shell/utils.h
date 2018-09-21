/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  utils.h
* @brief
*       功能：
*       <li>公共函数声明</li>
*/
#ifndef _UTILS_H
#define _UTILS_H

/************************头文件********************************/

#ifdef __cplusplus
extern "C" {
#endif

/************************宏定义********************************/

/************************类型定义******************************/

/************************接口声明******************************/

/**
 * @brief       从命令行参数表中获取指定选项对应的整数参数
 * @param[in]    argc  参数个数
 * @param[in]    argv  参数数组
 * @param[in]    opt   选项
 * @return      操作成功返回整数参数对应的整数，操作失败返回-1
**/
int ArgGetInt(int argc, char* argv[],char* opt);

/**
 * @brief       从命令行参数表中获取指定选项对应的字符串参数
 * @param[in]    argc  参数个数
 * @param[in]    argv  参数数组
 * @param[in]    opt   选项
 * @return      操作成功返回整数参数对应的整数，操作失败返回-1
**/
char* ArgGetStr(int argc, char* argv[],char* opt);

/**
 * 功能: 将单个的16进制ASCII字符转换成对应的数字。
 * @param ch    单个的16进制ASCII字符
 * @param intValue[OUT]   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
*/
int hex2num(unsigned char ch);

/**
 * 功能: 将16进制字符串转换成对应的32位整数。
 * @param ptr[OUT] 输入指向16进制字符串的指针，转换过程中指针同步前
 *        进。输出转换结束时的指针。
 * @param intValue[OUT]   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
*/
unsigned int hex2int(char **ptr, int *intValue);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif
