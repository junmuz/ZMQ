all: clean build

clean:
	rm client 

build:
	gcc main.c zre_group.c zre_interface.c zre_msg.c zre_peer.c zre_udp.c log.c -o client -lczmq -luuid -g
