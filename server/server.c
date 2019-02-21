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
#include <stdbool.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>



int port_pid;
pthread_t tid[1];
pthread_mutex_t mutex;

int user_ports[5];
int user_count;
char user_ip[5][16];




int main(int argc, char *argv[])
{
	if (argc!=1)
	{
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return 1;
	}



	// write something so that when a user connects to the server (intermediary) the intermediary sends back an
	// id number identifying the user on the server. Could be like a static integer that updates every time a 
	// new user connects (new operation: new_user.)

	user_count = 0;

	int port = 1140;


	setenv("PORT", "1140", 0);				// port is set by the first argument 
	setenv("SERVER", "localhost", 0);

	//inicializar_temas(fichero);
	//init_subscriber();	// inicializa estructura de datos subscriptores

	/* Initialize mutex*/
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);


	int s; //socket
	int s_conec; //accept()
	int socket;
	unsigned int tam_dir;
	struct sockaddr_in dir_cliente;
	//TCP_Msg_editor *evento;
	char buf[MAX_LEN];




	subscriber_arg_type *msg = malloc(sizeof(subscriber_arg_type));

	while ( 1 ) {

		s = get_socket_server(port);
		tam_dir=sizeof(dir_cliente);

		getsockname(s, (struct sockaddr*)&dir_cliente, &tam_dir);
		printf("local address: %s\n", inet_ntoa( dir_cliente.sin_addr));
		printf("local port: %d\n", (int) ntohs(dir_cliente.sin_port));

		if ((s_conec=accept(s, (struct sockaddr *)&dir_cliente, &tam_dir))<0){
			perror("error in accept");
			close(s);
			return -1;
		}

		memset(buf,0,strlen(buf)); //zero the entire buffer
		memset(msg->message,0,strlen(msg->message));

		if(read(s_conec, buf, MAX_LEN) < 0){
			perror("error in operation read ");
			close(s);
			return -1;
		}
		deserializeMsg(buf, msg);

		//close(s_conec);
		//close(s);
		printf ("OP Received: %d\n", msg->op);
		printf ("Buffer Content: %s\n", buf);
		//recepcion(mensaje);
		switch(msg->op)
		{

			case NEWUSER:
			
			user_ports[user_count] = 1025 + user_count;
			strcpy(user_ip[user_count], "localhost");

			msg->source= user_count;
			msg->op = NEWUSER;
			serializeMsg(buf,msg);
			if(write(s_conec, buf, MAX_LEN) < 0){
				perror("error in write of operation new user");
			}
			user_count++;
			break;


			case SENDMSG:

			printf("MSG: %s\n",msg->message);
			printf("server:%s|port:%d\n", user_ip[msg->destiny], user_ports[msg->destiny]);
			socket = get_socket_user(user_ip[msg->destiny],user_ports[msg->destiny]); // user_ports[msg->destiny] + 1025
			if (socket == -1) fprintf(stderr, "Error connecting to server\n");

			// build new msg
			msg->op = RCVMSG;
			
			// serialize new msg
			serializeMsg(buf, msg);
			printf("Buffer content before sending:%s\n", buf);
			if (write(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error sending msg back to user2\n");
			/*
			if (read(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error receaving reply from user2\n");

			deserializeMsg(buf,msg);
			if (msg->op == OK) {
				fprintf(stdout, "Message sent successfully\n");
			} else {
				fprintf(stdout, "Error: reply op OK not received\n");
			}
*/
			memset(buf,0,strlen(buf)); //zero the entire buffer
			memset(msg->message,0,strlen(msg->message));
			break;

		}



		close(s);
		memset(buf,0,strlen(buf)); //zero the entire buffer
		memset(msg->message,0,strlen(msg->message));
		printf("Loop Final part\n");
	}
	pthread_mutex_destroy(&mutex);
	if(msg!= NULL) {
		free(msg);
	}
	msg = NULL;
	return 0;
}
