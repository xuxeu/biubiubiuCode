/************************************************************************
*                �����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: lambdaIce.cpp,v $
 * Revision 1.4  2008/03/19 10:25:11  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.2  2008/03/19 09:03:00  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.1  2008/02/04 05:33:49  zhangxc
 * ��Ʊ������ƣ���ACoreOS CVS����ֿ����
 * ��д��Ա��������
 * ������Ա������F
 * ����������������ACoreOS�ֿ���ȡ�����µ�Դ���룬������ύ���µĲֿ�Ŀ¼�ṹ�й���
 * ��һ���ύsrcģ�顣
 *
 * Revision 1.3  2007/04/20 07:10:50  guojc
 * �����˶�ICE DCC���ܵ�֧��
 * �����˶�Ŀ��帴λ���ܵ�֧��
 * �����˶��ڴ��ȿ����õ�֧��
 * �����˶Ի���ģ�������Լ��ݵ�֧��
 *
 * Revision 1.2  2007/04/02 02:56:53  guojc
 * �ύ���µ�IS���룬֧��������IDEʹ�ã�֧�������ļ�����
 *
 * Revision 1.1  2006/11/13 03:40:55  guojc
 * �ύts3.1����Դ��,��VC8�±���
 *
 * Revision 1.2  2006/07/14 09:19:30  tanjw
 * 1.�޸�release������
 * 2.����ice��������
 *
 * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
 * no message
 *
 * Revision 1.2  2006/04/14 02:02:27  guojc
 * �����˶�CP15Э�������Ĵ����޸ĵ�֧��
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS��������
 *
 * Revision 1.1  2006/03/04 09:25:09  guojc
 * �״��ύICE Server��̬��Ĵ���
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * �����ļ���ʵ��RSP_lambdaIce��Ļ����ӿڡ�
 */

/**
 * @file     lambdaIce.cpp
 * @brief
 *    <li>���ܣ���װ�˺�ICE������Э������ӿ� </li>
 * @author     ������
 * @date     2006-02-20
 * <p>���ţ�ϵͳ��
 */

/**************************** ���ò��� *********************************/
#include "lambdaIce.h"
#include "Packet.h"
#include "rspTcpServer.h"
#include "IceServerComm.h"
/*************************** ǰ���������� ******************************/
using namespace std;
/**************************** ���岿�� *********************************/
T_MODULE T_CONST T_WORD INT2HEX_SIZE = 10;
/**************************** ʵ�ֲ��� *********************************/


/**
* @brief 
*     ����: ���÷���������
* @param[in] tpServer ����������ָ��
*/
T_VOID RSP_LambdaIce::SetServer(RSP_TcpServer *tpServer)
{
    m_server = tpServer;
}

/**
* @brief 
*     ����:��ѯICE��С��
* @param[in] cpBuf ������,��Ҫ���͸�ICE��һЩ��Ϣ
*/
T_VOID RSP_LambdaIce::QueryTargetEndian(int IceID)
{
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    if(pack == NULL)
    {
        return;
    }
    T_CHAR *pBuf = pack->GetData();
    //��װ��ѯ��С��������з���
    memcpy(pBuf, "i -e", strlen("i -e"));
    pBuf += strlen("i -e");
    pack->SetSize(pBuf - pack->GetData());
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}

/**
* @brief 
*     ����:ɾ����ICE�ĻỰ
* @param[in] ICEID
*/
T_VOID RSP_LambdaIce::DeleteSeesion(int IceID)
{
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    memcpy(pBuf, "d*", strlen("d*"));
    pack->SetSize(strlen("d*"));
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}
/**
* @brief 
*     ����:������ICE�ĻỰ
* @param[in] cpBuf ������,��Ҫ���͸�ICE��һЩ��Ϣ
* @param[in] cpBufSize ��������С
*/
T_VOID RSP_LambdaIce::CreateSeesion(T_CHAR *cpBuf, T_WORD cpBufSize, int IceID)
{
    if(cpBuf == NULL)
    {
        return;
    }
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    memcpy(pBuf, cpBuf, cpBufSize);
    pack->SetSize(cpBufSize);
    pack->SetDesAid(IceID);
    pack->SetDesSaid(ICE_SESS_MAN_ID);
    pack->SetSrcSaid(0);
    m_server->Enqueue(pack);
    return;
}

