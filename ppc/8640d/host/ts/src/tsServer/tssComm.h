/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tssComm.h
 * @Version        :  1.0.0
 * @brief           :  *   <li> 功能</li>tsmap公共函数及宏定义
                                <li>设计思想</li> 
                                <p> n     注意事项：n</p> 
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月24日 15:14:57
 * @History        : 
 *   
 */


#ifndef  _TSSYMMAN_COMM_H
#define _TSSYMMAN_COMM_H


#include "sysTypes.h"


#define SUCC    0
#define FAIL     -1

#define MAX_PATH_LEN    260


#define TARGET_ARCH_X86  0
#define TARGET_ARCH_PPC  2
#define TARGET_ARCH_ARM  1
#define TARGET_ARCH_MAX  3

#define TOOL_CHAIN_VER_296   0
#define TOOL_CHAIN_VER_344   1
#define TOOL_CHAIN_VER_MAX   2

#define DES_TA_MANAGER_ID (0)
#define TS_DEFAULT_TIMEOUT 128



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
 * @Funcname: ExtendESC
 * @brief        : 扩展文件全路径名中的\\字符，将一个\扩展为\\
 * @para[IN|OUT]   : pString文件全路径名
 * @para[IN]   : 输出缓冲长度
 * @return      :扩展是否成功
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月24日 13:34:11
 *   
**/
extern int ExtendESC(char* pString, int len);

/**
 * @Funcname: ResumeBlank
 * @brief        : 将字符串中的协议ASC码恢复为空格
 * @para[IN|OUT]   : pString字符串
 * @return      :恢复是否成功
 * @Author     : tangxp
 *  
 * @History: 1.  Created this function on 2009年2月12日 13:34:11
 *   
**/
extern int ResumeBlank(char* pString);

#endif


