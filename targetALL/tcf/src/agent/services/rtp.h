#include <tcf_config.h>
#include <framework/protocol.h>
#include "sem.h"
#define MAX_RTP_TASKS	200
#define MAX_DSP_SHLS 200
#define MAX_PATH_LENGTH 255
typedef struct
{
	char syscallname[30];
	int callnum;

}SysCallInfo;

extern SysCallInfo syscallinfo[];
extern SysCallInfo callinfo[];
#define RTP_LOCK_IT		0x1	/* take RTP lock - internal */
#define RTP_PRINT_HDR		0x2	/* print summary header - internal */
#define RTP_STATUS_STR_LENGTH   14
extern BOOL RtpSummaryShow(RTP_ID rtpId,int	lockNeeded,OutputStream *out);
int GetRtpTaskIdList(int rtpId, int rtpTaskIdList[]);
void rtpshlShow(OutputStream *out,int rtpId);
void rtpsemShow(OutputStream *out,int rtpId);
void rtpmsgqShow(OutputStream *out,int rtpId);
//void rtpfdShow(OutputStream *out,int rtpId);
STATUS RtpFdShow(OutputStream *out,RTP_ID rtpId);
void SysCallMonitor(int level,RTP_ID rtpId);
void RSysCallMonitor( int level,RTP_ID rtpId);
int rtpEdrMemShow(OutputStream *out,RTP_ID rtpId);
STATUS MemEdrPartShow(OutputStream *out, PART_ID partId);
