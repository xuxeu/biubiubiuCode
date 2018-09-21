#include "coreCmdProcess.h"
#include "CheckSubArgv.h"


static const basic_string <char>::size_type npos = -1;

#define MAX_ADDR 0xffffffff

static unsigned int siPreAdddr = 0;//前一个读取的ADDR的结束地址
static unsigned int siPreUnitCount = 0;//前一个读取的Unit单元
static unsigned int siPreUnitWidth = 0;//前一个读取的每个单元的长度
static unsigned int siCurrentAdddr = 0;//当前读取的ADDR地址
static unsigned int siCurrentUnitCount = 0;//当前读取的UNIT单元
static unsigned int siCurrentUnitWidth = 0;//当前读取的每个单元的长度

//当前用户输入的模式，是否全命令格式
typedef enum _USER_INPUT_MODE
{
	INPUT_ERROR = -1,
	INPUT_NO_BASEADDR = 0,
	INPUT_ONLY_BASEADDR,
	INPUT_BASEADDR_UNITCOUNT,
	INPUT_BASEADDR_UNITCOUNT_UNITWIDTH

}USER_INPUT_MODE;
/*

	针对每次ata创建目标机机器连接后，都必须对当前文件中的静态变量经行初始化
	设置每次读取地址ADDR的初始值为0
	设置每次读取单元长度的大小为128
	设置每次读取单元长度的宽度2
	

*/
void resetStaicReadMemeryValue(){
	siPreAdddr  = 0;
	siPreUnitCount = 128;
	siPreUnitWidth = 2;
}
/*
	每次读取内存地址后，自动保存上次内存的读取的结束地址,读取单元数量，单元数量的长度
*/
void savePreReadValue(unsigned int preAddr,unsigned int unitCount = 128 ,unsigned int unitWidth = 2){
	siPreAdddr = preAddr;
	siPreUnitCount = unitCount; 
	siPreUnitWidth = unitWidth;

}
/*
	保存每次读取内存地址的起始地址，读取单元数量,单元数量的长度
*/
void saveCurrentReadValue(unsigned int Addr,unsigned int unitCount,unsigned int unitWidth){
	siCurrentAdddr = Addr;
	siCurrentUnitCount  = unitCount ;
	siCurrentUnitWidth = unitWidth;
}


