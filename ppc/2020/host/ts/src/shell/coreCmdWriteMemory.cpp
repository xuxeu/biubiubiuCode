#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;

static const basic_string <char>::size_type npos = -1;

/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ����Ŀ��������� */
extern int curTargetName;

//��ǰ�û������ģʽ���Ƿ�ȫ�����ʽ
typedef enum _USER_INPUT_W_MODE
{
	INPUT_ERROR = -1,
	INPUT_NO_BASEADDR = 0,
	INPUT_ONLY_BASEADDR,
	INPUT_BASEADDR_UNITWIDTH

}USER_INPUT_W_MODE;
/*#####################������������Լ�����ǰһ��д��ַ######################*/
#define MAX_ADDR 0xffffffff

static unsigned int siPreAdddr = 0;//ǰһ��д��ADDR�Ľ�����ַ
static unsigned int siPreUnitWidth = 2;//ǰһ��д��ÿ����Ԫ�ĳ���
static unsigned int siCurrentAdddr = 0;//��ǰд��ADDR��ַ
static unsigned int siCurrentUnitWidth = 0;//��ǰд��ÿ����Ԫ�ĳ���

/*###########################################################################*/

/*
	ÿ��д�ڴ��ַ���Զ������ϴ��ڴ�Ķ�ȡ�Ľ�����ַ,��Ԫ�����ĳ���
*/
void setPreWriteValue(unsigned int preAddr = 0,unsigned int unitWidth = 2){
	siPreAdddr = preAddr;
	siPreUnitWidth = unitWidth;
}
/*
	����ÿ��д�ڴ��ַ����ʼ��ַ����ȡ��Ԫ����,��Ԫ�����ĳ���
*/
void saveCurrentWriteValue(unsigned int Addr,unsigned int unitWidth){
	siCurrentAdddr = Addr;
	siCurrentUnitWidth = unitWidth;
}

//��ȡ��Ԫ���
static bool getUnitWidth(string unitWidthStr,unsigned int& unitWidth){
	bool ret = true;
	if( unitWidthStr == "1" || unitWidthStr == "2" || unitWidthStr == "4"){
		unitWidth = atoi(unitWidthStr.c_str());
	}else{
		std::cout << "��Ԫ�����Ч" << std::endl;
		ret = false;
	}
	return ret;
}

