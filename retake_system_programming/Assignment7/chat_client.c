
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "chat.h"

CHAT_SHM* pShared;
int clientId;
int msgId;

void processMessage(MESSAGE* pMsg);
void printLog();
void checkMessage();
void sendMessage(); 
void sendMsgToClient(int clientId, MESSAGE* pMsg);
void sendMsgToAllClients(MESSAGE* pMsg);
void printMsg(MESSAGE* msg);
int getClientIdByPid(int clientPid); 
void release(int signo);

int
main(int argc, char* argv[])
{
	int i, shmId, what;
	MESSAGE msg;

	printf("Get a shared memory for the chat process.. ");
	// get shared memory
	shmId = shmget(CHAT_SHM_KEY, sizeof(CHAT_SHM), IPC_CREAT | 0666);

	if (shmId < 0) {
		fprintf(stderr, "\nERROR : %s\n", strerror(errno));
		exit(1);
	}

	printf("Success!\n");

	printf("Attach the shared memory to the process.. ");
	// attach the shared memory to the process
	pShared = (CHAT_SHM*) shmat(shmId, (void*)0, 0);

	if ((long)pShared == -1) {
		fprintf(stderr, "\nERROR : %s\n", strerror(errno));
		exit(1);
	}
	
	printf("Success!\n");

	
	printf("Get a message queue for the chatting process.. ");
	// get message queues for client
	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (pShared->mInfo.mClientPid[i] == 0) {
			msgId = msgget(CHAT_MSG_KEY(i), IPC_CREAT | 0666);
		
			if (msgId < 0) {
				fprintf(stderr, "\nERROR : %s\n", 
					strerror(errno));
				exit(1);
			}
		
			clientId = i;

			break;
		}
	}

	printf("Success!\n");

	printf("Register exiting code for releasing process.. ");
	if (signal(SIGINT, release) < 0 || signal(SIGTERM, release) < 0) {
		fprintf(stderr, "\nERROR : %s\n", strerror(errno));
		exit(1);
	}

	printf("Success!\n");

	printf("Send a message to connect to the server.. ");

	msg.mType = MSG_TYPE_CONNECT;
	msg.from = getpid();
	msg.to = 0;

	if (msgsnd(msgId, (void*)&msg, sizeof(MESSAGE),
		IPC_NOWAIT)) {
		fprintf(stderr, "\nERROR : %s\n", strerror(errno));
		exit(1);
	}

	printf("Success!\n");

	fputs("\nWelcome to chat program!\n", stdout);
	printf("Your client ID is %d\n\n", clientId);

	while (1) {
		printf("1. Check Message\n");
		printf("2. Send Message\n");
		printf("3. Chatting Log\n");
		printf("Input : ");
		fscanf(stdin, "%d", &what);

		switch(what) {
			case 1 : checkMessage(); break;
			case 2 : sendMessage(); break;
			case 3 : printLog(); break;
		}
	}
}

void printLog() {
	int start = pShared->mLogStartIndex, end = pShared->mLogEndIndex;
	MESSAGE_LOG* msgLogs = pShared->mMessageLogs;

	for (; start != end; start = ++start % MAX_LOGS) {
		printf("[ %d ] %s\n", msgLogs[start].mPid,
			msgLogs[start].mText);
	}
}

void checkMessage() {
	MESSAGE msg;

	while (msgrcv(msgId, (void*)&msg, sizeof(MESSAGE),
		MSG_TYPE_RECV_FROM_SERVER, IPC_NOWAIT) >= 0) {
		processMessage(&msg);
	}
}

void sendMessage() {
	int i;
	pid_t pid;
	CLIENT_INFO* clientInfo = &(pShared->mInfo);
	MESSAGE msg;

	printf("\n*** Client List ***\n");
	
	for (i = 0; i < MAX_CLIENTS; ++i) {
		pid = clientInfo->mClientPid[i];
		if (pid != 0) {
			printf("%d (PID : %d)\n", i, pid);
		}
	}

	printf("100 : SEND ALL\n");
	printf("Who do you want to send? ");
	fscanf(stdin, "%d", &i);
	printf("Input Message : \n");
	fflush(stdout);
	fflush(stdin);
	while(*fgets(msg.mText, MAX_TEXT_SIZE, stdin) == '\n');

	msg.mType = MSG_TYPE_SEND_TO_SERVER;
	msg.from = getpid();

	if (i == 100) {
		msg.to = 0;
	} else {
		msg.to = clientInfo->mClientPid[i];
	}

	if (msgsnd(msgId, (void*)&msg, sizeof(MESSAGE), IPC_NOWAIT) < 0) {
		fprintf(stderr, "ERROR : %s\n", strerror(errno));
	}
}

void processMessage(MESSAGE* pMsg) {
	CLIENT_INFO* clientInfo = &(pShared->mInfo);

	if (pMsg->mType == MSG_TYPE_RECV_FROM_SERVER) {
		printMsg(pMsg);
	}
}

void printMsg(MESSAGE* pMsg) {
	if (pMsg->to > 0)
		printf("[%d -> %d] %s\n", pMsg->from, pMsg->to, pMsg->mText);
	else printf("[%d -> ALL] %s\n", pMsg->from, pMsg->mText);
}

int getClientIdByPid(int clientPid) {
	int i;
	CLIENT_INFO* clientInfo = &(pShared->mInfo);

	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (clientInfo->mClientPid[i] == clientPid) return i;
	}

	return -1;
}

void release(int signo) {
	int i;
	pid_t clientPid;
	MESSAGE msg;

	msg.mType = MSG_TYPE_DISCONNECT;
	msg.from = getpid();
	msg.to = 0;
	msgsnd(msgId, (void*)&msg, sizeof(MESSAGE), IPC_NOWAIT);

	printf("Release before exiting the client..\n");

	pShared->mInfo.mClientPid[clientId] = 0;
	--(pShared->mInfo.mNumOfClients);

	shmdt(pShared);
	exit(0);
}
