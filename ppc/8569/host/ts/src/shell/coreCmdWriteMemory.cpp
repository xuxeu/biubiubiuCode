#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;

static const basic_string <char>::size_type npos = -1;

/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接目标机的名字 */
extern int curTargetName;

//当前用户输入的模式，是否全命令格式
typedef enum _USER_INPUT_W_MODE
{
	INPUT_ERROR = -1,
	INPUT_NO_BASEADDR = 0,
	INPUT_ONLY_BASEADDR,
	INPUT_BASEADDR_UNITWIDTH

}USER_INPUT_W_MODE;
/*#####################命令参数解析以及保存前一次写地址######################*/
#define MAX_ADDR 0xffffffff

static unsigned int siPreAdddr = 0;//前一个写的ADDR的结束地址
static unsigned int siPreUnitWidth = 2;//前一个写的每个单元的长度
static unsigned int siCurrentAdddr = 0;//当前写的ADDR地址
static unsigned int siCurrentUnitWidth = 0;//当前写的每个单元的长度

/*###########################################################################*/

/*
	每次写内存地址后，自动保存上次内存的读取的结束地址,单元数量的长度
*/
void setPreWriteValue(unsigned int preAddr = 0,unsigned int unitWidth = 2){
	siPreAdddr = preAddr;
	siPreUnitWidth = unitWidth;
}
/*
	保存每次写内存地址的起始地址，读取单元数量,单元数量的长度
*/
void saveCurrentWriteValue(unsigned int Addr,unsigned int unitWidth){
	siCurrentAdddr = Addr;
	siCurrentUnitWidth = unitWidth;
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
		//获取十进制的实际值
		ret = getUINT(startAddrStr,startAddr,true);
		if( !ret ){
			std::cout<< "地址无效" <<std::endl;
		}
	}
	return ret;
}
/*
	自动设置写地址时候的单元宽度
	返回值得:true 自动设置单元宽度成功
			 false 自动设置单元宽度失败 当前写内存溢出
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
	自动设置内存写地址，单元宽度
	返回值:true 自动设置单元宽度，写内存地址成功
		   false:当前写内存地址已经是最大值，不能继续写内存
*/
static bool autoSetBaseAddr(unsigned int& baseAddr,unsigned int& unitWidth){
	bool ret = true;
	baseAddr =siPreAdddr;
	unitWidth = siPreUnitWidth;
	ret = checkAddrIsOverflow(baseAddr,1,unitWidth,false);
	return !ret;

}
/*
	检查当前输入的起始地址,单元数量,单元长度是否有效
*/
static bool checkWriteAddrIsValid(unsigned int  baseAddr,unsigned int& unitWidth,USER_INPUT_W_MODE input_mode = INPUT_ERROR){
	bool ret = true;
	//当前读取内存地址在有效范围之内0~0xffffffff
	//当前单元长度不为0，则表示用户完整的输入命令格式，则需要做严格的命令格式判断
	if( input_mode == INPUT_BASEADDR_UNITWIDTH ){
		ret = checkAddrIsOverflow(baseAddr,1,unitWidth);
		ret = !ret;
	}else{
			//当前用户只输入了 m,baseAddr模式
			if(input_mode == INPUT_ONLY_BASEADDR ){
				unsigned int autoUnitIWidth = 0;
				ret = autoSetUnitCount(baseAddr,autoUnitIWidth);
				if( ret == false){
					std::cout << "当前写地址过大,请重新输入读写地址" << std::endl;
					ret = false;
				}else{
					unitWidth = autoUnitIWidth;
				}
			}else if(input_mode == INPUT_NO_BASEADDR ){
				//当前用户什么都没做，仅仅只输入了"m"这种模式
				unsigned int autoUnitIWidth = 0;
				unsigned int autoBaseAddr = 0;
				ret = autoSetBaseAddr(autoBaseAddr,autoUnitIWidth);
				if( ret){
					baseAddr = autoBaseAddr;
					unitWidth = autoUnitIWidth;
				}else{
					std::cout << "当前写内存地址已经为最大值，请重新输入需要写的内存地址"<< std::endl;
					//重新设置为默认的单元长度
					setPreWriteValue(siPreAdddr);
				}
			}//end of else
	
	}//end of if(unitSize != 0)
	if(ret){
		saveCurrentWriteValue(baseAddr,unitWidth);
		//保存前一次的单元宽度
		siPreUnitWidth = unitWidth;
	}
	return ret;
}


/*
	写内存参数检查
*/
bool coreCmdWriteMemoryCheckSubArgv(string& cmd){

	bool ret = true;
	vector<string> parameterList = getSubArgvFromCmd(cmd,",");
	vector<string>::size_type length = parameterList.size();
	USER_INPUT_W_MODE input_mode = INPUT_ERROR;
	vector<string> spaceList  = getSubArgvFromCmd(cmd);
	if(spaceList.size() > 1){
		std::cout << "命令格式错误" << std::endl;
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
				std::cout << "命令格式错误" << std::endl;
				ret = false;
				break;
	}
	
	if(ret == true){
		ret = checkWriteAddrIsValid(startAddr,unitWidth,input_mode);
	}
	return ret;
}


