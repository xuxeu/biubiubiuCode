#include "translateGdbErrorInfo.h"


//定义需要替换的关键字
#define REPALCE_KEYWORD "%s"
/*
	GDB错误信息全局变量定义
*/

GdbErrorDefine gErrorDefine[] = {
	GdbErrorDefine("No symbol table is loaded.  Use the \"file\" command","符号表文件未加载"),
	GdbErrorDefine("No symbol %s in current context","添加的符号表中不存在%s"),
	GdbErrorDefine("Invalid character %s in expression.","存在无效字符%s"),
	GdbErrorDefine("Numeric constant too large.","数值太大"),
	GdbErrorDefine("Division by zero","除数不能为0"),
	GdbErrorDefine("Invalid number %s.","无效数字%s"),
	GdbErrorDefine("A syntax error in expression, near %s.","表达式非法"),
	GdbErrorDefine()
};

/*
	GDB错误信息定义
*/
GdbErrorDefine::GdbErrorDefine(string gdbEErrorInfo,string gdbCErrorInfo){
	
	m_gdbEErrorInfo = gdbEErrorInfo;
	m_gdbCErrorInfo = gdbCErrorInfo;
	m_errorKeywordList.clear();
	InitErrorList();
}
/*
	GDB错误信息中的关键字体初始话
	其中以%s作为关键字的分割符号
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
	GDB错误信息中关键字全部匹配
	返回值：
		true：gdb错误信息中全字匹配
		false:当前错误信息不完全匹配
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

	//GDB中文信息替换
	string replaceErrorCInfo = m_gdbCErrorInfo;
	string::size_type joinPos = 0, index = 0;
	vector<string> errorSymbolList;
	bool ret = true;
	string info = gdbInfo;
	string::size_type errorSymbolPos = 0, prePos = 0,nextPos = 0;

	//保留除开关键字以外的字符串,主要用于打印相关信息
	for(vector<string>::size_type index = 0;index < m_errorKeywordList.size();index++){
		
		nextPos = info.find(m_errorKeywordList[index],prePos);
		if( nextPos == string::npos){
			ret  = false;
			break;
		}else{
			//保留除开关键字以外的的字符串
			if(nextPos-prePos > 0){
				string errorSymbol = info.substr(prePos,nextPos-prePos);
				errorSymbolList.push_back(errorSymbol);
			}
			prePos = nextPos + m_errorKeywordList[index].size();
		}
	}
	//处理除开关键字以外末尾还有字符串的情况
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
		//当前转换处的GDB中文字符串的大小如果小于maxBufferSize则直接拷贝,否则不做任何处理
		if( replaceErrorCInfo.size()+2 < string::size_type(maxBufferSize) ){
		
			strcpy(gdbInfo,replaceErrorCInfo.c_str());
		}
	}
}



//GDB英文转换类的实现
TranslateGdbErrorInfo* TranslateGdbErrorInfo::__instance = NULL;

TranslateGdbErrorInfo::TranslateGdbErrorInfo(){
	m_gdbErrorInfo.clear();
}

TranslateGdbErrorInfo::~TranslateGdbErrorInfo(){
	m_gdbErrorInfo.clear();
}

//错误信息注册函数
void TranslateGdbErrorInfo::InitErrorInfo(GdbErrorDefine* errorAttr){
	
	if( errorAttr == NULL )
		return ;
	for(int i = 0 ;errorAttr[i].getGdbEErrorInfo() != "";i++){
		m_gdbErrorInfo.push_back(errorAttr[i]);
	}
}

//设置注销函数
void TranslateGdbErrorInfo::destroy(){

	if(__instance != NULL){
		delete __instance;
		__instance = NULL;
	}

}

//获取对象实例
TranslateGdbErrorInfo* TranslateGdbErrorInfo::getInstance(){
	
	if(__instance == NULL){
		__instance = new TranslateGdbErrorInfo();
		__instance->InitErrorInfo(gErrorDefine);
		atexit(destroy);
	}
	return __instance;
}

//错误信息处理函数
void TranslateGdbErrorInfo::translateErrorInfo(T_CHAR* gdbInfo,int maxBufferSize){

	for(vector<GdbErrorDefine>::size_type index = 0 ; index < m_gdbErrorInfo.size(); index++){
	
		if(m_gdbErrorInfo[index].isMatch(gdbInfo)){
			m_gdbErrorInfo[index].replaceErrorInfo(gdbInfo,maxBufferSize);
			break;
		}
	
	}
}