#include "lock_forever.h"
#include <vxWorks.h>
#include <vsbConfig.h>
#include <framework/protocol.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <framework/exceptions.h>
#include <stdio.h>

static const char * LOCK = "Lock";
void command_lock_forever(char * token, Channel * c)
{
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	write_stream(&c->out, '[');
	get_lock(&c->out);
	write_stream(&c->out, ']');
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}
void command_pend_task(char * token, Channel * c)
{
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	write_stream(&c->out, '[');
	pend_tasks(&c->out);
	write_stream(&c->out, ']');
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}

static void command_task_trace(char * token, Channel * c)
{
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int taskId;
	sscanf(bufinp,"%x",&taskId);     /* 将读取到的 16进制字符串转换为10进制  */
	write_stream(&c->out, '[');
	tasktrace(&c->out,taskId);
	write_stream(&c->out,']');
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


void ini_lock_service(Protocol * proto,TCFBroadcastGroup * bcg) {

    add_command_handler(proto, LOCK, "LockForever", command_lock_forever);
    add_command_handler(proto, LOCK, "PendingTask", command_pend_task);//"tw" allTasks
    add_command_handler(proto, LOCK, "TaskTrace", command_task_trace); //"tt" task


}
