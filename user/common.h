

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>

#define TAM	64
#define MAX_LEN 256

#define SUBS 5
#define TEMAS 5

#define NTEMAS		32
#define NSUSCRIPTORES	1024


#define HOST_SERVER	"127.0.0.1"
#define HOST_CLIENT	"127.0.0.1"

/* Operaciones disponibles */
#define ALTA 		0 
#define BAJA		1 
#define RECEPCION	2 //El server recibe un evento generado por el editor
#define OK			3
#define ERROR		4
#define SENDMSG		5
#define RCVMSG		6
#define NEWUSER		7


typedef struct type_subscriber_test
{

	unsigned int source;
	unsigned int destiny;
	int op;
	char message[100];


} subscriber_arg_type;



int get_socket_server(int port);
int serializeMsg(char* buf,subscriber_arg_type *msg);
int deserializeMsg(char* buf, subscriber_arg_type *msg);
int get_socket_user(char* servidor, int port);
