/****************************************************************************
 *				北京科银京成技术有限公司 版权所有
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 *	20080311	创建文件。
 *
 *
 */

/**
 * @file taLson2evga.inc
 * @brief
 *	<li>功能：龙芯2EVGA显示设备驱动程序</li>
 * @date 	20080311
 */

/**************************** 引用部分 *****************************************/
#include "sysTypes.h"
#include "memory.h"
#include "taLson2fvga.h"
#include "string.h"

/*************************** 前向声明部分 ****************************************/
T_MODULE T_VOID ScrollUpLines(T_UWORD uwLines);
T_MODULE T_VOID PrintLF();
T_MODULE T_VOID PrintChar(T_UBYTE ubCh);
T_MODULE T_VOID Loong_video_hw_bitblt(int bpp, int sx, int sy, int dx, int dy, int w, int h);
T_MODULE T_VOID OutputChar(unsigned char ubChar);

/**************************** 定义部分 *****************************************/
static const int video_font_draw_table16[] = {
	    0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff };

static void memsetl(T_UWORD *p, int c, int v)
{
	while (c--)
	{
		*(p++) = v;
	}
}

unsigned int eorx, fgx, bgx;  //color pats

static T_UWORD* videoRamBase = (T_UWORD*)0x0;	
static T_UWORD cursorRow = 0;		
static T_UWORD cursorCol = 0;	
static T_UWORD winSizeX = 0;
static T_UWORD winSizeY = 0;
static T_UWORD gdfBytesPP = 0;
static T_UWORD gdfIndex = 0;
	
/**************************** 实现部分*************************************************/
/**
 * @brief
 *	功能:
 *		初始化显示设备。
 *
 *	实现内容：
 *		1、分配并初始化显存。
 *		2、初始化显示芯片。
 *		3、清屏。
 *		4、若有硬件光标，设置光标位置到屏幕起始位置。
 *
 *@param[in]	mode：屏幕显示模式。
 *
 *@return 无。
 */
T_VOID monitorDisplayInit(T_BYTE mode)
{

	// 初始化芯片
#ifdef __ATX_BOARD__
	videoRamBase	= (T_UWORD*)0xb4000000;
#else
	videoRamBase	= (T_UWORD*)0xb00b8000;//0xb4000000;//0x94000000|0xb4000000// 显存首地址
#endif
	winSizeX      = VIDEO_COLS;
	winSizeY      = VIDEO_ROWS;
    	gdfBytesPP    = 2;
	gdfIndex      = GDF_16BIT_565RGB;

	//Init drawing pats
	fgx = (((CONSOLE_FG_COL >> 3) << 27) |
			((CONSOLE_FG_COL >> 2) << 21) | ((CONSOLE_FG_COL >> 3) << 16) |
			((CONSOLE_FG_COL >> 3) << 11) | ((CONSOLE_FG_COL >> 2) << 5) |
			(CONSOLE_FG_COL >> 3));
	bgx = (((CONSOLE_BG_COL >> 3) << 27) |
			((CONSOLE_BG_COL >> 2) << 21) | ((CONSOLE_BG_COL >> 3) << 16) |
			((CONSOLE_BG_COL >> 3) << 11) | ((CONSOLE_BG_COL >> 2) << 5) |
			(CONSOLE_BG_COL >> 3));
	eorx = fgx ^ bgx;

	// 清屏 
	memsetl (videoRamBase, CONSOLE_SIZE, CONSOLE_BG_COL);

	// 硬件设备光标位置（设置为左上角）
	cursorRow = 0;
	cursorCol = 0;
}
 
/**
 *@brief
 *	功能:
 *		在当前位置输出字符。
 *
 *	实现内容：
 * 		1、在当前位置输出字符，并更新当前位置。
 *		2、若有硬件光标，刷新光标位置。
 *
 * @param[in]	ubCh：待被输出的字符。
 *
 * @return	无。
 */
T_VOID monitorDisplayOutch(T_BYTE ch)
{
	// 打印字符
	OutputChar(ch);

}

/*****************************RADEON部分START******************************************/
//2D引擎工作
void radeon_engine_flush (void)
{
	int i;
	
	//启动刷新
	OUTREGP(RADEON_RB2D_DSTCACHE_CTLSTAT, RADEON_RB2D_DC_FLUSH_ALL,
		~RADEON_RB2D_DC_FLUSH_ALL);
	
	for (i=0; i < 2000000; i++)
	{
		if (!(MMINL(RADEON_RB2D_DSTCACHE_CTLSTAT) & RADEON_RB2D_DC_BUSY))
		{
			break;
		}
	}
}

