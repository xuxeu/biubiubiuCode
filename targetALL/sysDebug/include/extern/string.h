/* string.h - string library header file */

/* Copyright 1992-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
02h,00oct00,sn   Removed size_t typedef
02g,17feb99,mrs  Add C++ support for NULL, (SPR #25079).
02f,22sep92,rrr  added support for c++
02e,14sep92,smb  added const to the prototype of bcopy.
02d,07sep92,smb  removed macros for memcpy and memset.
02c,30jul92,smb  added macros for memcpy and memset.
02b,09jul92,smb  removed extra memmove declarations.
02a,04jul92,jcf  cleaned up.
01b,03jul92,smb  macro override for strerror added.
01a,29jul91,rrr  written.
*/

#ifndef __INCstringh
#define __INCstringh

#ifdef __cplusplus
extern "C" {
#endif

extern void *memset (void *__s, int __c, unsigned int __n);
extern char *strcat (char *__s1, const char *__s2);
extern int 	strcmp (const char *__s1, const char *__s2);
extern char *strcpy (char *__s1, const char *__s2);
extern unsigned int strlen (const char *__s);
extern char *strncat (char *__s1, const char *__s2, unsigned int __n);
extern int 	strncmp (const char *__s1, const char *__s2, unsigned int __n);
extern void *memcpy (void *__s1, const void *__s2, unsigned int __n);
extern void *memcmp (char *__s1, char *__s2, unsigned int __n);

#ifdef __cplusplus
}
#endif

#endif /* __INCstringh */
