#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "network.h"
#include "http.h"

#define LOGLEVEL LOG_DEBUG
#define BUFFER_SIZE 30

void send_http_through_socket
(
	int fd,
	struct http_response response
 )
{
	char * message;
	int message_len = 0;
	int data_len = 0;
	const char newline[2] = "\n";
	const char ** headers = response.headers;
	char *auto_added_headers[ 2 ];
	const char ** data = response.data;
	
	for ( int line_no = 0; strlen( headers[ line_no ] ) != 0 ; line_no++ )
	{
		message_len += strlen( headers[ line_no ] ) + strlen( newline );
	}

	log_debug( "Header length is %d including new lines", message_len );
	/* For new line */
	message_len++;

	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		message_len += strlen( data[ line_no ] ) + strlen( newline );
		data_len += strlen( data[ line_no ] ) + strlen( newline );
	}

	log_debug( "Data length is %d including new lines", message_len );
	message = malloc( message_len );

	/* Actually writing data to the buffer */

	for ( int line_no = 0; strlen( headers[ line_no ] ) != 0 ; line_no++ )
	{
		strncat( message, headers[ line_no ], strlen( headers[ line_no ] ) );
		strncat( message, newline, strlen( newline ) );
	}

	/* For new line */
	strncat( message, newline, strlen( newline ) );

	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		strncat( message, data[ line_no ], strlen( data[ line_no ] ) );
		strncat( message, newline, strlen( newline ) );
	}

	log_info( "Strlen of message is %d", strlen( message ) );
	log_info( "Writing %d bytes to fd %d", message_len, fd );
	log_debug( "Writing: %s", message );

	write( fd, message, message_len );
	free( message );
}
void protocol
(
	int fd
)
{
	char buffer[BUFFER_SIZE];
	int read_size = 1;
	const char *headers[5] = { "HTTP/1.1 200 OK", "Sever: Custom", "Content-Length: 6", "Content-Type: text/html", ""};
	const char *data[5] = { "Hello", ""};
	struct http_response response;
	response.headers = headers;
	response.data = data;

	log_info( "Recieved fd %d to handle", fd );

	for ( int transmission_number = 1; read_size > 0 ; transmission_number++ ) {
		/* This is one transmission */
		/* also this will be refactored into the networking function soon */
		for 
		(
			int packet_number = 0;
			( read_size = read( fd, buffer, BUFFER_SIZE ) ) >= BUFFER_SIZE ;
			packet_number++
		) 
		{
			log_info( "Received %d bytes in packet number %d", read_size, packet_number );
		}

		log_info( "End of transmission %d", transmission_number );
		send_http_through_socket( fd, response );
		
		/* Call transmission handler */
	}
	log_info( "Exiting handler" );
}

int main( int argc, char ** argv ) 
{
	/* Options for our program, -f config file -p port -a bind address */
	/* -l for logging */
	int opt;
	int port = 8080;
	char address[ INET_ADDRSTRLEN ]  = "0.0.0.0";
	int loglevel = LOGLEVEL;

	log_set_level( loglevel );

	while ( ( opt = getopt( argc, argv, ":l:p:a:v" ) ) != -1 )
	{
		switch( opt )
		{
			case 'p':
				port = atoi( optarg );
				break;
			case 'a':
				strncpy( address, optarg, INET_ADDRSTRLEN );
				break;
			case 'v':
				log_set_level( --loglevel );
				break;
			case 'l':
				break;
			default:
				log_warn( "Unrecognized option %s", optarg );

		}
	}
	log_info( "Set port: %d", port );
	log_info( "Set address: %s", address );
	log_info( "Set loglevel: %d", loglevel );

	create_listener( address, port, protocol );

	return 0;
}
void log_test()
{
	log_fatal( "Fatal" );
	log_error( "Error" );
	log_warn ( "Warn"  );
	log_info ( "Info"  );
	log_debug( "Debug" );
	log_trace( "Trace" );
}
