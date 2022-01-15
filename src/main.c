#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "network.h"
#include "http.h"

#define LOGLEVEL LOG_DEBUG

void transmission_handler( int fd, char * transmission, struct sockaddr_in address )
{
	char *headers[4] = { "HTTP/1.1 200 OK", "Sever: Custom", "Content-Type: text/html", ""};
	char *data[5] = { "Hello", ""};
	struct http_response response;
	response.headers = headers;

	data[0] = inet_ntoa( address.sin_addr );

	response.data = data;

	log_info( "Received header:\n%s", transmission );

	send_http_through_socket( fd, response );
}


int main( int argc, char ** argv ) 
{
	/* Options for our program, -l for log file,
	 * -f config file -p port -a bind address */
	/* -v for logging */

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
	log_trace( "Set port: %d", port );
	log_trace( "Set address: %s", address );
	log_trace( "Set loglevel: %d", loglevel );

	create_listener( address, port, transmission_handler );

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
