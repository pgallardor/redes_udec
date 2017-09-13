#include <strings.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

extern int errno;

struct block_t {
	char tx[12]; //identidad del transmisor multicast
    char narch[32]; //nombre del archivo transmitido
    int nb; // número del bloque transmitido
    int bb; // numero de bytes en el bloque, bb==0 => terminó el archivo
    char bytes[1024];//bloque de bytes del archivo
};

typedef struct block_t bloque;

int conectarCon(char * host, char * puerto){
        int conexion, l;
        struct  sockaddr_in client;
        struct  hostent *hp, *gethostbyname();
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(atoi(puerto));
        if (isdigit(host[0]))
                client.sin_addr.s_addr = inet_addr(host);
        else {
                hp = gethostbyname(host);
                if (hp == NULL) {
                        perror ("¿Cual host ? ");
                        exit (1);
                }
                bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
        }
/*******************************************************************/
        conexion = socket(AF_INET, SOCK_STREAM, 0);
        if (conexion < 0) {
                perror ("no es posible crear socket");
                exit (1);
        }
/*******************************************************************/
        connect(conexion, (struct  sockaddr *)&client, sizeof(client)); //se conecta con el servidor
        return conexion;
}
main(int argc, char * argv[]){
        int received, file;
        char buffer[1024];
        bzero(buffer, sizeof(buffer));
        int conexion;
        bloque to_send;
        conexion=conectarCon(argv[1], argv[2]);
        file = open(argv[3], O_WRONLY | O_CREAT, 00644);  
        int file_size;

        do{
        	file_size = read(conexion, buffer, sizeof(buffer));
        	if (file_size <= 0) break;
        	write(file, buffer, file_size);
        	bzero(buffer, sizeof(buffer));
        }while(file_size > 0);

        close(file);


        //copypasta del transmisor, comentar antes de usar
        int sock;
        struct  sockaddr_in name;
        struct  hostent *hp, *gethostbyname();

        file = open(argv[3], O_RDONLY);
/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        hp = gethostbyname(argv[4]);
        if (hp == 0) {
                fprintf(stderr, "%s: host desconocido\n", argv[1]);
                exit(2);
        }
        bcopy(hp->h_addr, &name.sin_addr, hp->h_length);
        name.sin_family = AF_INET;
        name.sin_port = htons(atoi(argv[5]));

        strcpy(to_send.tx, "f80f41e4292");
	strcpy(to_send.narch, argv[3]);
	//to_send.tx = 0xf80f41e4292d;
        //to_send.narch = "lol.men";
        
        while(1){
        	to_send.nb = 0;
		file_size = 0;
        	do{
        		file_size = read(file, to_send.bytes, sizeof(to_send.bytes));
        		if (file_size <= 0) break;
        		to_send.bb = file_size;

        		if(sendto(sock, (void *)&to_send, sizeof(bloque), 0, (struct sockaddr * )&name, sizeof(name))<0)
            	    perror("enviando el datagrama");

        		to_send.nb += 1;
        		bzero(to_send.bytes, sizeof(to_send.bytes));
        	}while(file_size > 0);
		
		close(file);
		open(argv[3], O_RDONLY);
        }
        
        close(file);
        close(conexion);
}