//等待fifo空闲
void radeon_fifo_wait (int entries)
{
	int i;
	
	for (i=0; i<2000000; i++)
		if ((MMINL(RADEON_RBBM_STATUS) & 0x7f) >= entries)
			return;
}

//等待引擎空闲
void radeon_engine_idle (void)
{
	int i;
	
	//确保FIFO空闲
	radeon_fifo_wait (64);
	
	for (i=0; i<2000000; i++) {
		if (((MMINL(RADEON_RBBM_STATUS) & RADEON_RBBM_ACTIVE)) == 0) {
			radeon_engine_flush ();
			return;
		}
	}
}

//硬件滚屏
T_MODULE T_VOID Loong_video_hw_bitblt(int bpp, int sx, int sy, int dx, int dy, int w, int h)
{
	int val;
	int xdir,ydir;
	
	xdir = sx - dx;
	ydir = sy - dy;
	
	if ( xdir < 0 ) { sx += w-1; dx += w-1; }
	if ( ydir < 0 ) { sy += h-1; dy += h-1; }
	
	val = (0 << 8) | RADEON_GMC_CLR_CMP_CNTL_DIS;
	radeon_fifo_wait(3);
	MMOUTL(RADEON_DP_GUI_MASTER_CNTL,
		val
		| RADEON_GMC_BRUSH_NONE
		| RADEON_GMC_SRC_DATATYPE_COLOR
		| RADEON_ROP3_S
		| RADEON_DP_SRC_SOURCE_MEMORY );
	MMOUTL(RADEON_DP_WRITE_MASK, 0xffffffff);
	MMOUTL(RADEON_DP_CNTL, (xdir>=0 ? RADEON_DST_X_LEFT_TO_RIGHT : 0)
		| (ydir>=0 ? RADEON_DST_Y_TOP_TO_BOTTOM : 0));
	
	radeon_fifo_wait(3);
	MMOUTL(RADEON_SRC_Y_X, (sy << 16) | sx);
	MMOUTL(RADEON_DST_Y_X, (dy << 16) | dx);
	MMOUTL(RADEON_DST_HEIGHT_WIDTH, (h << 16) | w);
	
	radeon_engine_idle();
}
/******************************RADEON部分END****************************************/

/**
 * @brief
 *	功能:
 *		在当前位置输出退格符（'\b'）。
 *		注意，只能在当前位置所在行退格，不能退格到上一行。
 *
 *	实现内容：
 *		1、如果当前位置不是在行首，退格；否则，不退格。
 *		2、若有硬件光标，刷新光标位置。
 *
 *
 * @return 成功返回TRUE；否则，返回FALSE。
 */
T_MODULE T_BOOL OutputBS()
{

	T_UBYTE ch = ' ';

	// 如果光标所在位置的列号大于0，则退格一个字符；否则，就不退格
	if(cursorCol>0)
	{
		cursorCol--;
		PrintChar(ch);
		cursorCol--;
	}
	else
	{
		#if 0
		cursorCol = CONSOLE_COLS - 1;
		cursorRow--;
		if (cursorRow < 0)
		{
			cursorRow = 0;
			cursorCol = 0;
		}
		PrintChar(tpDev, ch);

		#endif
	}

	return TRUE;
}

/**
 *@brief
 *	功能:
 *		在当前位置输出回车符（'\r'）。
 *
 *	实现内容：
 *		1、把当前位置定位到当前行的行首。
 *		2、若有硬件光标，刷新光标位置。
 *
 *
 *@return 成功返回TRUE；否则，返回FALSE。
 */
T_MODULE T_BOOL OutputCR()
{
	
	cursorCol = 0;

	return TRUE;
}
/**
 * @brief
 *		向指定设备输出字符。
 *
 * @param[in]	bCh：待被输出的字符。
 *
 * @return 无。
 */
T_MODULE T_VOID OutputChar(unsigned char ubChar)
{
	switch (ubChar)
	{
	case '\b':
		OutputBS();

		break;
//	case '\t':
//		OutputHT();

//		break;
	case '\n':
		PrintLF();

		break;
	case '\r' :
		OutputCR();

		break;
//	case '\e' :
//		OutputESC();
//
//		break;
	case 7:
		/// Bell code must be inserted here

		break;
	default:
	PrintChar(ubChar);
	}
}

