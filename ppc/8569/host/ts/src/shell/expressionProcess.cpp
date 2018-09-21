/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  expressionProcess.cpp
* @brief  
*       ���ܣ�
*       <li>���ʽ����ִ����ʵ��</li>
*/

/************************ͷ�ļ�********************************/
#include "expressionProcess.h"

#include <stdio.h>

#include "shellMain.h"

#include "consoleManager.h"

#include "coreCmdProcess.h"

#include "CheckSubArgv.h"
using namespace std;

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/
/* tsServer����� */
int tsServerAid = -1;

static const basic_string <char>::size_type npos = -1;


/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰĿ������ӵĳ�ʱʱ�� */
extern int timeOut;

/* ��ǰ���ӵ�Ŀ�����*/
extern char curTargetName [NAME_SIZE + 1];


extern consoleManager *pConsoleManager;

extern coreCmdProcess *pCoreCmdProcess;


/************************ǰ������******************************/

/************************ʵ��*********************************/

/*
    ��ѯshellAgent�����Ƿ����
*/

static bool checkTargetShellAgentIsIdle(){

    bool ret = true;
    
    if(pCoreCmdProcess != NULL && pCoreCmdProcess->isCheckShellAgentIsIdle()){
    
        ret = pCoreCmdProcess->checkTargetShellAgentIsIdle();
    }
    return ret;

}

expressionProcess::expressionProcess()
{
    runFlag = true;    
}


expressionProcess::~expressionProcess()
{
    runFlag = false;
}


void expressionProcess::stop()
{
    runFlag = false;
    return;
}



/* ���������������Ĳ���ϵͳ������ */
int expressionProcess::insertData(string cmd)
{

    /* ����ȡ����������������� */
    expCmdListMtx.lock();
    expCmdList.push_back(cmd);
    expCmdListMtx.unlock();

    return SUCC;
}



