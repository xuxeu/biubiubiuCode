/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taIoAccess.h
 * @brief:
 *             <li>IO访问相关宏定义</li>
 */
#ifndef TA_IO_ACCESS_H
#define TA_IO_ACCESS_H

/************************头文件********************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************宏定义********************************/

#define i386_outport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = _value; \
     \
     __asm__ volatile ( "outb %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = _value; \
     \
     __asm__ volatile ( "outw %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = _value; \
     \
     __asm__ volatile ( "outl %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_inport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = 0; \
     \
     __asm__ volatile ( "inb %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = 0; \
     \
     __asm__ volatile ( "inw %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = 0; \
     \
     __asm__ volatile ( "inl %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

static __inline__ void i386_inport_bytes (unsigned short port, void * addr, unsigned long count)
{
     __asm__ __volatile__ ("rep ; ins" "b" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
 }

static __inline__ void i386_inport_words (unsigned short port, void * addr, unsigned long count)
{
    __asm__ __volatile__ ("rep ; ins" "w" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
}

static __inline__ void i386_inport_longs (unsigned short port, void * addr, unsigned long count)
{
     __asm__ __volatile__ ("rep ; ins" "l" : "=D" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
}
static __inline__ void i386_outport_bytes (unsigned short port, const void * addr, unsigned long count)
{
    __asm__ __volatile__ ("rep ; outs" "b" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
}

static __inline__ void i386_outport_words (unsigned short port, const void * addr, unsigned long count)
{
    __asm__ __volatile__ ("rep ; outs" "w" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
}

static __inline__ void i386_outport_longs (unsigned short port, const void * addr, unsigned long count)
{
    __asm__ __volatile__ ("rep ; outs" "l" : "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count));
}

#define BSP_OutByte( _port, _value ) i386_outport_byte( _port, _value )
#define BSP_OutWord( _port, _value ) i386_outport_word( _port, _value )
#define BSP_OutLong( _port, _value ) i386_outport_long( _port, _value )
#define BSP_InByte( _port, _value )  i386_inport_byte( _port, _value )
#define BSP_InWord( _port, _value )  i386_inport_word( _port, _value )
#define BSP_InLong( _port, _value )  i386_inport_long( _port, _value )

#define BSP_InBytes( _port, _addr,_count )  i386_inport_bytes( _port, _addr,_count )
#define BSP_InWords( _port, _addr,_count )  i386_inport_words( _port, _addr,_count )
#define BSP_InLongs( _port, _addr,_count )  i386_inport_longs( _port, _addr,_count )

#define BSP_OutBytes( _port, _addr,_count )  i386_outport_bytes( _port, _addr,_count )
#define BSP_OutWords( _port, _addr,_count )  i386_outport_words( _port, _addr,_count )
#define BSP_OutLongs( _port, _addr,_count )  i386_outport_longs( _port, _addr,_count )

/************************类型定义******************************/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TA_IO_ACCESS_H */

