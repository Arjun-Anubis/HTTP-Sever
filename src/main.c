#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "log.h"
#include "network.h"
#include "http.h"

#define LOGLEVEL LOG_DEBUG

void transmission_handler( int fd, char * transmission, struct sockaddr_in address, struct RootConfig config )
{
	char *headers[] = { "HTTP/1.1 200 OK", "Sever: Custom", "Content-Type: text/html", ""};
	char *data[5] = { "Hello", ""};
	struct http_response response;
	response.headers = headers;

	data[0] = inet_ntoa( address.sin_addr );

	response.data = data;

	log_info( "Received header:\n\"%s\"\n and thats it", transmission );

	send_http_through_socket( fd, response );
}


int main( int argc, char ** argv ) 
{
	/* Options for our program, -l for log file,
	 * -f config file -p port -a bind address */
	/* -v for logging */

	FILE  * config_file;
	char config[1024];
	/* Change this later */
	struct json_object * json_config;
	struct json_object * address_json;
	struct json_object * port_json;
	struct RootConfig root_config;
	struct astring _root_dir;
	struct astring * root_dir = &_root_dir;
	int opt;
	int port = 8080;
	char address[ INET_ADDRSTRLEN ]  = "0.0.0.0";
	int loglevel = LOGLEVEL;

	log_set_level( loglevel );

	config_file = fopen( "config.json", "r" );
	fread( config, 1024, 1, config_file );
	json_config = json_tokener_parse( config );

	json_object_object_get_ex( json_config, "address", &address_json );
	json_object_object_get_ex( json_config, "port", &port_json );

	strncpy( address, json_object_get_string( address_json ), INET_ADDRSTRLEN );
	port = json_object_get_int( port_json );

	while ( ( opt = getopt( argc, argv, ":l:r:p:a:v" ) ) != -1 )
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
			case 'r':
				 root_config.root_dir = create_astring( root_dir, optarg );
				break;
			default:
				log_warn( "Unrecognized option %s", optarg );

		}
	}
	
	log_trace( "Set port: %d", port );
	log_trace( "Set address: %s", address );
	log_trace( "Set loglevel: %d", loglevel );


	strcpy( address, root_config.address_as_string );
	root_config.port = port;
	create_listener( root_config, transmission_handler );

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
