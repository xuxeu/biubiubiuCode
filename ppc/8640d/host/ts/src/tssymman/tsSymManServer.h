/*
* 更改历史：
* 2008-09-23 zhangxu  北京科银技术有限公司
*                       创建该文件。
*/

/**
* @file  tsSymManServer.h
* @brief
*       功能：
*       <li>tssymman主体,处理处理文件上载、下载、查询、删除等</li>
*/

/************************头文件********************************/

#ifndef _TSSYMMAN_SERVER_H
#define _TSSYMMAN_SERVER_H

#include "common.h"

/************************宏定义********************************/
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


/************************类型定义******************************/

/*定义错误号*/
enum T_TsSymMan_ErrorNum
{
    ERR_INVALID_COMMAND = -100, //无效的命令字
    ERR_FILE_EXIST,                     //文件已经存在
    ERR_DELETE_FOLDER,      //删除符号文件目录失败
    ERR_OPEN_FILE,          //打开文件失败
    ERR_FOLDER_INEXIST, //目标机IP对于的目录不存在
    ERR_FILE_INEXIST,       //待下载的文件不存在
    ERR_READ_FILE,          //读文件失败
    ERR_DELETE_FILE,        //删除文件失败
    ERR_FOLDER_EMPTY,        //目录为空
    ERR_WRITE_FILE,          //写文件失败    
};

/*定义文件类型*/
enum T_TsSymMan_FileType
{
    TSSYMMAN_FILE_TYPE_ONLY = 0,
    TSSYMMAN_FILE_TYPE_NOT_ONLY
};

/*定义文件位置类型*/
enum T_TsSymMan_PlaceFlag
{
    TSSYMMAN_PLACE_FLAG_BEGIN = 0,
    TSSYMMAN_PLACE_FLAG_BEGIN_END,
    TSSYMMAN_PLACE_FLAG_CONTINUE,
    TSSYMMAN_PLACE_FLAG_END
};

/*定义文件打开类型结构*/
typedef struct
{
    int      placeFlag;
    char    *mode;
} T_MODE_TBL;

/*定义错误信息结构*/
typedef struct
{
    int      erNo;
    char    *erStr;
} T_ERR_TBL;

/*定义符号文件处理类*/
class tsSymMan
{
private:
    char    m_tmpData[MAX_MESSAGE_LEN]; //回复数据缓冲区
    int m_tmpDataLen; //回复数据长度
    TMutex m_mtxBuf; //操作回复数据的互斥量
    waitCondition m_wtData; //等待回复数据的信号量
    int m_curSrcAid; //当前命令包的源aid号
    int m_curSrcSaid; //当前命令包的源said号
    char m_symFilePath[MAX_PATH_LEN]; //存放符号表文件的路径

    /*消息回复*/
    int sendAck(char *ack, int len);

    /*上载文件的处理函数*/
    int uploadFile(char *data, int len);

    /*下载文件的处理函数*/
    int downloadFile(char *data, int len);

    /*查询当前目标机当前有哪些符号文件*/
    int queryFile(char *data, int len);

    /*删除目标机上的某符号文件*/
    int deleteFile(char *data, int len);

public:
    /*构造函数*/
    tsSymMan(char* pstrPath);

    /*收包函数*/
    int putpkt(T_UHWORD srcAid, T_UHWORD srcSaid, char *data, int len);

    /*发包函数*/
    int getpkt(T_UHWORD *desAid, T_UHWORD *desSaid, char *buf, int size);

    /*终止tssymman让getpkt直接返回*/
    void stop();

};

/************************接口声明******************************/
/**
 * @brief        :  通过返回值获取对应的回复字符串
 * @para[IN]  :errno, 错误码
 * @return      : 返回错误码对应的回复字符串
*/
char* getAckStr(int eno);

#endif

