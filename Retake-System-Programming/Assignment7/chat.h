
#if !defined _CHAT_H_
#define _CHAT_H_

#include <unistd.h>

#define CHAT_SHM_KEY		7777
#define CHAT_MSG_KEY(x)		(7778 + (x))

#define MAX_TEXT_SIZE		1024

typedef struct _MESSAGE {
	long mType;
	pid_t from, to;
	char mText[MAX_TEXT_SIZE];
} MESSAGE;

#define MSG_TYPE_CONNECT		1
#define MSG_TYPE_DISCONNECT		2
#define MSG_TYPE_SEND_TO_SERVER		3
#define MSG_TYPE_RECV_FROM_SERVER	11

#define MSG_SERVER			-10

#define MAX_CLIENTS		3

typedef struct _CLIENT_INFO {
	pid_t mServerPid;
	pid_t mClientPid[MAX_CLIENTS];
	size_t mNumOfClients;
} CLIENT_INFO;

typedef struct _MESSAGE_LOG {
	pid_t mPid;
	char mText[MAX_TEXT_SIZE];
} MESSAGE_LOG;


#define MAX_LOGS		100

typedef struct _CHAT_SHM {
	CLIENT_INFO mInfo;
	MESSAGE_LOG mMessageLogs[MAX_LOGS];
	int mLogStartIndex, mLogEndIndex;
} CHAT_SHM;

#endif
