#include "coreCmdProcess.h"
#include "CheckSubArgv.h"


static const basic_string <char>::size_type npos = -1;

#define MAX_ADDR 0xffffffff

static unsigned int siPreAdddr = 0;//ǰһ����ȡ��ADDR�Ľ�����ַ
static unsigned int siPreUnitCount = 0;//ǰһ����ȡ��Unit��Ԫ
static unsigned int siPreUnitWidth = 0;//ǰһ����ȡ��ÿ����Ԫ�ĳ���
static unsigned int siCurrentAdddr = 0;//��ǰ��ȡ��ADDR��ַ
static unsigned int siCurrentUnitCount = 0;//��ǰ��ȡ��UNIT��Ԫ
static unsigned int siCurrentUnitWidth = 0;//��ǰ��ȡ��ÿ����Ԫ�ĳ���

//��ǰ�û������ģʽ���Ƿ�ȫ�����ʽ
typedef enum _USER_INPUT_MODE
{
	INPUT_ERROR = -1,
	INPUT_NO_BASEADDR = 0,
	INPUT_ONLY_BASEADDR,
	INPUT_BASEADDR_UNITCOUNT,
	INPUT_BASEADDR_UNITCOUNT_UNITWIDTH

}USER_INPUT_MODE;
/*

	���ÿ��ata����Ŀ����������Ӻ󣬶�����Ե�ǰ�ļ��еľ�̬�������г�ʼ��
	����ÿ�ζ�ȡ��ַADDR�ĳ�ʼֵΪ0
	����ÿ�ζ�ȡ��Ԫ���ȵĴ�СΪ128
	����ÿ�ζ�ȡ��Ԫ���ȵĿ��2
	

*/
void resetStaicReadMemeryValue(){
	siPreAdddr  = 0;
	siPreUnitCount = 128;
	siPreUnitWidth = 2;
}
/*
	ÿ�ζ�ȡ�ڴ��ַ���Զ������ϴ��ڴ�Ķ�ȡ�Ľ�����ַ,��ȡ��Ԫ��������Ԫ�����ĳ���
*/
void savePreReadValue(unsigned int preAddr,unsigned int unitCount = 128 ,unsigned int unitWidth = 2){
	siPreAdddr = preAddr;
	siPreUnitCount = unitCount; 
	siPreUnitWidth = unitWidth;

}
/*
	����ÿ�ζ�ȡ�ڴ��ַ����ʼ��ַ����ȡ��Ԫ����,��Ԫ�����ĳ���
*/
void saveCurrentReadValue(unsigned int Addr,unsigned int unitCount,unsigned int unitWidth){
	siCurrentAdddr = Addr;
	siCurrentUnitCount  = unitCount ;
	siCurrentUnitWidth = unitWidth;
}


