/*
* 更改历史：
* 2008-09-24 zhangxu  北京科银技术有限公司
*                        创建该文件。
*/

/**
* @file  tsSymManComm.h
* @brief
*       功能：
*       <li>>tsSymManComm公共函数及宏定义</li>
*/

/************************头文件********************************/


#ifndef  _TSSYMMAN_COMM_H
#define _TSSYMMAN_COMM_H

#include "sysTypes.h"

/************************宏定义********************************/
#define MAX_PATH_LEN    260

/************************类型定义******************************/

/************************接口声明******************************/

/**
* 功能: 将单个的16进制ASCII字符转换成对应的数字。
* @param ch    单个的16进制ASCII字符
* @return 返回16进制字符对应的ASCII码
*/
extern int hex2num(unsigned char ch);

/**
* 功能: 将16进制字符串转换成对应的32位整数。
* @param ptr[OUT] 输入指向16进制字符串的指针，转换过程中指针同步前
*        进。输出转换结束时的指针。
* @param intValue[OUT]   转换后的32位整数
* @return 返回转换的16进制字符串长度。
*/
extern unsigned int hex2int(char **ptr, int *intValue);

/**
* 功能: 把int转换成16进制的字符串,必须保证size大小大于转换后的字符串大小
* @param ptr 保存16进制字符串的缓冲区
* @param size   缓冲区的大小
* @param intValue   要转换的整形
* @return 转换的大小
*/
extern unsigned int int2hex(char *ptr, int size,int intValue);

/**
 * @Funcname: CheckFolderExist
 * @brief        : 检查目录是否存在
 * @para[IN]   : strPath目录名
 * @return      : 目录存在与否
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008年9月24日 13:34:11
 *
**/
extern int CheckFolderExist(char* strPath);

/**
 * @Funcname: CheckFileExist
 * @brief        : 检查文件是否存在
 * @para[IN]   : strPath文件名
 * @return      : 文件存在与否
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008年9月24日 13:34:11
 *
**/
extern int CheckFileExist(char *strFile);

/**
 * @Funcname: DeleteDirectory
 * @brief        : 删除目录(包括空目录和非空目录)
 * @para[IN]   : strPath目录名
 * @return      : 删除是否成功
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008年9月24日 13:34:11
 *
**/
extern int DeleteDirectory(char *DirName);

/**
 * @Funcname: DeleteFolderAllFile
 * @brief        : 删除文件夹下所有文件
 * @para[IN]   : strPath目录名
 * @return      : 删除是否成功
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008年9月24日 13:34:11
 *
**/
extern int DeleteFolderAllFile(char *strPath);



#endif

