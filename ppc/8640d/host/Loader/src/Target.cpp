/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2012-7-24  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/**
* @file  Target.cpp
* @brief
*       ���ܣ�
*       <li>�����Ŀ���������֮��Ľ�������</li>
*/
/************************���ò���******************************/
#include "Target.h"

/************************���岿��******************************/

/************************ʵ�ֲ���******************************/

/**
 * @Funcname  : Target
 * @brief	  : ���캯��
 * @para[IN]  : tsip:ip��ַ tgtname��Ŀ������� tsport���˿�
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012��7��24�� 
 *	 
**/
Target::Target(string tsip, string tgtname, UINT32 tsport)
{
	m_tsIP = tsip;
	m_tsPort = tsport;
	m_tgtName = tgtname;
	m_tsFd = 0;
	m_desAid = 0;
	m_currentDesSaid = 0;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : ���������ָ���ظ�
 * @para[IN]  : cmd������ cmdCount��������� timeout����ʱʱ��
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012��7��24��  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount, UINT32 timeout)	
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;
	char buf[10] = {0};

	/* ���������� */
	if(NULL == cmd ||  0 == cmdCount)
	{
		return GetErrorNumber(INVALID_PARAMETER);
	}

    /* �������� */
    ret = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, src_said, cmd, cmdCount);
    if (ret < 0)
    {    
        return GetErrorNumber(TARGET_PUTPKT_FAIL);        
    }    

    /* �ȴ���� */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
        return GetErrorNumber(TARGET_OUTTIME);   
    }
	
	/* ���ܻظ� */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, buf, sizeof(buf));
	if (buf[0] != 'O' && buf[1] != 'K')
	{
		return GetErrorNumber(TARGET_GETPKT_FAIL);   
	}

	return TARGET_OK;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : ��������Ȼظ�,���ظ��ŵ�ָ����buffer��
 * @para[IN]  : cmd������ cmdCount��������� result����� resultCount��������� timeout����ʱʱ��
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012��7��24��  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount, char *result, UINT32 resultCount, UINT32 timeout)	
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;

	if(NULL == cmd || NULL == result || 0 == cmdCount)
	{
		return GetErrorNumber(INVALID_PARAMETER);
	}

    /* �������� */
    ret = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, src_said, cmd, cmdCount) ;
    if (ret < 0)
    {    
       return GetErrorNumber(TARGET_PUTPKT_FAIL);  
    }    

    /* �ȴ���� */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
       return GetErrorNumber(TARGET_OUTTIME);   
    }

	/* ���ܻظ� */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, result, resultCount);
	if (ret < 0)
    {    
        return GetErrorNumber(TARGET_GETPKT_FAIL);  
    }   
	
   	return TARGET_OK;
}

/**
 * @Funcname  : ExecCommand
 * @brief	  : ��������Ȼظ�
 * @para[IN]  : cmd������ cmdCount��������� 
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2012��7��24��  
 *	 
**/
UINT32 Target::ExecCommand(const char *cmd, UINT32 cmdCount)	
{
	UINT32 snedSzie = 0;
	UINT16 src_said = 0;
	
	snedSzie = TClient_putpkt(m_tsFd, m_desAid, m_currentDesSaid, 0,  cmd, cmdCount);
	
	return (snedSzie > 0) ? TARGET_OK : GetErrorNumber(TARGET_PUTPKT_FAIL);
}

/**
 * @Funcname  : ReceiveData
 * @brief	  : ��������
 * @para[IN]  : result����� cmdCount����������С   timeout����ʱʱ�� receSize��ʵ�ʽ��ܴ�С
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 Target::ReceiveData(char *result, UINT32 resultCount, UINT32 timeout, INT32 *receSize)
{
	INT32 ret = 0;
	UINT16 src_said = 0;
	UINT16 des_said = 0;
	UINT16 src_aid = 0;
    
	/* �ȴ���� */
    ret = TClient_wait(m_tsFd, timeout);
    if (TS_DATACOMING != ret)
    {
       return GetErrorNumber(TARGET_OUTTIME);   
    }

	/* �������� */
	ret = TClient_getpkt(m_tsFd, &src_aid, &des_said, &src_said, result, resultCount);
	if (ret < 0)
    {    
        return GetErrorNumber(TARGET_GETPKT_FAIL);  
    }
	
	/* ���ûظ����ݰ���С */
	*receSize = ret;

	return TARGET_OK;
}

/**
 * @Funcname  : Connect
 * @brief	  : ���ӵ�ָ����Ŀ���������
 * @para[IN]  : ��
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 Target::Connect()	
{
	/* ��ָ��IP��ַ�Ͷ˿ںŵ�TS��������*/
    m_tsFd = TClient_open(m_tsIP.c_str(), m_tsPort);
	if(m_tsFd < 0)
	{
		return GetErrorNumber(TARGET_TCLIENT_OPEN_FAIL);
	}

	/* ��ȡĿ���ID */
	m_desAid = TClient_getServerID(m_tsFd, const_cast<char*>(m_tgtName.c_str()));

	return (m_desAid > 0) ? TARGET_OK : GetErrorNumber(TARGET_TCLIENT_GETSERVERID_FAIL);
}

/**
 * @Funcname  : DisConnect
 * @brief	  : �Ͽ�����
 * @para[IN]  : ��
 * @reture	  : �ɹ�����TARGET_OK ʧ�ܷ�����Ӧ�����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 Target::DisConnect()	
{
	INT32 ret = 0;
		
	ret = TClient_close(m_tsFd);
	
	return (ret == TS_SUCCESS) ? TARGET_OK : GetErrorNumber(TARGET_DISCONNECT_FAIL);
}

/**
 * @Funcname  : SetDesSAid
 * @brief	  : ���ö��������
 * @para[IN]  : desSAid�����������
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
void Target::SetDesSAid(UINT16 desSAid)
{
	m_currentDesSaid = desSAid;
}

/**
 * @Funcname  : GetErrorNumber
 * @brief	  : ����ErrprStr��ȡErrprMum,��������ʽ����
 * @para[IN]  : string�������ַ���
 * @reture	  : ������ʽ�Ĵ����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 Target::GetErrorNumber(string str)
{
	return Util::GetDefault()->GetErrorNumber(str);
}