/* �������ݰ���Ŀ���shell���� */
int expressionProcess::sendDataToTsServer(char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    if(tsServerAid <= 0)
    {
        return FAIL;
    }

    /* �������ݰ���Ŀ��� */
    ret = TClient_putpkt(tsLinkId, tsServerAid, 0, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}


/* �������ݰ���Ŀ�����shellCallFun*/
int expressionProcess::sendDataToShellCallFun(char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    if(tsServerAid <= 0)
    {
        return FAIL;
    }

    /* �������ݰ���Ŀ��� */
    ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_SHELLAGENT_CALL_FUN, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}



/* ��ts�հ� */
int expressionProcess::recvDataFromTs(char* pBuf, int size)
{

    int ret_wait = 0,ret_getpkt = 0;
    unsigned short src_aid, des_said, src_said;
    extern bool ctrl_c_flag;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    /*�ȴ����ݵ���,������ctrl+c��ʱ��������*/
    ctrl_c_flag = false;
    while( !ctrl_c_flag  )
    {
        ret_wait = TClient_wait(tsLinkId, 1);
        if( TS_DATACOMING == ret_wait )
        {
            /* �����ݵ��������հ� */
            ret_getpkt = TClient_getpkt(tsLinkId, &src_aid, &des_said, &src_said, pBuf, size);
            if( ret_getpkt < 0 )
            {
                return FAIL;
            }
            return ret_getpkt;
        }
        
        /*����ȴ����ݵ�������*/
        else if( ret_wait != TS_TIME_OUT )
        {
            return FAIL;
        }
    }

    /*ctrl+c��������*/
    return STOP_CMD;

}



/* �������� */
int expressionProcess::processData()
{

    int ret = 0;
    int len = 0;
    int scanfret = 0;
    int nlines = 0;
    string cmdtemp;
    char recvBuf[2*MAX_MSG_SIZE] = {'\0'};
    char expBuf[MAX_MSG_SIZE] = {'\0'};
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    string::size_type noblankpos = 0;


    if (!expCmdList.empty())
    {
        /* �����������ȡ��δ��������� */
        expCmdListMtx.lock();
        cmdtemp = expCmdList.front();
        expCmdList.pop_front();
        expCmdListMtx.unlock();

        /*��ѯĿ���shell�����Ƿ�����*/
        ret = pCoreCmdProcess->queryTargetAgent();
        if(ret != SUCC)
        {
            return FAIL;
        }

        memset(expBuf, 0, sizeof(expBuf));

		if(strncmp(cmdtemp.c_str(), ": ", strlen(": ")) == 0)
        {
            if ( coreCmdSetDomainCheckSubArgv(cmdtemp) == false){
            
                return FAIL;
            }

		//��ѯOS�汾
		if( SUCC != pCoreCmdProcess->queryTargetAgentOsVersion() )
		{
			return FAIL;
		}
	//�����ǰĿ����Ĳ���ϵͳ�汾�뵱ǰ���Ʋ�ƥ�䣬��ֱ�Ӳ����κδ���
		if(pCoreCmdProcess->getOSVersion() != OS_2X_VERSION ){
			cout << "������ֻ֧��ACoreOS653����ϵͳ"<< endl;
			return FAIL;
		}

		if(pCoreCmdProcess->queryTargetAgentOsStauts() != SUCC){
				return FAIL;
		}
            /* ȥ��������setD*/
            cmdtemp.erase( 0, strlen(SET_USING_DOMAIN_STR) );

            /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
            noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
        	if( (string::npos  == noblankpos) || (0  == noblankpos) )
            {
                cout << "����Ĳ�ѯָ�����������ʽ����" << endl;
                return FAIL;
            }
            cmdtemp.erase(0, noblankpos);
    
            /* ��װ��ѯָ�������������ʽti:xxxx */
            if(strcmp(cmdtemp.c_str() ,"coreOS") == 0)
            {
                strcpy(pbuf,"setD:__kdomain__");
                len = strlen(pbuf);
            }
            else 
            {
                len = sprintf(pbuf, "setD:%s", cmdtemp.c_str());
            }
                
            /*�����setD����ͷ��͵�shellAgent*/
            sendDataToShellCallFun(pbuf, len);

            /* ����ִ�н�� */
            ret = recvDataFromTs( recvBuf, sizeof(recvBuf) );
            if(STOP_CMD == ret )
            {
                cout << endl;
            }
            else if(ret < 0)
            {
                cout << "�������ݳ���!" << endl;
            }

            /* �ظ�����ϢΪ�������,��ֱ�Ӵ�ӡ����ʧ�� */
            if('E' == recvBuf[0])
            {
                cout << "����" <<cmdtemp<<"������"<<endl;
                return FAIL;
            }

            cout<< "���" << endl;
            return SUCC;
            
        }
        else if(strncmp(cmdtemp.c_str(),"l ",strlen("l ")) == 0)
        {
            /* �������Ϊ�������� */
            if(expressionCmdDisasCheck(cmdtemp)){
                ret = m_processDisas(cmdtemp);
                if(ret != SUCC)
                {
                    return FAIL;
                }
            }else{
                return FAIL;
            }
        }
        else
        {
            /* ��װ��ѯ���ݰ�,��ʽe:TargetAID;symbolFile;expression */
            //�޸�BUG0006449��������������
            if(cmdtemp[cmdtemp.size() -1 ] == '\\'){
                printf("�������\n");
                return FAIL;
            }
            
            if(checkTargetShellAgentIsIdle() == false){
                std::cout << "Ŀ�������ִ�к������ã����Ժ�����" <<  std::endl;
                return FAIL;
            }
			if(!checkChineseCh(cmdtemp)){
				std::cout << "��֧������"<< std::endl;
				return FAIL;
			}	
            len = sprintf( expBuf, "e:%x;%s;%s", curTargetAID, curTargetName, cmdtemp.c_str() );
        
            /* �����ݷ��͵�tsServer */
            sendDataToTsServer(expBuf, len);
        }

        /* ����ִ�н�� */
        ret = recvDataFromTs( recvBuf, sizeof(recvBuf) );
        if(STOP_CMD == ret )
        {
            cout << endl;
        }
        else if(ret < 0)
        {
            cout << "�������ݳ���!" << endl;
        }
        else
        {
            cout <<  recvBuf << endl;
        }
    }

    return SUCC;
}



/* ��tsmap ��ȡtsServer��Aid�� */
int expressionProcess::getTsServerAidFromTsMap()
{
    int len = 0;
    int ret = 0;
    int tsserverid = 0;
    unsigned short src_aid, des_said, src_said;
    char quryBuf[100] = {'\0'};
    char* pStr = NULL;

    /* ��װ��ѯtsServerAid �����ݰ� */
    len = sprintf(quryBuf, "q:tsServerMap;%x;%x;%x;%s;",tsLinkId, 0, strlen("tsServer"), "tsServer");

    /* ���� */
    ret = TClient_putpkt(tsLinkId, TS_MAP_ID, 0, 0, quryBuf, len);
    if( ret < 0 )
    {
        return TSMAP_ERROR;
    }

    ret = TClient_wait(tsLinkId, timeOut);
    if(TS_DATACOMING != ret) 
    {
        return TSMAP_ERROR;
    }

    /* �հ� */
    memset(quryBuf, 0, sizeof(quryBuf));
    ret = TClient_getpkt(tsLinkId, &src_aid, &des_said, &src_said, quryBuf, sizeof(quryBuf));
    if ( ret < 0 )
    {
        return (TSMAP_ERROR);    
    }

    /* ��ѯʧ�� */
    pStr = strstr(quryBuf, "E05");
    if (NULL != pStr)
    {   
        return(TSMAP_ERROR);
    }

    /* �����ظ�����,��ʽOK; regAid;regSaid; �ҵ���Ӧ���� */
    pStr = strstr(quryBuf, "OK;");
    if (NULL == pStr)
    {
        return(FAIL);
    }
    
    pStr += strlen("OK;");
    hex2int(&pStr, &tsserverid);
    
    if(tsserverid <= 0)
    {
        return (FAIL);
    }

    tsServerAid = tsserverid;

    return SUCC;
}

    
