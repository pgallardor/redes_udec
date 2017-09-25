#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


// las direcciones multicast rango: 224.0.0.1 a 239.255.255.255

extern int errno;
char bufer[1024];

struct bloque {
          char tx[12];//identidad del transmisor multicast
          char narch[32];//nombre del archivo transmitido
          int nb;// número del bloque transmitido
          int bb;// numero de bytes en el bloque, bb==0 => terminó el archivo
          char bytes[1024];//bloque de bytes del archivo
};



main(int argc, char * argv[]){
        struct  sockaddr_in server,stFrom;
        struct ip_mreq multi;
        int adrl, sock;
	struct bloque bq;
/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
//        server.sin_port = 0; //deja que el sistema operativo le asigne numero de puerto
        server.sin_port = htons(atoi(argv[2])); //deja que el sistema operativo le asigne numero de puerto
/********************************************************************/
        multi.imr_multiaddr.s_addr=inet_addr(argv[1]);
        multi.imr_interface.s_addr=INADDR_ANY;
        if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multi, sizeof(multi))<0){
                perror("no se puede agregar multicast");
                exit(1);
        }
/********************************************************************/
        if (bind(sock, (struct  sockaddr *)&server, sizeof(server))<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
/********************************************************************/

        adrl = sizeof (struct sockaddr_in);
        getsockname(sock, (struct  sockaddr *)&server, &adrl);
        printf("Me asignaron el puerto: %d\n",ntohs(server.sin_port));

/********************************************************************/
/********************************************************************/
	int count = 0;
	int file;
	file = open(argv[3] ,O_WRONLY | O_CREAT, 00644);

    while(1){

	int addr_size = sizeof(struct sockaddr_in);
	int i = recvfrom(sock, (void *)&bq, sizeof(bq), 0,(struct sockaddr*)&stFrom, &addr_size);
	char name[100];

	if(bq.nb == 0 && count == 0) {
        strcpy(name,bq.narch);
        write(file,bq.bytes,bq.bb);
        count++;
    }
    else if(!strcmp(name,bq.narch) && bq.nb == count){
        write(file,bq.bytes,bq.bb);
        printf("Block number: %d - Block size: %d\n", bq.nb, bq.bb);
        count++;
    }

        if (i < 0) {
        	perror("recvfrom() fallo!\n");
        	exit(1);
        }

        printf("Desde el host:%s puerta:%d, nombre_bloque:%s\n",inet_ntoa(stFrom.sin_addr), ntohs(stFrom.sin_port), bq.narch);

        //puede fallar si el último bloque tiene tamaño 1024
        if (bq.bb < 1024) break;
        /*if (read(sock,bufer,1024)<0)perror("recibiendo datagrama");
        printf("ip: %d || %s\n",sock,bufer);
	*/
        }
    close(file);

}
