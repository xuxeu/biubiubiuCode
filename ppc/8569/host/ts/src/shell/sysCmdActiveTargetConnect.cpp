#include "consoleManager.h"
#include "CheckSubArgv.h"
#include <windows.h>
//#include "TClientAPI.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];

typedef enum _CONNECTTYPE_ENUM
{
	INIT_TYPE =0,
	IPCONNECT_TYPE = 1,
	COMCONNECT_TYPE = 2,
	OTHERS = 3
}CONNECTTYPE_ENUM;

/**
	���浱ǰ��������IP����COM�ڣ��˿ںŻ��߲�����
*/
static string sIpOrCom;
static unsigned int iPortOrBaud;
static CONNECTTYPE_ENUM eConnectType;
static string targetName;
static int defaultNameCount = 0;
#define DEFAULTNAME_STR "SHConnect"
#define DEFAULTRETRY_COUNT 100
#define DEFAULTTIME_OUT 100
/*
	ÿ�δ������ӻ��߼���Ŀ�������ʱ��
	�������ö��ڴ��е��ڴ���ʼ����Ԫ��������Ԫ���ȸ�λ

*/
extern void resetStaicReadMemeryValue();

/*
	ÿ�δ������ӻ��߼���Ŀ�������ʱ��
	��������д�ڴ��е��ڴ���ʼ,��Ԫ���ȸ�λ

*/
extern void setPreWriteValue(unsigned int preAddr =0 ,unsigned int unitWidth =2);
/*

	ÿ�δ������ӻ��߼���Ŀ�������
	�������õ�ǰ���ڵı���
*/
extern void setWindownTitle(const char* titleName);

