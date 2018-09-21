#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 当前目标机连接的AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];

/*
	获取文件的基本名称
*/
char *getBasename(const char* pFilePath){
	char* ret = const_cast<char*> (pFilePath);
	if(pFilePath != NULL && *pFilePath != '\0'){
		while(*pFilePath){
			if(*pFilePath == '\\'&&*(pFilePath+1)!= '\0'){
				ret = const_cast<char*>(pFilePath+1);
			}
			pFilePath++;
		}
	}
	return ret;
}
/* 添加符号表文件*/
int consoleManager::m_addSymbol(char* pFilePath)
{
    /* 如果不存在目标机连接*/
    if(curTargetAID < 0)
    {
        printf("请先绑定目标机\n");
        return FAIL;
    }

    /*为添加符号表和符号表列表文件做准备*/
    FILE *fp;
    int cur_line = 0;
    char *elfname;
    char fullSymbolListName[MAX_PATH_LEN] = {0};
    char fullSymbolFileName[MAX_PATH_LEN] = {0};
    char readSymbolListBuff[MAX_PATH_LEN] = {0};

    strncpy(fullSymbolFileName, cur_path, MAX_PATH_LEN); //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\"
    strcat(fullSymbolFileName, "symbolFiles\\");     //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\"
    strcat(fullSymbolFileName, curTargetName);   //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"
    strncpy(fullSymbolListName, fullSymbolFileName, MAX_PATH_LEN);   //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"

    strcat(fullSymbolFileName, "\\");    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
    strcat(fullSymbolListName, ".txt");     //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name.txt"

    /*获取用户输入路径中的文件名*/
    elfname = strrchr(pFilePath, '\\');
    if(NULL == elfname || strlen(pFilePath) >= MAX_CHARS - strlen("symbolFiles\\") )
    {
        printf("输入的路径不合法\n");
        return FAIL;
    }
    
    elfname++;
    strcat(fullSymbolFileName, elfname);    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"
	

    /*检查符号表列表文件中的记录*/
    if((fp = fopen(fullSymbolListName,"a+")) == NULL)
    {
        printf("符号表列表文件打开失败\n");
        printErrorInfo();
        return FAIL;
	}
	
	{
		char readElfFileBuff[5] = {0};
		FILE *file_elf = NULL;
		/*检查是否是elf文件*/
		if (!(file_elf = fopen(pFilePath,"rb")))
		{
			printf("符号表文件打开失败\n");
			printErrorInfo();
			return FAIL;
		}
		fread(readElfFileBuff,sizeof(char),4,file_elf);
		fclose(file_elf);
		if((readElfFileBuff[0]!= 0x7f)||(readElfFileBuff[1]!='E')|| \
			(readElfFileBuff[2]!='L')||(readElfFileBuff[3]!='F'))
		 {
		 printf("文件不是ELF文件\n");
		 return FAIL;
		 }
		
	}

    fseek( fp, 0, SEEK_SET);
	char* pFilePathBasename = getBasename(pFilePath);
    while(fgets(readSymbolListBuff, MAX_PATH_LEN, fp) > 0 && pFilePathBasename != NULL)
    {
//fullSymbolFileName文件中以#开头的为注释文件，其他的为用户添加的符号表文件
		if(readSymbolListBuff[0] != '#'){
			char* readSymbolBasename = getBasename(readSymbolListBuff);
			if(readSymbolBasename[strlen(readSymbolBasename)-1] == '\n'){
				readSymbolBasename[strlen(readSymbolBasename)-1] = '\0';
			}
		 if(strcmp(readSymbolBasename, pFilePathBasename) == 0)
			{
				fclose(fp);
				printf("与已添加的符号表重名:%s\n",readSymbolBasename);
				return SUCC;
			}
			cur_line++;
			if(cur_line >= MAX_LINES)
			{
				printf("添加的符号表已到最大数\n");
				DeleteFile(fullSymbolFileName);
				fclose(fp);
				return FAIL;
			}
		}
    }

    /*向符号表列表文件添加符号表路径和名称*/
    fseek( fp, 0, SEEK_END);
    fputs(pFilePath,fp);
    fputs("\n",fp);
    
    fclose(fp);
	
    /*复制符号表到"symbolFile"文件夹*/
    if(CopyFileA(pFilePath,fullSymbolFileName,false) == 0)
    {
        printf("添加符号表出错\n");
        printErrorInfo();
        return FAIL;
    }
    printf("添加符号表成功\n");
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
bool sysCmdAddSymbolCheckSubArgv(string&  cmd){
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