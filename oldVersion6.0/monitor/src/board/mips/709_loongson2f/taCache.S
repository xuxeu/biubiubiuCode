#include <regdef.h>

#define ssnop .word 0x0000000040
#define CPU_CYCLE_TWO ssnop;ssnop;ssnop;ssnop

.global monitorFlushCache
.ent monitorFlushCache
monitorFlushCache:
.frame fp, 0, ra
.set noreorder
	move t3, a0
	nop
	/*刷一级数据Cache,�?级Cache有四路，由虚拟地�?的[1:0]位标识，因此这里�?要分别刷新每路Cache,详见cache指令的Invalid WriteBack Invalidate(D)*/
	cache 1, 0(t3)
	CPU_CYCLE_TWO
	cache 1, 1(t3)
	CPU_CYCLE_TWO
	cache 1, 2(t3)
	CPU_CYCLE_TWO	
	cache 1, 3(t3)
	CPU_CYCLE_TWO

	/*刷一级指令Cache,这里不需要单独刷每路Cache,详见Cache指令的Index Invalidate(I)*/
	cache 0, 0(t3)

	jr ra

.end monitorFlushCache

