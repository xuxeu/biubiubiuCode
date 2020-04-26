#include "stdafx.h"
#include "Licence.h"
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"



/*
* @brief: 
*       构造函数
* @param[in] :target path 文件全路径
* @return: 
*   无
*/
Licence::Licence(char *path)
{
    filePath = "";
    //path为NULL 不拼路径
    if(NULL == path)
    {
        filePath.append("licence.ini");         
    }
    else
    {
        filePath.append(path);
        filePath.append(LICENCE_FILE_NAME);
    }
    
}
Licence::~Licence()
{
    
}


/*
* @brief: 
*       检查 licence 文件是否已存在。
* @param[in] :无
* @return: 
*   true; licence 文件已经存在
    false: licence 文件不存在
*/
bool Licence::LicenceFileIsExist()
{
    
    FILE *pf = fopen(filePath.c_str(),"r");

    if(NULL == pf)   //文件不存在
    {
        return false;
    }
    else             //文件存在
    {
        fclose(pf);
        return true;
    }
    

}

/*
* @brief:  
*       检查 licence 文件是否完整。
* @param[in] :无
* @return: 
*       ture; licence 完整
    false: licence 不完整
*/
/*
[Register]
isRegistered=1
isNeedApply=0
SerialNo=
*/
bool Licence::LicenceFileIsComplete()
{
    string str;
    string strOrig;
    unsigned int i = 0;
    bool status = true;  //状态记录
    
    if(false == LicenceFileIsExist())
    {
        return false;
    }

    FILE *pf = fopen(filePath.c_str(),"r");
    char buf[2];
    buf[0] = ' ';
    buf[1] = '\0';

    //读取文件到str中
    while(buf[0] != EOF)
    {
        buf[0] = fgetc(pf);
        strOrig.append(buf);
    }

    fclose(pf);

    //检查字符串是否存在
    if((-1 == strOrig.find("[Register]"))
        ||(-1 == strOrig.find("isRegistered"))
        ||(-1 == strOrig.find("isNeedApply"))
        ||(-1 == strOrig.find("SerialNo")))
    {
        
        return false;
    }

    //去除空格，并存入str
    for(i=0; i<strOrig.size(); i++)
    {
        if(' ' != strOrig.at(i))
        {
            buf[0] = strOrig.at(i);
            str.append(buf);
        }
    }

    //检查isRegistered
    if( (-1 == str.find("isRegistered=0"))&&(-1 == str.find("isRegistered=1")))
    {
        return false; 
    }
    
    //检查isNeedApply
    if( (-1 == str.find("isNeedApply=0"))&&(-1 == str.find("isNeedApply=1")))
    {
        return false;  
    }
    
    //检查SerialNo
    if(-1 == str.find("SerialNo="))
    {
       return false;
    }
    
    return true;
}

/*
* @brief: 
*       创建完整的 licence 文件。
* @param[in] :无
* @return: 
*   true; 创建成功
    false: 创建失败
*/
/*
[Register]
isRegistered=1
isNeedApply=0
SerialNo=
*/
bool Licence::CreateLicenceFile()
{
    string str = "[Register]\n"
                 "isRegistered=0\n"
                 "isNeedApply=1\n"
                  "SerialNo=\n";
    
    string file_bak = filePath;     
    file_bak.insert(file_bak.find(".ini"),"_bak");

    /* 删除之前的备份 */
    remove(file_bak.c_str());

    //备份
    rename(filePath.c_str(),file_bak.c_str());

    //fopen 创建一个licence.ini文件    
    FILE *pf = fopen(filePath.c_str(),"w");
    if(NULL == pf)
    { 
        return false;
    }

    fwrite(str.c_str(),str.size(),1,pf);
    fclose(pf);
    
    return true;
}

/*
* @brief: 
*       向 licence 文件写入序列号。
* @param[in] :无
* @return: 
*   true; 写入成功
    false: 写入失败
*/
/*
[Register]
isRegistered=1
isNeedApply=0
SerialNo=XADSSXQEHYBYBPSMMQTHMCWC
*/
bool Licence::WriteSerialNumber(CString sn)
{
    string str;
    char  lpAppName[] = "Register";
    char  lpKeyName[] = "SerialNo";

    if(false == LicenceFileIsComplete())
    {
        return false;
    }

    //写序列号
    WritePrivateProfileString(lpAppName,lpKeyName,sn,filePath.c_str());

    return true;
}

 /*
* @brief: 
*       修补licence 文件。
*       创建新的licence文件，并恢复注册码
* @param[in] :无
* @return: 
*       true: 修复成功
*       false: 修复失败
*/
bool Licence::RepairLicenceFile(void)
{
    char licence[128];
    int isRegistered = 0;
    int isNeedApply = 0;    
    DWORD licenceLen = 0;
    char  section[] = "Register";
    char  key[50] = "SerialNo";
    bool ret = false;
    struct stat fileInfo;
    char *pData = NULL;
    
    /* 从损坏licence.ini读取序列号 */
    licenceLen = GetPrivateProfileString(section, key, NULL, licence, sizeof(licence), filePath.c_str());

    /* 重建licence.ini文件 */
    ret = CreateLicenceFile();
    if (!ret)
    {
        return false;
    }

    if (licenceLen >= 1)
    {
        /* 恢复注册码 */
        WritePrivateProfileString(section, key, licence, filePath.c_str());

        /* 设置isRegistered */
        strcpy(key, "isRegistered");
        WritePrivateProfileString(section, key, "1", filePath.c_str());
    }

    return true;

}

