#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vxworks.h>
#include "netLib.h"
#include "ipProto.h"
#include "sockLib.h"
#include "taskLib.h"

/*********************Macros defination******************/
#define SHELL_DEFAULT_PORT 0x4321

/**********************Variable declaration***************/
LOCAL int shellServerInitialized = FALSE;
LOCAL int shellRaw_ioStdFd[3]; /* 进入shell server时的标准输入输出 ，用于shell client退出时的恢复*/

/**********************Function declaration***************/
static void shellAcceptor(int port, int size, int blen, int zbuf);
static int shellServer(int port);
int TargetShellTask(int size);

/**********************Function implement***************/

int shellInitialize(void)
{
	int ret;

	if (shellServerInitialized)
	{
		return OK;
	}

	shellServerInitialized = TRUE;

	ret = taskSpawn("shellAcceptor", 99, VX_FP_TASK, 1024 * 10,
			(FUNCPTR) shellServer, SHELL_DEFAULT_PORT, 0, 0, 0, 0, 0, 0, 0, 0,
			0);
	if (ret == ERROR)
	{
		shellServerInitialized = FALSE;
		return ret;
	}

	/* 保存标准输入输出，用于恢复 */
	shellRaw_ioStdFd[STD_OUT] = ioGlobalStdGet(STD_OUT);
	shellRaw_ioStdFd[STD_IN] = ioGlobalStdGet(STD_IN);
	shellRaw_ioStdFd[STD_ERR] = ioGlobalStdGet(STD_ERR);

	return ret;
}

int shellServer(int port)
{
	shellAcceptor(port, 44600, 8000, 0);

	/* 恢复到shell server未初始化的状态 */
	shellServerInitialized = FALSE;

	return OK;
}

void shellAcceptor(int port, int size, int blen, int zbuf)
{
	int sock; /* server socket descriptor */
	int snew; /* per-client socket descriptor */
	struct sockaddr_in serverAddr, clientAddr;
	int len; /* sizeof(clientAddr) */

	/* setup BSD socket to read blasts from */
	bzero((char *) &serverAddr, sizeof(serverAddr));

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("cannot open socket\n");
		return;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		printf("bind error\n");
		close(sock);
		return;
	}

	if (listen(sock, 5) < 0)
	{
		printf("listen failed\n");
		close(sock);
		return;
	}

	while (1)
	{
		bzero((char *) &clientAddr, sizeof(clientAddr));
		len = sizeof(clientAddr);
		while ((snew = accept(sock, (struct sockaddr *) &clientAddr, &len))
				== -1)
			;

		if (setsockopt(snew, SOL_SOCKET, SO_RCVBUF, (char *) &blen,
				sizeof(blen)) < 0) {
			printf("Shell server failed\n");
			close(sock);
			close(snew);
			return;
		}
		
		shellGenericInit(NULL, size, NULL, NULL, TRUE, FALSE, snew, snew, snew);
	}

	/* cleanup */
	close(sock);
}
