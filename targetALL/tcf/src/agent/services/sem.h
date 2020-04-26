#include <tcf_config.h>
#include <framework/protocol.h>
#include <spyext.h>
extern void ini_getsem_service(Protocol * proto,TCFBroadcastGroup * bcg);
int semDetailShow(SEM_ID semId,OutputStream *out);
extern void TTCinfoToString(char *buf,TASK_CPU_INFO *tcpu,char*status,int *Taskcpuindex,TASK_DESC *td);
