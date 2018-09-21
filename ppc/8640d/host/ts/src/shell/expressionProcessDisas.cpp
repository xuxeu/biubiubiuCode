#include "expressionProcess.h"
#include "CheckSubArgv.h"

/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接的目标机名*/
extern char curTargetName [NAME_SIZE + 1];

static const basic_string <char>::size_type npos = -1;

/*处理反汇编指令*/
int expressionProcess::m_processDisas(string &cmd)
{
    int len = 0;
    int scanfret = 0;
    int nlines = 0;
    char disasCmd[MAX_MSG_SIZE] = {'\0'};
    char expBuf[MAX_MSG_SIZE] = {'\0'};
    string::size_type noblankpos = 0, commapos = 0;

    /* 删除命令字"l" */
    cmd.erase(0,1);

     /* 查找第一个非空格的字符,并删除空格之前内容 */
    noblankpos = cmd.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "输入的反汇编命令格式错误" << endl;
        return FAIL;
    }
    cmd.erase(0, noblankpos);

    /* 解析命令字中的逗号 */
    commapos = cmd.find(",");
    if( (0 == commapos) || (npos == commapos) )
    {
        /* 命令字中不存在逗号,直接拷贝为反汇编命令字  */
        memcpy(disasCmd, cmd.c_str(), cmd.size());
    }
    else if (commapos > 0)
    {
        /* 指定反汇编行数,解析命令字 */
        strncpy(disasCmd, cmd.c_str(), commapos);

        cmd.erase(0, commapos+1);

        scanfret = sscanf( cmd.c_str(), "%d", &nlines );

        if( (1 == scanfret) && (nlines > 0) )
        {
            if(nlines > DISAS_MAX_LINES )
            {
                /*如果大于64行则设置为默认值*/
                nlines = DEFAULT_LINES;
            }
        }
        else
        {
            cout << "输入的反汇编命令格式错误" << endl;
            return FAIL;
        }
    }

    /* 组装查询数据包,格式o:TargetAID;symbolFile;disassemble expression */
    len = sprintf( expBuf, "o:%x;%s;x/%di %s", curTargetAID, curTargetName, nlines, disasCmd );

    /* 将数据发送到tsServer */
    sendDataToTsServer(expBuf, len);
    return SUCC;

}
//检查当前字符是否是字母、数字和下划线
static bool checkCharOfNameIsCStandard(char ch){
	
	bool ret ;
	if( ch == '_'){
		ret = true;
	}else if( ch >= '0' && ch <= '9'){
		ret = true;
	}else if( ch >= 'a' && ch <= 'z'){
		ret = true;
	}else if( ch >= 'A' && ch <= 'Z') {
		ret = true;
	}else{
		ret = false;
	}

}

//检查当前函数名称是否符合C语言标准规范
static bool checkFunctionNameIsValid(string FunctionName){
	bool ret = true;
	if(FunctionName.size() > 0){
		const char* pIdentifier = FunctionName.c_str();
		ret = checkCharOfNameIsCStandard(*pIdentifier);
		
		if(ret){
			if( *pIdentifier >= '0' && *pIdentifier <= '9' ){
				ret = false;
			}else{
				while(*pIdentifier){
					ret = checkCharOfNameIsCStandard(*pIdentifier);
					if( ret == false) break;
					pIdentifier++;
				}//end of while	
			}
		}//end if(ret)
	}else{
		ret = false;
	}
	
	return ret;
}
/*
	反汇编查看命令参数检查
*/
bool expressionCmdDisasCheck(string& cmd){
	bool ret = true;
	vector<string> parameterList = getSubArgvFromCmd(cmd,",");
	vector<string>::size_type length = parameterList.size();

	unsigned int defaultMaxLine = 0 ;
	
	switch(length){

		case 2:
			ret = getUINT(parameterList[1],defaultMaxLine,true);
			if( ret == false){
				break;
			}
		case 1:
#if 0
				ret = checkFunctionNameIsValid(parameterList[0]);
#endif	

			break;
		default:
			ret = false;
			break;
	}

	if(ret == false){
		std::cout <<"命令格式错误"<< std::endl;
	}
	return ret;

}