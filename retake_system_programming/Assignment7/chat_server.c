
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
int msgId[MAX_CLIENTS];
int shmId;

void processMessage(int client, MESSAGE* pMsg);
void sendMsgToClient(int clientId, MESSAGE* pMsg);
void sendMsgToAllClients(MESSAGE* pMsg);
void printMsg(MESSAGE* pMsg); 
void logMessage(MESSAGE* pMsg); 
int getClientIdByPid(int clientPid); 
void release(int signo);

int
main(int argc, char* argv[])
{
	int i;
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

	pShared->mInfo.mServerPid = getpid();
	pShared->mInfo.mNumOfClients = 0;
	pShared->mLogStartIndex = 0;
	pShared->mLogEndIndex = 0;
	
	printf("Get message queues for the catting process.. ");
	// get message queues for client
	for (i = 0; i < MAX_CLIENTS; ++i) {
		msgId[i] = msgget(CHAT_MSG_KEY(i), IPC_CREAT | 0666);
		
		if (msgId < 0) {
			fprintf(stderr, "\nERROR : %s\n", strerror(errno));
			exit(1);
		}
	}

	printf("Success!\n");

	printf("Register exiting code for releasing process.. ");
	if (signal(SIGINT, release) < 0 || signal(SIGTERM, release) < 0) {
		fprintf(stderr, "\nERROR : %s\n", strerror(errno));
		exit(1);
	}

	printf("Success!\n");

	printf("\nWelcome to chat server program!!\n");
	printf("Please press Ctrl + C to exit!\n\n");

	while (1) {
		for (i = 0; i < MAX_CLIENTS; ++i) {
			if (msgrcv(msgId[i], (void*)&msg, 
				sizeof(MESSAGE), MSG_SERVER, IPC_NOWAIT) > 0) {
				processMessage(i, &msg);
			}
		}
	}
}

void processMessage(int client, MESSAGE* pMsg) {
	CLIENT_INFO* clientInfo = &(pShared->mInfo);

	if (pMsg->mType == MSG_TYPE_CONNECT) {

		printf("[ %d ] %d client is connected!!\n", pMsg->from, client);
		clientInfo->mClientPid[client] = pMsg->from;
		++(clientInfo->mNumOfClients);

	} else if (pMsg->mType == MSG_TYPE_DISCONNECT) {

		printf("[ %d ] %d client is disconnected!!\n",
			pMsg->from, client);

	} else if (pMsg->mType == MSG_TYPE_SEND_TO_SERVER) {

		printMsg(pMsg);
		// if sending message to all clients
		if (pMsg->to == 0) {
			sendMsgToAllClients(pMsg);
			logMessage(pMsg);
		} else {
		// if sending message to a client
			int clientId = getClientIdByPid(pMsg->to);
			if (clientId >= 0) {
				sendMsgToClient(clientId, pMsg);
			}
		}
	}
}

int getClientIdByPid(int clientPid) {
	int i;
	CLIENT_INFO* clientInfo = &(pShared->mInfo);

	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (clientInfo->mClientPid[i] == clientPid) return i;
	}

	return -1;
}

void sendMsgToClient(int clientId, MESSAGE* pMsg) {
	MESSAGE sndMsg;

	sndMsg.mType = MSG_TYPE_RECV_FROM_SERVER;
	sndMsg.from = pMsg->from;
	sndMsg.to = pMsg->to;
	strncpy(sndMsg.mText, pMsg->mText, MAX_TEXT_SIZE);

	if (msgsnd(msgId[clientId], (void*)&sndMsg, sizeof(MESSAGE), 
		IPC_NOWAIT) < 0) {
		fprintf(stderr, "ERROR : %s\n", strerror(errno));
	}
}

void sendMsgToAllClients(MESSAGE* pMsg) {
	MESSAGE sndMsg;
	CLIENT_INFO* clientInfo = &(pShared->mInfo);
	int i;

	sndMsg.mType = MSG_TYPE_RECV_FROM_SERVER;
	sndMsg.from = pMsg->from;
	sndMsg.to = pMsg->to;
	strncpy(sndMsg.mText, pMsg->mText, MAX_TEXT_SIZE);

	for (i = 0; i < MAX_CLIENTS; ++i) {
		if (clientInfo->mClientPid[i] > 0) {
			if (msgsnd(msgId[i], (void*)&sndMsg, 
				sizeof(MESSAGE), IPC_NOWAIT) < 0) {
				fprintf(stderr, "ERROR : %s\n", 
					strerror(errno));
			}
		}
	}
}

void logMessage(MESSAGE* pMsg) {
	MESSAGE_LOG msgLog;

	msgLog.mPid = pMsg->from;
	strncpy(msgLog.mText, pMsg->mText, MAX_TEXT_SIZE);

	pShared->mMessageLogs[pShared->mLogEndIndex] = msgLog;

	pShared->mLogEndIndex = ++(pShared->mLogEndIndex) % MAX_LOGS;

	if (pShared->mLogStartIndex == pShared->mLogEndIndex) {
		pShared->mLogStartIndex = ++(pShared->mLogStartIndex) %
			MAX_LOGS;
	}

}

void printMsg(MESSAGE* pMsg) {
	if (pMsg->to > 0)
		printf("[%d -> %d] %s\n", pMsg->from, pMsg->to, pMsg->mText);
	else printf("[%d -> ALL] %s\n", pMsg->from, pMsg->mText);
}

void release(int signo) {
	int i;
	pid_t clientPid;

	printf("Release before exiting the server..\n");
	// send SIGINT signal to all clients
	// for exiting the clients process
	for (i = 0; i < MAX_CLIENTS; ++i) {
		clientPid = pShared->mInfo.mClientPid[i];
		if (clientPid > 0)
			kill(clientPid, SIGINT);
	}

	for (i = 0; i < MAX_CLIENTS; ++i) {
		msgctl(CHAT_MSG_KEY(i), IPC_RMID, NULL);
	}

	shmdt(pShared);
	shmctl(shmId, IPC_RMID, NULL);
	exit(0);
}
