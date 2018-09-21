/*
* 更改历史：
* 2005-02-08 郭建川  北京科银技术有限公司
*                    创建该文件。
*
*/


/**
*
* @file     MutexLock.h
* @brief
*      功能：
*      <li>信号量</li>
*
*/

#ifndef MUTEXLOCK_INCLUDE
#define MUTEXLOCK_INCLUDE


/************************头文件********************************/

/************************宏定义********************************/
/************************类型定义******************************/


class MutexLock
{
public:
    MutexLock ()
    {
        m_hMutex = ::CreateMutex (NULL, FALSE, NULL);
    };
    ~MutexLock ()
    {
        ::CloseHandle (m_hMutex);
    };
    void Lock ()
    {
        ::WaitForSingleObject (m_hMutex, INFINITE);
    };
    void Unlock ()
    {
        ::ReleaseMutex (m_hMutex);
    };
    bool IsLocked ()
    {
        int nReturn = ::WaitForSingleObject (m_hMutex, 0);

        if (nReturn == WAIT_TIMEOUT)
        {
            return true;
        }

        else
        {
            ::ReleaseMutex (m_hMutex);
            return false;
        }
    };
    HANDLE m_hMutex;
};

#endif