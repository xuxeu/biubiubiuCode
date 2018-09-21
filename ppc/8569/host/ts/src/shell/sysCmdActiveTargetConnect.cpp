#include "consoleManager.h"
#include "CheckSubArgv.h"
#include <windows.h>
//#include "TClientAPI.h"
/* 当前目标机连接的AID */
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
	保存当前解析出的IP或者COM口，端口号或者波特率
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
	每次创建连接或者激活目标机连接时候
	重新设置读内存中的内存起始，单元数量，单元长度复位

*/
extern void resetStaicReadMemeryValue();

/*
	每次创建连接或者激活目标机连接时候
	重新设置写内存中的内存起始,单元长度复位

*/
extern void setPreWriteValue(unsigned int preAddr =0 ,unsigned int unitWidth =2);
/*

	每次创建连接或者激活目标机连接
	重新设置当前窗口的标题
*/
extern void setWindownTitle(const char* titleName);

/*
	初始化目标机配置文件
*/
static int  initServerConfig(ServerCfg& serCfg){
	
	int ret = 0;
	int testServerNameIsExist = 0;
	//默认设置记录日记
	serCfg.isLog = 1;
	serCfg.channelCount = 1;
	//SAType:默认设置为TA方式
    strncpy_s(serCfg.extInfo, sizeof(serCfg.extInfo), "0*", _TRUNCATE);
	serCfg.reportType = 1;
	serCfg.currentChannel = 1;
	serCfg.state = 0;
	COMCONFIG* pComConfig = serCfg.pComConfig;
	//测是当前名称是否存在
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
	创建目标机连接并激活目标机
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
		std::cout << "初始服务器配置文件失败" << std::endl;
		return ret;
	}


    //创建serverAgent
    aid = TClient_createServer(tsLinkId, &serCfg);
    if(aid < 0)
    {
        printf("\n创建目标机连接失败\n");
        return aid;
    }
    //激活serverAgent
    ret = TClient_activeServer(tsLinkId, aid);
    if(ret < 0)
    {
        printf("\n创建目标机连接失败\n");

        //删除SA
        TClient_deleteServer(tsLinkId, aid);
        return ret;
    }


    /* 设置为当前目标机*/
    curTargetAID = aid;
    strcpy(curTargetName ,serCfg.name);
	char fullSymbolFilesFolder[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFilesFolder, cur_path, MAX_PATH_LEN);
    strcat(fullSymbolFilesFolder, "symbolFiles\\");
	strcat(fullSymbolFilesFolder, serCfg.name);
    strcat(fullSymbolFilesFolder, "\\");
	
    _mkdir(fullSymbolFilesFolder); 
	printf("绑定目标机成功\n");
	return 0;
}


