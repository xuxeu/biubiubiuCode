/*
* ������ʷ��
* 2008-09-23 zhangxu  ���������������޹�˾
*                       �������ļ���
*/

/**
* @file  tsSymManServer.h
* @brief
*       ���ܣ�
*       <li>tssymman����,�������ļ����ء����ء���ѯ��ɾ����</li>
*/

/************************ͷ�ļ�********************************/

#ifndef _TSSYMMAN_SERVER_H
#define _TSSYMMAN_SERVER_H

#include "common.h"

/************************�궨��********************************/
#define  TSSYMMAN_UPLOAD        'U'
#define  TSSYMMAN_DOWNLOAD     'L'
#define  TSSYMMAN_QUERY            'Q'
#define  TSSYMMAN_DELETE           'D'

#define  TSSYMMAN_SPLIT_FLAG                              ";"
#define  TSSYMMAN_SPLIT_FLAG_LEN                       1
#define  TSSYMMAN_FILENAME_SPLIT_FLAG              "|"
#define  TSSYMMAN_FILENAME_SPLIT_FLAG_LEN       1

#define MAX_IPV4_IP_LEN             16
#define MAX_MESSAGE_LEN         2048
#define MAX_READ_BUF_LEN            (2048-5)    //5 = strlen("OK:0;")

#define  ESTR_SYS                       "01;"
#define  ESTR_FILE_EXIST              "02;"
#define  ESTR_DELETE_FOLDER       "03;"
#define  ESTR_OPEN_FILE              "04;"
#define  ESTR_FOLDER_INEXIST    "06;"
#define  ESTR_FILE_INEXIST      "07;"
#define  ESTR_READ_FILE         "08;"
#define  ESTR_DELETE_FILE       "09;"
#define  ESTR_INVALID_COMMAND       "10;"
#define  ESTR_FOLDER_EMPTY      "11;"
#define  ESTR_WRITE_FILE      "12;"


/************************���Ͷ���******************************/

/*��������*/
enum T_TsSymMan_ErrorNum
{
    ERR_INVALID_COMMAND = -100, //��Ч��������
    ERR_FILE_EXIST,                     //�ļ��Ѿ�����
    ERR_DELETE_FOLDER,      //ɾ�������ļ�Ŀ¼ʧ��
    ERR_OPEN_FILE,          //���ļ�ʧ��
    ERR_FOLDER_INEXIST, //Ŀ���IP���ڵ�Ŀ¼������
    ERR_FILE_INEXIST,       //�����ص��ļ�������
    ERR_READ_FILE,          //���ļ�ʧ��
    ERR_DELETE_FILE,        //ɾ���ļ�ʧ��
    ERR_FOLDER_EMPTY,        //Ŀ¼Ϊ��
    ERR_WRITE_FILE,          //д�ļ�ʧ��    
};

/*�����ļ�����*/
enum T_TsSymMan_FileType
{
    TSSYMMAN_FILE_TYPE_ONLY = 0,
    TSSYMMAN_FILE_TYPE_NOT_ONLY
};

/*�����ļ�λ������*/
enum T_TsSymMan_PlaceFlag
{
    TSSYMMAN_PLACE_FLAG_BEGIN = 0,
    TSSYMMAN_PLACE_FLAG_BEGIN_END,
    TSSYMMAN_PLACE_FLAG_CONTINUE,
    TSSYMMAN_PLACE_FLAG_END
};

/*�����ļ������ͽṹ*/
typedef struct
{
    int      placeFlag;
    char    *mode;
} T_MODE_TBL;

/*���������Ϣ�ṹ*/
typedef struct
{
    int      erNo;
    char    *erStr;
} T_ERR_TBL;

/*��������ļ�������*/
class tsSymMan
{
private:
    char    m_tmpData[MAX_MESSAGE_LEN]; //�ظ����ݻ�����
    int m_tmpDataLen; //�ظ����ݳ���
    TMutex m_mtxBuf; //�����ظ����ݵĻ�����
    waitCondition m_wtData; //�ȴ��ظ����ݵ��ź���
    int m_curSrcAid; //��ǰ�������Դaid��
    int m_curSrcSaid; //��ǰ�������Դsaid��
    char m_symFilePath[MAX_PATH_LEN]; //��ŷ��ű��ļ���·��

    /*��Ϣ�ظ�*/
    int sendAck(char *ack, int len);

    /*�����ļ��Ĵ�����*/
    int uploadFile(char *data, int len);

    /*�����ļ��Ĵ�����*/
    int downloadFile(char *data, int len);

    /*��ѯ��ǰĿ�����ǰ����Щ�����ļ�*/
    int queryFile(char *data, int len);

    /*ɾ��Ŀ����ϵ�ĳ�����ļ�*/
    int deleteFile(char *data, int len);

public:
    /*���캯��*/
    tsSymMan(char* pstrPath);

    /*�հ�����*/
    int putpkt(T_UHWORD srcAid, T_UHWORD srcSaid, char *data, int len);

    /*��������*/
    int getpkt(T_UHWORD *desAid, T_UHWORD *desSaid, char *buf, int size);

    /*��ֹtssymman��getpktֱ�ӷ���*/
    void stop();

};

/************************�ӿ�����******************************/
/**
 * @brief        :  ͨ������ֵ��ȡ��Ӧ�Ļظ��ַ���
 * @para[IN]  :errno, ������
 * @return      : ���ش������Ӧ�Ļظ��ַ���
*/
char* getAckStr(int eno);

#endif

