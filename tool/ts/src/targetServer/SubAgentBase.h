/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  SubAgentBase.h
* @brief
*       ���ܣ�
*       <li> manager�Ķ������������Ϳ���������</li>
*/


#ifndef SUBAGENTBASE_INCLUDE
#define SUBAGENTBASE_INCLUDE

/************************ͷ�ļ�********************************/

#include "tsInclude.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

class Manager;

class ServerAgent;
class Packet;
class DeviceConfigBase;
class SubAgentBase
{
private:
    short SAID;    //���������
    bool logflag;  // ��־���
public:

    /**
    * ���ܣ����캯��,������ʼ�����������aid��
    * @param said ���������
    */
    SubAgentBase(short said)
    {
        SAID = said;
    }
    virtual ~SubAgentBase(void);
    /**
    * ���ܣ����ض��������
    */
    short getSaid()
    {
        return SAID;
    }
    /**
    * ���ܣ�������־���
    */
    void setLogFlag(bool flag)
    {
        logflag = flag;
    }
    /**
    * ���ܣ���ȡ�Ƿ���Ҫ��¼��־
    */
    bool getLogFlag()
    {
        return logflag;
    }
    /**
    * ���ܣ���ʼ����ǰ�Ķ�������
    */
    virtual void init() = 0;
    /**
    * ���ܣ�������ǰ�����ݰ�,��ִ�в���
    * @param pack ���ݱ�
    */
    virtual void process(Packet* pack) = 0;
};


class ControlSubAgent : public SubAgentBase
{

protected:

    PackLog packLoger;
    //end
    /**
    * ���ܣ���������ʽ
    * @param pack ���յĿ��ư�
    */
    bool parsePacket(Packet* pack);
    /**
    * ���ܣ�����TClient���͹�����GetID��������,���ѵõ���AID���ظ�TClient
    * @param pack ���յĿ��ư�
    */
    void handleGetID(Packet* pack) ;
    /**
    * ���ܣ�����TClient���͹�����createServer��������,���ѵõ���AID���ظ�TClient
    * @param pack ���յĿ��ư�
    */
    void handleCreateServer(Packet* pack);
    /**
    * ���ܣ�����TClient���͹�����createServer��������,���ѵõ���AID���ظ�TClient
    * @param block ȥ��createserver����
    * @param pack ���յĿ��ư�
    */
    void handleCreateServerWithDll(Packet* pack) ;
    /**
    * ���ܣ�����TClient���͹�����createServer��������,���ѵõ���AID���ظ�TClient
    * @param block ȥ��createserver��־������
    * @param pack ���յĿ��ư�
    */
    void handleDeleteServer(Packet* pack);
    /**
    * ���ܣ�����ʧ�ܻظ���Ϣ
    * @param pack Ҫ���͵����ݰ�


    * @param strErrMsg: ������Ϣ
    * @param size: ������Ϣ����
    */
    void sendErrorAck(Packet* pack, char *strErrMsg, UI size = FAILURE_ACK_LEN);
    /**
    * ���ܣ����óɹ�����Ϣ
    * @param pack Ҫ���͵����ݰ�
    */
    void setSuccessPacket(Packet* pack) ;
    /**
    * ���ܣ����ò����ͳɹ��Ļظ���
    * @param pack Ҫ���͵����ݰ�������Ŀ�ĺ�ԴID��
    */
    void sendSuccessPacket(Packet* pack) ;
    /**
    * ���ܣ���������Ŀ��SAID��AID
    * @param pack ���յĿ��ư�
    */
    void changePacketID(Packet* pack);
    /**
    * ���ܣ�����TCLIENT_OPEN����,����ԴAID��������
    * @param pack ���յĿ��ư�
    */
    void handleTClientOpen(Packet* pack) ;
    ////////////////////////////���ư�start
    /**
    * ���ܣ�����TClient_queryServer����,ָ��Ŀ�����ID�Ų��Ҹ�Ŀ��������ò�����״̬��Ϣ
    * @param block ȥ��queryServerState��־������
    * @param pack ���յĿ��ư�
    */
    void handleQueryServer(Packet* pack);
    /**
    * ���ܣ�����TClient_searchServer����,��ѯTS������ڵ����еĿ���Ŀ�����ID
    * @param pack ���յĿ��ư�
    */
    void handleSearchServer(Packet* pack);
    /**
    * ���ܣ�����TClient_activeServer����,����ָ���Ĵ�������״̬��Ŀ�����
    * @param block ȥ��SearchServer��־������
    * @param pack ���յĿ��ư�
    */
    void handleActiveServer(Packet* pack);
    /**
    * ���ܣ�����TClient_inActiveServer����,����ָ���Ĵ��ڼ���״̬��Ŀ�����
    * @param block ȥ��ActiveServer��־������
    * @param pack ���յĿ��ư�
    */
    void handleInactiveServer(Packet* pack) ;
    /**
    * ���ܣ�����TClient_getReport����,���߿�ѡ�õ������߳������ڸ÷������õ�TS�Զ��ϱ�����Ϣ
    * @param pack ���յĿ��ư�,ȥ��RegisterServer��־������
    * @param offset ����ƫ��λ��
    */
    void handleRegisterServer(Packet* pack) ;
    /**
    * ���ܣ�����TClient_unregister����,ע���ỰID,�ر�SOCKET
    * @param pack ���յĿ��ư�
    * @param offset ����ƫ��λ��
    */
    void handleUnregisterServer(Packet* pack) ;
    /**
    * ���ܣ�����TClient_modifyServer����,�޸�һ���Ѿ�������TS�е�Ŀ���������
    * @param block ȥ��modifyServer��־������
    * @param pack ���յĿ��ư�
    */
    void handleModifyServer(Packet* pack) ;
    /**
    * ���ܣ�����TClient_setRegisterType����,����sa������,�������ϱ�
    * @param pack ���յĿ��ư�
    * @param offset ����ƫ��λ��
    */
    void handleSetRegisterType(Packet* pack) ;

