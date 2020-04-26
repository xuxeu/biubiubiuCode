#include "lock_forever.h"
#include <vxWorks.h>
#include <vsbConfig.h>
#include <framework/protocol.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <framework/exceptions.h>
#include <services/edr.h>
#include <stdio.h>
#include <edrfunc.h>
static const char * EDR = "Edr";


void command_edr_show(char * token, Channel * c)
{

	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int flag;
	sscanf(bufinp,"%x",&flag);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	edrshow(0,0,0,0,&c->out,flag);
	/*
	 * flag :
	 *       ==0  ��ʾ��ǰ����ϵͳ��edr��Ϣ
	 *       ==1  ��ʾ�ϴ�ϵͳ����ʱ��edr��Ϣ
	 *       ==2  ��ʾ���ϴ�ϵͳ����ʱ��*edr��Ϣ
	 */
	write_stream(&c->out, ']');
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}




void ini_edr_service(Protocol * proto,TCFBroadcastGroup * bcg)
{

    add_command_handler(proto, EDR, "EdrShow", command_edr_show);

}
