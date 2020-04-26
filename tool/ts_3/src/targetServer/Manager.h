/*****************************************************************
 *    This source code has been made available to you by CoreTek on an AS-IS
 *    basis. Anyone receiving this source is licensed under CoreTek
 *    copyrights to use it in any way he or she deems fit, including
 *    copying it, modifying it, compiling it, and redistributing it either
 *    with or without modifications.
 *
 *    Any person who transfers this source code or any derivative work
 *    must include the CoreTek copyright notice, this paragraph, and the
 *    preceding two paragraphs in the transferred software.
 *
 *    COPYRIGHT   CoreTek  CORPORATION 2004
 *    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
 ***********************************************************************/

/**
 * TS�����߳�
 *
 * @file     Manager.h
 * @brief     
 * @author     ���
 * @date     2004-12-6
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */


#ifndef MANAGER_INCLUDE
#define MANAGER_INCLUDE


#include "tsInclude.h"
#include "Agent.h"
#include "config.h"
//#include "Packet.h"
//#include "log.h"

class Packet;
class AgentWithQueue;
class Listen;
////////������Ϣ���ļ���Ϊ 
//���߳�Ҫ��һ��ѭ��һֱ�������ݰ��ĵ���
class SubAgentBase;
typedef QList<AgentWithQueue*> AGENTLIST;
typedef QMap<short,QList<int>*> SHORT2LIST;
typedef QList<int> INTLIST;
//TS������������������
#define MAX_AGENT_NUM 256
//��ȡ��ǰ�����ϵ�DID
#define CURDID(a) (Manager::getInstance()->getCurDID(a))

//MANAGERģ���ID��
#define MANAGER_AID     0

class Manager : public AgentWithQueue
{
private:
    QString appDir;            //��ǰ��������Ŀ¼
    int listenPort;            //�����˿ں�
   
    bool runFlag;    
    Listen* pListen;
    static Manager* pManager;
    static TMutex mutexInstance;            //ȷ����ʼ����������

    /****************ӳ���******************/
    TMutex mtxMapSAID2SubAgent;             //mapSAID2SubAgent�Ļ�����
    QMap<short,SubAgentBase*> mapSAID2SubAgent;    //��������ŵ�������������ӳ��
    //������Ϣ����
    TMutex mtxMapAID2SA;            //��mapAID2SA���л������    
    QMap<short,AgentBase*> mapAID2SA;    //����aid��ServerAgent����

    //���ֵ�ID��
    TMutex mtxMapName2ID;           //mapName2ID�Ļ�����
    QMap<QString ,short> mapName2ID;       

    //�ļ����͹���ʱ���ڱ��洫���ڼ���ļ����;��
    QMap<string, FILE*> mapName2File;
    
    //SA_AID��ע�������б�
    //TMutex mtxMapAID2TypeList;
    //SHORT2LIST mapAID2TypeList;
    //ע�����͵�CA�����б�
    TMutex mtxAgentArray;
    QList<AgentBase*>* agentArray[MAX_REG_TYPE + 2]; 

    TMutex mtxFreeIDList;
    QList<US> freeIDList;              //�������ID ������
    //������,ͳ�Ƶ�ǰTS���ж��ٸ�SA/CA����
    static unsigned short counter ;    

    //CA���ӵ��Ի�ID��ӳ��
    map<US, US> mapLnk2DID;
    TMutex mtxMapDID;
    /**
    *���ܣ���ʼ������ID����
    */
    void initFreeIDList();
    
    /**
    * ���ܣ�ɾ������server
    */
    void deleteAllServer() ;
    /**
    * ���ܣ������µ�sa��״̬
    */
    void setNewState(Packet *pack, int regCount,char** pBuf,int putSize) ;
    bool mfCreateDirectory( QString &strDirectory );

    bool createDirectory( QString &strDirectory );
public:
    //TS�����ļ�����ʱ·��
    static const string tmpfilepath;
    
