/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsWorker.h
* @brief  
*       功能：
*       <li>定义Worker，实现文件操作的处理</li>
*/
/************************头文件********************************/
#ifndef _TSFSWORKER_H
#define _TSFSWORKER_H

#include "tsfsThread.h"

/************************宏定义********************************/
//数据包大小
#define TSFS_DATA_SIZE      TSFS_PACKET_DATASIZE

//目录,文件名长度
#define TSFS_NAME_SIZE      NAME_SIZE    

//处理命令成功
#define TSFS_PROCESS_SUCC   0

#define TSFS_SUCC           1
#define TSFS_FAIL           0

/************************类型定义******************************/

/**
* @brief
 * <li>功能：实现文件的具体操作。<li>
 * @auther 唐兴培
 * @version 1.0
 * @date 2007-10-31
*/
class TSFS_Worker :public threadWithQueue
{
private: 
    US m_lnkId;         //连接句柄
    int m_errno;        //错误值

    /*
     * @brief
     *   根据处理请求，并回复结果
     * @param pack[in]: 目标机的处理请求
     */
    void process(Packet *pack);

    /*
     * @brief
     *   将Windows错误码转换为PixOS错误码
     * @param winErr[in]: Windows错误码
     * @param pixErr[out]: PixOS错误码
     */
    void winErr2posixErr(int winErr, int *pixErr);

    /*
     * @brief
     *   将目标机端文件操作参数转换为主机端文件操作参数
     * @param taParam[in]: 目标机端文件操作参数
     * @param tsParam[out]: 主机端文件操作参数
     * @return   操作成功，则返回TSFS_SUCC
             操作失败，则返回TSFS_FAIL
     */
    int taParam2tsParam(int taParam, int *tsParam);

    /*
     * @brief
     *    将相对路径转换为绝对路径
     * @param tempPath[in]:    相对路径
     * @param fullPath[out]:   绝对路径
     * @return   操作成功，则返回TSFS_SUCC
                 操作失败，则返回TSFS_FAIL
     */
    int tempPath2fullPach(char *tempPath, char *fullPath);

    /*
     * @brief
     *    执行打开文件操作，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsOpenFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    执行关闭文件操作，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsCloseFile(char *pRecvData, Packet *sendPack);


    /*
     * @brief
     *    执行读取文件操作，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsReadFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    执行写文件操作，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsWriteFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    执行创建文件，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsCreateFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    执行删除文件，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsDeleteFile(char *pRecvData, Packet *sendPack);


    /*
     * @brief
     *    根据文件属性操作类型，调用对应功能函数处理
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int tsfsIOControl(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    获取文件中未读长度，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioNotRead(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    设置文件读写位置，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioSeek(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    获取文件读写位置，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioWhere(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    重命名文件，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioRename(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    创建目录，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioMkDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    删除目录，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioRmDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    读取目录项，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioReadDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    根据文件描述符,获取文件状态，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioFstatGetByFd(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    根据文件名字,获取文件状态，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioFstatGetByName(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    在指定长度处截断文件，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioTrunc(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    设置文件时间，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioTimeSet(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    检查是否为根目录
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    int fioCheckRoot(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    获取文件系统信息，并向目标机回复执行结果
     * @param *pRecvData[in]:    待处理的操作命令
     * @return   操作成功，则返回TURE
                 操作失败，则返回FALSE
     */
    //int fioFstatFsget(char *pRecvData, Packet *sendPack);

public:

    /**
     * @brief
     *    构造函数，完成Worker初始化工作，连接TS
     */
    TSFS_Worker(unsigned short size);

    /**
    * @brief
    *    析构函数，关闭连接
    */
    ~TSFS_Worker();
};

#endif