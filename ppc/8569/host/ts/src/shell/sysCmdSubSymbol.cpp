#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 当前目标机连接的AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];
/* 删除符号表文件*/
int consoleManager::m_subSymbol(char *pFilePath)
{
    /* 如果不存在目标机连接*/
    if(curTargetAID < 0)
    {
        printf("没有绑定目标机\n");
        return FAIL;
    }


    /*为删除符号表和符号表列表文件做准备*/
    FILE *fp;
    char *elfname;
    char fileContent[MAX_LINES][MAX_PATH_LEN] = {0};
    int i = 0, j,find_ok=0;
    char fullSymbolListName[MAX_PATH_LEN] = {0};
    char fullSymbolFileName[MAX_PATH_LEN] = {0};
    char readSymbolListBuff[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFileName, cur_path, MAX_PATH_LEN); //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\"
    strcat(fullSymbolFileName, "symbolFiles\\");     //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\"
    strcat(fullSymbolFileName, curTargetName);   //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"
    strncpy(fullSymbolListName, fullSymbolFileName, MAX_PATH_LEN);   //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"

    strcat(fullSymbolFileName, "\\");    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
    strcat(fullSymbolListName, ".txt");           //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name.txt"


    /*获取用户输入路径中的文件名*/
    elfname = strrchr(pFilePath, '\\');
    if( NULL == elfname )
    {
        printf("输入的路径不合法\n");
        return FAIL;
    }
    
    elfname++;
    strcat(fullSymbolFileName, elfname);    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"

    /*查询符号表列表文件中的记录*/
    if((fp = fopen(fullSymbolListName,"r")) == NULL)
    {
        printf("符号表列表文件打开失败!\n");
        printErrorInfo();
        return FAIL;
    }
    
    while((i < MAX_LINES ) && fgets(readSymbolListBuff, MAX_PATH_LEN, fp) > 0)
    {
		if(readSymbolListBuff[strlen(readSymbolListBuff)-1] == '\n'){
				readSymbolListBuff[strlen(readSymbolListBuff)-1] = '\0';
		}
        if(strcmp(pFilePath, readSymbolListBuff) != 0)
        {
            strncpy(fileContent[i++], readSymbolListBuff, MAX_PATH_LEN);
		}else{
			find_ok = 1;
		}
    }
	if( find_ok == 0){
		printf("符号表未添加\n");
		return FAIL;
	}
    if( i >= MAX_LINES )
    {
        printf("符号表列表文件记录有误\n");
        fclose(fp);
        return FAIL;
    }
    fclose(fp);

    /*将新的记录写入*/
    if((fp = fopen(fullSymbolListName,"w")) == NULL)
    {
        printf("符号表列表文件打开失败\n");
        printErrorInfo();
        return FAIL;
    }
    
    for(j=0; j<i; j++)
    {
        fprintf(fp, "%s\n", fileContent[j]);
    }
    
    fclose(fp);
    
    /*删除"symbolFile"文件夹下 的符号表*/
    if(DeleteFile(fullSymbolFileName) == false && find_ok != 0)
    {
        printf("删除符号表失败\n");
        printErrorInfo();
        return FAIL;
    }
	
    printf("删除符号表成功\n");
    return SUCC;
}
/*
	修改文件格式：
	若文件路径中含有'/'符号则处理为'\'
	若文件路径中含有'\\'符号则处理为'\'
	若文件路径中含有'//'符号则处理为'\'
	若文件路径中含有'\/'符号则处理为'\'
	BUG:
*/
static void modifyFilePath(string&  cmd){
	
	if(cmd.size() > 0){
		char* skipIndex = const_cast<char*>(cmd.c_str());
		char* baseIndex = skipIndex;
		//字符传处理，过滤掉多余的字符
		while(*skipIndex){
			if(*skipIndex == '\\' && (*(skipIndex+1) == '\\' || *(skipIndex+1) == '/') ){
				skipIndex++;
			}else if(*skipIndex =='/' && *(skipIndex+1) == '/'){
				skipIndex++;
			}else{
				if(*skipIndex == '/'){
					*skipIndex = '\\';
				}
				*baseIndex++ = *skipIndex++;
			}
		
		}//endof while
		//如果当前字符串中存在需要转换的字符，则重新设置字符串大小
		if( baseIndex > cmd.c_str()  && skipIndex > baseIndex){
			*baseIndex = '\0';
			cmd.resize(baseIndex-cmd.c_str());
		}
	}//end of if cmd.size()
}
bool sysCmdSubSymbolCheckSubArgv(string& cmd){
	bool ret = true;
	vector<string> parameterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = parameterList.size();
	if( 0 == length){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}else if(length > 1 ){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}else{
		//修改符号表中的路径参数
		modifyFilePath(cmd);
	}
	
	return ret;


}