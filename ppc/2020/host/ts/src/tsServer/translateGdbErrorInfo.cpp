#include "translateGdbErrorInfo.h"


//������Ҫ�滻�Ĺؼ���
#define REPALCE_KEYWORD "%s"
/*
	GDB������Ϣȫ�ֱ�������
*/

GdbErrorDefine gErrorDefine[] = {
	GdbErrorDefine("No symbol table is loaded.  Use the \"file\" command","���ű��ļ�δ����"),
	GdbErrorDefine("No symbol %s in current context","��ӵķ��ű��в�����%s"),
	GdbErrorDefine("Invalid character %s in expression.","������Ч�ַ�%s"),
	GdbErrorDefine("Numeric constant too large.","��ֵ̫��"),
	GdbErrorDefine("Division by zero","��������Ϊ0"),
	GdbErrorDefine("Invalid number %s.","��Ч����%s"),
	GdbErrorDefine("A syntax error in expression, near %s.","���ʽ�Ƿ�"),
	GdbErrorDefine()
};

/*
	GDB������Ϣ����
*/
GdbErrorDefine::GdbErrorDefine(string gdbEErrorInfo,string gdbCErrorInfo){
	
	m_gdbEErrorInfo = gdbEErrorInfo;
	m_gdbCErrorInfo = gdbCErrorInfo;
	m_errorKeywordList.clear();
	InitErrorList();
}
/*
	GDB������Ϣ�еĹؼ������ʼ��
	������%s��Ϊ�ؼ��ֵķָ����
*/
void GdbErrorDefine::InitErrorList(){
	if(m_gdbEErrorInfo == "" ){
		return ;
	}
	string::size_type prePos = 0,nextPos = string::npos;
	do{
		nextPos = m_gdbEErrorInfo.find(REPALCE_KEYWORD,prePos);
		string subStr = "";
		if(nextPos == string::npos){
			subStr = m_gdbEErrorInfo.substr(prePos);
			
		}else{
			subStr = m_gdbEErrorInfo.substr(prePos,nextPos-prePos);
			prePos = nextPos +strlen(REPALCE_KEYWORD);
		}
		
		if(subStr.size() > 0){
				m_errorKeywordList.push_back(subStr);
		}
	
	}while( nextPos != string::npos );
}
string GdbErrorDefine::getGdbEErrorInfo() const{
	return m_gdbEErrorInfo;
}

/*
	GDB������Ϣ�йؼ���ȫ��ƥ��
	����ֵ��
		true��gdb������Ϣ��ȫ��ƥ��
		false:��ǰ������Ϣ����ȫƥ��
*/
bool GdbErrorDefine::isMatch(string info){

	bool ret = true;
	string::size_type errorSymbolPos = 0, prePos = 0,nextPos = 0;
	for(vector<string>::size_type index = 0;index < m_errorKeywordList.size();index++){
		
		nextPos = info.find(m_errorKeywordList[index],prePos);
		if( nextPos == string::npos){
			ret  = false;
			break;
		}else{
			prePos = nextPos + m_errorKeywordList[index].size();
		}
	}

	return ret;
}
void GdbErrorDefine::replaceErrorInfo(T_CHAR* gdbInfo,int maxBufferSize){

	//GDB������Ϣ�滻
	string replaceErrorCInfo = m_gdbCErrorInfo;
	string::size_type joinPos = 0, index = 0;
	vector<string> errorSymbolList;
	bool ret = true;
	string info = gdbInfo;
	string::size_type errorSymbolPos = 0, prePos = 0,nextPos = 0;

	//���������ؼ���������ַ���,��Ҫ���ڴ�ӡ�����Ϣ
	for(vector<string>::size_type index = 0;index < m_errorKeywordList.size();index++){
		
		nextPos = info.find(m_errorKeywordList[index],prePos);
		if( nextPos == string::npos){
			ret  = false;
			break;
		}else{
			//���������ؼ�������ĵ��ַ���
			if(nextPos-prePos > 0){
				string errorSymbol = info.substr(prePos,nextPos-prePos);
				errorSymbolList.push_back(errorSymbol);
			}
			prePos = nextPos + m_errorKeywordList[index].size();
		}
	}
	//��������ؼ�������ĩβ�����ַ��������
	if( ret && prePos < info.size()){
			string errorSymbol = info.substr(prePos);
			errorSymbolList.push_back(errorSymbol);
	}
	if(ret){
		vector<string>::size_type vIndex = 0;
		while( (vIndex < errorSymbolList.size()) && ((joinPos = replaceErrorCInfo.find(REPALCE_KEYWORD)) != string::npos) )
		{
			replaceErrorCInfo.replace(joinPos,strlen(REPALCE_KEYWORD),errorSymbolList[vIndex]);
			joinPos += errorSymbolList[vIndex].size();
			vIndex++;
		}
		//��ǰת������GDB�����ַ����Ĵ�С���С��maxBufferSize��ֱ�ӿ���,�������κδ���
		if( replaceErrorCInfo.size()+2 < string::size_type(maxBufferSize) ){
		
			strcpy(gdbInfo,replaceErrorCInfo.c_str());
		}
	}
}



//GDBӢ��ת�����ʵ��
TranslateGdbErrorInfo* TranslateGdbErrorInfo::__instance = NULL;

TranslateGdbErrorInfo::TranslateGdbErrorInfo(){
	m_gdbErrorInfo.clear();
}

TranslateGdbErrorInfo::~TranslateGdbErrorInfo(){
	m_gdbErrorInfo.clear();
}

//������Ϣע�ắ��
void TranslateGdbErrorInfo::InitErrorInfo(GdbErrorDefine* errorAttr){
	
	if( errorAttr == NULL )
		return ;
	for(int i = 0 ;errorAttr[i].getGdbEErrorInfo() != "";i++){
		m_gdbErrorInfo.push_back(errorAttr[i]);
	}
}

//����ע������
void TranslateGdbErrorInfo::destroy(){

	if(__instance != NULL){
		delete __instance;
		__instance = NULL;
	}

}

//��ȡ����ʵ��
TranslateGdbErrorInfo* TranslateGdbErrorInfo::getInstance(){
	
	if(__instance == NULL){
		__instance = new TranslateGdbErrorInfo();
		__instance->InitErrorInfo(gErrorDefine);
		atexit(destroy);
	}
	return __instance;
}

//������Ϣ������
void TranslateGdbErrorInfo::translateErrorInfo(T_CHAR* gdbInfo,int maxBufferSize){

	for(vector<GdbErrorDefine>::size_type index = 0 ; index < m_gdbErrorInfo.size(); index++){
	
		if(m_gdbErrorInfo[index].isMatch(gdbInfo)){
			m_gdbErrorInfo[index].replaceErrorInfo(gdbInfo,maxBufferSize);
			break;
		}
	
	}
}