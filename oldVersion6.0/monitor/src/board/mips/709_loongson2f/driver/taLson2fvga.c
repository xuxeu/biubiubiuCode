/****************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 *
 * 		Copyright (C) 2000-2008 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 *	20080311	�����ļ���
 *
 *
 */

/**
 * @file taLson2evga.inc
 * @brief
 *	<li>���ܣ���о2EVGA��ʾ�豸��������</li>
 * @date 	20080311
 */

/**************************** ���ò��� *****************************************/
#include "sysTypes.h"
#include "memory.h"
#include "taLson2fvga.h"
#include "string.h"

/*************************** ǰ���������� ****************************************/
T_MODULE T_VOID ScrollUpLines(T_UWORD uwLines);
T_MODULE T_VOID PrintLF();
T_MODULE T_VOID PrintChar(T_UBYTE ubCh);
T_MODULE T_VOID Loong_video_hw_bitblt(int bpp, int sx, int sy, int dx, int dy, int w, int h);
T_MODULE T_VOID OutputChar(unsigned char ubChar);

/**************************** ���岿�� *****************************************/
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
	
/**************************** ʵ�ֲ���*************************************************/
/**
 * @brief
 *	����:
 *		��ʼ����ʾ�豸��
 *
 *	ʵ�����ݣ�
 *		1�����䲢��ʼ���Դ档
 *		2����ʼ����ʾоƬ��
 *		3��������
 *		4������Ӳ����꣬���ù��λ�õ���Ļ��ʼλ�á�
 *
 *@param[in]	mode����Ļ��ʾģʽ��
 *
 *@return �ޡ�
 */
T_VOID monitorDisplayInit(T_BYTE mode)
{

	// ��ʼ��оƬ
#ifdef __ATX_BOARD__
	videoRamBase	= (T_UWORD*)0xb4000000;
#else
	videoRamBase	= (T_UWORD*)0xb00b8000;//0xb4000000;//0x94000000|0xb4000000// �Դ��׵�ַ
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

	// ���� 
	memsetl (videoRamBase, CONSOLE_SIZE, CONSOLE_BG_COL);

	// Ӳ���豸���λ�ã�����Ϊ���Ͻǣ�
	cursorRow = 0;
	cursorCol = 0;
}
 
/**
 *@brief
 *	����:
 *		�ڵ�ǰλ������ַ���
 *
 *	ʵ�����ݣ�
 * 		1���ڵ�ǰλ������ַ��������µ�ǰλ�á�
 *		2������Ӳ����꣬ˢ�¹��λ�á�
 *
 * @param[in]	ubCh������������ַ���
 *
 * @return	�ޡ�
 */
T_VOID monitorDisplayOutch(T_BYTE ch)
{
	// ��ӡ�ַ�
	OutputChar(ch);

}

