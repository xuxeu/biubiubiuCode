#include "CheckSubArgv.h"
#include <string>
#include <vector>
using namespace std;
/*
	参数：
		cmd:需要过滤的字符串
	返回值:
		无
*/
void skipInvalidChFormCmd(string & cmd){
	
	if(cmd.size() > 0){
		char* skipIndex = const_cast<char*>(cmd.c_str());
		char* baseIndex = skipIndex;
		//字符串前面的空格以及制表符号
		while(*skipIndex){
			if(*skipIndex != '\t' && *skipIndex !=  ' '){
				break;
			}
			skipIndex++;
		}
		//去掉字符串中多余的空格以及指标符,同时将'\t'转换为空格
		while(*skipIndex){
			if ((*skipIndex == '\t' || *skipIndex == ' ') && (*(skipIndex+1) == '\t' || *(skipIndex+1) == ' ')){
				skipIndex++;
				continue;	
			}else{
				if(*skipIndex == '\t'){
					*skipIndex = ' ';
				}
				*baseIndex++ = *skipIndex++;
			}
		} 
		//去掉字符串中末尾的空格
		if(    baseIndex > cmd.c_str() ) {
			if(*(baseIndex-1) == ' ' || *(baseIndex-1) =='\t'){

				*--baseIndex= '\0';
			}else{

				*baseIndex = '\0';
			}
			cmd.resize(baseIndex-cmd.c_str());
		}//end of if
	}//end of if(cmd.size() > 0)

}

/*
	从命令列表中获取二级参数列表
	参数：
		cmd:需要解析的字符串
		findStr:查找字符串
	返回值：字符串列表
*/
	
vector<string> getSubArgvFromCmd(string cmd,string findStr){
	
	vector<string> retList ;
	string::size_type findPos = 0;
	retList.clear();
	//过滤掉第一个空格之前的数据
	findPos = cmd.find_first_of(" ");
	while( findPos != string::npos){
		string subArgv ;
		string::size_type nextPos = -1;
		nextPos = cmd.find_first_of(findStr,findPos+1);	
		if(nextPos == string::npos){
			subArgv = cmd.substr(findPos+1,cmd.size() - findPos);
			retList.push_back(subArgv);
			break;
		}else{
			subArgv = cmd.substr(findPos+1,nextPos-findPos-1);
			retList.push_back(subArgv);
		}
		findPos = nextPos;
	}
	return retList;
}


/*
	检查地址空间是否溢出
	参数：
		baseAddr:数据的起始地址
		unitCount:单元数量
		unitSize:单元长度
		isShowErrorFlags:是否显示错误信息
	返回值：
		true:表示溢出
		false:表示没有溢出
*/
bool checkAddrIsOverflow(unsigned int  baseAddr,unsigned int unitCount,unsigned int unitSize,bool isShowErrorFlags ){

	bool ret = false;
	if( baseAddr >= 0 && baseAddr <= 0xffffffff){
		//当前内存空间中剩余的字节大小，按照输入的单元格式对齐
		unsigned int leftSize = (0xffffffff - baseAddr )&~(unitSize-1);
		//当前内存空间中按照输入单元格式对齐后剩余的字节大小
		unsigned int baseAddrLeftSize = ( 0xffffffff - baseAddr ) | (unitSize-1);
		//当前内存空间中，按照单元格式对齐后，可以读取的对大数量
		unsigned int leftCount = (leftSize >>(unitSize/2)) == 0xffffffff ? (leftSize >>(unitSize/2)):(leftSize >>(unitSize/2))+1;
		//整个内存空间中剩余的可以访问的数量
		unsigned int totalLeftSize = 0xffffffff -  baseAddr ;

		if( unitCount > leftCount){
			if(isShowErrorFlags){
				std::cout << "溢出" << std::endl;
			}
			ret = true;
		}else{
			if( baseAddrLeftSize > totalLeftSize && leftCount == unitCount){
				if(isShowErrorFlags){
				std::cout << "溢出" << std::endl;
				}
				ret = true;
			}else{
				ret  = false;
			}
		}
		
	}else{
		if(isShowErrorFlags){
			std::cout << "溢出" << std::endl;
		}
		ret = true;
		
	}
	return ret;

}

/*
	获取无符号整形
	参数;uiGetNoStr:无符号整形字符串
		  uiGetNo:待转换的无符号整形
		  zeroIsValid:字符0是否有效
    返回值:
		true:转换成功
		false:转换失败
*/
bool getUINT(string uiGetNoStr,unsigned int& uiGetNo,bool zeroIsValid ,int *errNum ){
#define MAX_UI_STR "4294967295"
	bool ret = true;
	uiGetNo = 0;
	string checkMaxStr = MAX_UI_STR;
	if(uiGetNoStr == "0" && zeroIsValid == false){
		//std::cout << uiGetNoStr <<":当前数字,请输入有效的十进制数字1~MAX_UINT"<<std::endl; 
		if(NULL != errNum){
			*errNum = 1;
		}
		ret = false;
	}else if(uiGetNoStr.size() <= checkMaxStr.size() && uiGetNoStr.size() > 0 ){
		const char* pUnitCout = uiGetNoStr.c_str();
		while( *pUnitCout ){
			
			if(*pUnitCout < '0' || *pUnitCout > '9'){
				//std::cout << uiGetNoStr <<":当前输入数字无效,请输入有效的无符号十进制数字"<<std::endl; 
				ret = false;
				if(NULL != errNum){
					*errNum = 1;
				}
				break;
			}
			
			if( ( pUnitCout - uiGetNoStr.c_str() + 1) == checkMaxStr.size()){
				if(*pUnitCout > '5'){
					//std::cout << uiGetNoStr <<":当前数字无效,超过MAX_UINT,请输入有效的无符号十进制数字1~MAX_UINT"<<std::endl; 
					ret = false;
					if(NULL != errNum){
						*errNum = 2;
					}
					break;	
				}
			}
			uiGetNo = uiGetNo* 10 + ( *pUnitCout-'0' );
			pUnitCout++;
		}
	}else{
		//std::cout << uiGetNoStr <<":当前数字无效,超过MAX_UINT,请输入有效的十进制数字1~MAX_UINT"<<std::endl; 
		if(NULL != errNum){
			*errNum = 2;
		}
		ret = false;
	}
	return ret;
}
/*
	C解析器模式下检查是否包含中文字符
*/
bool checkChineseCh(string str){
	bool ret = true;
	if(str.size() > 0){
		char* index = const_cast<char*>(str.c_str());
		while(*index){
			if(*index < 0 && *(index+1)< 0){
				ret = false;
				break;
			}
			index++;
		}
	
	}
	return ret;
	
}
