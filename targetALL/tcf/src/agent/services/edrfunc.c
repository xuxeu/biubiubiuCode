
#include <vxWorks.h>
#include <vsbConfig.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <edrLib.h>
#include <pmLib.h>
#include <private/edrLibP.h>

#include <dbgLib.h>
#include <intLib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <trcLib.h>
#include <sysSymTbl.h>
#include <hookLib.h>
#include <private/windLibP.h>
#include <private/funcBindP.h>
#include <private/cplusLibP.h>
#include <private/vmLibP.h>
#include <framework/json.h>

extern int edrflag;
extern int edrpath;
IMPORT int	edrInitStatus;
IMPORT UINT32	edrMissedErrors;
extern int edrDebug;
LOCAL const char* edrSeverityName [] =
    {
    "0x00",
    "FATAL",
    "NON-FATAL",
    "WARNING",
    "INFO",
    "0x05",
    "0x06",
    "0x07",
    "0x08",
    "0x09",
    "0x0a",
    "0x0b",
    "0x0c",
    "0x0d",
    "0x0e",
    "0x0f"
    };
static const char* edrStatusMsg
    (
    int		statusCode	/* edrInitStatus code */
    )
    {
    char *msg = "";

    switch (statusCode)
	{
	case EDR_INIT_OK:
	    msg = "edrLib initialized OK";
	    break;

	case EDR_INIT_UNINITIALIZED:
	    msg = "edrLib was not initialized";
	    break;

	case EDR_INIT_edrErrLogCreate_FAILED:
	    msg = "edrErrLogCreate failed";
	    break;

	case EDR_INIT_edrErrLogAttach_FAILED:
	    msg = "edrErrLogAttach failed";
	    break;

	case EDR_INIT_pmRegionSize_FAILED:
	    msg = "pmRegionSize failed";
	    break;

	case EDR_INIT_pmRegionAddr_FAILED:
	    msg = "pmRegionAddr failed";
	    break;

	case EDR_INIT_pmRegionProtect_FAILED:
	    msg = "pmRegionProtect failed";
	    break;

	case EDR_INIT_pmRegionCreate_FAILED:
	    msg = "pmRegionCreate failed";
	    break;

	case EDR_INIT_PM_FAILURE:
	    msg = "pmLib arena invalid";
	    break;

	case EDR_INIT_ERROR_LOG_NEWER:
	    msg = "error log appeared to be newer than edrLib";
	    break;

	default:
	    msg = "unknown edrLib error";
	    break;
	}

    return (msg);
    }
static void* edrLogBaseGet (void)
    {
    void *	addr = NULL;

    if ((addr = pmRegionAddr (edrPmArena, edrPmRegion ())) == NULL)
	{
	errnoSet (S_edrLib_PMREGION_ERROR);
	edrInitStatus = EDR_INIT_pmRegionAddr_FAILED;
	return (NULL);
	}

    return (addr);
    }

void delete_header(char *sbuf, EDR_ERROR_RECORD*	pER,int offset,char *s,char *showbuf)
	{
		int i,j;
		sprintf(sbuf,
		"%s", ((char *)pER) + offset);
		j=0;
		for(i=0;i<100;i++)
		{
			if(j==3)
			{
				s=&sbuf[i];
				break;
			}
			if(sbuf[i]=='\n')
			{
				j++;
			}

		}
		strcat(showbuf,s);
	}

