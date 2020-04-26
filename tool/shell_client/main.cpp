#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include "time.h"
#include <winCon.h>
#ifdef WIN32
#pragma comment(lib, "wsock32")
#endif
#include <iostream>
#include <string>
using namespace std;

#define BUF_SIZE 20000

const char* filename = "shell_client_config.txt";

#define REV_BUF_SIZE 1460

const string defaultTag("Host Shell");

SOCKET  socketC; // 通信socket

void shell_client(const char * ip_addr,const char *redirect,string targetName);
void UpdateWindowTitle(const string &targetName);

BOOL WINAPI exitCommand(DWORD crtlType);
void printWelcome();

static void usage()
{
	printf("【使用说明】\n");
	printf("在程序目录创建一个文件：%s\n", filename);
	printf("1)文件第一行表示服务器IP\n");
	printf("2)文件第二行表示是否对输出进行重定向\n");
}

static void logonInfo()
{
	printf("\nTarget is connected\n");
	//printf("------------------------------------------------\n");
	printWelcome();
}
void printWelcome()
{
 cout << "\n" << "\
                      /---                         |                     \n\
                    //////---                      |                     \n\
                  //////////---                    |                     \n\
                //////////////---                  |                     \n\
              //////////////////---                |                     \n\
             /////////////////////---              |                     \n\
          /--- /////////////////////---            | CoreTek  LambdaPRO  \n\
        /////--- /////////////////////---          |                     \n\
      /////////--- /////////////////////---        |                     \n\
    /////////////--- /////////////////////---      |  Development System \n\
  /////////////////--- /////////////////////---    |                     \n\
   //////////////////--- /////////////////////---  |                     \n\
     /////////////--- /--- /////////////////---    |  Host  Based  Shell \n\
       /////////--- /////--- /////////////---      |                     \n\
         /////--  /////////--- /////////---        |                     \n\
           /--  /////////////--- /////---          |  Version  2.2       \n\
              /////////////////--- /--             |                     \n\
               //////////////////---               |                     \n\
                 //////////////---                 |                     \n\
                   //////////---                   |                     \n\
                     //////---                     |                     \n\
                       //---                       |" << "\n" << endl;
 
 
 return;
}


/*用于打印收到的信息*/
static DWORD WINAPI _print_thread(LPVOID ptr){
	SOCKET shellSock = (SOCKET)ptr;
	char* recvbuf = (char *)malloc(REV_BUF_SIZE);
	int iret;
	while(1){

		iret = recv (shellSock, recvbuf, REV_BUF_SIZE, 0);
		if(iret >0){
			recvbuf[iret]='\0';
			printf("%s",recvbuf);
		}
		else
			break;
	}
	return 1;
}

static void print_time()
{
	struct tm * time_ret;
	__time64_t now;
	_time64(&now);
	time_ret = _localtime64(&now);
	printf("Time now d:%d, h:%d, m:%d, s:%d\n", time_ret->tm_mday, time_ret->tm_hour, time_ret->tm_min, time_ret->tm_sec);
}

int main(int argc, char** argv)
{
	int packet_size = 0;
	char ip_addr[20];
	int time_delay = 0;
	char redirect[20];
	char targetName[50] = "";

	FILE* fd = fopen(filename, "r");
	if (fd == NULL)
	{
		printf("fopen %s -----Fail", filename);
		usage();
		Sleep(5000);
		exit(1);
	}
	fscanf(fd, "%s", ip_addr);
	fscanf(fd,"%s",redirect);
	fscanf(fd,"%s",targetName);
	fclose(fd);

	//获取目标机连接名
	
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1,1), &wsaData);
	SetConsoleCtrlHandler(exitCommand,TRUE);
	
	while(1)
		shell_client(ip_addr,redirect,targetName);

	WSACleanup();
	return 0;
}


