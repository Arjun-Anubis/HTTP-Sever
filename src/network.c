#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "log.h"
#include "network.h"

#define CONNECTION_QUEUE_SIZE 5

void tcp_protocol
(
	int fd,
	void (*transmission_handler)( int, char *, struct sockaddr_in, struct RootConfig config ),
	int connection_number,
	struct sockaddr_in address,
	struct RootConfig config
)
{
	/* Doc String */
	char buffer[BUFFER_SIZE];
	char * transmission;
	int transmission_size = 1;
	int read_size = 1;

	log_info
	(
		"Entering TCP Handler for connection number %d from %s port %d",
	   	connection_number,
	   	inet_ntoa( address.sin_addr ),
	   	address.sin_port 
	);

	for ( int transmission_number = 1; read_size > 0 ; transmission_number++ ) {

		transmission_size = 1;
		transmission = malloc( 1 );
		int packet_number;
		/* This is one transmission */
		/* also this will be refactored into the networking function soon */
		for 
		(
			packet_number = 0;
			( read_size = read( fd, buffer, BUFFER_SIZE ) );
			packet_number++
		) 
		{
			log_trace( "Received %d bytes in packet number %d", read_size, packet_number );
			transmission_size += read_size;
			log_trace( "Transmission is now %d bytes long", transmission_size );
			transmission = realloc( transmission, transmission_size );
			strncat ( transmission, buffer, read_size );

			if ( read_size < BUFFER_SIZE ) 
			{
				log_info( "Recieved less than buffer_length breaking tranmission" );
				break;
			}
		}

		log_info( "End of transmission %d on connection number %d", transmission_number, connection_number );
		log_info( "TCP handler, strlen transmission is %d and transmission_size is %d", strlen( transmission ), transmission_size );
		if ( read_size == 0 )
			if ( packet_number == 0 )
			{
				log_info( "Exiting due to empty packet" );
				break;
			}

		strncpy( transmission, transmission, transmission_size );
		transmission_handler( fd, transmission, address, config );
		free( transmission );
		
		/* Call transmission handler */
	}
	log_info( "Exiting TCP handler for connection number %d", connection_number );
	close( fd );
}

int create_listener
(
 	struct RootConfig  config,
	void (*transmission_handler)( int, char *, struct sockaddr_in, struct RootConfig config )
)
{
	int sock;
	int connection;
	int opt = 1;
	/* char buffer[ BUFFER_SIZE ]; */
	char address_as_string[ INET_ADDRSTRLEN ];
	int port = config.port;
	strcpy( address_as_string, config.address_as_string );
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

	log_info( "Recieved root_dir %s", config.root_dir.string );
	log_info( "Length is %d", config.root_dir.length );
	for ( int pid, i = 0; ; i++ )
	{
		if ( ( connection = accept( sock, ( struct sockaddr * )&bind_address, ( socklen_t * )&addrlen ) ) < 0 )
		{
			perror( "accept" );
			return errno;
		}
		if ( ( pid = fork() ) == 0 )
		{
			log_info( "Connection from %s port %d connection number %d", inet_ntoa ( bind_address.sin_addr ), bind_address.sin_port, i );
			tcp_protocol( connection, transmission_handler, i, bind_address, config );
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
