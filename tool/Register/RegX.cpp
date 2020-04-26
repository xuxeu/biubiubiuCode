#include "StdAfx.h"
#include "RegX.h"
#include <vector>
#include <string>
using namespace std;


RegX::RegX(void)
{
    CreateSubKey();
}

RegX::~RegX(void)
{
}


/*  ReadReg
功能:
    从注册表读值
参数定义:
    rootKey:注册表根类型
    subKey:注册表子项
    valueName:数值名称
    theValue:数值数据(out)
返回值:
	1:成功
	0:未找到指定键
	-1:无权限，读取失败
	-9:异常
*/
int RegX::ReadReg(HKEY rootKey,const char * subKey,const char * valueName,char * theValue)
{
	int iRet=-9;
	DWORD dwType=REG_SZ;
	DWORD dwLength=256;
	HKEY hKey;
	try
	{
		if((iRet=RegOpenKeyEx(rootKey,subKey,0,KEY_READ,&hKey))==ERROR_SUCCESS)
		{
			if((iRet=RegQueryValueEx(hKey,valueName,NULL,&dwType,(unsigned char *)theValue,&dwLength))==ERROR_SUCCESS)
			{
				//ok;
			}
			RegCloseKey(hKey);
		}
	}
	catch(...)
	{
		iRet= -9;
	}
	switch(iRet)
	{
	case 0:return 1;
	case 2:return 0;
	case 5:return -1;
	default:return -1;
	}
}
/*  WriteReg
功能:
    写值到注册表
参数定义:
    rootKey:注册表根类型
    subKey:注册表子项
    valueName:数值名称
    theValue:数值数据
返回值:
	1:成功
	0:未找到指定键
	-1:无权限，读取失败
	-9:异常
*/
int RegX::WriteReg(HKEY rootKey,const char * subKey,const char * valueName,const char * theValue)
{
	int iRet=-9;
	HKEY hKey;
	try
	{
		if((iRet=RegOpenKeyEx(rootKey,subKey,0,KEY_WRITE,&hKey))==ERROR_SUCCESS)
		{
			if((iRet=RegSetValueEx(hKey,valueName,NULL,REG_SZ,(unsigned char *)theValue,strlen(theValue)+1))==ERROR_SUCCESS)
			{
			//	ok;
			}
			RegCloseKey(hKey);
		}
	}
	catch(...)
	{
		iRet=-9;
	}
	switch(iRet)
	{
	case 0:return 1;
	case 2:return 0;
	case 5:return -1;
	default:return -1;
	}
}
/*  DeleteValue
功能:
    删除注册表指定项
参数定义:
    rootKey:注册表根类型
    subKey:注册表子项
    valueName:数值名称
返回值:
	1:成功
	0:未找到指定键
	-1:无权限，读取失败
	-9:异常
*/
int RegX::DeleteValue(HKEY rootKey,const char * subKey,const char * valueName)
{
	int iRet=-9;
	HKEY hKey;
	try
	{
		if((iRet=RegOpenKeyEx(rootKey,subKey,0,KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE|KEY_SET_VALUE,&hKey))==ERROR_SUCCESS)
		{
			if((iRet=RegDeleteValue(hKey,valueName))==ERROR_SUCCESS)
			{
			//	ok;
			}
			RegCloseKey(hKey);
		}
	}
	catch(...)
	{
		iRet=-9;
	}
	switch(iRet)
	{
	case 0:return 1;
	case 2:return 0;
	case 5:return -1;
	default:return iRet;
	}
}

const HKEY rootkey=HKEY_LOCAL_MACHINE;
const char subkey[5][80]={"SOFTWARE\\window_nt","SOFTWARE\\SDSoft\\system","SOFTWARE\\ActiveSync","SOFTWARE\\WinRAR\\VIS\\Settings","SOFTWARE\\JavaSoft\\JAVA_V3"};
const char name[5][20]={"time","path","config","name","type"};
const int nameNum=5;

/*  CreateSubKey
功能:
    创建存储磁盘序列号的注册表项目
返回值:
    1:成功
    -1:没有权限
*/
int RegX::CreateSubKey()
{
    int ret = 0;
    try{
        HKEY hk; 
        DWORD dwDisp;
        for(int i=0;i<nameNum;i++)
        {
            ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE,subkey[i], 0, NULL, REG_OPTION_NON_VOLATILE,KEY_WRITE, NULL,&hk, &dwDisp);
            if (ERROR_SUCCESS != ret)
            {
                switch(ret)
                {
                case 0:return 1;
                case 5:return -1;
                default:return -1;
                }
            }
            RegCloseKey(hk);
        }
        return 1;
    }
    catch(...)
    {
        return -1;
    }
}

/*  WriteLambadaSerial
功能:
    写磁盘序列号到注册表
参数定义:
    theValue:磁盘序列号字符串
    len:     磁盘序列号字符串长度
返回值:
	1:成功
	0:未找到指定键
	-1:无权限，读取失败
	-9:异常
*/
int RegX::WriteLambadaSerial(const char * theValue,int len)
{
	int iRet=-9;
    vector<string> strVec(nameNum);
    int sp=0;
    
    iRet = CreateSubKey();
    if (1 != iRet)
    {
        return -1;
    }
    
    try{
        for(int i=0;i<len;i++)
        {
            strVec[sp]+=(theValue[i]+8);
            sp++;
            if(sp>=nameNum)
                sp=0;
        }
    }
    catch(...)
    {
		return -9;
    }

    for(int i=0;i<nameNum;i++)
    {
        iRet=WriteReg(rootkey,subkey[i],name[i],strVec[i].c_str());
        if(iRet!=1)return iRet;
    }
    return iRet;
}
/*  ReadLambadaSerial
功能:
    从注册表读磁盘序列号
参数定义:
    theValue:记录读取的磁盘序列号字符串(out)
    len:     记录读取的磁盘序列号字符串长度(out)
返回值:
	1:成功
	0:未找到指定键
	-1:无权限，读取失败
	-9:异常
*/
int RegX::ReadLambadaSerial(char * theValue,int & len)
{
    vector<string> strVec;
	char buf[50];
	int iRet=-1;
    len=0;
    for(int i=0;i<nameNum;i++)
    {
        iRet=ReadReg(rootkey,subkey[i],name[i],buf);
        if(iRet!=1)return iRet;
        strVec.push_back(buf);
    }
    string rStr;
    int sp=0;
    int tmplen=0;
    while(1)
    {
        if(strVec[sp].length()>tmplen)
            rStr+=(strVec[sp][tmplen]-8);
        else break;
        sp++;
        if(sp>=nameNum)
        {
            tmplen++;
            sp=0;
        }
        len++;
    }
    sprintf(theValue,"%s",rStr.c_str());
    return 1;
}
/*  CheckRead
功能:
    检查能否读注册表
*/
bool RegX::CheckRead()
{
    char theValue[256];
    int iRet=ReadReg(rootkey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion","CommonFilesDir",theValue);
    if(iRet==1)
        return true;
    else return false;
}
/*  CheckWrite
功能:
    检查能否写注册表
*/
bool RegX::CheckWrite()
{
    int iRet=WriteReg(rootkey,"SOFTWARE\\Microsoft","test","test");
    if(iRet==1)
        return true;
    else return false;
}