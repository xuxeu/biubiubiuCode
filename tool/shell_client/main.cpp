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

SOCKET  socketC; // ͨ��socket

void shell_client(const char * ip_addr,const char *redirect,string targetName);
void UpdateWindowTitle(const string &targetName);

BOOL WINAPI exitCommand(DWORD crtlType);
void printWelcome();

static void usage()
{
	printf("��ʹ��˵����\n");
	printf("�ڳ���Ŀ¼����һ���ļ���%s\n", filename);
	printf("1)�ļ���һ�б�ʾ������IP\n");
	printf("2)�ļ��ڶ��б�ʾ�Ƿ����������ض���\n");
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


/*���ڴ�ӡ�յ�����Ϣ*/
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

	//��ȡĿ���������
	
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

	/*���÷��ͳ�ʱ*/
	time_out = 1000;//in milliseconds 
	if (setsockopt(socketC, SOL_SOCKET, SO_SNDTIMEO,(char *) &time_out, sizeof (time_out)) < 0)
	{
		printf ("Shell server failed\n");
		closesocket(socketC);
	    return;
	}

	//�����ض���
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
			//��ȷ����ӡ��Ϣ
		}
	} 
	else if(strcmp(redirect,"REDIRECT_N") == 0)//���ò��ض���
	{
		len = send(socketC, ch_redirect_n, strlen(ch_redirect_n), 0);
		if(len<= 0)
		{
			printf("-->CANCEL redirect ERROR\n");
		}
		else
		{
			//printf("-->redirect CANCELED\n");
			//��ȷ����ӡ��Ϣ
		}
	}

    //��ȡ�����ض���Ļظ���Ϣ ������ӡ��Ϣ �˶δ�����ǹ��˵��ض������õĻظ���Ϣ
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
				cout<<"->";//��ӡ��ʾ����
			    break;
			}
		}		
	}
	free(recvbuf);

	//�������Ʒ�����
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

	//���ô��ڱ���
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

		//���״̬�Ƿ���״̬����ʼ����
		len = send(socketC, sendbuf, i+1, 0);
		if(len<= 0)
		{
			break;
		}
		Sleep(0);//ǿ�Ƶ���
	}
    closesocket(socketC);

}

/**
 *�˳�ʱ����exit��������˳�shell���񣬲��ͷ���Դ
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
 *      ���ô��ڱ���
 * @return: 
 *      None
 */   
void UpdateWindowTitle(const string &targetName)
{
    char hostName[260] = "\0";
    string title;

    if (targetName.empty())
    {
        //Ŀ�������Ϊ�գ�δ����Ŀ���
        title = defaultTag;
    }
    else
    {
        //Ŀ������Ʋ�Ϊ��
        //��ȡ��������
        gethostname(hostName, sizeof(hostName));

        //��֯��������:targetName@hostName - Host Shell
        title = targetName + " - " + defaultTag;
    }

    //���õ�ǰ���ڱ���
     //���ⳤ�����64K
    const unsigned int maxTitilLength = 1024*64;
    if (title.length() > maxTitilLength)
    {
        return;
    }

    //���õ�ǰ���ڱ���
    SetConsoleTitle(title.c_str());

}
