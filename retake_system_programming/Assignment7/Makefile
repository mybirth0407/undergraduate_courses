
all : chat_client chat_server

chat_client : chat_client.c chat.h
	gcc -o chat_client chat_client.c

chat_server : chat_server.c chat.h
	gcc -o chat_server chat_server.c

clean :
	rm chat_client chat_server
