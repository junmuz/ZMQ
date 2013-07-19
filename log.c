#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int redis_log(char *address, char *key, char *val) {
        int sfd;
        struct sockaddr_in serv_addr;
        char sentbuf[100];
	sprintf(sentbuf, "*3\r\n$3\r\nSET\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", strlen(key), key, strlen(val), val);
//        strcpy(sentbuf,"*3\r\n$3\r\nSET\r\n$3\r\nxyz\r\n$8\r\n12345678\r\n");
        if ((sfd = socket (AF_INET, SOCK_STREAM, 6)) == -1) {           //For TCP
                printf("Unable to Log to REDIS Server");
        }

        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(6379);

        if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0) {
                printf("\n inet_pton error occured\n");
                return 1;
        }

        if( connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                printf("\n Error : Connect Failed \n");
                return 1;
        }

        write(sfd, sentbuf, strlen(sentbuf));

        close (sfd);
        return 0;
}
