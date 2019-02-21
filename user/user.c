#include "common.h"
#include "user.h"
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>



pthread_mutex_t mutex;
pthread_t tid[1];
void (*functionPtr)(const char*,const char*);

int volatile PortSource;


void* doSomeThing(void *arg)
{


	//printf("\n Recibo mensajes por aquí en bucle\n");


	//unsigned int source;
	//unsigned int destiny;
	//char message[100];
	char buf[MAX_LEN];
	//int port = 28660;
	subscriber_arg_type *msg = malloc(sizeof(subscriber_arg_type));


    //unsigned long i = 0;
    pthread_t id = pthread_self();
    if(pthread_equal(id,tid[0]))
    {
        printf("\n\tEDSU.MES\n");
    }
    else
    {
        printf("\n Second thread processing\n");
    }
    //for(i=0; i<(0xFFFFFFFF);i++);
	pthread_mutex_lock(&mutex);

    int port = PortSource + 1025; // ports under 1024 require root access
	//printf("My port: %d\n", port);
    int s;
    int s_conec;
    unsigned int tam_dir;
    struct sockaddr_in dir_cliente;
    //TCP_Msg_editor *evento;
    //evento = (TCP_Msg_editor*) malloc(sizeof(TCP_Msg_editor));
	pthread_mutex_unlock(&mutex);
    s = get_socket_server(port);

    while(1) {


        tam_dir=sizeof(dir_cliente);
        if ((s_conec=accept(s, (struct sockaddr *)&dir_cliente, &tam_dir))<0){
            perror("error en accept");
            close(s);
            int ret = 1;
            pthread_exit(&ret);
        }
        memset(buf,0,strlen(buf));
        memset(msg->message,0,strlen(msg->message));

        if(read(s_conec, buf, MAX_LEN) < 0){
            perror("Error receiving message from intermediary");
            close(s);
            int ret = 1;
            pthread_exit(&ret);
        }
		//printf("Contenido del buffer1:%s\n", buf);
        deserializeMsg(buf, msg);
        if(msg->op == RCVMSG){

			//printf("Contenido del buffer2:%s\n", buf);
			fprintf(stdout, "\n\tID%d: %s\n\n",msg->source, msg->message);
			//msg->op = OK;
			//serializeMsg(buf,msg);
			//if (write(s_conec, buf, MAX_LEN) < 0) fprintf(stderr, "Error sending OK op msg back to intermediary\n");
        }
        else {
            printf("Error message: %s \n", msg->message);
        }


		//close(s); // persistente o 1 conexion por notificacion?

        memset(buf,0,strlen(buf));
        memset(msg->message,0,strlen(msg->message));

    }
	free(msg);
	close(s_conec);
	close(s);



    return NULL;
}


static bool leer_orden(char *message) {
	char linea[256];
	bool leido = false;
	
	do {
			//Introduzca una línea con la operación (0 = alta; 1 = baja) y el tema
        	//printf("\nIntroduzca una línea con el usuario destino y el mensaje\n");
		if (fgets(linea, 256, stdin)== NULL) return false; // EOF
		if ((sscanf(linea, "%99[^\n]%*c", message) >= 1)) //"%64s%64s%99[^\n]%*c", tema, valor,message) > 2)
			leido = true;
		else
        		printf("\nError format input\n");
	} while (!leido);
	return true;
}

int send_message() {
	unsigned int destiny;
	char message[100];
	char buf[MAX_LEN];
	char *server = "localhost";
	int port = 1140; //28660
	int socket = -1;
	char linea[256];

	pthread_mutex_lock(&mutex);
	//printf("\n envio mensajes por aquí en bucle\n");

	subscriber_arg_type *msg = malloc(sizeof(subscriber_arg_type));

	// first we get our id from intermediary

	socket = get_socket_user(server,port);
	if (socket == -1) fprintf(stderr, "Error 1 creating user\n");
	msg->op = NEWUSER;
	serializeMsg(buf,msg);
	if (write(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error 2 creating user\n");
	if (read(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error 3 creating user\n");
	deserializeMsg(buf, msg);

	PortSource = msg->source;	// We know our id at the server now
	close(socket);

	pthread_mutex_unlock(&mutex);

	printf("ID%d\n",PortSource);


	fgets(linea, 256, stdin);// EOF
	sscanf(linea, "%u", &destiny); //"%64s%64s%99[^\n]%*c", tema, valor,message) > 2)


	while (leer_orden(message))
	{

		msg->source = PortSource;
		msg->destiny = destiny;
		strcpy(msg->message,message);
		msg->op = SENDMSG;
		serializeMsg(buf , msg);

		socket = get_socket_user(server,port);
		if (socket == -1) fprintf(stderr, "Error connecting to intermediary\n");

		if (write(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error sending msg to intermediary\n");
		//if (read(socket, buf, MAX_LEN) < 0) fprintf(stderr, "Error receaving reply from intermediary\n");
		//deserializeMsg(buf, msg);

		//if (msg->op != OK) fprintf(stderr, "Error: reply message was not OK from intermediary\n");

		// clean/clear buffers memset
		memset(msg->message,0,strlen(msg->message));
		memset(buf,0,strlen(buf));
		close(socket);


	}

	free(msg);


	return 0;
}

int init_user() {


	/* Initialize mutex*/
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);



	pthread_create(&(tid[0]), NULL, &doSomeThing, NULL);


	send_message(); // function that controls the sending of messages from the user point of view. Main process/thread

	return 0;
}
