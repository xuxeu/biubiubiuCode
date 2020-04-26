#ifndef _UTIL_H
#define _UTIL_H

unsigned int hex2int(char **ptr, int *intValue);
unsigned int int2hex(char *ptr, int size,int intValue);

int hex2num(unsigned char vubCh);

//int hexStringToInt(char *pbuf, US buflen, int value);
int fromhex (int a);
int hex2bin (const char *hex, char *bin, int count);

#endif