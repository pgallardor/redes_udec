#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

extern int errno;
int numero;
int obtenerSocket(char * puerto){
        struct  sockaddr_in server;
        int x, sock;
/********************************************************************/
        sock= socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(atoi(puerto));  //lee numero de puerto desde teclado
        x = bind(sock, (struct  sockaddr *)&server, sizeof(server));
        if (x<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
/********************************************************************/
        if (listen(sock, 5) < 0) {
                perror ("no listen");
                exit (1);
        }
  return sock;
}
int aceptarConexion(int sock){
        struct  sockaddr_in server;
        int adrl, conexion;

/********************************************************************/
        adrl = sizeof (struct sockaddr_in);
        printf("Servidor esperando a cliente\n");
        conexion = accept(sock, (struct  sockaddr *)&server, &adrl); //espera y acepta conexion del cliente
        return conexion;     
}


    
main(int argc, char * argv[]){
        int sock, l, conexion, leido;
        int file = open(argv[2], O_RDONLY);
        char buffer[1024];
        bzero(buffer, sizeof(buffer));
/********************************************************************/
        sock = obtenerSocket(argv[1]);
        conexion = aceptarConexion(sock); //espera y acepta conexion del cliente
/********************************************************************/
        int sent_size;

        do{
            sent_size = read(file, buffer, sizeof(buffer));
            if (sent_size <= 0) break;
            write(conexion, buffer, sent_size);
            //l=read(conexion, &numero, sizeof(numero)); //recibe del cliente
            bzero(buffer, sizeof(buffer));

        }while(sent_size > 0);
        
        close(file);
        close(conexion);
}

