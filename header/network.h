#include "utils.h"
#include <arpa/inet.h>

struct RootConfig {
	const char address_as_string[ INET_ADDRSTRLEN ];
	int port;
	struct astring root_dir;
};
int create_listener
(
 	struct RootConfig config,
	void (* transmission_handler)( int, char *, struct sockaddr_in, struct RootConfig config )
);
