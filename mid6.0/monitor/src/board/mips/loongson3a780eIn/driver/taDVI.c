/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taVga.c
 * @brief:
 *             <li>ʵ�ֶ�vga�Ļ�����ʾ���ܵ�����</li>
 */_
#define __ATX_BOARD__
/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taDVI.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/
static void ScrollUpLines(T_UWORD uwLines);
static void PrintLF(void);
static void PrintChar(T_UBYTE ubCh);
static void OutputChar(unsigned char ubChar);

/************************ģ�����*****************************/
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
	videoRamBase	= (T_UWORD*)0xc0144000;
#else
	videoRamBase	= (T_UWORD*)0xbe000000;
#endif

	/* ���� */
	memset((void*)0xc0144000,0,2400000);

	cursorRow = 0;
	cursorCol = 0;
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

#ifdef __ATX_BOARD__
	xx = cursorCol * VIDEO_FONT_WIDTH;
	yy = cursorRow * VIDEO_FONT_HEIGHT + VIDEO_LOGO_HEIGHT;
#else
	xx = cursorCol ;
	yy = cursorRow ;
#endif

	/* ���ַ��ŵ���굱ǰ���Դ��е�λ�ô� */
	video_drawchar(videoRamBase,xx, yy, ubCh);

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
