

#include <tcf_config.h>
#include <framework/protocol.h>

//extern void ini_cpushow(Protocol * proto);
extern void ini_cpushow_service(Protocol * proto,TCFBroadcastGroup * bcg);
#ifndef TASKID_MAX
#define TASKID_MAX 500
#endif
