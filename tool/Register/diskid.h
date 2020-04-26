#ifndef __DISKID_H_
#define __DISKID_H_
/*
#ifdef _cplusplus
extern "C"{
#endif
*/

#define READ_SN_OK 0
#define READ_SN_FAIL_NO_RIGHTS -1
#define READ_SN_FAIL_INVALID_SN -2
#define READ_SN_FAIL_SCSI -3
#define READ_DUMMY_SN -4
#define READ_SN_NOT_SUPPORT_OS -5


extern char HardDriveSerialNumber[1024];
extern char HardDriveModelNumber[1024];
void getHardDriverSerialID(char *id, int *len);

/*
 * @brief: 
 *      读取硬盘序列号
 * @return: 
 *      READ_SN_OK: 读取成功
 *      READ_SN_FAIL_NO_RIGHTS: 没有权限读取
 */
int getHardDriveComputerID(void);


/*
 * @brief: 
 *      设置硬盘序列号
 * @return: 
 */
void setHardDriverSerialID(char *id);

/*
 * @brief: 
 *      验证硬盘序列号是否有效
 * @param[in] pSN: 硬盘序列号
 * @return: 
 *      true: 有效
 *      false: 无效
 */
bool CheckoutHardDriveSerialNumber(char *pSN, int snLen);

/*
#ifdef _cplusplus
}
#endif
*/

int CreateDriveSerialNumber(char *pSN, int snLen);
#endif