/*
	��ʼ��Ŀ��������ļ�
*/
static int  initServerConfig(ServerCfg& serCfg){
	
	int ret = 0;
	int testServerNameIsExist = 0;
	//Ĭ�����ü�¼�ռ�
	serCfg.isLog = 1;
	serCfg.channelCount = 1;
	//SAType:Ĭ������ΪTA��ʽ
    strncpy_s(serCfg.extInfo, sizeof(serCfg.extInfo), "0*", _TRUNCATE);
	serCfg.reportType = 1;
	serCfg.currentChannel = 1;
	serCfg.state = 0;
	COMCONFIG* pComConfig = serCfg.pComConfig;
	//���ǵ�ǰ�����Ƿ����
	do {
		defaultNameCount++;
		memset(serCfg.name,0,NAME_SIZE);
		sprintf_s(serCfg.name,NAME_SIZE,"%s%d",DEFAULTNAME_STR,defaultNameCount);
		testServerNameIsExist = TClient_getServerID(tsLinkId, serCfg.name); 
	}while(testServerNameIsExist > 0 );

                
	switch(eConnectType){
	
		case IPCONNECT_TYPE:
			pComConfig->way = UDP;
			pComConfig->protocolType = UDP;
			strcpy_s(pComConfig->NET.ip,IP_BUF_LEN,sIpOrCom.c_str());
			pComConfig->NET.port = iPortOrBaud ? iPortOrBaud :1118;
			pComConfig->NET.retry = DEFAULTRETRY_COUNT;
			pComConfig->NET.timeout = DEFAULTTIME_OUT;
			break;
		
		case COMCONNECT_TYPE:
			pComConfig->way = UART;
			pComConfig->protocolType  = UART;
			strcpy_s(pComConfig->UART.channel,UART_NAME_BUF_LEN,sIpOrCom.c_str());
			pComConfig->UART.baud = iPortOrBaud ? iPortOrBaud : 115200;
			pComConfig->UART.retry = DEFAULTRETRY_COUNT;
			pComConfig->UART.timeout = DEFAULTTIME_OUT;
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
	
}
/*
	����Ŀ������Ӳ�����Ŀ���
*/
static int createTargetConnect(char* cur_path){

	int ret = 0;
	int aid = -1;
	ServerCfg serCfg;
    COMCONFIG comCfg;
     
    memset(&serCfg, 0, sizeof(ServerCfg));
    memset(&comCfg, 0, sizeof(COMCONFIG));
    serCfg.pComConfig = &comCfg;

	if(initServerConfig(serCfg) != 0)
	{
		std::cout << "��ʼ�����������ļ�ʧ��" << std::endl;
		return ret;
	}


    //����serverAgent
    aid = TClient_createServer(tsLinkId, &serCfg);
    if(aid < 0)
    {
        printf("\n����Ŀ�������ʧ��\n");
        return aid;
    }
    //����serverAgent
    ret = TClient_activeServer(tsLinkId, aid);
    if(ret < 0)
    {
        printf("\n����Ŀ�������ʧ��\n");

        //ɾ��SA
        TClient_deleteServer(tsLinkId, aid);
        return ret;
    }


    /* ����Ϊ��ǰĿ���*/
    curTargetAID = aid;
    strcpy(curTargetName ,serCfg.name);
	char fullSymbolFilesFolder[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFilesFolder, cur_path, MAX_PATH_LEN);
    strcat(fullSymbolFilesFolder, "symbolFiles\\");
	strcat(fullSymbolFilesFolder, serCfg.name);
    strcat(fullSymbolFilesFolder, "\\");
	
    _mkdir(fullSymbolFilesFolder); 
	printf("��Ŀ����ɹ�\n");
	return 0;
}


static int activeTargetConnect(char* cur_path,char* pTargetName = NULL,bool isDefaultTargetConnect = false){
	int ret = 0;
    int idtmp = 0;

    char fullSymbolFilesFolder[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFilesFolder, cur_path, MAX_PATH_LEN);
    strcat(fullSymbolFilesFolder, "symbolFiles\\");

    /* ��ѯ��Ӧ���Ƶ�Ŀ���id */ 
	if(isDefaultTargetConnect && pTargetName != NULL){
		idtmp = TClient_getServerID(tsLinkId, pTargetName) ;
	}else{
		idtmp = TClient_getServerID(tsLinkId, const_cast<char*> (targetName.c_str())) ;
	}

    if( idtmp == TS_SERVICE_NOT_EXIST)
    {
        printf("������Ŀ���%s\n", targetName.c_str());
        return FAIL;
	}else if(idtmp < 0){
		printf("TS���ӶϿ�\n");
		return FAIL;
	
	}
	
    /* ����Ŀ������� */
    ret = TClient_activeServer(tsLinkId, idtmp);
    if( idtmp < 0)
    {
        printf("��Ŀ���%sʧ��\n", targetName.c_str());
        return FAIL;
    }

    /* ����Ϊ��ǰĿ���*/
    curTargetAID = idtmp; 
	if(isDefaultTargetConnect && pTargetName != NULL){
	
		strcpy(curTargetName ,pTargetName);

		printf("��Ŀ����ɹ�\n",pTargetName);

		/*��sysmbolFiles�ļ������½���ΪcurTargetName���ļ���*/
		strcat(fullSymbolFilesFolder, pTargetName);
	
	}else{
		 
		strcpy(curTargetName ,targetName.c_str());

		printf("��Ŀ����ɹ�\n",targetName.c_str());

		/*��sysmbolFiles�ļ������½���ΪcurTargetName���ļ���*/
		strcat(fullSymbolFilesFolder, targetName.c_str());
	}
   
    
	strcat(fullSymbolFilesFolder, "\\");
    _mkdir(fullSymbolFilesFolder); 

    return SUCC;
}
/*
	��Ŀ�������,����󶨳ɹ������õ�ǰ��������
*/
static int bindTargetConnect(char* cur_path,char* pTargetName = NULL ,bool isDefaultTargetConnect = false){
	
	int ret = 0;

	switch(eConnectType){
		case IPCONNECT_TYPE:
		case COMCONNECT_TYPE:
			ret = createTargetConnect(cur_path);
			break;
		case OTHERS:
			ret = activeTargetConnect(cur_path);
			break;
		default:
			if(isDefaultTargetConnect){
				targetName = pTargetName;
				eConnectType = OTHERS;
				ret = activeTargetConnect(cur_path,pTargetName,isDefaultTargetConnect);
			}
			break;
	}

	//���ô������ƣ��Զ����ö�д�ڴ��е�ȫ�־�̬����
	if( SUCC == ret ){
		resetStaicReadMemeryValue();
		setPreWriteValue();
		if(eConnectType == IPCONNECT_TYPE){
			setWindownTitle(sIpOrCom.c_str());
		}else if(eConnectType == COMCONNECT_TYPE){
			setWindownTitle(sIpOrCom.c_str());
		}else if(eConnectType == OTHERS){
			setWindownTitle(targetName.c_str());
		}else{
			setWindownTitle("");
		}
	}

	return ret;
}

/*
	���ܣ�������ߴ����µ�Ŀ�������
	pTargetName:����Ŀ�������
	isDefaultTargetConnect:�Ƿ񼤻�Ĭ��Ŀ�������,��Ҫ��hostshell��������ʱ�򣬻ἤ��һ��Ĭ�ϵ�target����
*/
int consoleManager::m_activeTargetConnect(char* pTargetName,bool isDefaultTargetConnect)
{
	
    return bindTargetConnect(cur_path,pTargetName,isDefaultTargetConnect);

}

static bool checkIpIsValid(string ipStr){
	
	bool ret = true;
	string::size_type prePos = 0, findPos = 0;
	int loopCount = 4;
	while( loopCount >= 1 && ret == true) {
		string ipField;
		unsigned int ipInt = 0;
		findPos = ipStr.find(".",prePos);
		const char* pIpFieldStr = NULL;
		
		if(findPos == string::npos){
			ipField = ipStr.substr(prePos);
		}else{
			ipField = ipStr.substr(prePos,findPos-prePos);
		}
		
		pIpFieldStr = ipField.c_str();
		
		while(*pIpFieldStr){
			if(*pIpFieldStr > '9' || *pIpFieldStr < '0'){
				std::cout <<  "IP��ַ��Ч" << std::endl;
				ret = false;
				break;
			}
			pIpFieldStr++;	
		}
		
		if(!ret){
			break;
		}
		
		ipInt = atoi(ipField.c_str());
		
		if(ipInt >= 0 && ipInt <= 255){
			if((loopCount == 4 || loopCount == 1) && ipInt == 0){
				std::cout << "IP��ַ��Ч" << std::endl;
				ret = false;
			}
		}else{
			std::cout <<  "IP��ַ��Ч" << std::endl;	
			ret = false;
		}
		prePos = findPos+1;
		loopCount--;
	}
	if(ret){
		sIpOrCom = ipStr;
	}

	return ret;

}
/*
	��鵱ǰ�ַ�����IP��COM��Ŀ�������
	�����ҵ���ǰ���ӷ�ʽΪCOMģʽ���ҵ�ǰCOM�ַ�������Ϊ��0~9���ַ����������ʾ
	�����ҵ���ǰ���ַ�����IP��ʽ���̶�ģʽΪxx.xx.xx.xx,����IP�ַ����е����ֵ���Ч���ʣ���IP�ַ������ַ���Ч����ʾ������Ϣ
	������Ϊ��Ŀ������ơ����ǵ�Ŀ������ƿ��ܻ���xxx.xx .xxx .xx.xxx.xx
*/
static bool checkIpOrComIsValid(string ipOrCom,CONNECTTYPE_ENUM& connectType)
{
	bool ret = true;
	connectType = OTHERS;
	string::size_type findIpPos = string::npos,findCom = string::npos;
	findCom = ipOrCom.find("COM");
	int findIpCount = 0;
	
	//��ǰ����COMΪ��һ���ַ�
	if(findCom != string::npos && findCom == 0){

		string comNo = ipOrCom.substr(strlen("COM"));

		if(comNo.size() > 0 ){
			
			const char* pComNoStr = comNo.c_str();
			unsigned int comNumber = 0;
			while(*pComNoStr){
				
				if(*pComNoStr < '0' || *pComNoStr > '9'){
					ret = false;
					break;
				}
				comNumber = comNumber*10 + *pComNoStr - '0';
				pComNoStr++;
			}
			if(ret){
				connectType = COMCONNECT_TYPE;
				sIpOrCom = ipOrCom;
				//�����������Ϊ0~256
				if( comNumber > 256 ){
					ret = false;
					std::cout <<  "���ڶ˿ں���Ч" << std::endl;
				}
			}else if(comNo.size() > 6){
				std::cout <<  "���ڶ˿ں���Ч" << std::endl;
			}else{
				std::cout <<  "���ڶ˿ں���Ч" << std::endl;
			}
	
		}else{
			
			std::cout << "���ڶ˿ں���Ч" << std::endl;
			ret = false;
		}//end of check Com
	//���IP������Ŀ�����������,�������IP����ģʽ����ΪĿ�������	
	}else{
		
		findIpPos = 0;
		
		while(( findIpPos = ipOrCom.find_first_of(".",findIpPos)) && (findIpPos != 0) && (findIpPos != string::npos))
		{
			if(ipOrCom.size() > findIpPos){
				findIpPos++;
			}
			findIpCount++;
		}

		//��ǰ�ַ�������IP��ַģʽ
		if( findIpCount != 3){

		//��ǰ�ַ�����IP��ַģʽ
		}else{
			connectType = IPCONNECT_TYPE;
			ret = checkIpIsValid(ipOrCom);	
		
		}
	}

	return ret;
}
#define MAX_PORT 65535
#define MAX_PORT_STR "65535"
static bool checkIpPort(string port){

	bool ret = true;
	string::size_type length = port.size();
	if( length > 0 && length <= strlen(MAX_PORT_STR) ){
		
		const char* pPort =  port.c_str();
		
		while(*pPort){
			if(*pPort < '0' || *pPort > '9'){
				ret = false;
				break;
			}
			pPort++;
		}
		if(ret){
			unsigned int portInt = atoi( port.c_str());
			if(portInt <= 0 || portInt > MAX_PORT){
				//std::cout << "�˿ں���Ч" << std::endl;
				ret = false;
			}else{
				iPortOrBaud = portInt;
			}
		}

		if(!ret){
			std::cout << "�˿ں���Ч" << std::endl;
		}
	}else{
		std::cout  << "�˿ں���Ч" << std::endl;
		ret = false;
	}
	
	return ret;
}

static bool checkComBaud(string baud){

	bool ret = true;
	
	if( baud == "110" || \
		baud == "300" || \
		baud == "1200" || \
		baud == "2400" || \
		baud == "4800" || \
		baud == "9600" || \
		baud == "19200" || \
		baud == "38400" || \
		baud == "57600" || \
		baud == "115200" || \
		baud == "230400" || \
		baud == "460800" || \
		baud == "921600" 
	){
		iPortOrBaud = atoi(baud.c_str());
	}else{
		std::cout <<  "��������Ч" << std::endl;
		ret = false;
	}	
	return ret;
}
bool sysCmdAtaCheckSubArgv(string&  cmd){

	
	/* ���ھ�� */
	HANDLE sUart ; 

	bool ret = true;
	eConnectType = INIT_TYPE;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();

	//���첽�ķ�ʽ�򿪴���
	sUart = CreateFile( paraterList[0].c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL );
	
	if(length == 0 )
	{
		std::cout << "�����ʽ����"<< std::endl;
		ret  = false;
	}
	else if( length >= 3)
	{
		std::cout << "�����ʽ����"<< std::endl;
		ret  = false;
	}
	else
	{
		//��鵱ǰ���ӽӷ�ʽ
		//IP COM Ŀ�������
		string ipOrCom;
		CONNECTTYPE_ENUM connectType = OTHERS;

		if(checkIpOrComIsValid(paraterList[0],connectType) == true)
		{
			eConnectType = connectType;
			
			if(connectType == IPCONNECT_TYPE)
			{
				if(length == 2 && checkIpPort(paraterList[1]) != true)
				{
					ret = false;
				}
			}
			else if(connectType == COMCONNECT_TYPE)
			{
				if(length == 2 && checkComBaud(paraterList[1]) != true)
				{
					ret = false;
				}
				
				if (sUart == INVALID_HANDLE_VALUE)
				{
					std::cout << paraterList[0] << ":�豸������" << std::endl;
					ret = false;
				}
				else
				{
					CloseHandle(sUart);
				}
			}
			else
			{
				if(length != 1)
				{
					std::cout << cmd << ":�����ʽ����" << std::endl;
					ret = false;
				}
				else
				{
					eConnectType = OTHERS;
					targetName = paraterList[0];
				}
			}
		}
		else
		{
			ret = false;
		}
	}

	return ret;

}