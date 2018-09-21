#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
/* ��ѯ��ǰTS�е�Ŀ���������Ϣ */
int consoleManager::m_listTargetInfo()
{
    int saCount = 0;
    int aidArray[256];
    ServerCfg serCfg;
    COMCONFIG comCfg1;
    COMCONFIG comCfg2;
    int curSaNmu = 0;
    int ret = 0;
    char taState[10];

    memset(aidArray, 0, sizeof(aidArray));

    //��ȡ����SA��AID
    saCount = getAllServerAid(aidArray, sizeof(aidArray));   
	if(saCount == -1){
		printf("��ѯ����Ŀ�������ʧ��\n");
		return FAIL;
	}

    //��ȡÿ��SA������
    while( (curSaNmu < saCount) && (curSaNmu < sizeof(aidArray) - 1) && (0 != aidArray[curSaNmu]))
    {   
        memset(&serCfg, 0 ,sizeof(ServerCfg));
        memset(&comCfg1, 0 ,sizeof(COMCONFIG));
        memset(&comCfg2, 0 ,sizeof(COMCONFIG));
        serCfg.pComConfig = &comCfg1;
        comCfg1.next = &comCfg2;
        serCfg.aid = aidArray[curSaNmu++];

        //��ȡSA����
        ret = TClient_queryServer(tsLinkId, &serCfg) ;
        if(TS_SUCCESS != ret)
        {
            printf("��ȡĿ�������ʧ��, aid:%d\n", aidArray[curSaNmu - 1]);
            continue;
        }

        //�ж����ӷ�ʽ
        if(DLL == serCfg.pComConfig->way)
        {
            continue;
        }
        else if(serCfg.extInfo[0]=='1')
        {
            continue;
        }

        //��ȡ״̬
        memset(taState, 0, sizeof(taState));
        switch(serCfg.state)
        {
        case SASTATE_INACTIVE:
            strncpy_s(taState, sizeof(taState), "Inactive", _TRUNCATE);
            break;

        case SASTATE_ACTIVE:
            strncpy_s(taState, sizeof(taState), "Active", _TRUNCATE);
            break;

        case SASTATE_DELETED:
            strncpy_s(taState, sizeof(taState), "Delete", _TRUNCATE);
            break;

        default:
            strncpy_s(taState, sizeof(taState), "Unknow", _TRUNCATE);
        }

        //��ӡĿ�����Ϣ
        printf("%2d:%15s %2d %8s ",  
                    curSaNmu,
                    serCfg.name,
                    serCfg.aid,
                    taState);
        //��1ͨ������
        switch(serCfg.pComConfig->way)
        {
        case UDP:
            printf("UDP %16s %6d %4d %4d",
                        serCfg.pComConfig->NET.ip,
                        serCfg.pComConfig->NET.port,
                        serCfg.pComConfig->NET.timeout,
                        serCfg.pComConfig->NET.retry);
            break;

        case UART:
            printf("UART%16s %6d %4d %4d",
                        serCfg.pComConfig->UART.channel,
                        serCfg.pComConfig->UART.baud,
                        serCfg.pComConfig->UART.timeout,
                        serCfg.pComConfig->UART.retry);
            break;

        case USB:
            printf("USB  %s  ", serCfg.pComConfig->USB.GUID);
            break;

        case DLL:
            break;
    
        default:
            printf("��1ͨ��:δ֪ͨ������");
        }

        
        /* ��ǰĿ������� */
        if(curTargetAID == serCfg.aid)
        {
            printf(" *Using");
        }

        printf("\n");

    }

    if(0 == curSaNmu)
    {
        printf("��ǰû�д����κ�Ŀ���\n");
    }
    
    return SUCC;
    
}


bool sysCmdListCheckSubArgv(string& cmd){

	bool ret = true;
	if(cmd != "list"){
		
		std::cout << "�����ʽ����" << std::endl;
		
		ret = false;
	
	}
	return ret;

}