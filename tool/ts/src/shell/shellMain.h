/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  shellMain.h
* @brief
*       ���ܣ�
*       <li>shell��������غ궨��</li>
*/

#ifndef _SHELLMAIN_H
#define _SHELLMAIN_H


/************************ͷ�ļ�********************************/

/************************�궨��********************************/
#ifdef __cplusplus
extern "C" {
#endif

/* Ĭ�ϵȴ��հ�ʱ��,��λ�� */
#define DEFAULT_WAIT_TIME (10)


#define SUCC (0)
#define FAIL (-1)

/*��ctrl+c��ֹ�����ķ���ֵ*/
#define STOP_CMD (-2)


#define PROMPT_SYMBOL ("****************************************")

#define BLANK_STR        (" ")

#define MAX_MSG_SIZE (1024)

#define MAX_NAME_MSG_SIZE (49)

#ifdef __cplusplus
}
#endif
/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

#endif