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
 *    将可变参数列表中的参数按照<fmt>指定的格式进行格式化，并把格式化后的数据输
 *    出到指定设备。
 * @param[in]: fmt: 格式字符串
 * @param[in]: ...: 可变参数列表
 * @return:	
 *    无  
 * @qualification method: 分析/测试
 * @derived requirement: 否	
 * @function location: API
 * @implements: DR.1.11
 */
void printk(char *fmt, ...);

/**
 * @req
 * @brief: 
 *    将可选参数按指定的格式转换后输出到指定的缓冲区，并返回输出的字符个数(不包
 *    括结束字符)。
 * @param[in]: fmt: 格式化字符串
 * @param[in]: ...: 可变参数列表
 * @param[out]: buffer: 被写入的缓冲区指针
 * @return: 
 *    输出的字符个数。
 *    ERROR：参数错误或输出字符失败。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @implements: DR.1.1
 */
int sprintf(char *buffer, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __INCstdioh */