static STATUS EdrErrorRecordDecode
    (
    EDR_ERROR_RECORD*	pER,		/* pointer to error record  */
    char *		pBuf,		/* pointer to output buffer */
    int			bufSize,	/* size of output buffer    */
    char * showbuf
    )
    {
    const char *	fac;
    char		buf[64];
    char sbuf[10000];
    char *s;

    FUNCPTR		edrPrintFn;
    EDR_PRINTF_BUFFER	edrPrintBuffer;

    extern void regsShowOutput (REG_SET *pRegs, FUNCPTR prtFunc, int prtArg);

    /* set up the output for the display routines */

    if (pBuf != NULL)
	{
	edrPrintBuffer.buf = pBuf;
	edrPrintBuffer.size = bufSize;
	edrPrintFn = printToEDRbuf;
	}
    else
	{
	/* no buffer, direct it to stdout */

	edrPrintBuffer.buf = NULL;
	edrPrintBuffer.size = 0;
	edrPrintFn = NULL;
	}

    switch (pER->kind & EDR_FACILITY_MASK)
	{
	case EDR_FACILITY_KERNEL:
	    fac = "KERNEL";
	    break;

	case EDR_FACILITY_INTERRUPT:
	    fac = "INTERRUPT";
	    break;

	case EDR_FACILITY_BOOT:
	    fac = "BOOT";
	    break;

	case EDR_FACILITY_REBOOT:
	    fac = "REBOOT";
	    break;

	case EDR_FACILITY_INIT:
	    fac = "INIT";
	    break;

	case EDR_FACILITY_USER:
	    fac = "USER";
	    break;

	case EDR_FACILITY_RTP:
	    fac = "RTP";
	    break;

	default:
	    fac = "Unknown";
	    break;
	}

    sprintf(sbuf,"%s/%s",edrSeverityName [pER->kind & EDR_SEVERITY_MASK], fac);
    strcat(showbuf,sbuf);
    sprintf(sbuf,"~%d", pER->bootCycle);
    strcat(showbuf,sbuf);
    sprintf(sbuf,"~%d.%d.%d",
	    (pER->osVersion >> EDR_OS_VERSION_MAJOR_SHIFT)
		& EDR_OS_VERSION_MAJOR_MASK,
	    (pER->osVersion >> EDR_OS_VERSION_MINOR_SHIFT)
		& EDR_OS_VERSION_MINOR_MASK,
	    (pER->osVersion >> EDR_OS_VERSION_MAINT_SHIFT)
		& EDR_OS_VERSION_MAINT_MASK);
    strcat(showbuf,sbuf);
    sprintf(sbuf,"~%d", pER->cpuNumber);
    strcat(showbuf,sbuf);

    /* beware - ctime() appends a , remove it */

    snprintf (buf, sizeof(buf), "%s", ctime (&pER->time));
    buf [strlen (buf) - 1] = '\0';;

    sprintf(sbuf,"~%s (ticks = %d)", buf, pER->ticks);
    strcat(showbuf,sbuf);

    if (pER->offsetTaskName != 0)
	{
	sprintf(sbuf,"~\"%s\" (0x%08x)",
		((char *)pER) + pER->offsetTaskName,
		pER->taskId);
	strcat(showbuf,sbuf);
	}
    else
	{
	sprintf(sbuf,
		"~0x%08x", pER->taskId);
	strcat(showbuf,sbuf);
	}
    strcat(showbuf,"~");
    if (pER->rtpId != (int)NULL)
	{
	if (pER->offsetRtpName != 0)
	    {
	    sprintf(sbuf,
		    "\"%s\" (0x%08x)",
		    ((char *)pER) + pER->offsetRtpName, pER->rtpId);
	    strcat(showbuf,sbuf);
	    }
	else
	    {
	    sprintf(sbuf,
		    "0x%08x", pER->rtpId);
	    strcat(showbuf,sbuf);
	    }

	sprintf(sbuf,
		    "RTP Address Space:   0x%08x -> 0x%08x",
		    pER->rtpAddrStart, pER->rtpAddrEnd);
	strcat(showbuf,sbuf);
	}
    strcat(showbuf,"~");
    if (pER->offsetFileName != (int)NULL)
    {
	sprintf(sbuf,"%s:%d",
		((char *)pER) + pER->offsetFileName, pER->lineNumber);//Injection Point:
	strcat(showbuf,sbuf);
    }

    strcat(showbuf,"~");
    if (pER->offsetPayload != (int)NULL)
    {
	sprintf(sbuf,
		"%s", ((char *)pER) + pER->offsetPayload);
	strcat(showbuf,sbuf);
    }

    strcat(showbuf,"~");
    if (pER->offsetMemoryMap != (int)NULL)
	{
    	delete_header(sbuf, pER,pER->offsetMemoryMap,s,showbuf);//去掉信息的前三行
	}

    strcat(showbuf,"~");
    if (pER->offsetExcinfo != (int)NULL)
    {
    	delete_header(sbuf, pER,pER->offsetExcinfo,s,showbuf);
    }

    strcat(showbuf,"~");
    if (pER->offsetRegisters != (int)NULL)
    {
    	delete_header(sbuf, pER,pER->offsetRegisters,s,showbuf);
    }

    strcat(showbuf,"~");
    if (pER->offsetDisassembly != (int)NULL)
    {
    	delete_header(sbuf, pER,pER->offsetDisassembly,s,showbuf);
    }

    strcat(showbuf,"~");
    if (pER->offsetTraceback != (int)NULL)
	{
    	delete_header(sbuf, pER,pER->offsetTraceback,s,showbuf);
	}
    else
    {
    	strcat(showbuf,"$");
    }
    return (OK);
    }

