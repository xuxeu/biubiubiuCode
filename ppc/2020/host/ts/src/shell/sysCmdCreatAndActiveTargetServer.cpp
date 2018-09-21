#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName [] ;
/* ����������Ŀ�������*/
int consoleManager::m_creatAndActiveTargetServer()
{
   
    int aid = -1;
    int ret = 0;
    ServerCfg serCfg;
    COMCONFIG com1Cfg;
        
    memset(&serCfg, 0, sizeof(ServerCfg));
    memset(&com1Cfg, 0, sizeof(COMCONFIG));
    serCfg.pComConfig = &com1Cfg;
    com1Cfg.next = NULL;
    
    /* ��ʾ�������*/
    if(inputSerCfg(&serCfg) != SUCC)
    {
        return FAIL;
    }

    //����serverAgent
    aid = TClient_createServer(tsLinkId, &serCfg);
    if(aid < 0)
    {
        printf("\n����Ŀ�������ʧ��!\n");
        return aid;
    }

    //����serverAgent
    ret = TClient_activeServer(tsLinkId, aid);
    if(ret < 0)
    {
        printf("\n����Ŀ�������ʧ��!\n");

        //ɾ��SA
        TClient_deleteServer(tsLinkId, aid);
        return ret;
    }

    printf("\n����Ŀ������ӳɹ�!\n");

    /* ����Ϊ��ǰĿ���*/
    curTargetAID = aid;
    strcpy(curTargetName ,serCfg.name);

    return aid;
}


bool sysCmdCtaCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "cta"){
		std::cout <<"cta:�����ʽ����,��ǰ�����޲���������������"<<std::endl;
		ret = false;
	}

	return ret;

}