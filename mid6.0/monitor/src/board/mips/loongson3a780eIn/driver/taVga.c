/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taVga.c
 * @brief:
 *             <li>ʵ�ֶ�vga�Ļ�����ʾ���ܵ�����</li>
 */
#define __ATX_BOARD__
/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taVga.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/
static void ScrollUpLines(T_UWORD uwLines);
static void PrintLF(void);
static void PrintChar(T_UBYTE ubCh);
static void Loong_video_hw_bitblt(int bpp, int sx, int sy, int dx, int dy, int w, int h);
static void OutputChar(unsigned char ubChar);

/************************ģ�����*****************************/
static const int video_font_draw_table16[] = {0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff };
unsigned int eorx, fgx, bgx;
static T_UWORD* videoRamBase = (T_UWORD*)0x0;	
static T_UWORD cursorRow = 0;		
static T_UWORD cursorCol = 0;	
static T_UWORD winSizeX = 0;
static T_UWORD winSizeY = 0;
static T_UWORD gdfBytesPP = 0;
static T_UWORD gdfIndex = 0;
static void memsetl (T_UWORD *p, T_WORD c, T_UWORD v)
{
	while (c--)
	{
		*(p++) = v;
	}
}

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ʼ������̨
 * @return:
 *      ��
 */
void taDisplayDeviceOpen(void)
{
#ifdef __ATX_BOARD__
	videoRamBase	= (T_UWORD*)0xc0000000;
#else
	videoRamBase	= (T_UWORD*)0xbe000000;
#endif
	winSizeX      = VIDEO_COLS;
	winSizeY      = VIDEO_ROWS;
  	gdfBytesPP    = 2;
	gdfIndex      = GDF_16BIT_565RGB;

	fgx = (((CONSOLE_FG_COL >> 3) << 27) |
			((CONSOLE_FG_COL >> 2) << 21) | ((CONSOLE_FG_COL >> 3) << 16) |
			((CONSOLE_FG_COL >> 3) << 11) | ((CONSOLE_FG_COL >> 2) << 5) |
			(CONSOLE_FG_COL >> 3));
	bgx = (((CONSOLE_BG_COL >> 3) << 27) |
			((CONSOLE_BG_COL >> 2) << 21) | ((CONSOLE_BG_COL >> 3) << 16) |
			((CONSOLE_BG_COL >> 3) << 11) | ((CONSOLE_BG_COL >> 2) << 5) |
			(CONSOLE_BG_COL >> 3));
	eorx = fgx ^ bgx;

	/* ���� */
	memsetl(videoRamBase, CONSOLE_SIZE, CONSOLE_BG_COL);

	cursorRow = 0;
	cursorCol = 0;
}
 
void radeon_engine_flush (void)
{
	int i;
	
	/* ����ˢ�� */
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

/* �ȴ�fifo���� */
void radeon_fifo_wait (int entries)
{
	int i;
	
	for (i=0; i<2000000; i++)
		if ((MMINL(RADEON_RBBM_STATUS) & 0x7f) >= entries)
			return;
}

/* �ȴ�������� */
void radeon_engine_idle (void)
{
	int i;
	
	/* ȷ��FIFO���� */
	radeon_fifo_wait (64);
	
	for (i=0; i<2000000; i++) 
	{
		if (((MMINL(RADEON_RBBM_STATUS) & RADEON_RBBM_ACTIVE)) == 0) 
		{
			radeon_engine_flush ();
			return;
		}
	}
}

/* Ӳ������ */
static void Loong_video_hw_bitblt(int bpp, int sx, int sy, int dx, int dy, int w, int h)
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

/**
 * @brief
 *	����:
 *		�ڵ�ǰλ������˸����'\b'��
 *		ע�⣬ֻ���ڵ�ǰλ���������˸񣬲����˸���һ��
 *	ʵ�����ݣ�
 *		1�������ǰλ�ò��������ף��˸񣻷��򣬲��˸�
 *		2������Ӳ����꣬ˢ�¹��λ�á�
 * @return �ɹ�����TRUE�����򣬷���FALSE
 */
T_MODULE T_BOOL OutputBS(void)
{
	T_UBYTE ch = ' ';

	/* ����������λ�õ��кŴ���0�����˸�һ���ַ������򣬾Ͳ��˸� */
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
 *	����:
 *		�ڵ�ǰλ������س�����'\r'��
 *	ʵ�����ݣ�
 *		1���ѵ�ǰλ�ö�λ����ǰ�е�����
 *		2������Ӳ����꣬ˢ�¹��λ��
 *@return �ɹ�����TRUE�����򣬷���FALSE
 */
static T_BOOL OutputCR(void)
{
	cursorCol = 0;

	return TRUE;
}

/**
 * @brief
 *		��ָ���豸����ַ���
 * @param[in]	bCh������������ַ�
 * @return ��
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

/* д�ַ� */
static void video_drawchars (int *addr, int xx, int yy, unsigned char *s, int count)
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
*		�ڹ�굱ǰλ�ô�ӡ���з�����ӡ�󣬲����������λ��
* @return ��
*/
static void PrintLF(void)
{
	int upLine = SC_ROLL_Up;
	
	/* �ƶ�ָ�� */
	cursorRow += upLine;
	cursorCol = 0;

	/* ����Ƿ���Ҫ���� */
	if (cursorRow >= CONSOLE_ROWS)
	{
		/* ���Ϲ���upLine�� */
		ScrollUpLines(upLine);
	}

	cursorCol = 0;
}

/**
 * @brief
 *		�ڹ�굱ǰλ�ô�ӡָ���ַ�����ӡ�󣬲����������λ��
 * @param[in] ubCh��������ӡ���ַ�
 * @return ��
 */
static void PrintChar(T_UBYTE ubCh)
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

	/* ���ַ��ŵ���굱ǰ���Դ��е�λ�ô� */
	video_drawchars(addr,xx, yy, &ubCh, 1);

	/* ����кż�1 */
	cursorCol++;
}

/**
* @brief
*		ʹ��Ļ�������Ϲ���ָ������
* @param[in] uwLines����Ļ�������Ϲ���������
* @return ��
*/
static void ScrollUpLines(T_UWORD uwLines)
{
	T_UWORD *addrFirst = videoRamBase;
	T_UWORD *addrLast = videoRamBase + (CONSOLE_ROWS-uwLines)*CONSOLE_COLS * 2;

	/* ȷ�����ֻ���ƶ���Ļ��Ļ������� */
 	if (uwLines > CONSOLE_ROWS)
 	{
 		uwLines = CONSOLE_ROWS;
 	}

	/* ����Ļ������ǰ�ƶ�lines�У��Ƴ����ֶ��� */
	while(addrFirst < addrLast)
	{
		*addrFirst = *(addrFirst+ uwLines*CONSOLE_COLS);
		addrFirst++;
	}
	
	/* ��Ļ������ǰ�ƶ�lines�к���Ļ���lines��Ӧ����ʾΪ�հף�û���κ��ַ�������ÿո���� */
	memsetl(addrLast, CONSOLE_ROW_SIZE >> 2, CONSOLE_BG_COL);
	
	/* �ָ���ָ��,ָ�����һ�� */
	cursorRow -= uwLines;
	cursorRow--;
}

/*
 * @brief:
 *      �����豸����ַ�
 * @param[in]: ch:�ַ�
 * @return:
 *      ��
 */
void taDisplayDeviceOutputChar(T_CHAR ch)
{
	/* ��ӡ�ַ� */
	OutputChar(ch);
}
