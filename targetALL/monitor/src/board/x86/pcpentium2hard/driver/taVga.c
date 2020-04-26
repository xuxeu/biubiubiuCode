
/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taVga.c
 * @brief:
 *             <li>实现对vga的基本显示功能的驱动</li>
 */



/************************头 文 件******************************/
#include "taTypes.h"
#include "taVga.h"
#include "config_ta.h"

#ifdef CONFIG_TA_DISP_VGA

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/*
 * Physical address of start of video text memory.
 */
static T_UHWORD *videoRam = (T_UHWORD *)CRT_VGA_FB;

/*
 * Pointer for current output position in display.
 */
static T_UHWORD *videoRamPtr = (T_UHWORD *)CRT_VGA_FB;
static T_UBYTE  videoRows = CRT_VGA_NUM_ROWS;   /* Number of rows in display.    */
static T_UBYTE  videoCols = CRT_VGA_NUM_COLS;   /* Number of columns in display. */
static T_UBYTE  cursRow   = 0;                  /* Current cursor row.           */
static T_UBYTE  cursCol   = 0;                  /* Current cursor column.        */

/************************全局变量*****************************/

/************************函数实现*****************************/

static void scrollUp(T_UBYTE lines)
{
    /*
    * Number of blank display cells on bottom of window.
    */
    T_UHWORD blankCount;
    
    /*
    * Source and destination pointers for memory copy operations.
    */
    T_UHWORD *ptrDst, *ptrSrc;

    /*
    * Move window's contents up.
    */
    if (lines < videoRows) 
    {
        /*
         * Number of non-blank cells on upper part of display (total - blank).
         */
        T_UHWORD nonBlankCount;

        blankCount = lines * videoCols;
        nonBlankCount = CRT_DISPLAY_CELL_COUNT - blankCount;
        ptrSrc = videoRam + blankCount;
        ptrDst = videoRam;

        while (nonBlankCount--)
          *ptrDst++ = *ptrSrc++;
    }     
    else 
    {
	    /* Clear the whole display. */
	    blankCount = CRT_DISPLAY_CELL_COUNT;
	    ptrDst = videoRam;
    }

    /* Fill bottom with blanks. */
    while (blankCount-- > 0)
	    *ptrDst++ = CRT_BLANK;
} 

static void setHardwareCursorPos(T_UHWORD videoCursor)
{
    CRT_VGA_WRITE_CRTC(0x0e, (videoCursor >> 8) & 0xff);
    CRT_VGA_WRITE_CRTC(0x0f, videoCursor & 0xff);
}

static void updateVideoRamPtr(void)
{
    videoRamPtr = videoRam + cursRow * videoCols + cursCol;
}

static void printCHAR(T_UBYTE character)
{
    *videoRamPtr++ = (CRT_WHITE <<8) | character;
    cursCol++;
    if(cursCol == videoCols) 
    {
        cursCol = 0;
        cursRow++;
        if(cursRow==videoRows) 
	    {
	        cursRow--;
	        scrollUp(1);
	        videoRamPtr -= videoCols;
	     }
    }
}

static void printLF(void)
{
    cursRow++;
    if (cursRow == videoRows)
    {
        cursRow--;
        scrollUp(1);
    }
    cursCol = 0;
    updateVideoRamPtr();
}

static void printCR(void)
{
    cursCol = 0;
    updateVideoRamPtr();
}

static void printHT(void)
{
    do 
    {
        printCHAR(' ');
    }
    while (cursCol % CRT_TABSIZE);

}

static void printESC(void)
{
    printLF();
    printCR();
}

static void printBS(void)
{
    /*
    * Move cursor back one cell.
    */
    if (cursCol > 0)
    {
        cursCol--;    
                   
        /*
        * Write a whitespace.
        */
        *(--videoRamPtr) = CRT_BLANK;
    }
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
    switch (ch) 
    {
    case '\b':
      printBS();
      break;

    case '\t':
      printHT();
      break;

    case '\n':
      printLF();
      break;

    case '\r' :
      printCR();
      break;

    case '\e' :
      printESC();
      break;

    case 7:

      /*
       * Bell code must be inserted here
       */

      break;

    default:
      printCHAR (ch);
      break;

    }
    setHardwareCursorPos(videoRamPtr - videoRam);
}

/*
 * @brief:
 *      初始化控制台
 * @return:
 *      无
 */
void taDisplayDeviceOpen(void)
{
    scrollUp(videoRows);             /*Clear entire screen*/
    setHardwareCursorPos(0);         /*Cursor at upper left corner*/
    CRT_VGA_WRITE_CRTC(0x0a, 0x0e);  /*Crt cursor start*/
}

#endif
