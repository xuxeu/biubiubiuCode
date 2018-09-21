/*
* 更改历史：
*
*/

/**
* @file     sysType.h
* @brief
*     <li>功能：类型重定义 /li>

*/

/************************头文件********************************/
#ifndef _SYSTYPES_H
#define _SYSTYPES_H

#ifdef __cplusplus
extern "C" {
#endif
    /************************宏定义********************************/

#define T_VOID          void
#define T_VVOID     volatile void

#define T_MODULE        static
#define T_INLINE        T_MODULE inline
#define T_EXTERN        extern
#define T_VOLATILE  volatile
#define T_CONST     const

#ifndef SUCC
#define SUCC        0
#endif

#ifndef FAIL
#define FAIL        -1
#endif


#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#ifndef NULL
#define NULL        0
#endif

#define FUNC_INT extern "C" __declspec(dllexport) int
#define FUNC_VOID extern "C" __declspec(dllexport) void

#define IS_DEBUG 0
#define IS_SYS_LOG 1
#ifdef __cplusplus

    /************************类型定义******************************/
    typedef signed char                     T_BYTE;     /*8-bit signed integer */
    typedef  unsigned char                  T_UBYTE;    /*8-bit unsigned integer */

    typedef signed short                    T_HWORD;    /*16-bit signed integer */
    typedef unsigned short                  T_UHWORD;   /*16-bit unsigned integer */

    typedef signed int                      T_WORD;     /*32-bit signed integer */
    typedef  unsigned int                   T_UWORD;    /*32-bit unsigned integer */
    typedef char                            T_CHAR;

    /**********************************************
     *  When in VC enviroment:
     *    typedef LONGLONG    T_DWORD;
     *    typedef ULONGLONG   T_UDWORD;
     *
     **********************************************/
    typedef signed long long                   T_DWORD;    /*64-bit signed integer */
    typedef  unsigned long long             T_UDWORD;   /*64-bit unsigned integer */

    typedef double                          T_DOUBLE;
    typedef  float                                T_FLOAT;

    typedef  unsigned int                      T_BOOL;    /*8-bit unsigned integer */

    typedef volatile signed char               T_VBYTE;
    typedef  volatile unsigned char         T_VUBYTE;

    typedef volatile signed short           T_VHWORD;
    typedef volatile unsigned short         T_VUHWORD;

    typedef volatile signed int             T_VWORD;
    typedef volatile unsigned int              T_VUWORD;

    /**********************************************
     *  When in VC enviroment:
     *    typedef volatile LONGLONG    T_VDWORD;
     *    typedef volatile ULONGLONG   T_VUDWORD;
     *
     **********************************************/

    typedef volatile signed long long       T_VDWORD;
    typedef volatile unsigned long long     T_VUDWORD;

    typedef volatile double                 T_VDOUBLE;
    typedef volatile float                     T_VFLOAT;

    typedef volatile unsigned int           T_VBOOL;


}
#endif
/************************接口声明******************************/
#endif