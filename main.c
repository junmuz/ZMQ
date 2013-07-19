#include <czmq.h>

#include "include/zre.h"
#define MAX_BUFLEN	100
#define USEC 1 

int main (int argc, char *argv [])
{

/*
	ARGV[1]		Redis Server
	ARGV[2]		Login Name
*/

	int status;
	int fd, rd_len;
    	char temp[MAX_BUFLEN + 1];
	char buf[MAX_BUFLEN];
	char name[15];

        char msg[2 * MAX_BUFLEN];

        int msg_id = 0;
 
   	int result = 0;
    	int i = 0;
	fd_set read_fds;
	int num_peers = 0;

	if ((argc != 5) && (argc != 3)) {
		printf("Incorrct Arguments (Correct Format %s <Redis IP> <Login Name> <Peer IP> <Peer Port>\n", argv[0]);
		exit (-1);

	}

	fd = open("/dev/stdin", O_RDONLY);
	if (fd < 0) {
		printf("Unable to open stdin\n");
		exit (-1);
	}
	zre_interface_t *interface;
	if (argc ==5) {
    		interface = zre_interface_new (&argv[3]);
	}
	else {
		interface = zre_interface_new(NULL);
	}
    	zre_interface_join (interface, "GLOBAL");

    while (true) {
	FD_SET(fd, &read_fds);
	if (num_peers > 0) {
	select(fd + 1, &read_fds, NULL, NULL, NULL);
	if (FD_ISSET(fd, &read_fds)) {
                	zmsg_t *outgoing = zmsg_new ();
			int i = 0;
//			buf[0] = '\0';
			temp[0] = '\0';
			strncpy(buf, argv[2], strlen(argv[2]));
                	buf[strlen(argv[2])] = '\0';
			strncat(buf, ">>", 2);
			FD_CLR(fd, &read_fds);

			rd_len = read (fd, temp, MAX_BUFLEN);

			temp[rd_len - 1] = '\0';
			strncat(buf, temp, rd_len);
			
                	zmsg_addstr (outgoing, "GLOBAL");
              		zmsg_addstr (outgoing, buf);
			
	        	zre_interface_shout (interface, &outgoing);

//	                sprintf(msg,"redis-cli -h %s SET %s-%d \"%s\"\n", argv[1], name, msg_id++, buf);
			sprintf(msg, "%s-%d", argv[2], msg_id++);
			if (strlen(temp) > 1) {
//				system (msg);
				status = redis_log(argv[1], msg, buf);
				printf("\n");
			}
      			zmsg_destroy (&outgoing);
		}
	}

        zmsg_t *incoming = zre_interface_recv (interface);
        if (!incoming)
            break;              //  Interrupted

        //  If new peer, say hello to it and wait for it to answer us
        char *event = zmsg_popstr (incoming);
        if (streq (event, "ENTER")) {
            char *peer = zmsg_popstr (incoming);
            printf ("Peer Entered\n");
            zmsg_t *outgoing = zmsg_new ();
            zmsg_addstr (outgoing, peer);
            zmsg_addstr (outgoing, "Hello");
            zre_interface_whisper (interface, &outgoing);
	    zmsg_destroy(&outgoing);
            free (peer);
	    num_peers++;
        }
        else
        if (streq (event, "EXIT")) {
            char *peer = zmsg_popstr (incoming);
            printf ("Peer Exited\n", peer);
            free (peer);
	    num_peers--;
        }

        free (event);
        zmsg_destroy (&incoming);
    }

	close (fd);
    zre_interface_destroy (&interface);
    
    return 0;

}