/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ����Ŀ��������� */
extern int curTargetName;
/* �������ڴ������Ԫ */
int coreCmdProcess::readMemUnit(unsigned int addr, int size, char* pbuf, int bufsize)
{

    int ret = 0;
    int len = 0;

    if(NULL == pbuf)
    {
        return FAIL;
    }

    memset(pbuf, 0, bufsize);

    /* ��װ���ڴ������ */
    len = sprintf(pbuf, "m%x,%x", addr, size);

    /* �����������shell agent */
    ret = sendDataToShellAgent( pbuf, len );
    if(ret < 0)
    {
        return FAIL;
    }

    /* ���ն�ȡ��� */
    ret = recvDataFromTs( pbuf, bufsize );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "��������ʧ��!" << endl;
        return FAIL;
    }
    return SUCC;
}
/*
	���ܣ�������С16�ֽڵ�Ԫ��ȡ�ڴ��ַ
	readMemoryAddr:��ȡ�ڴ����ʼ��ַ
	BlockDataSize:��ȡ�ڴ�������С
	readErrAddr:��ȡ�ڴ�����λ��
	saveMsgBuf:����ÿ�ζ�ȡ�ڴ���е��ַ���
*/
 bool coreCmdProcess::autoReadBigBlockData(unsigned int readMemoryAddr,unsigned int BlockDataSize,unsigned int& readErrAddr,string& saveMsgBuf){

    bool bRet = true;
	//�ֶζ�ȡ�ڴ��ַ����ʮ���ֽڶ���
	unsigned int readSize = BlockDataSize;
	unsigned int ret = 0;
	char msgBuf[MAX_MSG_SIZE] = "\0";
	unsigned int currentReadSize = 0;
	//��ȡ��ַ���Ȱ���16�ֽڶ�ȡ
	unsigned int currentReadStartAddr = readMemoryAddr & 0xfffffff0;
	//��ȡ��ַ�ٰ��ն����ַ��ȡ
	unsigned int resetReadStart =  ((readMemoryAddr & 0xf) / m_defaultControlMemWidth) * m_defaultControlMemWidth;
	readErrAddr = readMemoryAddr;
	currentReadStartAddr += resetReadStart;
	while(readSize > 0){
		memset(msgBuf,0,MAX_MSG_SIZE);
		//ÿ������ȡ��Ԫ����Ϊ16���ֽ�
		currentReadSize =( 0x10 - (currentReadStartAddr & 0xf)) > BlockDataSize ? BlockDataSize :( 0x10 - (currentReadStartAddr & 0xf));
		ret = readMemUnit(currentReadStartAddr, currentReadSize, msgBuf, sizeof(msgBuf));
		if(SUCC != ret)
		{
		 	break;
		}else{
			//��ǰ��ȡ16���ֽڵ��ڴ�鵥Ԫ�������ʾ��ǰ�Ѿ��Ƕ�ȡ�������ڴ浥Ԫ
			if(!strcmp(msgBuf,"E01")){
				readErrAddr = currentReadStartAddr;
				bRet = false;
				break;
			}else{
				saveMsgBuf += msgBuf;
			}

		}
		currentReadStartAddr += currentReadSize;
		readSize -= currentReadSize;
	}
	return bRet;

}
/*
	��ȡ�ڴ��ַ
*/
int coreCmdProcess::m_readMemory(string cmd)
{

	int ret = 0;
	char msgBuf[MAX_MSG_SIZE] = "\0";
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
	
	

	unsigned int readsize = siCurrentUnitCount * siCurrentUnitWidth;
	unsigned int saveReadSize ;
	unsigned int currentBaseAddr = siCurrentAdddr;

	string saveMsgBuf = "";

	saveReadSize = readsize;
	setDefaultControlMemWidth(siCurrentUnitWidth);
	bool isShowStartFlags = true;
	bool saveReadMemorySend = true;


	while( readsize > 0 ){
		
		memset(msgBuf,0,MAX_MSG_SIZE);
		unsigned int currentReadSize = readsize >= 256  ? 256 : readsize;
		unsigned int residueAddr = 0;
		unsigned int baseAddr ;
		unsigned int readMemorySend;
		m_defaultControlMemUnits = currentReadSize / siCurrentUnitWidth;
		/* ʵ�ʷ��͵Ķ��ڴ�ֵ��Ҫ����ָ���Ŀ�ȶ��� */ 
		baseAddr = currentBaseAddr & (0XFFFFFFF0);
		residueAddr = currentBaseAddr & (0XF);
		residueAddr = ( residueAddr / m_defaultControlMemWidth) * m_defaultControlMemWidth;
		
		//��һ��ȡ�ڴ��ʱ���Ȳ�ѯ��ǰλ���Ƿ�ɶ�
		if(saveReadMemorySend){
			unsigned int readErrAddr = 0; 
			string tmp;
			m_readMemorySend = baseAddr + residueAddr;
			unsigned int leftSize = 0x10 - ( m_readMemorySend & 0xf );
			unsigned int firstReadSize = readsize < leftSize ? readsize : leftSize;
			if(!autoReadBigBlockData(m_readMemorySend,firstReadSize,readErrAddr,tmp)){
				printf("�ڴ��޷�����\n");
				return FAIL;
			}
			saveReadMemorySend = false;
		}
		
		readMemorySend =baseAddr + residueAddr;
		/* ��Ŀ����˶��ڴ� */
		ret = readMemUnit(readMemorySend, currentReadSize, msgBuf, sizeof(msgBuf));
		if(SUCC != ret)
		{
		 return FAIL;
		}
		/*
			�ڶ�ȡ�����ڴ浥Ԫ��ʱ�򣬿��ܻ���ֲ��ֵ�Ԫ�ܹ���ȡ�ڴ��ַ�������ֵ�Ԫ���ܽ��ж�ȡ�������
			��ִζ�ȡ�ڴ��ַ���������ȡ��������
		*/
		if(!strcmp(msgBuf,"E01")){
			unsigned int readErrAddr = 0;
			autoReadBigBlockData(readMemorySend,currentReadSize,readErrAddr,saveMsgBuf);
			//��ǰ��ȡ�ڴ浥Ԫһ���ַ���ʱ��ռ��saveMsgBuf�ַ�����2���ַ�
			showReadMemUnit(m_readMemorySend,const_cast<char*>(saveMsgBuf.c_str()),saveMsgBuf.size()/2);
			printf("0x%08x:�ڴ��޷�����\n",readErrAddr);
			//�����ڴ��ȡ����ֱ�ӱ��浱ǰ�ڴ��ȡ�ĳ����λ��
			savePreReadValue(readErrAddr,siCurrentUnitCount,siCurrentUnitWidth);
			return FAIL;
		}else{
			saveMsgBuf += msgBuf;
		}
		readsize -= currentReadSize;
		currentBaseAddr += currentReadSize;
		
	}
	/* ��ָ����ʽ��ʾ��ȡ�ڴ� */
	showReadMemUnit(m_readMemorySend, const_cast<char*>(saveMsgBuf.c_str()),saveReadSize);
	savePreReadValue(siCurrentAdddr+siCurrentUnitCount * siCurrentUnitWidth,siCurrentUnitCount,siCurrentUnitWidth);

    
	return SUCC;
}
/* ��ָ����ʽ��ʾ�ڴ浥Ԫ */
int coreCmdProcess::showReadMemUnit(int addr, char* pbuf,unsigned int readsize)
{
    int baseAddr = 0;
    int residueAddr = 0;
    char* showMem = NULL;
    char* showMem1 = pbuf;
    char bakMem[32]; 
    memset(bakMem, 0, 32);

    int havePrintUnit = 16;
    int d_Number[16];
    memset(d_Number, 0, 16);
    int value, ret;
    char d_buf[3];
    memset(d_buf, 0, 3);
    char *p_buf = d_buf;
    int num1 = 0,num2 = 1;

    if(NULL == pbuf)
    {
        return SUCC;
    }

    showMem = pbuf;
	

    /* ��ȡ�ڴ��ַ,��4�ֽڶ��� */
    baseAddr = addr & (0xFFFFFFF0);
    residueAddr = addr & (0xF);

    /* �����ܹ���Ҫ��ӡ���ڴ浥Ԫ����  */
    //readsize = m_defaultControlMemUnits * m_defaultControlMemWidth;
	
	printf("          NOTE: memory values are displayed in hex\n");

	if(readsize){
		printf("             ");
		for( int i = 0;i < (16/m_defaultControlMemWidth);i++){
			printf("% *x ",m_defaultControlMemWidth*2,i*m_defaultControlMemWidth);
		}
		printf("\n");
	}

    while(readsize > 0)
    {
        printf("0x%08x:  ", baseAddr);
        baseAddr += 0x10;

        havePrintUnit = EACH_LINE_PRINT_UNITS;
		
        for(int i = 0; i < 32; i++)
            {
                bakMem[i] = *(showMem1++);
            }
        //printf("%s",bakMem);
        
        /*��ӡ�ڴ���Ϣ*/
        while(havePrintUnit > 0)
        {
            
            /* �������δ�����ڴ���Ϣ,���ӡ�ո���� */
            while(residueAddr > 0)
            {
                /* ��ӡ�ո񲹳�δ�����ڴ� */
                putBlankUnit(m_defaultControlMemWidth);
                putBlankStr(havePrintUnit,m_defaultControlMemWidth);

                residueAddr -=m_defaultControlMemWidth;
            }

            /* ��ӡ�ڴ浥Ԫ */
            putMemUnit(showMem, m_defaultControlMemWidth);
            putBlankStr(havePrintUnit,m_defaultControlMemWidth);

            readsize -=m_defaultControlMemWidth;
            
            /* �������ڴ浥Ԫ�Ѿ���ӡ��,����δ��������һ�� */
            if( (0 == readsize) && (havePrintUnit > 0) )
            {
                /* ��ӡ�ո񲹳������һ�� */
                while(havePrintUnit > 0)
                {
                    /* ��ӡ�ո񲹳�δ�����ڴ� */
                    putBlankUnit(m_defaultControlMemWidth);
                    putBlankStr(havePrintUnit,m_defaultControlMemWidth);
                }
                printf("\n");

                return SUCC;
            }
        }
        
        /*��ӡASCII���ʽ�ڴ���Ϣ*/

    memset(d_Number, 0, 16);
    memset(d_buf, 0, 3);
    num1 = 0;
    num2 = 1;
        printf("\t*");
        for(int j = 0;j < 16;j++)
        {
            d_buf[0] = bakMem[num1];
            d_buf[1] = bakMem[num2];
            d_buf[2] = '\0';
            num1 += 2;
            num2 += 2;
            //p_buf = d_buf;
            ret = hex2int(&p_buf, &value);
            p_buf = d_buf;
            d_Number[j] = value;
        }

        for(int num3 = 0;num3<16;num3++)
        {
            /*�ж��Ƿ�Ϊ�ɴ�ӡ�ַ����粻�ɴ�ӡ���ӡ"."*/
            if(isprint(d_Number[num3]) != 0)
            {
                printf("%c",d_Number[num3]);
            }
            else
            {
                printf(".");
            }
        }
        printf("*");
        printf("\n");
    }

    return SUCC;
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
	
		ret = getUINT(startAddrStr,startAddr,true);
		if(ret == false){
			std::cout <<"��ַ��Ч" <<std::endl;
		}
	}
	return ret;
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
	�Զ������û���Ԫ����
	����ֵ��
		true:��ʾ���óɹ�
		false:��ʾ�û����õ�unitCount���󣬱�ʾʧ��
*/
static bool autoSetUnitWidth(unsigned int  baseAddr,unsigned int unitCount,unsigned int& autoUnitIWidth){

	bool ret = false;
	autoUnitIWidth = siPreUnitWidth;
	while(autoUnitIWidth != 0){
		ret = checkAddrIsOverflow(baseAddr,unitCount,autoUnitIWidth,false);
		if( ret == false){
			//��checkAddrIsOverflow�����з���ֵΪtrue,��ʾ���.false��ʾû�����
			ret = !ret;
			break;
		}else{
			autoUnitIWidth /= 2;
		}
	}
	if(!autoUnitIWidth){
		ret = false;
	}
	return ret;
}
/*
	�Զ������û���Ԫ����
	����ֵ:
		true����ʾ�ɹ�
		false:��ʾʧ�ܣ���baseAddr������Χ
*/
static bool autoSetUnitCount(unsigned int  baseAddr,unsigned int& unitCount ,unsigned int& autoUnitIWidth){
	bool ret = false;
	unitCount = siPreUnitCount;
	autoUnitIWidth = siPreUnitWidth;
	unsigned int leftUnitCount = (0xffffffff - baseAddr)>>(autoUnitIWidth/2);
	unitCount =  leftUnitCount < unitCount ? leftUnitCount:unitCount;
	if( unitCount == 0){
		ret = false;
	}else{
		ret = true;
	}
#if 0
	while(autoUnitIWidth != 0){
		ret = autoSetUnitWidth(baseAddr,unitCount,autoUnitIWidth);
		if( ret == true ){
			break;
		}else{
			autoUnitIWidth /= 2;
		}
	}
	if(!autoUnitIWidth){
		ret = false;
	}
#endif

	return ret;
}