/*************************************内存写操作**************************************/


 /* 写内存操作 */
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

    /* 每次写内存操作的字节宽度 */
    eachReadSize = m_defaultControlMemWidth;

    /* 循环执行操作 */
    while(1)
    {
        /* 读取当前操作内存 */
        memset(msgBuf, 0, sizeof(msgBuf));
        ret = readMemUnit(currentWriteMemAddr, eachReadSize, msgBuf, sizeof(msgBuf));
        if(SUCC != ret)
        {
            return FAIL;
        }
		if(strcmp(msgBuf,"E01") == 0){
			printf("内存无法访问\n");
			return FAIL;
		}
        pMem = msgBuf;

        /* 打印当前内存值 */
        printf("0x%08x:  ", currentWriteMemAddr);
        putMemUnit(pMem, m_defaultControlMemWidth);
        printf("-");

        /* 从命令行读入一行命令 */
        pgetstr = gets_s(inputbuf, sizeof(inputbuf));
        if( (NULL == pgetstr) || (0 == *pgetstr) )
        {
            currentWriteMemAddr += m_defaultControlMemWidth;
            continue;
        }

        /* 输入的是结束操作符,则退出内存读操作 */
        if(0 == strncmp(inputbuf, ".",strlen(".")) )
        {
            return SUCC;
        }

		/* 检查输入字符是否合法,输入的字符应该为16进制字符串 */
		for(i = 0; i < strlen(inputbuf); i++)
		{
			if( !isxdigit(inputbuf[i]))
			{
				cout << "输入非法字符" << endl;
				return FAIL;
			}
		}

		saveWriteAddr = currentWriteMemAddr;
        inputstrlen = strlen(inputbuf);

        /* 如果用户输入以0x开头的字符,去掉开头字符 */
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

        /* 输入数据超出限制,提示错误信息,并退出 */
        if( inputstrlen > (2*m_defaultControlMemWidth) )
        {
            printf("溢出\n");
            return FAIL;
        }

        /* 按0扩充内存内容为指定长度 */
        for(i = 1; i <= inputstrlen; i++)
        {
            inputbuf[(2*m_defaultControlMemWidth) - i] = inputbuf[inputstrlen-i];
        }

        for(i = 0; i < (2*m_defaultControlMemWidth - inputstrlen); i++)
        {
            inputbuf[i] = '0';
        }

        /* 添加结束符  */
        inputbuf[(2*m_defaultControlMemWidth)] = '\0';


        if( 2 == m_defaultControlMemWidth )
        {
            /* 转换字节序*/
            swapMemUnit16Bit(inputbuf);
        }

        if( 4 == m_defaultControlMemWidth )
        {
            /* 转换字节序*/
            swapMemUnit32Bit(inputbuf);
        }

        /* 将字符流转换为内存值 */
        hex2memEx(inputbuf, 2*m_defaultControlMemWidth);

        /* 向目标机端发送写内存操作,完成内存写操作 */
        ret = writeMemUnit(currentWriteMemAddr, m_defaultControlMemWidth, inputbuf, sizeof(inputbuf));
        if(FAIL == ret)
        {
			cout << "写内存操作失败" << endl;
            return FAIL;
        }

        currentWriteMemAddr += m_defaultControlMemWidth;

		//siPreAdddr = currentWriteMemAddr;
		setPreWriteValue(saveWriteAddr+m_defaultControlMemWidth,m_defaultControlMemWidth);

    }

    return SUCC;
 }





/* 基本读内存操作单元 */
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

    /* 组装写内存命令包格式Xaddr,size:xxxx */
    len = sprintf(pbuf, "X%x,%x:%s", addr, writesize, pwrtbuf);

    /* 发送命令包到shell agent */
    ret = sendDataToShellAgent( pbuf, len );
    if(ret < 0)
    {
        return FAIL;
    }

    /* 接收读取结果 */
    memset(pbuf, 0, sizeof(pbuf));
    ret = recvDataFromTs( pbuf, pbufsize );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "接收数据失败" << endl;
        return FAIL;
    }

    /* 判断执行结果 */
    if(0 != strcmp(pbuf, "OK"))
    {
        return FAIL;
    }

    return SUCC;
}



/* 
	写内存操作 
*/


int coreCmdProcess::m_writeMemory(string cmd)
{
	int ret = 0;
	unsigned int width = siCurrentUnitWidth;
    unsigned int writeMemStartAddr = 0;
	

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

	
	writeMemStartAddr = siCurrentAdddr&~(siCurrentUnitWidth-1);

    
    /* 保存用户输入的内存长度操作 */
    setDefaultControlMemWidth(width);

    /* 向目标机端读内存 */
    ret = writeMemLoop(writeMemStartAddr);
    if(SUCC != ret)
    {

        return FAIL;
    }

    return SUCC;
}