/*
	��ȡ��ʼ��ַ��֧��16���ƻ���10����
	16���Ʊ����0x����0X���ţ�����Ĭ������Ϊ10����
*/
static bool getStartAddr(string startAddrStr,unsigned int& startAddr){
#define MAX_HEX_STR "0xFFFFFFFF"
	bool ret = true;
	startAddr = 0; 
	if( (startAddrStr.find("0x") == 0 )|| (startAddrStr.find("0X") == 0) ){
		string maxHexStr = MAX_HEX_STR;
		if(startAddrStr.size() <= maxHexStr.size() ){
			const char* pStartAddr = startAddrStr.c_str();
			pStartAddr += 2;
			while(*pStartAddr){
				unsigned int currentInt = 0;
				if(*pStartAddr >= '0' && *pStartAddr <= '9'){
					currentInt = *pStartAddr - '0';
				}else if(*pStartAddr >= 'a' && *pStartAddr <= 'f'){
					currentInt = *pStartAddr - 'a' + 10;
				}else if(*pStartAddr >= 'A' && *pStartAddr <= 'F'){
					currentInt = *pStartAddr - 'A' + 10;
				}else{
					std::cout <<"��ַ��Ч" <<std::endl;
					ret = false;
					break;
				}
				startAddr = startAddr*16 + currentInt;
				pStartAddr++;
			}
		}else{
			std::cout <<"��ַ��Ч" <<std::endl;
			ret = false;
		}
	}else{
		//��ȡʮ���Ƶ�ʵ��ֵ
		ret = getUINT(startAddrStr,startAddr,true);
		if( !ret ){
			std::cout<< "��ַ��Ч" <<std::endl;
		}
	}
	return ret;
}
/*
	�Զ�����д��ַʱ��ĵ�Ԫ���
	����ֵ��:true �Զ����õ�Ԫ��ȳɹ�
			 false �Զ����õ�Ԫ���ʧ�� ��ǰд�ڴ����
*/
static bool autoSetUnitCount(unsigned int baseAddr,unsigned int& unitWidth){
	
	bool ret = true;
	unitWidth = siPreUnitWidth;

	while(unitWidth){
	
		ret = checkAddrIsOverflow(baseAddr,1,unitWidth,false);
		if(!ret){
			ret =!ret;
			break;
		}
		unitWidth /= 2;
	}
	if(unitWidth == 0){
		ret = false;
	}
	return ret;
}
/*
	�Զ������ڴ�д��ַ����Ԫ���
	����ֵ:true �Զ����õ�Ԫ��ȣ�д�ڴ��ַ�ɹ�
		   false:��ǰд�ڴ��ַ�Ѿ������ֵ�����ܼ���д�ڴ�
*/
static bool autoSetBaseAddr(unsigned int& baseAddr,unsigned int& unitWidth){
	bool ret = true;
	baseAddr =siPreAdddr;
	unitWidth = siPreUnitWidth;
	ret = checkAddrIsOverflow(baseAddr,1,unitWidth,false);
	return !ret;

}
/*
	��鵱ǰ�������ʼ��ַ,��Ԫ����,��Ԫ�����Ƿ���Ч
*/
static bool checkWriteAddrIsValid(unsigned int  baseAddr,unsigned int& unitWidth,USER_INPUT_W_MODE input_mode = INPUT_ERROR){
	bool ret = true;
	//��ǰ��ȡ�ڴ��ַ����Ч��Χ֮��0~0xffffffff
	//��ǰ��Ԫ���Ȳ�Ϊ0�����ʾ�û����������������ʽ������Ҫ���ϸ�������ʽ�ж�
	if( input_mode == INPUT_BASEADDR_UNITWIDTH ){
		ret = checkAddrIsOverflow(baseAddr,1,unitWidth);
		ret = !ret;
	}else{
			//��ǰ�û�ֻ������ m,baseAddrģʽ
			if(input_mode == INPUT_ONLY_BASEADDR ){
				unsigned int autoUnitIWidth = 0;
				ret = autoSetUnitCount(baseAddr,autoUnitIWidth);
				if( ret == false){
					std::cout << "��ǰд��ַ����,�����������д��ַ" << std::endl;
					ret = false;
				}else{
					unitWidth = autoUnitIWidth;
				}
			}else if(input_mode == INPUT_NO_BASEADDR ){
				//��ǰ�û�ʲô��û��������ֻ������"m"����ģʽ
				unsigned int autoUnitIWidth = 0;
				unsigned int autoBaseAddr = 0;
				ret = autoSetBaseAddr(autoBaseAddr,autoUnitIWidth);
				if( ret){
					baseAddr = autoBaseAddr;
					unitWidth = autoUnitIWidth;
				}else{
					std::cout << "��ǰд�ڴ��ַ�Ѿ�Ϊ���ֵ��������������Ҫд���ڴ��ַ"<< std::endl;
					//��������ΪĬ�ϵĵ�Ԫ����
					setPreWriteValue(siPreAdddr);
				}
			}//end of else
	
	}//end of if(unitSize != 0)
	if(ret){
		saveCurrentWriteValue(baseAddr,unitWidth);
		//����ǰһ�εĵ�Ԫ���
		siPreUnitWidth = unitWidth;
	}
	return ret;
}