static bool autoSetBaseAddr(unsigned int&  baseAddr,unsigned int& unitCount ,unsigned int& autoUnitIWidth ){
	bool ret = false;
	baseAddr = siPreAdddr;
	unitCount = siPreUnitCount;
	autoUnitIWidth = siPreUnitWidth;
	//ȡֵ��ΧΪ[0~0xfffffff)
	if( 0xffffffff - baseAddr >= 1){
		ret = autoSetUnitCount(baseAddr,unitCount,autoUnitIWidth);
	}else{
		ret = false;
	}
	return ret;
	
}
/*
	��鵱ǰ�������ʼ��ַ,��Ԫ����,��Ԫ�����Ƿ���Ч
*/
static bool checkReadAddrIsValid(unsigned int  baseAddr=0,unsigned int unitCount=0,unsigned int unitWidth=0,USER_INPUT_MODE userInputMode = INPUT_ERROR){
	bool ret = true;
	//��ǰ��ȡ�ڴ��ַ����Ч��Χ֮��0~0xffffffff
	//��ǰ��Ԫ���Ȳ�Ϊ0�����ʾ�û����������������ʽ������Ҫ���ϸ�������ʽ�ж�
	if( userInputMode ==  INPUT_BASEADDR_UNITCOUNT_UNITWIDTH){
		ret = checkAddrIsOverflow(baseAddr,unitCount,unitWidth);
		ret = !ret;
	}else{
		//��ǰ�û�ֻ������"d,baseAddr,unitCount"����ģʽ
		if(userInputMode == INPUT_BASEADDR_UNITCOUNT ){
			unsigned int autoUnitIWidth = 0;
			//�Զ����õ�ǰ��Ԫ���ȣ������Ԫ����Ϊ0�����ʾ�û�����ĵ�Ԫ�����Ѿ����
			ret = autoSetUnitWidth(baseAddr,unitCount,autoUnitIWidth);
			if( !autoUnitIWidth && ret == false){
				std::cout << "���" << std::endl;
				ret = false;
			}else{
				unitWidth = autoUnitIWidth;
				ret = true;
			}
		}else{
			//��ǰ�û�ֻ������ d,baseAddrģʽ
			if(userInputMode == INPUT_ONLY_BASEADDR){
				unsigned int autoUnitIWidth = 0;
				unsigned int autoUnitCount = 0;
				ret = autoSetUnitCount(baseAddr,autoUnitCount,autoUnitIWidth);
				if( ret == false){
					std::cout << "���" << std::endl;
					ret = false;
				}else{
					unitCount = autoUnitCount;
					unitWidth = autoUnitIWidth;
				}
			}else if(userInputMode == INPUT_NO_BASEADDR){
				//��ǰ�û�ʲô��û��������ֻ������"d"����ģʽ
				unsigned int autoUnitIWidth = 0;
				unsigned int autoUnitCount = 0;
				unsigned int autoBaseAddr = 0;
				ret = autoSetBaseAddr(autoBaseAddr,autoUnitCount,autoUnitIWidth);
				if( ret){
					baseAddr = autoBaseAddr;
					unitCount = autoUnitCount;
					unitWidth = autoUnitIWidth;
				}else{
					std::cout << "���"<< std::endl;
					//��������ΪĬ�ϵĵ�Ԫ��������Ԫ����
					savePreReadValue(siPreAdddr);
				}
			}else{
				ret = false;
			}//end of else
		
		}//end of if(unitCount != 0)
	
	}//end of if(unitSize != 0)

	if(ret){
		saveCurrentReadValue(baseAddr,unitCount,unitWidth);
	}
	return ret;
}