static int activeTargetConnect(char* cur_path,char* pTargetName = NULL,bool isDefaultTargetConnect = false){
	int ret = 0;
    int idtmp = 0;

    char fullSymbolFilesFolder[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFilesFolder, cur_path, MAX_PATH_LEN);
    strcat(fullSymbolFilesFolder, "symbolFiles\\");

    /* 查询对应名称的目标机id */ 
	if(isDefaultTargetConnect && pTargetName != NULL){
		idtmp = TClient_getServerID(tsLinkId, pTargetName) ;
	}else{
		idtmp = TClient_getServerID(tsLinkId, const_cast<char*> (targetName.c_str())) ;
	}

    if( idtmp == TS_SERVICE_NOT_EXIST)
    {
        printf("不存在目标机%s\n", targetName.c_str());
        return FAIL;
	}else if(idtmp < 0){
		printf("TS连接断开\n");
		return FAIL;
	
	}
	
    /* 激活目标机连接 */
    ret = TClient_activeServer(tsLinkId, idtmp);
    if( idtmp < 0)
    {
        printf("绑定目标机%s失败\n", targetName.c_str());
        return FAIL;
    }

    /* 设置为当前目标机*/
    curTargetAID = idtmp; 
	if(isDefaultTargetConnect && pTargetName != NULL){
	
		strcpy(curTargetName ,pTargetName);

		printf("绑定目标机成功\n",pTargetName);

		/*在sysmbolFiles文件夹下新建名为curTargetName的文件夹*/
		strcat(fullSymbolFilesFolder, pTargetName);
	
	}else{
		 
		strcpy(curTargetName ,targetName.c_str());

		printf("绑定目标机成功\n",targetName.c_str());

		/*在sysmbolFiles文件夹下新建名为curTargetName的文件夹*/
		strcat(fullSymbolFilesFolder, targetName.c_str());
	}
   
    
	strcat(fullSymbolFilesFolder, "\\");
    _mkdir(fullSymbolFilesFolder); 

    return SUCC;
}
/*
	绑定目标机连接,如果绑定成功则设置当前窗口名称
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

	//设置窗口名称，自动设置读写内存中的全局静态函数
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
	功能：激活或者创建新的目标机链接
	pTargetName:激活目标机名称
	isDefaultTargetConnect:是否激活默认目标机链接,主要是hostshell在启动的时候，会激活一个默认的target连接
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
				std::cout <<  "IP地址无效" << std::endl;
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
				std::cout << "IP地址无效" << std::endl;
				ret = false;
			}
		}else{
			std::cout <<  "IP地址无效" << std::endl;	
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
	检查当前字符串是IP、COM、目标机名称
	若查找到当前连接方式为COM模式，且当前COM字符串后面为非0~9的字符，则错误提示
	若查找到当前的字符串是IP方式，固定模式为xx.xx.xx.xx,则检查IP字符串中的数字的有效性质，若IP字符串中字符无效则提示错误信息
	否则认为是目标机名称。考虑到目标机名称可能会有xxx.xx .xxx .xx.xxx.xx
*/
static bool checkIpOrComIsValid(string ipOrCom,CONNECTTYPE_ENUM& connectType)
{
	bool ret = true;
	connectType = OTHERS;
	string::size_type findIpPos = string::npos,findCom = string::npos;
	findCom = ipOrCom.find("COM");
	int findIpCount = 0;
	
	//当前查找COM为第一个字符
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
				//串口最大数量为0~256
				if( comNumber > 256 ){
					ret = false;
					std::cout <<  "串口端口号无效" << std::endl;
				}
			}else if(comNo.size() > 6){
				std::cout <<  "串口端口号无效" << std::endl;
			}else{
				std::cout <<  "串口端口号无效" << std::endl;
			}
	
		}else{
			
			std::cout << "串口端口号无效" << std::endl;
			ret = false;
		}//end of check Com
	//检查IP或者是目标机名称名称,如果不是IP，则模式设置为目标机名称	
	}else{
		
		findIpPos = 0;
		
		while(( findIpPos = ipOrCom.find_first_of(".",findIpPos)) && (findIpPos != 0) && (findIpPos != string::npos))
		{
			if(ipOrCom.size() > findIpPos){
				findIpPos++;
			}
			findIpCount++;
		}

		//当前字符串不是IP地址模式
		if( findIpCount != 3){

		//当前字符串是IP地址模式
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
				//std::cout << "端口号无效" << std::endl;
				ret = false;
			}else{
				iPortOrBaud = portInt;
			}
		}

		if(!ret){
			std::cout << "端口号无效" << std::endl;
		}
	}else{
		std::cout  << "端口号无效" << std::endl;
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
		std::cout <<  "波特率无效" << std::endl;
		ret = false;
	}	
	return ret;
}
bool sysCmdAtaCheckSubArgv(string&  cmd){

	
	/* 串口句柄 */
	HANDLE sUart ; 

	bool ret = true;
	eConnectType = INIT_TYPE;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();

	//以异步的方式打开串口
	sUart = CreateFile( paraterList[0].c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL );
	
	if(length == 0 )
	{
		std::cout << "命令格式错误"<< std::endl;
		ret  = false;
	}
	else if( length >= 3)
	{
		std::cout << "命令格式错误"<< std::endl;
		ret  = false;
	}
	else
	{
		//检查当前连接接方式
		//IP COM 目标机名称
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
					std::cout << paraterList[0] << ":设备不可用" << std::endl;
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
					std::cout << cmd << ":命令格式错误" << std::endl;
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