/*
	д�ڴ�������
*/
bool coreCmdWriteMemoryCheckSubArgv(string& cmd){

	bool ret = true;
	vector<string> parameterList = getSubArgvFromCmd(cmd,",");
	vector<string>::size_type length = parameterList.size();
	USER_INPUT_W_MODE input_mode = INPUT_ERROR;
	vector<string> spaceList  = getSubArgvFromCmd(cmd);
	if(spaceList.size() > 1){
		std::cout << "�����ʽ����" << std::endl;
		ret = false;
		return ret;
	}
	unsigned int startAddr = 0;
	unsigned int unitWidth = 0;
	switch(length){
			case 2:
				if( input_mode == INPUT_ERROR ){
					input_mode = INPUT_BASEADDR_UNITWIDTH;
				}
				ret = getUnitWidth(parameterList[1],unitWidth);
				
			case 1:
				if( input_mode == INPUT_ERROR ){
					input_mode = INPUT_ONLY_BASEADDR;
				}
				if(ret){
					ret = getStartAddr(parameterList[0],startAddr);
				}
				break;
			case 0:
				if( input_mode == INPUT_ERROR ){
					input_mode = INPUT_NO_BASEADDR;
				}
				break;
			default:
				std::cout << "�����ʽ����" << std::endl;
				ret = false;
				break;
	}
	
	if(ret == true){
		ret = checkWriteAddrIsValid(startAddr,unitWidth,input_mode);
	}
	return ret;
}


/*************************************�ڴ�д����**************************************/


 /* д�ڴ���� */
int coreCmdProcess::writeMemLoop(unsigned int addr)
{

    unsigned int currentWriteMemAddr = 0;
    int eachReadSize = 0;
    char msgBuf[MAX_MSG_SIZE] = {'\0'};
    int ret = 0;
	size_t i = 0;
    char* pMem = NULL;
    char inputbuf[MAX_MSG_SIZE] = {'\0'};
    size_t inputstrlen = 0;
    char* pgetstr = NULL;
	unsigned int saveWriteAddr = 0;
    currentWriteMemAddr = addr;

    /* ÿ��д�ڴ�������ֽڿ�� */
    eachReadSize = m_defaultControlMemWidth;

    /* ѭ��ִ�в��� */
    while(1)
    {
        /* ��ȡ��ǰ�����ڴ� */
        memset(msgBuf, 0, sizeof(msgBuf));
        ret = readMemUnit(currentWriteMemAddr, eachReadSize, msgBuf, sizeof(msgBuf));
        if(SUCC != ret)
        {
            return FAIL;
        }
		if(strcmp(msgBuf,"E01") == 0){
			printf("�ڴ��޷�����\n");
			return FAIL;
		}
        pMem = msgBuf;

        /* ��ӡ��ǰ�ڴ�ֵ */
        printf("0x%08x:  ", currentWriteMemAddr);
        putMemUnit(pMem, m_defaultControlMemWidth);
        printf("-");

        /* �������ж���һ������ */
        pgetstr = gets_s(inputbuf, sizeof(inputbuf));
        if( (NULL == pgetstr) || (0 == *pgetstr) )
        {
            currentWriteMemAddr += m_defaultControlMemWidth;
            continue;
        }

        /* ������ǽ���������,���˳��ڴ������ */
        if(0 == strncmp(inputbuf, ".",strlen(".")) )
        {
            return SUCC;
        }

		/* ��������ַ��Ƿ�Ϸ�,������ַ�Ӧ��Ϊ16�����ַ��� */
		for(i = 0; i < strlen(inputbuf); i++)
		{
			if( !isxdigit(inputbuf[i]))
			{
				cout << "����Ƿ��ַ�" << endl;
				return FAIL;
			}
		}

		saveWriteAddr = currentWriteMemAddr;
        inputstrlen = strlen(inputbuf);

        /* ����û�������0x��ͷ���ַ�,ȥ����ͷ�ַ� */
        if( (0 == strncmp(inputbuf, "0x", strlen("0x"))) || (0 == strncmp(inputbuf, "0X", strlen("0X"))) )
        {
            for(i = 0; i < inputstrlen - 2; i++)
            {
                inputbuf[i] = inputbuf[i+2];
            }
            inputbuf[inputstrlen-1] = '\0';
            inputbuf[inputstrlen-2] = '\0';
        }

        inputstrlen = strlen(inputbuf);

        /* �������ݳ�������,��ʾ������Ϣ,���˳� */
        if( inputstrlen > (2*m_defaultControlMemWidth) )
        {
            printf("���\n");
            return FAIL;
        }

        /* ��0�����ڴ�����Ϊָ������ */
        for(i = 1; i <= inputstrlen; i++)
        {
            inputbuf[(2*m_defaultControlMemWidth) - i] = inputbuf[inputstrlen-i];
        }

        for(i = 0; i < (2*m_defaultControlMemWidth - inputstrlen); i++)
        {
            inputbuf[i] = '0';
        }

        /* ��ӽ�����  */
        inputbuf[(2*m_defaultControlMemWidth)] = '\0';


        if( 2 == m_defaultControlMemWidth )
        {
            /* ת���ֽ���*/
            swapMemUnit16Bit(inputbuf);
        }

        if( 4 == m_defaultControlMemWidth )
        {
            /* ת���ֽ���*/
            swapMemUnit32Bit(inputbuf);
        }

        /* ���ַ���ת��Ϊ�ڴ�ֵ */
        hex2memEx(inputbuf, 2*m_defaultControlMemWidth);

        /* ��Ŀ����˷���д�ڴ����,����ڴ�д���� */
        ret = writeMemUnit(currentWriteMemAddr, m_defaultControlMemWidth, inputbuf, sizeof(inputbuf));
        if(FAIL == ret)
        {
			cout << "д�ڴ����ʧ��" << endl;
            return FAIL;
        }

        currentWriteMemAddr += m_defaultControlMemWidth;

		//siPreAdddr = currentWriteMemAddr;
		setPreWriteValue(saveWriteAddr+m_defaultControlMemWidth,m_defaultControlMemWidth);

    }

    return SUCC;
 }





