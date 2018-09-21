/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	utils.h
 * @brief	共用函数库函数声明
 * @author 	张宇旻
 * @date 	2007年12月17日
 * @History
 */

/**
 * @brief       从命令行参数表中获取指定选项对应的字符串参数
 * @param[in]	argc  参数个数
 * @param[in]	argv  参数数组
 * @param[in]	opt   选项
 * @return      操作成功返回指向字符串参数的指针，操作失败返回NULL
**/
char* ArgGetStr(int argc, char* argv[],char* opt);

/**
 * @brief       从命令行参数表中获取指定选项对应的整数参数
 * @param[in]	argc  参数个数
 * @param[in]	argv  参数数组
 * @param[in]	opt   选项
 * @return      操作成功返回整数参数对应的整数，操作失败返回-1
**/
int ArgGetInt(int argc, char* argv[],char* opt);

/**
 * @brief       确认命令行参数表中是否存在某个选项
 * @param[in]	argc  参数个数
 * @param[in]	argv  参数数组
 * @param[in]	opt   选项
 * @return      存在该选项返回TRUE，失败返回FALSE
**/
int ArgInclude(int argc, char* argv[],char* opt);

/**
 * @brief       大端转换成小端
 * @param[in]	cnt  被转换对象的size
 * @param[in]	ptr  指向转换对象的指针
 * @return      无
**/
void big2little(int cnt, void *ptr);

/**
 * @brief       带刷新的控制台打印
 * @param[in]	form  打印格式，与printf完全一致
 * @param[in]	...  可变参数表
 * @return      无
**/
void SL_Print(char *form,...);

int __fetch_long(int *dest, char* source, int bytes);

int IsSameWord( char *str1, char *str2 );
int FindItem( char *ItemStr, int argc, char *argv[] );

int fnUtility_ParaGetInt( char *ItemStr, int argc,char *argv[] );
char* fnUtility_ParaGetStr( char *ItemStr, int argc, char *argv[] );

