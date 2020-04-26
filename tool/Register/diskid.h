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
 *      ��ȡӲ�����к�
 * @return: 
 *      READ_SN_OK: ��ȡ�ɹ�
 *      READ_SN_FAIL_NO_RIGHTS: û��Ȩ�޶�ȡ
 */
int getHardDriveComputerID(void);


/*
 * @brief: 
 *      ����Ӳ�����к�
 * @return: 
 */
void setHardDriverSerialID(char *id);

/*
 * @brief: 
 *      ��֤Ӳ�����к��Ƿ���Ч
 * @param[in] pSN: Ӳ�����к�
 * @return: 
 *      true: ��Ч
 *      false: ��Ч
 */
bool CheckoutHardDriveSerialNumber(char *pSN, int snLen);

/*
#ifdef _cplusplus
}
#endif
*/

int CreateDriveSerialNumber(char *pSN, int snLen);
#endif