int edrshow
	    (
	    int	start,		/* starting point */
	    int	count,		/* number of records to show */
	    int facility,	/* limit to specified facility */
	    int severity,	/* limit to specified severity */
	    OutputStream *out,
	    int flag
	    )
	    {
		EDR_ERR_LOG *	pLog;
		EDR_ERR_LOG_ITER	iter;
		int			total;
		int			errnoSave = errno;

		/* make sure that ED&R initialized okay - if not, say why not */

		if (edrInitStatus != EDR_INIT_OK)
		{
		const char *s = edrStatusMsg (edrInitStatus);

		if (s == NULL)
			{
			printf ("Unknown edrLib initialization failure : status was %d",
				edrInitStatus);
			}
		else
			{
			printf ("ED&R was unable to initialize.");
			printf ("Check that PM_RESERVED_MEM and EDR_ERRLOG_SIZE are "
				"properly defined in the BSP");
			printf ("edrLibInit failure was : %s", s);
			}

		return (ERROR);
		}
		/* find the address of the log itself */
		if((flag==1)||(flag==2))//显示上次/上上次操作系统的edr信息
		{
			char *EDRLogPath;
			EDRLogPath=(char *)edrpath;
			int fd=0;
			int fp=0;
			int num=0;
			char *fbuf=calloc(1,40);
		if(edrflag==0)//本次操作系统运行过程中，未进行将edr信息写入文件,Num为上次系统运行时的edr信息文件的编号
		{
			sprintf(fbuf,"%sedrLogNum.txt",EDRLogPath);
			fp=open(fbuf,O_CREAT|O_RDWR,777);
			read(fp,fbuf,1);
			num=atoi(fbuf);
			if(flag==2)//查看上上次系统的edr信息
			{
				num-=1;
			}
			if(num<1)
			{
				num+=3;
			}
			sprintf(fbuf,"%sedrLog%d.txt",EDRLogPath,num);
			fd=open(fbuf,O_CREAT|O_RDWR,777);
		}
		else//本次操作系统运行过程中，已进行将edr信息写入文件，Num为当前系统运行过程的edr文件编号
		{
			sprintf(fbuf,"%sedrLogNum.txt",EDRLogPath);
			fp=open(fbuf,O_CREAT|O_RDWR,777);
			read(fp,fbuf,1);
			num=atoi(fbuf);
			if(flag==2)//查看上上次系统的edr信息
			{
				num-=2;
			}
			else//查看上次系统的edr信息
			{
				num-=1;
			}
			if(num<1)
			{
				num+=3;
			}
			sprintf(fbuf,"%sedrLog%d.txt",EDRLogPath,num);
			fd=open(fbuf,O_CREAT|O_RDWR,777);
		}
			close(fp);
			free(fbuf);

			char *buf=calloc(1,600);
			read(fd,buf,600);//读取文件固定部分
			pLog=(EDR_ERR_LOG *)buf;
			int i=(int)&(pLog->nodeList.node);
			int j=(int)pLog;
			i=i-j+((pLog)->nodeList.nextNode * (pLog)->header.nodeSize);//获取edr信息的大小
			free(buf);
			char *buff=calloc(1,i);
			lseek(fd,0,SEEK_SET);//回到文件起始位置
			read(fd,buff,i);//获取完整的edr信息（读取完整的edr信息便于解析）
			close(fd);
			pLog=(EDR_ERR_LOG *)buff;
		}
		else//获取当前系统运行过程的edr信息，直接从内存地址读取
		{
			if ((pLog = (EDR_ERR_LOG*) edrLogBaseGet ()) == NULL)
			{
			printf ("edrShow: cannot get address of persistent memory region");
			return (ERROR);
			}
		}
		/* Walk the error log, printing each record. We set the count to the
		 * absolute value of 'num' unless it was ZERO, in which case we want
		 * to print the entire log.
		 */

		if ((total = edrErrLogNodeCount (pLog)) == ERROR)
		{
		printf ("edrShow: the ED&R log is corrupt, unable to display records.");
		return (ERROR);
		}

		if (count == 0)
		{
		/* show them all */

		count = total;
		}
		int SIZE=0;
		SIZE=(10240*total+500);//回复信息的buf的大小
		char showbuf[SIZE];
		bzero(showbuf,sizeof(showbuf));

		sprintf (showbuf,"%d bytes (%d pages)~%d bytes~%d~%x~%u (old) + %u (recent)~%d~%d~%d",
		pLog->header.size, pLog->header.nPages,pLog->header.payloadSize,pLog->nodeList.maxNodeCount,
		pLog->header.cpuType,pLog->missedErrors, edrMissedErrors,total,
		pLog->bootCount,pLog->genCount);


		if (edrErrLogIterCreate (pLog, &iter, start, count))
		{
		EDR_ERR_LOG_NODE *	pNode;
		char *			buf;

		/* allocate a buffer to hold a record */

		if ((buf = malloc (pLog->header.payloadSize)) == NULL)
			return (ERROR);

		while ((pNode = edrErrLogIterNext (&iter)) != NULL)
			{
			EDR_ERROR_RECORD *pRecInLog = (EDR_ERROR_RECORD*) pNode->data;
			EDR_ERROR_RECORD *pER = (EDR_ERROR_RECORD *) buf;
			int genCount = pNode->genCount;
			int position;

			/* get logical position of record within the log */

			position = edrErrLogNodePosition(pLog, pNode)+1;

			memcpy (pER, pRecInLog, pRecInLog->size);

			/* ensure node wasn't updated during copy */

			if (genCount == pNode->genCount)
			{

			/* skip empty (recovered) records */

			if (pRecInLog->size == 0)
				{
				printf ("Incomplete record!");
				if (edrDebug)
				printf ("Record address is 0x%x", pRecInLog);
				}

			/* only display records meeting the display criteria */

			if (pRecInLog->size != 0 &&
				(facility == 0 ||
				(facility !=0 && facility == (pER->kind & EDR_FACILITY_MASK))) &&
				(severity == 0 ||
				(severity !=0 && severity == (pER->kind & EDR_SEVERITY_MASK))))
				{

				if (edrDebug)
				{
				printf ("Address/Size:        0x%x/%d",
					pRecInLog, pRecInLog->size);
				printf ("Generation Count:    %d", genCount);
				}
				strcat(showbuf,"*#");
				EdrErrorRecordDecode (pER, NULL, 0,showbuf);
				}
			}
			else
			{
			printf ("Record was updated while trying to be displayed!");
			}
			}

		if (buf != NULL)
			free (buf);
		}
		errno = errnoSave;
		json_write_string(out,showbuf);
		if(flag==1)
		{
			free(pLog);
		}
		return (OK);
    }
