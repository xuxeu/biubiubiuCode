

#ifndef _LICENCE_FILE_DETECT_
#define _LICENCE_FILE_DETECT_

#include <string>

#define LICENCE_FILE_NAME "licence.ini"

using namespace std ;

class Licence
{
public:
    string filePath;

    /*
    * @brief: 
    *       ���캯��
    * @param[in] :target path �ļ�·��
    * @return: 
    *   ��
    */
    Licence(char *path);
    
    /*
    * @brief: 
    *       ��������
    * @param[in] :target path �ļ�·��
    * @return: 
    *   ��
    */
    ~Licence();


    
    /*
    * @brief: 
    *       ��� licence �ļ��Ƿ��Ѵ��ڡ�
    * @param[in] :��
    * @return: 
    *   true; licence �ļ��Ѿ�����
       false: licence �ļ�������
    */
    bool LicenceFileIsExist();

    /*
    * @brief:  
    *       ��� licence �ļ��Ƿ�������
    * @param[in] :��
    * @return: 
    *       ture; licence ����
            false: licence ������
    */
    bool LicenceFileIsComplete();

    /*
    * @brief: 
    *       ���������� licence �ļ���
    * @param[in] :��
    * @return: 
    *       true; �����ɹ�
           false: ����ʧ��
    */
    bool CreateLicenceFile();

    /*
    * @brief: 
    *       �� licence �ļ�д�����кš�
    * @param[in] :��
    * @return: 
    *       true; д��ɹ�
           false: д��ʧ��
    */
    bool WriteSerialNumber(CString sn);

     /*
    * @brief: 
    *       �޲�licence �ļ���
    *       �����µ�licence�ļ������ָ�ע����
    * @param[in] :��
    * @return: 
    *       true: �޸��ɹ�
    *       false: �޸�ʧ��
    */
    bool RepairLicenceFile(void);

};

#endif