/*
* ������ʷ��
* 2005-02-08 ������  ���������������޹�˾
*                    �������ļ���
*
*/


/**
*
* @file     MutexLock.h
* @brief
*      ���ܣ�
*      <li>�ź���</li>
*
*/

#ifndef MUTEXLOCK_INCLUDE
#define MUTEXLOCK_INCLUDE


/************************ͷ�ļ�********************************/

/************************�궨��********************************/
/************************���Ͷ���******************************/


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