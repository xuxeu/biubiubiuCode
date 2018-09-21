/*
* �����ʷ��
* 2012-05-18  ��Ӣ��      ��_msl_os_rawvector_install��if�жϵı��ʽ��Ϊ�߼����ʽ���Ҳ���ȱ�ٵ�{}��(BUG6540)
* 2012-04-21  ��ѧǿ      ��mslRawVectorSetup�޸�Ϊ�ڲ��ӿ�_msl_raw_vector_setup��(BUG6343)
* 2012-04-21  ��ѧǿ      _msl_exceptionstack_init����������PPC�쳣��ʱջ�ռ��ַ��(BUG6549)
* 2010-08-30  ������      �������ļ���
*/


/**************************** ���ò��� *********************************/
//#include "acoreos.h"
//#include "archMsl.h"
//#include "pageHeap.h"
//#include <stdlib.h>
#include "taTypes.h"

/*************************** ǰ���������� ******************************/

/**************************** ���岿�� *********************************/
/* MSL �쳣ջ��С*/
#define MSL_EXCEPTION_STACKSIZE (0x1000)

/* Raw�������壬�����������κ�ƽ̨�������256�� */
void* _Raw_Vector_table[256];

/* MSL�쳣ջ���� */
UINT8 taExceptionStack[MSL_EXCEPTION_STACKSIZE]  __attribute__((__aligned__(4096))) ;


/* ta �쳣ջջ�ס�ջ����ջ��С�Լ������쳣��ʶ*/
void *taExceptionStackBottom;
void *taExceptionStackTop;
UINT32 taExceptionStackSize;
UINT32 exceptionTwiceFlag;

/* MSL��OS���쳣Raw��������ڱ�����ʵ��ϵͳ�����Դ����OS�Ե����쳣�ӹ��л� */
static void *mslExceptionEntry;
static void *osExceptionEntry;

#define EXCEPTION_FRAME_SIZE  (0xB0)

/*ʹ�ô˵�ַ����MSLӳ���OSӳ����Ҫʹ�õ��쳣��ʱջ�ռ�*/
#define EXP_TMP_STACK_BASE (0xBA0)

#if defined(__PPC__)
/* PPC�ܹ�ʹ�ô˵�ַ��ʱ�����ж��쳣����ʱ�Ĳ���������*/
UINT32 expTmpStack[EXCEPTION_FRAME_SIZE/4] ;
#endif

/* MSL �쳣ջջ�ס�ջ����ջ��С�Լ������쳣��ʶ*/
extern void *mslExceptionStackBottom;
extern void *mslExceptionStackTop;
extern UINT32 mslExceptionStackSize;
extern UINT32 exceptionTwiceFlag;

extern taNullRawHandler;
/**************************** ʵ�ֲ��� *********************************/

/**
 * @brief
 *     ��ʼ��Raw������ʹ���е�������ȱʡ����������nullRawHandler��
 *     �ú���Ӧ����MSL��������ʼ���б�����,ֻ�������һ�Ρ�
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
 *     ����Raw��������handlerΪNULLʱ����װȱʡ��nullRawHandler������װ
 *     handlerָ���Ĵ������handler����Ч����ʹ���߱��ϣ��������в�������Ч�Լ�顣
 *     Raw�����Ĵ������Ӧ���ǻ�����ʽ����ͬ��CPUƽ̨���������п��ܲ�ͬ��0x80��0xFF����
 *     �ֱ�����ϵͳ���úͿ�������
 *     ���û���װ�Ļ�������������쳣������Ĳ������緵�ػ��쳣����������ֹͣ��ǰִ����ȡ�
 * @param[in] vector: �����ţ���ΧΪ0-255��
 * @param[in] handler: ��������ַ��NULL��ʾ�ָ�ȱʡ������򣬷����ʾ��װָ���Ĵ������
 * @return
 *     TRUE: ��ʾ�ɹ�
 *     FALSE: ��ʾʧ��
 * @implements DM.1.1.52,DM.1.2.80,DM.1.3.80,DM.1.4.80
 */
void taInstallVectorHandle( int vector, void *handler)
{
	/* ������ΧΪ0-255 */
	if( vector >= 256 )
	{
		return FALSE;
	}

	/* handlerΪ����װȱʡ��nullRawHandler */
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
 *      ��ʼ��ta �쳣ջ��
 * @return 
 *      TRUE: ��ʾ�ɹ�
 *      FALSE: ��ʾʧ��
 * @implements DM.1.6 
 */
BOOL taExceptionstackInit( void )
{
    /* ��ʼ�� MSL �쳣ջ*/
    taExceptionStackBottom = (void *)taExceptionStack;
    
    taExceptionStackTop = (void *)((UINT32)taExceptionStackBottom + MSL_EXCEPTION_STACKSIZE);

    taExceptionStackSize = MSL_EXCEPTION_STACKSIZE;

#if defined(__PPC__)
    /*�����쳣��ʱջ�ռ�*/
    *(UINT32 *)EXP_TMP_STACK_BASE = (UINT32)expTmpStack;
#endif

    return (TRUE);
}

