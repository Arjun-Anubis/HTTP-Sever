#include <arpa/inet.h>

int create_listener
(
	const char address_as_string[ INET_ADDRSTRLEN ],
	int port,
	void (* transmission_handler)( int, char *, struct sockaddr_in )
);
