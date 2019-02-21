/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso server), si es que las hubiera.
*/

#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <netdb.h>


int get_socket_server(int port)
{
	int s;
	struct sockaddr_in dir;
	int opcion=1;
	//int port;

	// get port form env var
	//char** endptr = 0;
	//port = (int) strtol(getenv("PUERTO"), endptr, 10);

	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creating socket");
		return 1;
	}

	/* Para reutilizar puerto inmediatamente
	*/
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion))<0){
		perror("error in setsockopt");
		return 1;
	}

	bzero((char *) &dir, sizeof(dir));
	dir.sin_addr.s_addr=INADDR_ANY;
	dir.sin_port=htons(port);
	dir.sin_family=PF_INET;

	if (bind(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error in bind");
		close(s);
		return 1;
	}

	if (listen(s, 5) < 0) {
		perror("error in listen");
		close(s);
		return 1;
	}
	fprintf(stdout, "Server socket: after listen\n");
	return s; /* returns socket */
}

/* int serializeMsg2(char* buf , TCP_Msg_editor* msg){
	sprintf(buf, "%d ", msg->op);
	sprintf(buf + strlen(buf), "%d ", msg->pid);
    int i;
	for (i = 0; i < strlen(msg->Tema); i++){
		sprintf(buf+ strlen(buf), "%c", msg->Tema[i]);
	}
	sprintf(buf + strlen(buf), " ");
	for (i = 0; i < strlen(msg->Valor); i++){
		sprintf(buf+ strlen(buf), "%c", msg->Valor[i]);
	}
	sprintf(buf + strlen(buf), " ");
	for (i = 0; i < strlen(msg->msg); i++){
		sprintf(buf+ strlen(buf), "%c", msg->msg[i]);
	}

    return 0;
} */

//subscriber_arg_type *msg
	//unsigned int source;
	//unsigned int destiny;
	//int op;
	//char message[100];


int serializeMsg(char* buf , subscriber_arg_type *msg){
	sprintf(buf, "%u ", msg->source);
	sprintf(buf + strlen(buf), "%u ", msg->destiny);
	sprintf(buf + strlen(buf), "%d ", msg->op);
    int i;
	for (i = 0; i < strlen(msg->message); i++){
		sprintf(buf+ strlen(buf), "%c", msg->message[i]);
	}
	//sprintf(buf + strlen(buf), " ");


    return 0;
}

/* FUNCIONA
int serializeMsg(char* buf , TCP_Msg_editor* msg){
	sprintf(buf, "%d ", msg->op);
	sprintf(buf + strlen(buf), "%d ", msg->pid);
    int i;
	for (i = 0; i < TAM; i++){
		sprintf(buf+ strlen(buf), "%c", msg->Tema[i]);
	}
	sprintf(buf + strlen(buf), " ");
	for (i = 0; i < TAM; i++){
		sprintf(buf+ strlen(buf), "%c", msg->Valor[i]);
	}

    return 0;
}
*/
int deserializeMsg(char* buf, subscriber_arg_type *msg){
	char* start = buf;
	msg->source = strtol(start,NULL,10); // msg->op = atoi(start);
	start++;
	msg->destiny = strtol(start,NULL,10); // msg->pid = atoi(start);
	start++;
	start++;
	msg->op = strtol(start,NULL,10); // msg->pid = atoi(start);
	start++;

/*
	int j = 0;
	while(start[j] == ' ') {
		start++;
	}
*/
	start++;
	// message deserialization starts here 
	// TODO: testing
	start++;
	int i;
	for(i = 0; i < 100; i++){
		//if(start[0] == ' ' ) break;  // eliminar esto si funciona bien
		msg->message[i] = *start;
		start++;
	}
		//msg->message[i] = '\0';

	// msg->Tema[i] = '\0'; //eliminar esto si funciona bien
	return 0;
}

/* 
int deserializeMsg2(char* buf, TCP_Msg_editor* msg){
	char* start = buf;
	msg->op = strtol(start,NULL,10); // msg->op = atoi(start);
	start++;
	msg->pid = strtol(start,NULL,10); // msg->pid = atoi(start);
	start++;
	int j = 0;
	while(start[j] != ' ') {
		start++;
	}
	int i;
	start++;
	for(i = 0; i < TAM; i++){
		if(start[0] == ' ' ) break;
		msg->Tema[i] = *start;
		start++;
	}
	msg->Tema[i] = '\0';
	start++;
	for(i = 0; i < TAM; i++){
		if(isalnum(*start) == 0) break;
		msg->Valor[i] = *start;
		start++;
	}
	msg->Valor[i] = '\0';

	// message deserialization starts here 
	// TODO: testing
	start++;
	for(i = 0; i < 100; i++){
		//if(start[0] == ' ' ) break;  // eliminar esto si funciona bien
		msg->msg[i] = *start;
		start++;
	}
	// msg->Tema[i] = '\0'; //eliminar esto si funciona bien
	return 0;
} */

int get_socket_user(char* servidor, int port) {
    int s; // socket.
	printf("Address %s:%d\n",servidor,port);
    struct sockaddr_in dir;
    struct hostent *host_info;
    if ((s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("error creating socket");
        return -1;
    }
    host_info=gethostbyname(servidor);
    memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
    dir.sin_port=htons(port);
    dir.sin_family=AF_INET;
    if (connect(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
        perror("error in connect2");
        close(s);
        return -1;
    }
    return s;
}