/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接目标机的名字 */
extern int curTargetName;
/* 基本读内存操作单元 */
int coreCmdProcess::readMemUnit(unsigned int addr, int size, char* pbuf, int bufsize)
{

    int ret = 0;
    int len = 0;

    if(NULL == pbuf)
    {
        return FAIL;
    }

    memset(pbuf, 0, bufsize);

    /* 组装读内存命令包 */
    len = sprintf(pbuf, "m%x,%x", addr, size);

    /* 发送命令包到shell agent */
    ret = sendDataToShellAgent( pbuf, len );
    if(ret < 0)
    {
        return FAIL;
    }

    /* 接收读取结果 */
    ret = recvDataFromTs( pbuf, bufsize );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "接收数据失败!" << endl;
        return FAIL;
    }
    return SUCC;
}
/*
	功能：按照最小16字节单元读取内存地址
	readMemoryAddr:读取内存的起始地址
	BlockDataSize:读取内存的总体大小
	readErrAddr:读取内存出错的位置
	saveMsgBuf:保留每次读取内存的中的字符码
*/
 bool coreCmdProcess::autoReadBigBlockData(unsigned int readMemoryAddr,unsigned int BlockDataSize,unsigned int& readErrAddr,string& saveMsgBuf){

    bool bRet = true;
	//分段读取内存地址按照十六字节对齐
	unsigned int readSize = BlockDataSize;
	unsigned int ret = 0;
	char msgBuf[MAX_MSG_SIZE] = "\0";
	unsigned int currentReadSize = 0;
	//读取地址首先按照16字节读取
	unsigned int currentReadStartAddr = readMemoryAddr & 0xfffffff0;
	//读取地址再按照对齐地址读取
	unsigned int resetReadStart =  ((readMemoryAddr & 0xf) / m_defaultControlMemWidth) * m_defaultControlMemWidth;
	readErrAddr = readMemoryAddr;
	currentReadStartAddr += resetReadStart;
	while(readSize > 0){
		memset(msgBuf,0,MAX_MSG_SIZE);
		//每次最大读取单元数量为16个字节
		currentReadSize =( 0x10 - (currentReadStartAddr & 0xf)) > BlockDataSize ? BlockDataSize :( 0x10 - (currentReadStartAddr & 0xf));
		ret = readMemUnit(currentReadStartAddr, currentReadSize, msgBuf, sizeof(msgBuf));
		if(SUCC != ret)
		{
		 	break;
		}else{
			//当前读取16个字节的内存块单元出错，则表示当前已经是读取到最大的内存单元
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
	读取内存地址
*/
int coreCmdProcess::m_readMemory(string cmd)
{

	int ret = 0;
	char msgBuf[MAX_MSG_SIZE] = "\0";
	//查询OS版本
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	//如果当前目标机的操作系统版本与当前名称不匹配，则直接不做任何处理
	if(m_osVersion != OS_2X_VERSION ){
		cout << "该命令只支持ACoreOS653操作系统"<< endl;
		return FAIL;
	}
	//查询OS是否启动
	if( SUCC != queryTargetAgentOsStauts() )
	{
		return FAIL;
	}
	
	//查询代理是否启动
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
		/* 实际发送的读内存值需要按照指定的宽度对齐 */ 
		baseAddr = currentBaseAddr & (0XFFFFFFF0);
		residueAddr = currentBaseAddr & (0XF);
		residueAddr = ( residueAddr / m_defaultControlMemWidth) * m_defaultControlMemWidth;
		
		//第一读取内存的时候，先查询当前位置是否可读
		if(saveReadMemorySend){
			unsigned int readErrAddr = 0; 
			string tmp;
			m_readMemorySend = baseAddr + residueAddr;
			unsigned int leftSize = 0x10 - ( m_readMemorySend & 0xf );
			unsigned int firstReadSize = readsize < leftSize ? readsize : leftSize;
			if(!autoReadBigBlockData(m_readMemorySend,firstReadSize,readErrAddr,tmp)){
				printf("内存无法访问\n");
				return FAIL;
			}
			saveReadMemorySend = false;
		}
		
		readMemorySend =baseAddr + residueAddr;
		/* 向目标机端读内存 */
		ret = readMemUnit(readMemorySend, currentReadSize, msgBuf, sizeof(msgBuf));
		if(SUCC != ret)
		{
		 return FAIL;
		}
		/*
			在读取部分内存单元的时候，可能会出现部分单元能够读取内存地址，而部分单元不能进行读取的情况。
			则分次读取内存地址。并保存读取到的数据
		*/
		if(!strcmp(msgBuf,"E01")){
			unsigned int readErrAddr = 0;
			autoReadBigBlockData(readMemorySend,currentReadSize,readErrAddr,saveMsgBuf);
			//当前读取内存单元一个字符的时候，占用saveMsgBuf字符串中2个字符
			showReadMemUnit(m_readMemorySend,const_cast<char*>(saveMsgBuf.c_str()),saveMsgBuf.size()/2);
			printf("0x%08x:内存无法访问\n",readErrAddr);
			//由于内存读取错误，直接保存当前内存读取的出错的位置
			savePreReadValue(readErrAddr,siCurrentUnitCount,siCurrentUnitWidth);
			return FAIL;
		}else{
			saveMsgBuf += msgBuf;
		}
		readsize -= currentReadSize;
		currentBaseAddr += currentReadSize;
		
	}
	/* 按指定格式显示读取内存 */
	showReadMemUnit(m_readMemorySend, const_cast<char*>(saveMsgBuf.c_str()),saveReadSize);
	savePreReadValue(siCurrentAdddr+siCurrentUnitCount * siCurrentUnitWidth,siCurrentUnitCount,siCurrentUnitWidth);

    
	return SUCC;
}
/* 按指定格式显示内存单元 */
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
	

    /* 获取内存地址,按4字节对齐 */
    baseAddr = addr & (0xFFFFFFF0);
    residueAddr = addr & (0xF);

    /* 计算总共需要打印的内存单元数量  */
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
        
        /*打印内存信息*/
        while(havePrintUnit > 0)
        {
            
            /* 如果存在未对齐内存信息,则打印空格填充 */
            while(residueAddr > 0)
            {
                /* 打印空格补充未读到内存 */
                putBlankUnit(m_defaultControlMemWidth);
                putBlankStr(havePrintUnit,m_defaultControlMemWidth);

                residueAddr -=m_defaultControlMemWidth;
            }

            /* 打印内存单元 */
            putMemUnit(showMem, m_defaultControlMemWidth);
            putBlankStr(havePrintUnit,m_defaultControlMemWidth);

            readsize -=m_defaultControlMemWidth;
            
            /* 读到的内存单元已经打印完,但并未填充完最后一行 */
            if( (0 == readsize) && (havePrintUnit > 0) )
            {
                /* 打印空格补充完最后一行 */
                while(havePrintUnit > 0)
                {
                    /* 打印空格补充未读到内存 */
                    putBlankUnit(m_defaultControlMemWidth);
                    putBlankStr(havePrintUnit,m_defaultControlMemWidth);
                }
                printf("\n");

                return SUCC;
            }
        }
        
        /*打印ASCII码格式内存信息*/

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
            /*判断是否为可打印字符，如不可打印则打印"."*/
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
	获取起始地址，支持16进制或者10进制
	16进制必须带0x或者0X符号，否则默认设置为10进制
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
					std::cout <<"地址无效" <<std::endl;
					ret = false;
					break;
				}
				startAddr = startAddr*16 + currentInt;
				pStartAddr++;
			}
		}else{
			std::cout <<"地址无效" <<std::endl;
			ret = false;
		}
	}else{
	
		ret = getUINT(startAddrStr,startAddr,true);
		if(ret == false){
			std::cout <<"地址无效" <<std::endl;
		}
	}
	return ret;
}
//获取单元宽度
static bool getUnitWidth(string unitWidthStr,unsigned int& unitWidth){
	bool ret = true;
	if( unitWidthStr == "1" || unitWidthStr == "2" || unitWidthStr == "4"){
		unitWidth = atoi(unitWidthStr.c_str());
	}else{
		std::cout << "单元宽度无效" << std::endl;
		ret = false;
	}
	return ret;
}
/*
	自动设置用户单元长度
	返回值：
		true:表示设置成功
		false:表示用户设置的unitCount过大，表示失败
*/
static bool autoSetUnitWidth(unsigned int  baseAddr,unsigned int unitCount,unsigned int& autoUnitIWidth){

	bool ret = false;
	autoUnitIWidth = siPreUnitWidth;
	while(autoUnitIWidth != 0){
		ret = checkAddrIsOverflow(baseAddr,unitCount,autoUnitIWidth,false);
		if( ret == false){
			//在checkAddrIsOverflow函数中返回值为true,表示溢出.false表示没有溢出
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
	自动设置用户单元数量
	返回值:
		true：表示成功
		false:表示失败，且baseAddr超出范围
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
	//取值范围为[0~0xfffffff)
	if( 0xffffffff - baseAddr >= 1){
		ret = autoSetUnitCount(baseAddr,unitCount,autoUnitIWidth);
	}else{
		ret = false;
	}
	return ret;
	
}
/*
	检查当前输入的起始地址,单元数量,单元长度是否有效
*/
static bool checkReadAddrIsValid(unsigned int  baseAddr=0,unsigned int unitCount=0,unsigned int unitWidth=0,USER_INPUT_MODE userInputMode = INPUT_ERROR){
	bool ret = true;
	//当前读取内存地址在有效范围之内0~0xffffffff
	//当前单元长度不为0，则表示用户完整的输入命令格式，则需要做严格的命令格式判断
	if( userInputMode ==  INPUT_BASEADDR_UNITCOUNT_UNITWIDTH){
		ret = checkAddrIsOverflow(baseAddr,unitCount,unitWidth);
		ret = !ret;
	}else{
		//当前用户只输入了"d,baseAddr,unitCount"这种模式
		if(userInputMode == INPUT_BASEADDR_UNITCOUNT ){
			unsigned int autoUnitIWidth = 0;
			//自动设置当前单元长度，如果单元长度为0，则表示用户输入的单元数量已经溢出
			ret = autoSetUnitWidth(baseAddr,unitCount,autoUnitIWidth);
			if( !autoUnitIWidth && ret == false){
				std::cout << "溢出" << std::endl;
				ret = false;
			}else{
				unitWidth = autoUnitIWidth;
				ret = true;
			}
		}else{
			//当前用户只输入了 d,baseAddr模式
			if(userInputMode == INPUT_ONLY_BASEADDR){
				unsigned int autoUnitIWidth = 0;
				unsigned int autoUnitCount = 0;
				ret = autoSetUnitCount(baseAddr,autoUnitCount,autoUnitIWidth);
				if( ret == false){
					std::cout << "溢出" << std::endl;
					ret = false;
				}else{
					unitCount = autoUnitCount;
					unitWidth = autoUnitIWidth;
				}
			}else if(userInputMode == INPUT_NO_BASEADDR){
				//当前用户什么都没做，仅仅只输入了"d"这种模式
				unsigned int autoUnitIWidth = 0;
				unsigned int autoUnitCount = 0;
				unsigned int autoBaseAddr = 0;
				ret = autoSetBaseAddr(autoBaseAddr,autoUnitCount,autoUnitIWidth);
				if( ret){
					baseAddr = autoBaseAddr;
					unitCount = autoUnitCount;
					unitWidth = autoUnitIWidth;
				}else{
					std::cout << "溢出"<< std::endl;
					//重新设置为默认的单元数量，单元长度
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
		std::cout << "命令格式错误" << std::endl;
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
							std::cout << "命令格式错误" << std::endl;
						}else{
							std::cout << "溢出" << std::endl;
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
				std::cout << "命令格式错误" << std::endl;
				ret = false;
				break;
	}
	
	if(ret == true){
		ret = checkReadAddrIsValid(startAddr,unitCount,unitWidth,inputMode);
	}
	return ret;
}