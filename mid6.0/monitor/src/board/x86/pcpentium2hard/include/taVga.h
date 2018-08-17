/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taVga.h
 * @brief:
 *             <li>定义VGA相关的宏、数据结构、函数接口等</li>
 */
#ifndef TA_VGA_H
#define TA_VGA_H

/************************头文件********************************/
#include "taIoAccess.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************宏定义********************************/

/*
 * Bitmap video origins in text mode.
 */
#define CRT_V_MONO	0xb0000        /* In monochrome */
#define CRT_V_COLOR	0xb8000        /* In color	 */


/*
 * Video Option Byte location. It must be maintained
 * by the BIOS.
 */
#define CRT_VIDEO_MODE_ADDR          	0x449

/*
 * Video controller base IO address location in
 * BIOS data area
 */
#define CRT_DISPLAY_CRT_BASE_IO_ADDR  	0x463

/*
 * Number of collums and lines locations for the
 * actual video Configuration
 */
#define CRT_NB_MAX_COL_ADDR			0x44a
#define CRT_NB_MAX_ROW_ADDR			0x484

/*
 * Miscellaneous information set by the BIOS in offset video_mode,
 */
#define CRT_VGAMODE7	0x7	/* VGA mode 7 */

/*
 * Color codes in text mode for background and foreground.
 */
#define CRT_BLACK		0x0
#define CRT_BLUE		0x1
#define CRT_GREEN		0x2
#define CRT_CYAN		0x3
#define CRT_RED	   	    0x4
#define CRT_MAGENTA		0x5
#define CRT_BROWN		0x6
#define CRT_WHITE		0x7		   /* White on Black background colour.   */

#define CRT_GRAY		0x8
#define CRT_LT_BLUE		0x9
#define CRT_LT_GREEN	0xa
#define CRT_LT_CYAN		0xb
#define CRT_LT_RED		0xc
#define CRT_LT_MAGENTA	0xd
#define CRT_YELLOW		0xe
#define CRT_LT_WHITE	0xf

#define CRT_BLINK       0x8	   /* Mask used to determine blinking */
#define CRT_OFF         0
#define CRT_ON          1

/*
 * CRT Controller register offset definitions
 */
#define CRT_CC_CURSSTART	0x0a   /* Cursor start scan line */
#define CRT_CC_CURSEND	    0x0b   /* Cursor end scan line */
#define CRT_CC_STARTADDRHI  0x0c   /* start video ram addr msb */
#define CRT_CC_STARTADDRLO  0x0d   /* start video ram lsb */
#define CRT_CC_CURSHIGH	    0x0e   /* Cursor high location (8 msb)	*/
#define CRT_CC_CURSLOW	    0x0f   /* Cursor low location (8 msb) */
#define CRT_CC_VRETSTART	0x10   /* vertical synchro start lsb */
#define CRT_CC_VRETEND	    0x11   /* vertical syn end (also it control) */
#define CRT_CC_VDISPEND	    0x12   /* vertical display end lsb register */

#define CRT_VGA_WRITE_CRTC(reg, val) \
	BSP_OutByte(0x3d4, reg); \
	BSP_OutByte(0x3d5, val)

#define CRT_VGA_FB		    0xb8000
#define CRT_VGA_NUM_ROWS	25
#define CRT_VGA_NUM_COLS	80
#define CRT_DISPLAY_CELL_COUNT (CRT_VGA_NUM_ROWS * CRT_VGA_NUM_COLS)
                                             /* Number of display cells.            */
#define CRT_TABSIZE 4                        /* Number of spaces for TAB (\t) char. */
#define CRT_BLANK  ( ' ' | (CRT_WHITE << 8)) /* Blank character.                    */

/************************类型定义******************************/

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TA_VGA_H */
