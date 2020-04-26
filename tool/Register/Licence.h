

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
    *       构造函数
    * @param[in] :target path 文件路径
    * @return: 
    *   无
    */
    Licence(char *path);
    
    /*
    * @brief: 
    *       析构函数
    * @param[in] :target path 文件路径
    * @return: 
    *   无
    */
    ~Licence();


    
    /*
    * @brief: 
    *       检查 licence 文件是否已存在。
    * @param[in] :无
    * @return: 
    *   true; licence 文件已经存在
       false: licence 文件不存在
    */
    bool LicenceFileIsExist();

    /*
    * @brief:  
    *       检查 licence 文件是否完整。
    * @param[in] :无
    * @return: 
    *       ture; licence 完整
            false: licence 不完整
    */
    bool LicenceFileIsComplete();

    /*
    * @brief: 
    *       创建完整的 licence 文件。
    * @param[in] :无
    * @return: 
    *       true; 创建成功
           false: 创建失败
    */
    bool CreateLicenceFile();

    /*
    * @brief: 
    *       向 licence 文件写入序列号。
    * @param[in] :无
    * @return: 
    *       true; 写入成功
           false: 写入失败
    */
    bool WriteSerialNumber(CString sn);

     /*
    * @brief: 
    *       修补licence 文件。
    *       创建新的licence文件，并恢复注册码
    * @param[in] :无
    * @return: 
    *       true: 修复成功
    *       false: 修复失败
    */
    bool RepairLicenceFile(void);

};

#endif