bool coreCmdReadMemoryCheckSubArgv(string& cmd){
	
	bool ret = true;
	vector<string> parameterList = getSubArgvFromCmd(cmd,",");
	vector<string> spaceList  = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = parameterList.size();
	USER_INPUT_MODE inputMode = INPUT_ERROR;
	unsigned int startAddr = 0;
	unsigned int unitCount = 0;
	unsigned int unitWidth = 0;
	int errNo = 0;
	if(spaceList.size() > 1){
		std::cout << "�����ʽ����" << std::endl;
		ret = false;
		return ret;
	}
	switch(length){
			case 3:
				inputMode = INPUT_BASEADDR_UNITCOUNT_UNITWIDTH;
				ret = getUnitWidth(parameterList[2],unitWidth);
			case 2:
				if(inputMode == INPUT_ERROR){
					inputMode = INPUT_BASEADDR_UNITCOUNT;
				}
				if(ret){
					ret = getUINT(parameterList[1],unitCount,false, &errNo);
					if(!ret){
						if(errNo == 1){
							std::cout << "�����ʽ����" << std::endl;
						}else{
							std::cout << "���" << std::endl;
						}
					}
				}
			case 1:
				if(inputMode == INPUT_ERROR){
					inputMode = INPUT_ONLY_BASEADDR;
				}
				if(ret){
					ret = getStartAddr(parameterList[0],startAddr);
				}
				break;
			case 0:
				inputMode = INPUT_NO_BASEADDR;
				break;
			default:
				std::cout << "�����ʽ����" << std::endl;
				ret = false;
				break;
	}
	
	if(ret == true){
		ret = checkReadAddrIsValid(startAddr,unitCount,unitWidth,inputMode);
	}
	return ret;
}