//写字符
T_MODULE T_VOID video_drawchars (int *addr, int xx, int yy, unsigned char *s, int count)
{
#ifdef __ATX_BOARD__
	T_UBYTE *cdat = NULL;
	T_UBYTE *dest = NULL;
	T_UBYTE *dest0 = NULL;
	T_WORD rows = 0;
	T_WORD offset = 0;
	int c = 0;

	offset = (yy * VIDEO_LINE_LEN + xx * VIDEO_PIXEL_SIZE)/4;
	dest0 = addr + offset;

	while (count--)
	{
		c = *s;
		cdat = video_fontdata + c * VIDEO_FONT_HEIGHT;
		for (rows = VIDEO_FONT_HEIGHT, dest = dest0;
		     rows--;
		     dest += VIDEO_LINE_LEN)
		{
			T_UBYTE bits = *cdat++;
			
			((T_UWORD *) dest)[0] =  ((video_font_draw_table16 [bits >> 6] & eorx) ^ bgx);
			((T_UWORD *) dest)[1] =  ((video_font_draw_table16 [bits >> 4 & 3] & eorx) ^ bgx);
			((T_UWORD *) dest)[2] =  ((video_font_draw_table16 [bits >> 2 & 3] & eorx) ^ bgx);
			((T_UWORD *) dest)[3] =  ((video_font_draw_table16 [bits & 3] & eorx) ^ bgx);			
		}
		dest0 += VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE;
		s++;
	}
#else
	T_WORD offset = yy*CONSOLE_COLS +xx;
	T_WORD c = 0;
	short *a = (short*)addr;

	while (count--)
	{
		c = *s;

		a[offset]=0x0700|c;
		s++;
		offset++;
	}
#endif
}

/**
* @brief
*		在光标当前位置打印换行符，打印后，并计算光标的新位置。
*
*
* @return 无。
*/
T_MODULE T_VOID PrintLF()
{
	int upLine = SC_ROLL_Up;   //滚屏行数
	//移动指针
	cursorRow += upLine;
	cursorCol = 0;

	//检查是否需要滚屏
	if (cursorRow >= CONSOLE_ROWS)
	{
		//向上滚屏upLine行
		ScrollUpLines(upLine);
	}

	cursorCol = 0;
}

/**
 * @brief
 *		在光标当前位置打印指定字符，打印后，并计算光标的新位置。
 *
 * @param[in]	ubCh：待被打印的字符。
 *
 * @return 无。
 */
T_MODULE T_VOID PrintChar(T_UBYTE ubCh)
{
	T_UWORD xx = 0;
	T_UWORD yy = 0;
	T_UWORD *addr = videoRamBase;

#ifdef __ATX_BOARD__
	xx = cursorCol * VIDEO_FONT_WIDTH;
	yy = cursorRow * VIDEO_FONT_HEIGHT + VIDEO_LOGO_HEIGHT;
#else
	xx = cursorCol ;
	yy = cursorRow ;
#endif

	// 把字符放到光标当前在显存中的位置处
	video_drawchars(addr,xx, yy, &ubCh, 1);

	// 光标列号加1
	cursorCol++;

	// 如果光标已经到达一行的末端，则打印返回符
	if (cursorCol >= CONSOLE_COLS)
	{
		PrintLF();
	}
}


/**
* @brief
*		使屏幕内容向上滚动指定行数。
*
* @param[in]	uwLines：屏幕内容向上滚动的行数。
*
* @return 无。
*/
T_MODULE T_VOID ScrollUpLines(T_UWORD uwLines)
{
//	T_UWORD *addrFirst = videoRamBase;
//	T_UWORD *addrSecond = videoRamBase + CONSOLE_ROW_SIZE*uwLines/4;
	T_UWORD *addrLast = videoRamBase + CONSOLE_ROW_SIZE/4*(CONSOLE_ROWS-uwLines);//一共30行字符（0~29）

	// 确保最多只能移动屏幕屏幕最大行数
 	if (uwLines > CONSOLE_ROWS)
 	{
 		uwLines = CONSOLE_ROWS;
 	}

	// 把屏幕内容向前移动lines行，移出部分丢弃
	#ifdef SOFT_UP 
		// 软件实现滚屏
		memcpyl (addrFirst, addrSecond,CONSOLE_SCROLL_SIZE >> 2);
	#else
		// 硬件加速滚屏
		Loong_video_hw_bitblt (VIDEO_PIXEL_SIZE,			// bytes per pixel
			0,	                                			// 起始地址 x
			VIDEO_FONT_HEIGHT*uwLines,              		// 起始地址 y
			0,	                                			// 目的地址 x
			0,	                                			// 目的地址 y
			VIDEO_VISIBLE_COLS,	                    		// 屏幕宽
			VIDEO_VISIBLE_ROWS - VIDEO_FONT_HEIGHT*uwLines	// 屏幕高
			);
	#endif

	
	// 屏幕内容向前移动lines行后，屏幕最后lines行应该显示为空白，没有任何字符，因此用空格代替
	// 此处只能通过软件来实现,硬件加速源码未找到
	memsetl (addrLast, CONSOLE_ROW_SIZE >> 2, CONSOLE_BG_COL);

	//恢复行指针,指向最后一行
	cursorRow -= uwLines;
}