/* �������ڴ������Ԫ */
int coreCmdProcess::writeMemUnit(unsigned int addr, int writesize, char* pwrtbuf, int bufsize)
{

    int ret = 0;
    int len = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    int pbufsize = MAX_MSG_SIZE;

    if(NULL == pwrtbuf)
    {
        return FAIL;
    }

    /* ��װд�ڴ��������ʽXaddr,size:xxxx */
    len = sprintf(pbuf, "X%x,%x:%s", addr, writesize, pwrtbuf);

    /* �����������shell agent */
    ret = sendDataToShellAgent( pbuf, len );
    if(ret < 0)
    {
        return FAIL;
    }

    /* ���ն�ȡ��� */
    memset(pbuf, 0, sizeof(pbuf));
    ret = recvDataFromTs( pbuf, pbufsize );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "��������ʧ��" << endl;
        return FAIL;
    }

    /* �ж�ִ�н�� */
    if(0 != strcmp(pbuf, "OK"))
    {
        return FAIL;
    }

    return SUCC;
}



/* 
	д�ڴ���� 
*/


int coreCmdProcess::m_writeMemory(string cmd)
{
	int ret = 0;
	unsigned int width = siCurrentUnitWidth;
    unsigned int writeMemStartAddr = 0;
	

	//��ѯOS�汾
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	//�����ǰĿ����Ĳ���ϵͳ�汾�뵱ǰ���Ʋ�ƥ�䣬��ֱ�Ӳ����κδ���
	if(m_osVersion != OS_2X_VERSION ){
		cout << "������ֻ֧��ACoreOS653����ϵͳ"<< endl;
		return FAIL;
	}
	//��ѯOS�Ƿ�����
	if( SUCC != queryTargetAgentOsStauts() )
	{
		return FAIL;
	}
	
	//��ѯ�����Ƿ�����
	if( SUCC != queryTargetShellAgent() )
	{
		return FAIL;
	}

	
	writeMemStartAddr = siCurrentAdddr&~(siCurrentUnitWidth-1);

    
    /* �����û�������ڴ泤�Ȳ��� */
    setDefaultControlMemWidth(width);

    /* ��Ŀ����˶��ڴ� */
    ret = writeMemLoop(writeMemStartAddr);
    if(SUCC != ret)
    {

        return FAIL;
    }

    return SUCC;
}