    /**
    * ���ܣ��õ������˿ں�
    */
    QString getAppDir() 
    {
        return appDir;
    }
    /**
    * ���ܣ��õ������˿ں�
    */
    int getListenPort() 
    {
        return listenPort;
    }
    /**
    * ���ܣ����ü����˿ں�
    */
    void setListenPort(int port) 
    {
        listenPort = port;
    }
    /**
     * @Funcname:  sendSAReportToCA
     * @brief        :  ����SA���ϱ���Ϣ��ָ����CA
     * @para1      : AgentWithQueue * pSA ���ϱ���Ϣ��SA����
     * @para2      : caAid�� �ϱ���ϢĿ�ĵأ�0��ʾ���е�CA������
     * @return      : 
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2007��1��8�� 11:26:18
     *   
    **/
    void sendSAReportToCA(AgentWithQueue * pSA, US caAid = 0);
    /**
    *���ܣ�����ĳ������SA���ϱ�
    * regType: ��Ҫ�ϱ���ע�����ͺ�
    * caAid�� �ϱ���ϢĿ�ĵأ�0��ʾ���е�CA������
    */
    void sendRegTypeSAToCA(int regType, US caAid = 0);

    /**
    * ����: ͨ��AID�ŵõ�serverAgent����
    * @param aid aid��
    * return ServerAgent* NULL��ʾû�д�aid��,
    */
    AgentBase* getServerAgentFromID(short aid);

    /**
    * ���ܣ�ɾ��ָ��server
    * @param aid serverAgent��aid��
    */
    int deleteServer(int aid);
    /**
    * ���ܣ��������ֵ�SA��AID��ӳ��
    * @param aid serverAgent��aid��
    */
    QMap<QString ,short>& getName2AIDMap()
    {
        return mapName2ID;
    }
    /**
    * ���ܣ��޸�ӳ��mapAID2SA
    * @param oldName Ҫ�滻��ԭ������
    * @param newName �滻�������
    */
    void modifyName2AIDMap(QString oldName,QString newName) ;
    /**
    * ���ܣ��Ѷ����������
    * @param pSubAgent ��SubAgent�������map����mapSAID2SubAgent��
    */
    void addSubAgent(SubAgentBase* pSubAgent) ;
    /**
    * ���ܣ���serverAgent����
    * @param pServerAgent ��sa�������map����mapAID2SA��
    */
    void addServerAgent(AgentBase* pServerAgent) ;
    /**
    * �õ�Ψһʵ��.
    * @return Manager* Ψһʵ��
    */
    static Manager* getInstance() 
    {
        if (pManager == NULL) 
        {
            TMutexLocker locker(&mutexInstance);
            if (pManager == NULL)
                pManager = new Manager();
        }    
        return pManager;
    }
    Manager(unsigned short aid = MANAGER_AID);
    ~Manager();
    /**
    * ���ܣ���������߳�,��ں���
    */
    void main();
    /**
    * ���ܣ����������ļ��ɹ����������������ӦServerAgent�ͼ����߳�ListenerThread
    * @return
    *
    */
    void loadConfigure() ;
    /**
    * ���ܣ���������Ϣ���浽�ļ���ȥ
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    bool storeConfigure();
    /**
    * ����: ͨ�����ֵõ���Ӧ��ID��
    * @param name ����
    * return ID ��-1��ʾû��
    */
    int getIDFromName(const QString& name);
    /**
    * ���ܣ�����ID��
    * @return ID��
    */
    unsigned short createID();
    /**
    * ���ܣ��ͷ�ָ����AID��������ӵ�����AID����
    * @return 
    */
    void freeAid(US aid);
    /**
    * ���ܣ��˳�manager
    */
    void stop();
    /**
    * ���ܣ�����TClient_register����,TS����ע��,ע���,TS����TC�Զ�������Ϣ��״̬
    * @param pack ���յĿ��ư�
    * @return TS_SUCCESS��ʾ�ɹ�������ʧ��
    */
    int registerServer(Packet* pack,int regType);
    /**
    * ���ܣ�����TClient_unregister����,ע���ỰID,�ر�SOCKET
    * @param pack ���յĿ��ư�
    * @return TS_SUCCESS��ʾ�ɹ�������ʧ��
    */
    int unRegisterServer(Packet* pack,int regType) ;

