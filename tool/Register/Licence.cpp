#include "stdafx.h"
#include "Licence.h"
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"



/*
* @brief: 
*       ���캯��
* @param[in] :target path �ļ�ȫ·��
* @return: 
*   ��
*/
Licence::Licence(char *path)
{
    filePath = "";
    //pathΪNULL ��ƴ·��
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
*       ��� licence �ļ��Ƿ��Ѵ��ڡ�
* @param[in] :��
* @return: 
*   true; licence �ļ��Ѿ�����
    false: licence �ļ�������
*/
bool Licence::LicenceFileIsExist()
{
    
    FILE *pf = fopen(filePath.c_str(),"r");

    if(NULL == pf)   //�ļ�������
    {
        return false;
    }
    else             //�ļ�����
    {
        fclose(pf);
        return true;
    }
    

}

/*
* @brief:  
*       ��� licence �ļ��Ƿ�������
* @param[in] :��
* @return: 
*       ture; licence ����
    false: licence ������
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
    bool status = true;  //״̬��¼
    
    if(false == LicenceFileIsExist())
    {
        return false;
    }

    FILE *pf = fopen(filePath.c_str(),"r");
    char buf[2];
    buf[0] = ' ';
    buf[1] = '\0';

    //��ȡ�ļ���str��
    while(buf[0] != EOF)
    {
        buf[0] = fgetc(pf);
        strOrig.append(buf);
    }

    fclose(pf);

    //����ַ����Ƿ����
    if((-1 == strOrig.find("[Register]"))
        ||(-1 == strOrig.find("isRegistered"))
        ||(-1 == strOrig.find("isNeedApply"))
        ||(-1 == strOrig.find("SerialNo")))
    {
        
        return false;
    }

    //ȥ���ո񣬲�����str
    for(i=0; i<strOrig.size(); i++)
    {
        if(' ' != strOrig.at(i))
        {
            buf[0] = strOrig.at(i);
            str.append(buf);
        }
    }

    //���isRegistered
    if( (-1 == str.find("isRegistered=0"))&&(-1 == str.find("isRegistered=1")))
    {
        return false; 
    }
    
    //���isNeedApply
    if( (-1 == str.find("isNeedApply=0"))&&(-1 == str.find("isNeedApply=1")))
    {
        return false;  
    }
    
    //���SerialNo
    if(-1 == str.find("SerialNo="))
    {
       return false;
    }
    
    return true;
}

/*
* @brief: 
*       ���������� licence �ļ���
* @param[in] :��
* @return: 
*   true; �����ɹ�
    false: ����ʧ��
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

    /* ɾ��֮ǰ�ı��� */
    remove(file_bak.c_str());

    //����
    rename(filePath.c_str(),file_bak.c_str());

    //fopen ����һ��licence.ini�ļ�    
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
*       �� licence �ļ�д�����кš�
* @param[in] :��
* @return: 
*   true; д��ɹ�
    false: д��ʧ��
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

    //д���к�
    WritePrivateProfileString(lpAppName,lpKeyName,sn,filePath.c_str());

    return true;
}

 /*
* @brief: 
*       �޲�licence �ļ���
*       �����µ�licence�ļ������ָ�ע����
* @param[in] :��
* @return: 
*       true: �޸��ɹ�
*       false: �޸�ʧ��
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
    
    /* ����licence.ini��ȡ���к� */
    licenceLen = GetPrivateProfileString(section, key, NULL, licence, sizeof(licence), filePath.c_str());

    /* �ؽ�licence.ini�ļ� */
    ret = CreateLicenceFile();
    if (!ret)
    {
        return false;
    }

    if (licenceLen >= 1)
    {
        /* �ָ�ע���� */
        WritePrivateProfileString(section, key, licence, filePath.c_str());

        /* ����isRegistered */
        strcpy(key, "isRegistered");
        WritePrivateProfileString(section, key, "1", filePath.c_str());
    }

    return true;

}