    /**
    * ���ܣ�����TClient_clearRegisterType����,���sa��ע������,�������ϱ�
    * @param pack ���յĿ��ư�
    * @param offset ����ƫ��λ��
    */
    void handleClearRegisterType(Packet* pack) ;

    /**
     * @Funcname:  handleQueryHostSerialPort
     * @brief        :  �����ѯTS����������������
     * @para1      :  pack ���յĿ��ư�
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��1��10�� 11:57:38
     *
    **/
    void handleQueryHostSerialPort(Packet* pack);

    /**
     * @Funcname:  handleSetSystemLogLevel
     * @brief        :  ��������ϵͳ��־����ļ���
     * @para1      : pack ���յĿ��ư�
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��1��10�� 11:58:42
     *
    **/
    void handleSetSystemLogLevel(Packet* pack);

    /**
     * @Funcname:  handleSetAgentLogFlag
     * @brief        :  �������÷���������־���
     * @para1      : pack ���յĿ��ư�
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��1��10�� 11:59:41
     *
    **/
    void handleSetAgentLogFlag(Packet* pack);

    /**
     * @Funcname:  handleCloseClient
     * @brief        :  ����ͻ��˹ر����ӵ�����,��Ҫ���ϵͳ����֮��ص�����
     * @para1      : pack ���յĿ��ư�
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��1��11�� 11:27:49
     *
    **/
    void handleCloseClient(Packet* pack);

    /**
     * @Funcname:  handleUpLoadFile
     * @brief        :  ����TSAPI�ϴ��ļ�
     * @para1      : pack�����
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��3��2�� 18:00:06
     *
    **/
    void handleUpLoadFile(Packet* pack);

    /**
     * @Funcname:  handleDownLoadFile
     * @brief        :  ����TSAPI�����ļ�
     * @para1      : pack�����
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��3��2�� 18:00:06
     *
    **/
    void handleDownLoadFile(Packet* pack);

    /**
     * @Funcname:  handleDeleteFile
     * @brief        :  ����ɾ���ļ�����
     * @para1      : pack�����
     * @return      :
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��3��12�� 11:31:31
     *
    **/
    void handleDeleteFile(Packet* pack);

    /**
     * @Funcname:  queryHostSerialPort
     * @brief        : ��ѯTS����������������
     * @para1      :  pBuf ���ղ�ѯ���ݵĻ���
     * @para2      :  bufsize: ������ֽڴ�С
     * @return      :  ��ѯ�ɹ��򷵻��ֽڳ��ȣ����򷵻�-1
     * @Author     :  qingxiaohai
     *
     * @History: 1.  Created this function on 2007��1��10�� 11:57:38
     *
    **/
    int queryHostSerialPort(char* pBuf, US bufsize);


    ////////////////////////////���ư�end
    /**
    * ���ܣ�����TClient���͹�����createServer��������,���ѵõ���AID���ظ�TClient
    * @param deviceConfig �豸������Ϣ
    * @param name sa������
    * @param isLog sa�Ƿ���Ҫ��־��¼
    * @param protocolType sa��Ӧ��Э��
    * @param state sa��״̬
    * @return ServerAgent* ����sa����ָ��
    */
    ServerAgent* createSA(DeviceConfigBase* deviceConfig, QString name, bool isLog=0,
                          int protocolType=0, int state=1) ;

public:
    ControlSubAgent(short said=0);

    ~ControlSubAgent()
    {
    }

    /**
    * ���ܣ���ʼ����ǰ�Ķ�������
    * @param SAID ���������
    */
    virtual void init() ;

    /**
    * ���ܣ�������ǰ�����ݰ�,��ִ�в���
    * @param pack ���ݱ�
    */
    virtual void process(Packet* pack);
    /**
    * ���ܣ��ṩ��MANAGERģ������¼���ư���־
    *
    */
    void writePackForManager(PackAct act, const Packet &pack)
    {
        packLoger.write(act, pack);
    }
};

/************************�ӿ�����******************************/
#endif