    /**
    * ���ܣ�����TClient_setRegisterType����,����sa��ע������
    * @param aid sa��aid��
    * @param regType Ҫע������ͺ�
    * @param isStore �Ƿ񱣴�,Ĭ��trueΪд�ļ�,falseΪ��д�ļ�
    * @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
    */
    bool setRegisterType(short aid,int regType,bool isStore = true);
    /**
    * ���ܣ�����TClient_unregister����,ע���ỰID,�ر�SOCKET
    * @param aid sa��aid��
    * @param regType Ҫ��������ͺ�
    * @return true��ʾע��ɹ�,false��ʾ��ע���˴�aid
    */
    bool clearRegisterType(short aid) ;

    /**
    * ���ܣ�����TClient_changeChannelType����,����ͨ���л�
    * @param aid sa��aid��
    * @param channelType Ҫ�л���ͨ������
    * @param ackAid �����л�ͨ�������Դaid��
    * @return TS_SUCCESS��ʾ�ɹ�������ʧ��
    */
    int changeChannel(short aid, int channelType, short ackAid) ;

    /**
     * @Funcname:  getCurDID
     * @brief        :  ��ȡ��ǰ���ӵĶԻ�ID
     * @para1      : fd, ָ��������
     * @return      : ����ָ�����ӵĵ�ǰ�Ի���
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2006��12��26�� 15:41:57
     *   
    **/
    int getCurDID(US fd)
    {
        TMutexLocker locker(&mtxMapDID);
        map<US, US>::iterator m_ItrDID = mapLnk2DID.find(fd);
        if(m_ItrDID == mapLnk2DID.end())
        {
            sysLoger(LWARNING, "Main::getCurDID: error linker id %d.", fd);
            return -1;
        }
        return m_ItrDID->second;
    }

        /**
     * @Funcname:  setCurDID
     * @brief        : ���õ�ǰ���ӵĶԻ�ID
     * @para1      : fd, ָ��������
     * @return      : ��
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2006��12��26�� 15:41:57
     *   
    **/
    void setCurDID(US fd, US DID)
    {
        TMutexLocker locker(&mtxMapDID);
        map<US, US>::iterator m_ItrDID = mapLnk2DID.find(fd);
        if(m_ItrDID == mapLnk2DID.end())
        {
            sysLoger(LINFO, "Main::setCurDID: New link id %d.", fd);
            mapLnk2DID.insert(pair<US, US>(fd, DID));
            return;
        }
        m_ItrDID->second = DID;
        return;
    }
    /**
     * @Funcname:  isSACountReachedToMAX
     * @brief        :  �жϵ�ǰ����������Ƿ�ﵽTS��������ֵ
     * @return      : 
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2007��1��8�� 14:38:37
     *   
    **/
    bool isAgentCountReachedToMAX()
    {
        if(counter >= MAX_AGENT_NUM)
        {
            return true;
        }
        return false;
    }

    /**
     * @Funcname:  insertFileToMap
     * @brief        :  ���ļ�����Ӧ���ļ������������mapName2File��
     * @param     : fn,�ļ���;  fp�ļ����
     * @return      : 
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2007��3��2�� 17:38:37
     *   
    **/
    bool insertFileToMap(string fn, FILE *fp);
    
    /**
     * @Funcname:  getFileFromName
     * @brief        :  �����ļ�����mapName2File�л�ȡ�ļ����
     * @param     : fn,�ļ���; ���� fp�ļ����
     * @return      : 
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2007��3��2�� 17:38:37
     *   
    **/
    FILE* getFileFromName(string fn);
        /**
     * @Funcname:  removeAndCloseFileFromMap
     * @brief        :  �����ļ����ӱ�mapName2File���Ƴ��ļ�������ر��ļ�
     * @param     : fn,�ļ���
     * @return      : 
     * @Author     :  qingxiaohai
     *  
     * @History: 1.  Created this function on 2007��3��2�� 17:38:37
     *   
    **/
    bool removeAndCloseFileFromMap(string fn);
        
};
#endif // !defined(MANAGER_INCLUDE)
