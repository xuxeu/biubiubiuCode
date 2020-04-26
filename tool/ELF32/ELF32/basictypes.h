/**************************************************************************
 *
 *                   北京科银技术有限公司        版权所有
 *  CopyRight (C)  2000-2010   Coretek System Inc. All Rights Reserved
 *
***************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  amwyga，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  BasicTypes.h
 * @brief
 *       <li>功能：基本类型定义</li>
 */


#ifndef BASICTYPES_H_
#define BASICTYPES_H_

    typedef    signed char                    INT8;     ///< 有符号8bit
    typedef    unsigned char                 UINT8;  ///< 无符号8bit
    
    typedef    signed short                    INT16;           ///< 有符号16bit
    typedef    unsigned short                 UINT16; ///< 无符号16bit
    
    typedef    signed int                        INT32;     ///< 有符号32bit
    typedef    unsigned int                     UINT32;  ///< 无符号32bit
    
    typedef    signed long long                INT64;    ///< 有符号64bit
    typedef    unsigned long long             UINT64; ///<无符号64bit

 //   typedef    unsigned int                    BOOL;
	typedef    int                    BOOL;
//    typedef    int                    Boolean;
//    typedef    int                    boolean;
    
    typedef    float                            SINGLE;
    typedef    float                            FLOAT;
    typedef    double                         DOUBLE;    

#endif /*BASICTYPES_H_*/
