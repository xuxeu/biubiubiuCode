#include "expressionProcess.h"
#include "CheckSubArgv.h"

/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ���ӵ�Ŀ�����*/
extern char curTargetName [NAME_SIZE + 1];

static const basic_string <char>::size_type npos = -1;

/*�������ָ��*/
int expressionProcess::m_processDisas(string &cmd)
{
    int len = 0;
    int scanfret = 0;
    int nlines = 0;
    char disasCmd[MAX_MSG_SIZE] = {'\0'};
    char expBuf[MAX_MSG_SIZE] = {'\0'};
    string::size_type noblankpos = 0, commapos = 0;

    /* ɾ��������"l" */
    cmd.erase(0,1);

     /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
    noblankpos = cmd.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "����ķ���������ʽ����" << endl;
        return FAIL;
    }
    cmd.erase(0, noblankpos);

    /* �����������еĶ��� */
    commapos = cmd.find(",");
    if( (0 == commapos) || (npos == commapos) )
    {
        /* �������в����ڶ���,ֱ�ӿ���Ϊ�����������  */
        memcpy(disasCmd, cmd.c_str(), cmd.size());
    }
    else if (commapos > 0)
    {
        /* ָ�����������,���������� */
        strncpy(disasCmd, cmd.c_str(), commapos);

        cmd.erase(0, commapos+1);

        scanfret = sscanf( cmd.c_str(), "%d", &nlines );

        if( (1 == scanfret) && (nlines > 0) )
        {
            if(nlines > DISAS_MAX_LINES )
            {
                /*�������64��������ΪĬ��ֵ*/
                nlines = DEFAULT_LINES;
            }
        }
        else
        {
            cout << "����ķ���������ʽ����" << endl;
            return FAIL;
        }
    }

    /* ��װ��ѯ���ݰ�,��ʽo:TargetAID;symbolFile;disassemble expression */
    len = sprintf( expBuf, "o:%x;%s;x/%di %s", curTargetAID, curTargetName, nlines, disasCmd );

    /* �����ݷ��͵�tsServer */
    sendDataToTsServer(expBuf, len);
    return SUCC;

}
//��鵱ǰ�ַ��Ƿ�����ĸ�����ֺ��»���
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

//��鵱ǰ���������Ƿ����C���Ա�׼�淶
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
	�����鿴����������
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
		std::cout <<"�����ʽ����"<< std::endl;
	}
	return ret;

}