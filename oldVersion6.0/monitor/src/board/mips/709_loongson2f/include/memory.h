#ifndef __MEMORY_MIPS_H__
#define __MEMORY_MIPS_H__



#define CACHED_MEMORY_ADDR	0x80000000
#define UNCACHED_MEMORY_ADDR	0xa0000000
#define	CACHED_TO_PHYS(x)	((unsigned)(x) & 0x1fffffff)
#define	UNCACHED_TO_PHYS(x) 	((x) & 0x1fffffff)
#define	PHYS_TO_UNCACHED(x) 	((unsigned)(x) | UNCACHED_MEMORY_ADDR)
#define	CACHED_TO_UNCACHED(x)	(PHYS_TO_UNCACHED(CACHED_TO_PHYS(x)))


#define PCI_IO_BASE		0xbfd00000

#define BSP_OutByte( _port, _value )  \
do{ \
   (*((volatile T_UBYTE *)((T_UWORD)(PCI_IO_BASE + _port))) ) = (_value) ; \
 }while(0)


#define BSP_OutHWord( _port, _value )   \
do{ \
   (*((volatile T_UHWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) = (_value) ; \
 }while(0)


#define BSP_OutWord( _port, _value )   \
do{ \
   (*((volatile T_UWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) = (_value) ; \
 }while(0)

#define BSP_OutLong( _port, _value )   \
do{ \
   (*((volatile T_UWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) = (_value) ; \
 }while(0)

#define BSP_InByte( _port, _value )   \
do{ \
   (_value) = (*((volatile T_UBYTE *)((T_UWORD)(PCI_IO_BASE + _port))) ) ; \
 }while(0)

#define BSP_InHWord( _port, _value )    \
do{ \
   (_value) = (*((volatile T_UHWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) ; \
 }while(0)

#define BSP_InWord( _port, _value )    \
do{ \
   (_value) = (*((volatile T_UWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) ; \
 }while(0)

#define BSP_InLong( _port, _value )    \
do{ \
   (_value) = (*((volatile T_UWORD *)((T_UWORD)(PCI_IO_BASE + _port))) ) ; \
 }while(0)

#endif /* __MEMORY_MIPS_H__ */
