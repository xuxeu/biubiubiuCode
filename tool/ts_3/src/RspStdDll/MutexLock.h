/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
*
* @file 	MutexLock.h
* @brief 	信号量
* @author 	郭建川
* @date 	2005-02-08
* @version  1.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef MUTEXLOCK_INCLUDE
#define MUTEXLOCK_INCLUDE

class MutexLock
{
public:
	MutexLock () {
	    m_hMutex = ::CreateMutex (NULL, FALSE, NULL);
	};
	~MutexLock () {
	    ::CloseHandle (m_hMutex);
	};
	void Lock () {
	    ::WaitForSingleObject (m_hMutex, INFINITE);
	};
	void Unlock () {
	    ::ReleaseMutex (m_hMutex);
	};
	bool IsLocked () {
	    int nReturn = ::WaitForSingleObject (m_hMutex, 0);
	    if (nReturn == WAIT_TIMEOUT) {
		return true;
	    } else {
		::ReleaseMutex (m_hMutex);
		return false;
	    }
	};
	HANDLE m_hMutex;
};

#endif