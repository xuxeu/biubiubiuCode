/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsWorker.h
* @brief  
*       ���ܣ�
*       <li>����Worker��ʵ���ļ������Ĵ���</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _TSFSWORKER_H
#define _TSFSWORKER_H

#include "tsfsThread.h"

/************************�궨��********************************/
//���ݰ���С
#define TSFS_DATA_SIZE      TSFS_PACKET_DATASIZE

//Ŀ¼,�ļ�������
#define TSFS_NAME_SIZE      NAME_SIZE    

//��������ɹ�
#define TSFS_PROCESS_SUCC   0

#define TSFS_SUCC           1
#define TSFS_FAIL           0

/************************���Ͷ���******************************/

/**
* @brief
 * <li>���ܣ�ʵ���ļ��ľ��������<li>
 * @auther ������
 * @version 1.0
 * @date 2007-10-31
*/
class TSFS_Worker :public threadWithQueue
{
private: 
    US m_lnkId;         //���Ӿ��
    int m_errno;        //����ֵ

    /*
     * @brief
     *   ���ݴ������󣬲��ظ����
     * @param pack[in]: Ŀ����Ĵ�������
     */
    void process(Packet *pack);

    /*
     * @brief
     *   ��Windows������ת��ΪPixOS������
     * @param winErr[in]: Windows������
     * @param pixErr[out]: PixOS������
     */
    void winErr2posixErr(int winErr, int *pixErr);

    /*
     * @brief
     *   ��Ŀ������ļ���������ת��Ϊ�������ļ���������
     * @param taParam[in]: Ŀ������ļ���������
     * @param tsParam[out]: �������ļ���������
     * @return   �����ɹ����򷵻�TSFS_SUCC
             ����ʧ�ܣ��򷵻�TSFS_FAIL
     */
    int taParam2tsParam(int taParam, int *tsParam);

    /*
     * @brief
     *    �����·��ת��Ϊ����·��
     * @param tempPath[in]:    ���·��
     * @param fullPath[out]:   ����·��
     * @return   �����ɹ����򷵻�TSFS_SUCC
                 ����ʧ�ܣ��򷵻�TSFS_FAIL
     */
    int tempPath2fullPach(char *tempPath, char *fullPath);

    /*
     * @brief
     *    ִ�д��ļ�����������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsOpenFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ִ�йر��ļ�����������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsCloseFile(char *pRecvData, Packet *sendPack);


    /*
     * @brief
     *    ִ�ж�ȡ�ļ�����������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsReadFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ִ��д�ļ�����������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsWriteFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ִ�д����ļ�������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsCreateFile(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ִ��ɾ���ļ�������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsDeleteFile(char *pRecvData, Packet *sendPack);


    /*
     * @brief
     *    �����ļ����Բ������ͣ����ö�Ӧ���ܺ�������
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int tsfsIOControl(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ��ȡ�ļ���δ�����ȣ�����Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioNotRead(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    �����ļ���дλ�ã�����Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioSeek(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ��ȡ�ļ���дλ�ã�����Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioWhere(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    �������ļ�������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioRename(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ����Ŀ¼������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioMkDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ɾ��Ŀ¼������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioRmDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ��ȡĿ¼�����Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioReadDir(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    �����ļ�������,��ȡ�ļ�״̬������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioFstatGetByFd(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    �����ļ�����,��ȡ�ļ�״̬������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioFstatGetByName(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ��ָ�����ȴ��ض��ļ�������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioTrunc(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    �����ļ�ʱ�䣬����Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioTimeSet(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ����Ƿ�Ϊ��Ŀ¼
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    int fioCheckRoot(char *pRecvData, Packet *sendPack);

    /*
     * @brief
     *    ��ȡ�ļ�ϵͳ��Ϣ������Ŀ����ظ�ִ�н��
     * @param *pRecvData[in]:    ������Ĳ�������
     * @return   �����ɹ����򷵻�TURE
                 ����ʧ�ܣ��򷵻�FALSE
     */
    //int fioFstatFsget(char *pRecvData, Packet *sendPack);

public:

    /**
     * @brief
     *    ���캯�������Worker��ʼ������������TS
     */
    TSFS_Worker(unsigned short size);

    /**
    * @brief
    *    �����������ر�����
    */
    ~TSFS_Worker();
};

#endif