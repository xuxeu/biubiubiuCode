/*
* 变更历史：
* 2012-05-18  周英豪      将_msl_os_rawvector_install中if判断的表达式改为逻辑表达式并且补充缺少的{}。(BUG6540)
* 2012-04-21  邱学强      将mslRawVectorSetup修改为内部接口_msl_raw_vector_setup。(BUG6343)
* 2012-04-21  邱学强      _msl_exceptionstack_init中增加设置PPC异常临时栈空间地址。(BUG6549)
* 2010-08-30  尹立孟      创建该文件。
*/


/**************************** 引用部分 *********************************/
//#include "acoreos.h"
//#include "archMsl.h"
//#include "pageHeap.h"
//#include <stdlib.h>
#include "taTypes.h"

/*************************** 前向声明部分 ******************************/

/**************************** 定义部分 *********************************/
/* MSL 异常栈大小*/
#define MSL_EXCEPTION_STACKSIZE (0x1000)

/* Raw向量表定义，该向量表在任何平台都是最多256项 */
void* _Raw_Vector_table[256];

/* MSL异常栈数组 */
UINT8 taExceptionStack[MSL_EXCEPTION_STACKSIZE]  __attribute__((__aligned__(4096))) ;


/* ta 异常栈栈底、栈顶、栈大小以及二次异常标识*/
void *taExceptionStackBottom;
void *taExceptionStackTop;
UINT32 taExceptionStackSize;
UINT32 exceptionTwiceFlag;

/* MSL和OS的异常Raw向量的入口变量，实现系统级调试代理和OS对调试异常接管切换 */
static void *mslExceptionEntry;
static void *osExceptionEntry;

#define EXCEPTION_FRAME_SIZE  (0xB0)

/*使用此地址保存MSL映像和OS映像都需要使用的异常临时栈空间*/
#define EXP_TMP_STACK_BASE (0xBA0)

#if defined(__PPC__)
/* PPC架构使用此地址临时保存中断异常产生时的部分上下文*/
UINT32 expTmpStack[EXCEPTION_FRAME_SIZE/4] ;
#endif

/* MSL 异常栈栈底、栈顶、栈大小以及二次异常标识*/
extern void *mslExceptionStackBottom;
extern void *mslExceptionStackTop;
extern UINT32 mslExceptionStackSize;
extern UINT32 exceptionTwiceFlag;

extern taNullRawHandler;
/**************************** 实现部分 *********************************/

/**
 * @brief
 *     初始化Raw向量表，使所有的向量的缺省处理函数都是nullRawHandler。
 *     该函数应该在MSL的启动初始化中被调用,只允许调用一次。
 * @return
 *     none
 */
void taInitVectorTable( void )
{
	UINT32 vector;

	for( vector = 0; vector < 256; vector++ )
	{
		_Raw_Vector_table[vector] = taNullRawHandler;
	}
}

/**
 * @brief
 *     设置Raw向量表，当handler为NULL时，安装缺省的nullRawHandler，否则安装
 *     handler指定的处理程序。handler的有效性由使用者保障，本函数中不进行有效性检查。
 *     Raw向量的处理程序应该是汇编程序方式，不同的CPU平台其上下文有可能不同。0x80和0xFF向量
 *     分别用于系统调用和空向量。
 *     由用户安装的汇编程序负责处理结束异常结束后的操作，如返回或到异常发生处或者停止当前执行体等。
 * @param[in] vector: 向量号，范围为0-255。
 * @param[in] handler: 处理程序地址，NULL表示恢复缺省处理程序，否则表示安装指定的处理程序。
 * @return
 *     TRUE: 表示成功
 *     FALSE: 表示失败
 * @implements DM.1.1.52,DM.1.2.80,DM.1.3.80,DM.1.4.80
 */
void taInstallVectorHandle( int vector, void *handler)
{
	/* 向量范围为0-255 */
	if( vector >= 256 )
	{
		return FALSE;
	}

	/* handler为空则安装缺省的nullRawHandler */
	if( handler == NULL )
	{
		_Raw_Vector_table[vector] = taNullRawHandler;
	}
	else
	{
		_Raw_Vector_table[vector] = handler;
	}

	return (TRUE);
}

/**
 * @brief
 *      初始化ta 异常栈。
 * @return 
 *      TRUE: 表示成功
 *      FALSE: 表示失败
 * @implements DM.1.6 
 */
BOOL taExceptionstackInit( void )
{
    /* 初始化 MSL 异常栈*/
    taExceptionStackBottom = (void *)taExceptionStack;
    
    taExceptionStackTop = (void *)((UINT32)taExceptionStackBottom + MSL_EXCEPTION_STACKSIZE);

    taExceptionStackSize = MSL_EXCEPTION_STACKSIZE;

#if defined(__PPC__)
    /*设置异常临时栈空间*/
    *(UINT32 *)EXP_TMP_STACK_BASE = (UINT32)expTmpStack;
#endif

    return (TRUE);
}

