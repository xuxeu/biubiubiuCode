/* stdio.h - stdio header file */




#ifndef __INCstdioh
#define __INCstdioh

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*outputfunc_ptr)(char c);
/**
 * @req
 * @brief:
 *    ���ɱ�����б��еĲ�������<fmt>ָ���ĸ�ʽ���и�ʽ�������Ѹ�ʽ�����������
 *    ����ָ���豸��
 * @param[in]: fmt: ��ʽ�ַ���
 * @param[in]: ...: �ɱ�����б�
 * @return:	
 *    ��  
 * @qualification method: ����/����
 * @derived requirement: ��	
 * @function location: API
 * @implements: DR.1.11
 */
void printk(char *fmt, ...);

/**
 * @req
 * @brief: 
 *    ����ѡ������ָ���ĸ�ʽת���������ָ���Ļ�������������������ַ�����(����
 *    �������ַ�)��
 * @param[in]: fmt: ��ʽ���ַ���
 * @param[in]: ...: �ɱ�����б�
 * @param[out]: buffer: ��д��Ļ�����ָ��
 * @return: 
 *    ������ַ�������
 *    ERROR���������������ַ�ʧ�ܡ�
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @implements: DR.1.1
 */
int sprintf(char *buffer, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __INCstdioh */
