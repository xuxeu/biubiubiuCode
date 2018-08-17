/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taVga.c
 * @brief:
 *             <li>实现对vga的基本显示功能的驱动</li>
 */_
#define __ATX_BOARD__
/************************头 文 件******************************/
#include "taTypes.h"
#include "taDVI.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/
static void ScrollUpLines(T_UWORD uwLines);
static void PrintLF(void);
static void PrintChar(T_UBYTE ubCh);
static void OutputChar(unsigned char ubChar);

/************************模块变量*****************************/
static T_UWORD* videoRamBase = (T_UWORD*)0x0;	
static T_UWORD cursorRow = 0;		
static T_UWORD cursorCol = 0;	
unsigned char draw_color_1 = 0xff;
unsigned char back_color_1 = 0x0;
unsigned char draw_color_2 = 0xff;
unsigned char back_color_2 = 0x0;
static void memsetl (T_UWORD *p, T_WORD c, T_UWORD v)
{
	while (c--)
	{
		*(p++) = v;
	}
}

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      初始化控制台
 * @return:
 *      无
 */
void taDisplayDeviceOpen(void)
{
#ifdef __ATX_BOARD__
	videoRamBase	= (T_UWORD*)0xc0144000;
#else
	videoRamBase	= (T_UWORD*)0xbe000000;
#endif

	/* 清屏 */
	memset((void*)0xc0144000,0,2400000);

	cursorRow = 0;
	cursorCol = 0;
}

/**
 * @brief
 *	功能:
 *		在当前位置输出退格符（'\b'）
 *		注意，只能在当前位置所在行退格，不能退格到上一行
 *	实现内容：
 *		1、如果当前位置不是在行首，退格；否则，不退格
 *		2、若有硬件光标，刷新光标位置。
 * @return 成功返回TRUE；否则，返回FALSE
 */
T_MODULE T_BOOL OutputBS(void)
{
	T_UBYTE ch = ' ';

	/* 如果光标所在位置的列号大于0，则退格一个字符；否则，就不退格 */
	if(cursorCol>0)
	{
		cursorCol--;
		PrintChar(ch);
		cursorCol--;
	}
	
	return TRUE;
}

/**
 *@brief
 *	功能:
 *		在当前位置输出回车符（'\r'）
 *	实现内容：
 *		1、把当前位置定位到当前行的行首
 *		2、若有硬件光标，刷新光标位置
 *@return 成功返回TRUE；否则，返回FALSE
 */
static T_BOOL OutputCR(void)
{
	cursorCol = 0;

	return TRUE;
}

/**
 * @brief
 *		向指定设备输出字符。
 * @param[in]	bCh：待被输出的字符
 * @return 无
 */
static void OutputChar(unsigned char ubChar)
{
	switch (ubChar)
	{
	case '\b':
		OutputBS();
		break;
	case '\n':
		PrintLF();
		break;
	case '\r' :
		OutputCR();
		break;
	default:
		PrintChar(ubChar);
	}
}

/* 写字符 */
static void video_drawchar(int *addr, int xx, int yy, unsigned char c)
{
#ifdef __ATX_BOARD__
    int i,j;
	unsigned char *VGA_DEV;
	unsigned char *base;
    unsigned char line;
	VGA_DEV = (unsigned int  *)addr;
    base = &((VGA_DEV)[yy*VIDEO_COLS * VIDEO_PIXEL_SIZE + xx * VIDEO_PIXEL_SIZE]);

    for(i=0; i<VIDEO_FONT_HEIGHT; i++) 
	{
		line = video_fontdata[c * VIDEO_FONT_HEIGHT + i];

		for(j=VIDEO_FONT_WIDTH * VIDEO_PIXEL_SIZE; j > 0; j -= 2 ) 
		{
			base[ j  ]   = (line & 0x1) ? draw_color_1 : back_color_1;
			base[ j - 1] = (line & 0x1) ? draw_color_2 : back_color_2; 
			line = line >> 1;
		}
		base += (VIDEO_COLS * VIDEO_PIXEL_SIZE);
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
*		在光标当前位置打印换行符，打印后，并计算光标的新位置
* @return 无
*/
static void PrintLF(void)
{
	int upLine = SC_ROLL_Up;
	
	/* 移动指针 */
	cursorRow += upLine;
	cursorCol = 0;

	/* 检查是否需要滚屏 */
	if (cursorRow >= CONSOLE_ROWS)
	{
		/* 向上滚屏upLine行 */
		ScrollUpLines(upLine);
	}

	cursorCol = 0;
}

/**
 * @brief
 *		在光标当前位置打印指定字符，打印后，并计算光标的新位置
 * @param[in] ubCh：待被打印的字符
 * @return 无
 */
static void PrintChar(T_UBYTE ubCh)
{
	T_UWORD xx = 0;
	T_UWORD yy = 0;

#ifdef __ATX_BOARD__
	xx = cursorCol * VIDEO_FONT_WIDTH;
	yy = cursorRow * VIDEO_FONT_HEIGHT + VIDEO_LOGO_HEIGHT;
#else
	xx = cursorCol ;
	yy = cursorRow ;
#endif

	/* 把字符放到光标当前在显存中的位置处 */
	video_drawchar(videoRamBase,xx, yy, ubCh);

	/* 光标列号加1 */
	cursorCol++;
}

/**
* @brief
*		使屏幕内容向上滚动指定行数
* @param[in] uwLines：屏幕内容向上滚动的行数
* @return 无
*/
static void ScrollUpLines(T_UWORD uwLines)
{
	T_UWORD *addrFirst = videoRamBase;
	T_UWORD *addrLast = videoRamBase + (CONSOLE_ROWS-uwLines)*CONSOLE_COLS * 2;

	/* 确保最多只能移动屏幕屏幕最大行数 */
 	if (uwLines > CONSOLE_ROWS)
 	{
 		uwLines = CONSOLE_ROWS;
 	}

	/* 把屏幕内容向前移动lines行，移出部分丢弃 */
	while(addrFirst < addrLast)
	{
		*addrFirst = *(addrFirst+ uwLines*CONSOLE_COLS);
		addrFirst++;
	}
	
	/* 屏幕内容向前移动lines行后，屏幕最后lines行应该显示为空白，没有任何字符，因此用空格代替 */
	memsetl(addrLast, CONSOLE_ROW_SIZE >> 2, CONSOLE_BG_COL);
	
	/* 恢复行指针,指向最后一行 */
	cursorRow -= uwLines;
	cursorRow--;
}

/*
 * @brief:
 *      回显设备输出字符
 * @param[in]: ch:字符
 * @return:
 *      无
 */
void taDisplayDeviceOutputChar(T_CHAR ch)
{
	/* 打印字符 */
	OutputChar(ch);
}