/**
* @brief 
*     ����: ��װдCP15�Ĵ����Ŀ��ư�
* @param[in] wNum CP15�Ĵ�����
* @param[in] wValue �Ĵ���ֵ
*/
T_VOID RSP_LambdaIce::WriteCp15Reg(T_WORD wNum, T_WORD wValue)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽ Dnn,XXXX
    memcpy(pBuf, RSP_SET_CPP_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wNum);    //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);
    //ת���������ֽڵ����δ洢
    m_server->Int2Char(wValue, pBuf, m_server->GetTargetEndian());
    pBuf += 4;
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ��CP15�Ĵ����Ŀ��ư�
* @param[in] wNum CP15�Ĵ�����
* @param[in] wValue �Ĵ���ֵ
*/
T_VOID RSP_LambdaIce::ReadCp15Reg(T_WORD wNum)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽ Dnn,XXXX
    memcpy(pBuf, RSP_GET_CPP_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wNum);    //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}


/**
* @brief 
*     ����: ��װд�ڴ���ư�
* @param[in] wAddr �ڴ��ַ
* @param[in] cpBuf ������
* @param[in] wOffset ƫ�Ƶ�ַ
* @param[in] wSize Ҫд���ڴ��С
*/
T_VOID RSP_LambdaIce::WriteMemory(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    
    //��װЭ�黺�壬��ʽ Xaddr,size:data
    memcpy(pBuf, RSP_WRITE_MEMORY, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr);    //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize);    //��С
    memcpy(pBuf,COLON_CHAR, COLON_CHAR_LEN);    //����:
    pBuf += COLON_CHAR_LEN;

    T_CHAR *pContent = cpBuf + wOffset;           //�ڴ�����
    memcpy(pBuf, pContent, wSize);
    pBuf += wSize;


    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װд�ڴ���ư�
* @param[in] wAddr �ڴ��ַ
* @param[in] cpBuf ������
* @param[in] wOffset ƫ�Ƶ�ַ
* @param[in] wSize Ҫд���ڴ��С
* @param[in] wDelay ��ʱʱ�䳤��
*/
T_VOID RSP_LambdaIce::WriteMemoryWithDelay(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize, T_WORD wDelay)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    
    //��װЭ�黺�壬��ʽ Maddr,size,data
    memcpy(pBuf, "M", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr);    //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize);    //��С
    memcpy(pBuf,"#",strlen("#"));    //����#��
    pBuf += strlen("#");

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wDelay);    //��С
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);


    T_CHAR *pContent = cpBuf + wOffset;           //�ڴ�����
    memcpy(pBuf, pContent, wSize);
    pBuf += wSize;


    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ���ڴ���ư�
* @param[in] wAddr �ڴ��ַ
* @param[in] wSize Ҫ�����ڴ��С
*/
T_VOID RSP_LambdaIce::ReadMemory(T_WORD wAddr, T_WORD wSize)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽmaddr,size
    memcpy(pBuf, RSP_READ_MEMORY, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wSize); //��С
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);

}

/**
* @brief 
*     ����: ��װд���мĴ������ư�
* @param[in] waRegs �Ĵ�������
* @param[in] waRegs �Ĵ������鳤��
*/
T_VOID RSP_LambdaIce::WriteAllRegisters(T_WORD *waRegs, T_WORD wNum)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽGxxxx
    memcpy(pBuf, RSP_SET_ALL_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    //�Ĵ��������Դ�˷�ʽ����
    for(T_WORD i = 0; i < wNum; i++)
    {
        m_server->Int2Char(waRegs[i], pBuf, TRUE);//TRUE);
        pBuf += 4; //ÿ���Ĵ�����ֵռ4�ֽ�
    }

    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);

}

/**
* @brief 
*     ����: ��װ�����мĴ������ư�
*/
T_VOID RSP_LambdaIce::ReadAllRegisters()
{    
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽg
    memcpy(pBuf, RSP_GET_ALL_REGISTER, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);
}


/**
* @brief 
*     ����: ��װ��������Ŀ������ư�
*/
T_VOID RSP_LambdaIce::ContinueTarget()
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽc
    memcpy(pBuf, RSP_RUN_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װֹͣĿ������ư�
*/
T_VOID RSP_LambdaIce::StopTarget()
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽ!
    memcpy(pBuf, RSP_STOP_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);

}

