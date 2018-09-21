#include "CheckSubArgv.h"
#include <string>
#include <vector>
using namespace std;
/*
	������
		cmd:��Ҫ���˵��ַ���
	����ֵ:
		��
*/
void skipInvalidChFormCmd(string & cmd){
	
	if(cmd.size() > 0){
		char* skipIndex = const_cast<char*>(cmd.c_str());
		char* baseIndex = skipIndex;
		//�ַ���ǰ��Ŀո��Լ��Ʊ����
		while(*skipIndex){
			if(*skipIndex != '\t' && *skipIndex !=  ' '){
				break;
			}
			skipIndex++;
		}
		//ȥ���ַ����ж���Ŀո��Լ�ָ���,ͬʱ��'\t'ת��Ϊ�ո�
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
		//ȥ���ַ�����ĩβ�Ŀո�
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
	�������б��л�ȡ���������б�
	������
		cmd:��Ҫ�������ַ���
		findStr:�����ַ���
	����ֵ���ַ����б�
*/
	
vector<string> getSubArgvFromCmd(string cmd,string findStr){
	
	vector<string> retList ;
	string::size_type findPos = 0;
	retList.clear();
	//���˵���һ���ո�֮ǰ������
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
	����ַ�ռ��Ƿ����
	������
		baseAddr:���ݵ���ʼ��ַ
		unitCount:��Ԫ����
		unitSize:��Ԫ����
		isShowErrorFlags:�Ƿ���ʾ������Ϣ
	����ֵ��
		true:��ʾ���
		false:��ʾû�����
*/
bool checkAddrIsOverflow(unsigned int  baseAddr,unsigned int unitCount,unsigned int unitSize,bool isShowErrorFlags ){

	bool ret = false;
	if( baseAddr >= 0 && baseAddr <= 0xffffffff){
		//��ǰ�ڴ�ռ���ʣ����ֽڴ�С����������ĵ�Ԫ��ʽ����
		unsigned int leftSize = (0xffffffff - baseAddr )&~(unitSize-1);
		//��ǰ�ڴ�ռ��а������뵥Ԫ��ʽ�����ʣ����ֽڴ�С
		unsigned int baseAddrLeftSize = ( 0xffffffff - baseAddr ) | (unitSize-1);
		//��ǰ�ڴ�ռ��У����յ�Ԫ��ʽ����󣬿��Զ�ȡ�ĶԴ�����
		unsigned int leftCount = (leftSize >>(unitSize/2)) == 0xffffffff ? (leftSize >>(unitSize/2)):(leftSize >>(unitSize/2))+1;
		//�����ڴ�ռ���ʣ��Ŀ��Է��ʵ�����
		unsigned int totalLeftSize = 0xffffffff -  baseAddr ;

		if( unitCount > leftCount){
			if(isShowErrorFlags){
				std::cout << "���" << std::endl;
			}
			ret = true;
		}else{
			if( baseAddrLeftSize > totalLeftSize && leftCount == unitCount){
				if(isShowErrorFlags){
				std::cout << "���" << std::endl;
				}
				ret = true;
			}else{
				ret  = false;
			}
		}
		
	}else{
		if(isShowErrorFlags){
			std::cout << "���" << std::endl;
		}
		ret = true;
		
	}
	return ret;

}

/*
	��ȡ�޷�������
	����;uiGetNoStr:�޷��������ַ���
		  uiGetNo:��ת�����޷�������
		  zeroIsValid:�ַ�0�Ƿ���Ч
    ����ֵ:
		true:ת���ɹ�
		false:ת��ʧ��
*/
bool getUINT(string uiGetNoStr,unsigned int& uiGetNo,bool zeroIsValid ,int *errNum ){
#define MAX_UI_STR "4294967295"
	bool ret = true;
	uiGetNo = 0;
	string checkMaxStr = MAX_UI_STR;
	if(uiGetNoStr == "0" && zeroIsValid == false){
		//std::cout << uiGetNoStr <<":��ǰ����,��������Ч��ʮ��������1~MAX_UINT"<<std::endl; 
		if(NULL != errNum){
			*errNum = 1;
		}
		ret = false;
	}else if(uiGetNoStr.size() <= checkMaxStr.size() && uiGetNoStr.size() > 0 ){
		const char* pUnitCout = uiGetNoStr.c_str();
		while( *pUnitCout ){
			
			if(*pUnitCout < '0' || *pUnitCout > '9'){
				//std::cout << uiGetNoStr <<":��ǰ����������Ч,��������Ч���޷���ʮ��������"<<std::endl; 
				ret = false;
				if(NULL != errNum){
					*errNum = 1;
				}
				break;
			}
			
			if( ( pUnitCout - uiGetNoStr.c_str() + 1) == checkMaxStr.size()){
				if(*pUnitCout > '5'){
					//std::cout << uiGetNoStr <<":��ǰ������Ч,����MAX_UINT,��������Ч���޷���ʮ��������1~MAX_UINT"<<std::endl; 
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
		//std::cout << uiGetNoStr <<":��ǰ������Ч,����MAX_UINT,��������Ч��ʮ��������1~MAX_UINT"<<std::endl; 
		if(NULL != errNum){
			*errNum = 2;
		}
		ret = false;
	}
	return ret;
}
/*
	C������ģʽ�¼���Ƿ���������ַ�
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