void shell_client(const char * ip_addr,const char * redirect,string targetName)
{
	int len, time_out;
	int port = 0x4321;	/*default port*/
    char sendbuf[BUF_SIZE];
	char  *ch_redirect = "ioGlobalStdSet(1,ioTaskStdGet(0,1))\n" ;
	char *ch_redirect_n = "ioGlobalStdSet(1,ioGlobalStdGet())\n" ;

    SOCKADDR_IN serverAddr;
    socketC = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip_addr);

	printf("------------------------------------------------\n");
	printf("Connecting the target at %s ", ip_addr);
    while (connect (socketC, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) < 0)
		printf(".");
	logonInfo();

	/*设置发送超时*/
	time_out = 1000;//in milliseconds 
	if (setsockopt(socketC, SOL_SOCKET, SO_SNDTIMEO,(char *) &time_out, sizeof (time_out)) < 0)
	{
		printf ("Shell server failed\n");
		closesocket(socketC);
	    return;
	}

	//设置重定向
	if(strcmp(redirect,"REDIRECT_Y") == 0)
	{
		len = send(socketC, ch_redirect, strlen(ch_redirect), 0);
		if(len<= 0)
		{
			printf("-->redirect ERROR\n");
		}
		else
		{
			//printf("-->redirect OK\n");
			//正确不打印信息
		}
	} 
	else if(strcmp(redirect,"REDIRECT_N") == 0)//设置不重定向
	{
		len = send(socketC, ch_redirect_n, strlen(ch_redirect_n), 0);
		if(len<= 0)
		{
			printf("-->CANCEL redirect ERROR\n");
		}
		else
		{
			//printf("-->redirect CANCELED\n");
			//正确不打印信息
		}
	}

    //收取设置重定向的回复信息 但不打印信息 此段代码就是过滤掉重定向设置的回复信息
	char* recvbuf = (char *)malloc(REV_BUF_SIZE);
	int iret;
	while(1)
	{
		iret = recv (socketC, recvbuf, REV_BUF_SIZE, 0);
		if(iret >0){
			recvbuf[iret]='\0';
			string temp = recvbuf;

			if(temp.find("ioGlobalStdSet") < temp.size())
			{
				cout<<"->";//打印提示符号
			    break;
			}
		}		
	}
	free(recvbuf);

	//启动控制服务器
	DWORD threadId;
	HANDLE print_thread =
	CreateThread(NULL,
					 16*1024,
					 _print_thread,
					 (LPVOID) socketC,
					 0,
					 &threadId);
	if (print_thread == NULL)
	{
		printf("Create print thread error\n");
		exit(1);
	}

	//设置窗口标题
	if(targetName == "")
	{
        UpdateWindowTitle(ip_addr);
	}
	else
	{
	    UpdateWindowTitle(targetName+"("+ip_addr+")");
	}

	while(1){
		int i =0;
		int ch;
		for(i = 0; i<BUF_SIZE ;i++)
		{
			ch = getchar();
			sendbuf[i] = ch;
			if(ch == '\n')
				break;
		}
		if(i >= BUF_SIZE)
			continue;

		//如果状态是发送状态，开始发送
		len = send(socketC, sendbuf, i+1, 0);
		if(len<= 0)
		{
			break;
		}
		Sleep(0);//强制调度
	}
    closesocket(socketC);

}

/**
 *退出时发送exit命令，让其退出shell任务，并释放资源
 *
 **/
BOOL WINAPI exitCommand(DWORD crtlType)
{
	char *exitCommand = "exit\n";
	int len;
	switch(crtlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		{
			len = send(socketC, exitCommand, strlen(exitCommand), 0);
			if(len <= 0)
			{
				printf("exit error\n");
			}else {
				printf("exit ok\n");
			}
			closesocket(socketC);
			break;
		}
	}

	return FALSE;
}


/*
 * @brief: 
 *      设置窗口标题
 * @return: 
 *      None
 */   
void UpdateWindowTitle(const string &targetName)
{
    char hostName[260] = "\0";
    string title;

    if (targetName.empty())
    {
        //目标机名称为空，未连接目标机
        title = defaultTag;
    }
    else
    {
        //目标机名称不为空
        //获取主机名称
        gethostname(hostName, sizeof(hostName));

        //组织标题名称:targetName@hostName - Host Shell
        title = targetName + " - " + defaultTag;
    }

    //设置当前窗口标题
     //标题长度最大64K
    const unsigned int maxTitilLength = 1024*64;
    if (title.length() > maxTitilLength)
    {
        return;
    }

    //设置当前窗口标题
    SetConsoleTitle(title.c_str());

}
