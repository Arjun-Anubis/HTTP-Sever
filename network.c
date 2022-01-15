#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"


#define CONNECTION_QUEUE_SIZE 5
/* #define BUFFER_SIZE 30 */

int create_listener
(
	const char address_as_string[ INET_ADDRSTRLEN ],
	int port,
	int * protocol( int fd )
)
{
	int sock;
	int connection;
	int opt = 1;
	/* char buffer[ BUFFER_SIZE ]; */
	struct sockaddr_in bind_address;
	int addrlen = sizeof( bind_address );


	/* Configure address */
	/* log_warn( "Using 0.0.0.0 instead of %s", address_as_string ); */
	inet_aton( address_as_string, &( bind_address.sin_addr ) );
	bind_address.sin_port = htons( port );
	bind_address.sin_family = AF_INET;
	
	if ( ( sock = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror ( "socket error: " );
		return errno;
	}
	if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt) ) )
	{
		perror ( "setsockopt: " );
		return errno;
	}
	if ( bind( sock, ( struct sockaddr * )&bind_address, sizeof( bind_address ) ) < 0 )
	{
		perror( "Bind failed" );
		return errno;
	}	
	if ( listen( sock, CONNECTION_QUEUE_SIZE ) < 0 )
	{
		perror( "listen" );
		return errno;
	}

	for ( int pid, i = 0; ; i++ )
	{
		if ( ( connection = accept( sock, ( struct sockaddr * )&bind_address, ( socklen_t * )&addrlen ) ) < 0 )
		{
			perror( "accept" );
			return errno;
		}
		if ( ( pid = fork() ) == 0 )
		{
			log_info( "Connection from %s port %d", inet_ntoa ( bind_address.sin_addr ), bind_address.sin_port );
			protocol( connection );
		}
		else
		{
			log_info
			(
				"Spawned listener for connection number %d from %s on port %d pid no %d", 
				i,
				inet_ntoa( bind_address.sin_addr ), 
				bind_address.sin_port,
				pid
			);
		}
	}
}
