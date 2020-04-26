/*
* 更改历史：
* 2007-11-02  qingxiaohai 北京科银技术有限公司
*                           创建该文件。
*/

/**
* @file  tsmapComm.h
* @brief
*       功能：
*       <li>tsmap公共函数及宏定义</li>
*/


/************************头文件********************************/

/************************宏定义********************************/
#ifndef  _TSMAP_COMM_H
#define _TSMAP_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#define SUCC         0
#define FAIL          -1

#define US unsigned short
#define UI unsigned int
#define UC unsigned char
#define UL unsigned long

#define  TSMAP_REGISTER               'r'
#define  TSMAP_UNREGISTER             'u'
#define  TSMAP_QUERY                  'q'
#define  TSMAP_EXCEPTION              'e'

#define  TSMAP_SPLIT_STR              ";"
#define   TSMAP_SPLIT_STR_LEN       1
#define   MAX_TBLNM_LEN                 100

#define  EINVCMD                            -2
#define  EISTFAIL                            -3             //生成数据表失败
#define  ENOTPMT                           -4             //不允许重复注册
#define  ENEXIT                               -5            //不存在匹配的数据

#define  ESTR_SYS                         "E01;"        //系统内部错误
#define  ESTR_INVCMD                  "E02;"        //无效的命令格式
#define  ESTR_ISTFAIL                  "E03;"        //系统保存数据错误
#define  ESTR_NOTPMT                 "E04;"        //不允许重复存储，该命令下只允许有一个数据
#define  ESTR_NEXIT                     "E05;"        //未找到匹配项


    /************************类型定义******************************/

    /************************接口声明******************************/

    /**
     * @brief        :  通过返回值获取对应的回复字符串
     * @para[IN]  :errno, 错误码
     * @return      : 返回错误码对应的回复字符串
    */
    char* getAckStr(int eno);

    /**
    * 功能: 将单个的16进制ASCII字符转换成对应的数字。
    * @param ch    单个的16进制ASCII字符
    * @return 返回16进制字符对应的ASCII码
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

    /**
    * 功能: 把int转换成16进制的字符串,必须保证size大小大于转换后的字符串大小
    * @param ptr 保存16进制字符串的缓冲区
    * @param size   缓冲区的大小
    * @param intValue   要转换的整形
    * @return 转换的大小
    */
    unsigned int int2hex(char *ptr, int size,int intValue);

#ifdef __cplusplus
}
#endif

#endif

