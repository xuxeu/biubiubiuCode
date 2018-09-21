#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];
/* ɾ�����ű��ļ�*/
int consoleManager::m_subSymbol(char *pFilePath)
{
    /* ���������Ŀ�������*/
    if(curTargetAID < 0)
    {
        printf("û�а�Ŀ���\n");
        return FAIL;
    }


    /*Ϊɾ�����ű�ͷ��ű��б��ļ���׼��*/
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


    /*��ȡ�û�����·���е��ļ���*/
    elfname = strrchr(pFilePath, '\\');
    if( NULL == elfname )
    {
        printf("�����·�����Ϸ�\n");
        return FAIL;
    }
    
    elfname++;
    strcat(fullSymbolFileName, elfname);    //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"

    /*��ѯ���ű��б��ļ��еļ�¼*/
    if((fp = fopen(fullSymbolListName,"r")) == NULL)
    {
        printf("���ű��б��ļ���ʧ��!\n");
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
		printf("���ű�δ���\n");
		return FAIL;
	}
    if( i >= MAX_LINES )
    {
        printf("���ű��б��ļ���¼����\n");
        fclose(fp);
        return FAIL;
    }
    fclose(fp);

    /*���µļ�¼д��*/
    if((fp = fopen(fullSymbolListName,"w")) == NULL)
    {
        printf("���ű��б��ļ���ʧ��\n");
        printErrorInfo();
        return FAIL;
    }
    
    for(j=0; j<i; j++)
    {
        fprintf(fp, "%s\n", fileContent[j]);
    }
    
    fclose(fp);
    
    /*ɾ��"symbolFile"�ļ����� �ķ��ű�*/
    if(DeleteFile(fullSymbolFileName) == false && find_ok != 0)
    {
        printf("ɾ�����ű�ʧ��\n");
        printErrorInfo();
        return FAIL;
    }
	
    printf("ɾ�����ű�ɹ�\n");
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
bool sysCmdSubSymbolCheckSubArgv(string& cmd){
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