/*****************************RADEON����START******************************************/
//2D���湤��
void radeon_engine_flush (void)
{
	int i;
	
	//����ˢ��
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

//�ȴ�fifo����
void radeon_fifo_wait (int entries)
{
	int i;
	
	for (i=0; i<2000000; i++)
		if ((MMINL(RADEON_RBBM_STATUS) & 0x7f) >= entries)
			return;
}

//�ȴ��������
void radeon_engine_idle (void)
{
	int i;
	
	//ȷ��FIFO����
	radeon_fifo_wait (64);
	
	for (i=0; i<2000000; i++) {
		if (((MMINL(RADEON_RBBM_STATUS) & RADEON_RBBM_ACTIVE)) == 0) {
			radeon_engine_flush ();
			return;
		}
	}
}

//Ӳ������
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
/******************************RADEON����END****************************************/

/**
 * @brief
 *	����:
 *		�ڵ�ǰλ������˸����'\b'����
 *		ע�⣬ֻ���ڵ�ǰλ���������˸񣬲����˸���һ�С�
 *
 *	ʵ�����ݣ�
 *		1�������ǰλ�ò��������ף��˸񣻷��򣬲��˸�
 *		2������Ӳ����꣬ˢ�¹��λ�á�
 *
 *
 * @return �ɹ�����TRUE�����򣬷���FALSE��
 */
T_MODULE T_BOOL OutputBS()
{

	T_UBYTE ch = ' ';

	// ����������λ�õ��кŴ���0�����˸�һ���ַ������򣬾Ͳ��˸�
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
 *	����:
 *		�ڵ�ǰλ������س�����'\r'����
 *
 *	ʵ�����ݣ�
 *		1���ѵ�ǰλ�ö�λ����ǰ�е����ס�
 *		2������Ӳ����꣬ˢ�¹��λ�á�
 *
 *
 *@return �ɹ�����TRUE�����򣬷���FALSE��
 */
T_MODULE T_BOOL OutputCR()
{
	
	cursorCol = 0;

	return TRUE;
}
/**
 * @brief
 *		��ָ���豸����ַ���
 *
 * @param[in]	bCh������������ַ���
 *
 * @return �ޡ�
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

//д�ַ�
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
*		�ڹ�굱ǰλ�ô�ӡ���з�����ӡ�󣬲����������λ�á�
*
*
* @return �ޡ�
*/
T_MODULE T_VOID PrintLF()
{
	int upLine = SC_ROLL_Up;   //��������
	//�ƶ�ָ��
	cursorRow += upLine;
	cursorCol = 0;

	//����Ƿ���Ҫ����
	if (cursorRow >= CONSOLE_ROWS)
	{
		//���Ϲ���upLine��
		ScrollUpLines(upLine);
	}

	cursorCol = 0;
}

/**
 * @brief
 *		�ڹ�굱ǰλ�ô�ӡָ���ַ�����ӡ�󣬲����������λ�á�
 *
 * @param[in]	ubCh��������ӡ���ַ���
 *
 * @return �ޡ�
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

	// ���ַ��ŵ���굱ǰ���Դ��е�λ�ô�
	video_drawchars(addr,xx, yy, &ubCh, 1);

	// ����кż�1
	cursorCol++;

	// �������Ѿ�����һ�е�ĩ�ˣ����ӡ���ط�
	if (cursorCol >= CONSOLE_COLS)
	{
		PrintLF();
	}
}


/**
* @brief
*		ʹ��Ļ�������Ϲ���ָ��������
*
* @param[in]	uwLines����Ļ�������Ϲ�����������
*
* @return �ޡ�
*/
T_MODULE T_VOID ScrollUpLines(T_UWORD uwLines)
{
//	T_UWORD *addrFirst = videoRamBase;
//	T_UWORD *addrSecond = videoRamBase + CONSOLE_ROW_SIZE*uwLines/4;
	T_UWORD *addrLast = videoRamBase + CONSOLE_ROW_SIZE/4*(CONSOLE_ROWS-uwLines);//һ��30���ַ���0~29��

	// ȷ�����ֻ���ƶ���Ļ��Ļ�������
 	if (uwLines > CONSOLE_ROWS)
 	{
 		uwLines = CONSOLE_ROWS;
 	}

	// ����Ļ������ǰ�ƶ�lines�У��Ƴ����ֶ���
	#ifdef SOFT_UP 
		// ���ʵ�ֹ���
		memcpyl (addrFirst, addrSecond,CONSOLE_SCROLL_SIZE >> 2);
	#else
		// Ӳ�����ٹ���
		Loong_video_hw_bitblt (VIDEO_PIXEL_SIZE,			// bytes per pixel
			0,	                                			// ��ʼ��ַ x
			VIDEO_FONT_HEIGHT*uwLines,              		// ��ʼ��ַ y
			0,	                                			// Ŀ�ĵ�ַ x
			0,	                                			// Ŀ�ĵ�ַ y
			VIDEO_VISIBLE_COLS,	                    		// ��Ļ��
			VIDEO_VISIBLE_ROWS - VIDEO_FONT_HEIGHT*uwLines	// ��Ļ��
			);
	#endif

	
	// ��Ļ������ǰ�ƶ�lines�к���Ļ���lines��Ӧ����ʾΪ�հף�û���κ��ַ�������ÿո����
	// �˴�ֻ��ͨ�������ʵ��,Ӳ������Դ��δ�ҵ�
	memsetl (addrLast, CONSOLE_ROW_SIZE >> 2, CONSOLE_BG_COL);

	//�ָ���ָ��,ָ�����һ��
	cursorRow -= uwLines;
}