/**
* @brief 
*     ����: ��װ����Ŀ������ư�
*   resetType��0  CPUӲ����λ��ֻ�ǽ�Ŀ���CPU����Ӳ����λ������Ŀ�����Χ�豸���и�λ��
*   resetType��1  ����Ӳ����λ��ֻ�ǽ�Ŀ������ΧӲ���豸���и�λ������CPU���и�λ��
*   resetType��2  CPU����ͬʱӲ����λ��
*/
T_VOID RSP_LambdaIce::ReSetTarget(T_UWORD resetType)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽRtype
    memcpy(pBuf, RSP_RESET_TARGET, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pBuf += Int2hex(pBuf, 10, resetType);
    
    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ��ѯĿ���CPUID CODE
*/
T_VOID RSP_LambdaIce::QueryCPUIDCode()
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽi
    memcpy(pBuf, RSP_GET_TARGET_CPUCODE, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ��ѯĿ������ư�
*/
T_VOID RSP_LambdaIce::QueryTarget()
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽq
    memcpy(pBuf, RSP_QUERY_TARGET_STATE, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ��DCC���ܵĿ��ư�
*/
T_VOID RSP_LambdaIce::OpenDccFunction(T_WORD dccAddr)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽAxxxx(addr)
    memcpy(pBuf, LAMBDA_ICE_START_DCC, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    pBuf += Int2hex(pBuf, 10, dccAddr);
    pack->SetSize(pBuf - pack->GetData());

    //�����ɣ����뵽����ȥ
    m_server->Enqueue(pack);

    return;
}

/**
* @brief 
*     ����: ��װ����ϵ��۲��
* @param[in] type �ϵ�����
*                           0������ϵ�
*                           1��Ӳ���ϵ�
*                           2��д�۲��
*                           3�����۲��
*                           4�����ʹ۲��
* @param[in] wAddr �ϵ��ַ
* @param[in] wWidth �ϵ���
*/
T_VOID RSP_LambdaIce::InsertBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, RSP_SET_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    //type
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, type); //����Ϊ0 ����ϵ�
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //addr
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //length
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װɾ���ϵ��۲��
* @param[in] type �ϵ�����
*                           0������ϵ�
*                           1��Ӳ���ϵ�
*                           2��д�۲��
*                           3�����۲��
*                           4�����ʹ۲��
* @param[in] wAddr �ϵ��ַ
* @param[in] wWidth �ϵ���
*/
T_VOID RSP_LambdaIce::DeleteBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();
    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, RSP_DELETE_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;
    //type
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, type); //����Ϊ0 ����ϵ�
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //addr
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG, CONTROL_FLAG_LEN);    //���ӿ��Ʒ���
    pBuf += CONTROL_FLAG_LEN;
    //length
    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

#if 0
/**
* @brief 
*     ����: ��װ����Ӳ���ϵ���ư�
* @param[in] wAddr �ϵ��ַ
* @param[in] wWidth �ϵ���
*/
T_VOID RSP_LambdaIce::InsertBreakPoint(T_WORD wAddr, T_WORD wWidth)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, RSP_SET_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 0); //����Ϊ0 ����ϵ�
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);

}

/**
* @brief 
*     ����: ��װɾ��Ӳ���ϵ���ư�
* @param[in] wAddr �ϵ��ַ
* @param[in] wWidth �ϵ���
*/
T_VOID RSP_LambdaIce::RemoveBreakPoint(T_WORD wAddr, T_WORD wWidth)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, RSP_DELETE_BREAKPOINT, RSP_CMD_LEN);
    pBuf += RSP_CMD_LEN;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 1); //����Ϊ1 ɾ���ϵ�
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}

/**
* @brief 
*     ����: ��װ����۲����ư�
* @param[in] wAddr �۲���ַ
* @param[in] wWidth �۲����
* @param[in] wAccess �۲������
*/
T_VOID RSP_LambdaIce::InsertWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, "w", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 0); //����Ϊ0 ����۲��
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAccess); //�۲������
    
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);

}

/**
* @brief 
*     ����: ��װɾ���۲����ư�
* @param[in] wAddr �۲���ַ
* @param[in] wWidth �۲����
* @param[in] wAccess �۲������
*/
T_VOID RSP_LambdaIce::RemoveWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽP
    memcpy(pBuf, "w", 1);
    pBuf += 1;

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, 1); //����Ϊ1 ɾ���۲��
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);


    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAddr); //��ַ
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wWidth); //���
    memcpy(pBuf,CONTROL_FLAG,strlen(CONTROL_FLAG));    //���ӿ��Ʒ���
    pBuf += strlen(CONTROL_FLAG);

    pBuf += Int2hex(pBuf, INT2HEX_SIZE, wAccess); //�۲������
    
    
    pack->SetSize(pBuf - pack->GetData());
    //�����ɣ����뵽������ȥ
    m_server->Enqueue(pack);
}
#endif

/*
* ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
* @param[out] cpPtr ����16�����ַ����Ļ�����
* @param[in] wSize   �������Ĵ�С
* @param[in] wIntValue   Ҫת��������
* @return ת���Ĵ�С
*/
T_UWORD RSP_LambdaIce::Int2hex(T_CHAR *cpPtr, T_WORD wSize,T_WORD wIntValue)
{
    memset(cpPtr,'\0',wSize);    //���ô�С,��󻺳����Ĵ�С
    sprintf(cpPtr,"%x",wIntValue);
    return strlen(cpPtr);
}