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
        conexion=conectarCon(argv[1], argv[2]);
        file = open("recibido.men", O_WRONLY | O_CREAT, 00644);  
        //write(conexion, &numero, sizeof(numero));   //escribe al servidor
        while(read(conexion, buffer, sizeof(buffer)) > 0){
                write(file, buffer, sizeof(buffer));
                bzero(buffer, sizeof(buffer));  
        }
        
        close(file);
        close(conexion);
}

