#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];

/*
	��ȡ�ļ��Ļ�������
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
/* ��ӷ��ű��ļ�*/
int consoleManager::m_addSymbol(char* pFilePath)
{
    /* ���������Ŀ�������*/
    if(curTargetAID < 0)
    {
        printf("���Ȱ�Ŀ���\n");
        return FAIL;
    }

    /*Ϊ��ӷ��ű�ͷ��ű��б��ļ���׼��*/
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

    /*��ȡ�û�����·���е��ļ���*/
    elfname = strrchr(pFilePath, '\\');
    if(NULL == elfname || strlen(pFilePath) >= MAX_CHARS - strlen("symbolFiles\\") )
    {
        printf("�����·�����Ϸ�\n");
        return FAIL;
    }
    
    elfname++;
    strcat(fullSymbolFileName, elfname);    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"
	

    /*�����ű��б��ļ��еļ�¼*/
    if((fp = fopen(fullSymbolListName,"a+")) == NULL)
    {
        printf("���ű��б��ļ���ʧ��\n");
        printErrorInfo();
        return FAIL;
	}
	
	{
		char readElfFileBuff[5] = {0};
		FILE *file_elf = NULL;
		/*����Ƿ���elf�ļ�*/
		if (!(file_elf = fopen(pFilePath,"rb")))
		{
			printf("���ű��ļ���ʧ��\n");
			printErrorInfo();
			return FAIL;
		}
		fread(readElfFileBuff,sizeof(char),4,file_elf);
		fclose(file_elf);
		if((readElfFileBuff[0]!= 0x7f)||(readElfFileBuff[1]!='E')|| \
			(readElfFileBuff[2]!='L')||(readElfFileBuff[3]!='F'))
		 {
		 printf("�ļ�����ELF�ļ�\n");
		 return FAIL;
		 }
		
	}

    fseek( fp, 0, SEEK_SET);
	char* pFilePathBasename = getBasename(pFilePath);
    while(fgets(readSymbolListBuff, MAX_PATH_LEN, fp) > 0 && pFilePathBasename != NULL)
    {
//fullSymbolFileName�ļ�����#��ͷ��Ϊע���ļ���������Ϊ�û���ӵķ��ű��ļ�
		if(readSymbolListBuff[0] != '#'){
			char* readSymbolBasename = getBasename(readSymbolListBuff);
			if(readSymbolBasename[strlen(readSymbolBasename)-1] == '\n'){
				readSymbolBasename[strlen(readSymbolBasename)-1] = '\0';
			}
		 if(strcmp(readSymbolBasename, pFilePathBasename) == 0)
			{
				fclose(fp);
				printf("������ӵķ��ű�����:%s\n",readSymbolBasename);
				return SUCC;
			}
			cur_line++;
			if(cur_line >= MAX_LINES)
			{
				printf("��ӵķ��ű��ѵ������\n");
				DeleteFile(fullSymbolFileName);
				fclose(fp);
				return FAIL;
			}
		}
    }

    /*����ű��б��ļ���ӷ��ű�·��������*/
    fseek( fp, 0, SEEK_END);
    fputs(pFilePath,fp);
    fputs("\n",fp);
    
    fclose(fp);
	
    /*���Ʒ��ű�"symbolFile"�ļ���*/
    if(CopyFileA(pFilePath,fullSymbolFileName,false) == 0)
    {
        printf("��ӷ��ű����\n");
        printErrorInfo();
        return FAIL;
    }
    printf("��ӷ��ű�ɹ�\n");
    return SUCC;
    
}
/*
	�޸��ļ���ʽ��
	���ļ�·���к���'/'��������Ϊ'\'
	���ļ�·���к���'\\'��������Ϊ'\'
	���ļ�·���к���'//'��������Ϊ'\'
	���ļ�·���к���'\/'��������Ϊ'\'
	BUG:
*/
static void modifyFilePath(string&  cmd){
	
	if(cmd.size() > 0){
		char* skipIndex = const_cast<char*>(cmd.c_str());
		char* baseIndex = skipIndex;
		//�ַ����������˵�������ַ�
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
		//�����ǰ�ַ����д�����Ҫת�����ַ��������������ַ�����С
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
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}else if(length > 1 ){
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}else{
		//�޸ķ��ű��е�·������
		modifyFilePath(cmd);
	}
	return ret;

}