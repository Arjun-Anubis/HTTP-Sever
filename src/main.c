#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <magic.h>

#include "log.h"
#include "network.h"
#include "http.h"

#define LOGLEVEL LOG_DEBUG

void transmission_handler( int fd, char * transmission, struct sockaddr_in address, struct RootConfig config )
{
	int requested_file;
	char * headers[] = { "HTTP/1.1 200 OK", "Sever: Custom", "Content-Type: text/html", ""};
	char * data[5];
	char * transmission_cpy;
	struct http_response response;
	struct http_request request;
	struct astring full_path;
	struct astring file;
	struct stat file_stat;
	const char * mime;
	magic_t magic;


	transmission_cpy = malloc( strlen( transmission ) );
	strcpy( transmission_cpy, transmission );

	/* data[0] = inet_ntoa( address.sin_addr ); */
	/* data[1] = config.root_dir.string; */
	/* data[2] = ""; */


	log_info( "Received header:\n%s", transmission );

	request = http_request_parser( transmission_cpy );
	if ( request.type < 0 )
	{
		log_warn( "Malformed request, dropping" );
		log_debug( "Request header: %s", transmission );
		return;
	}
	log_info( "Type: %d", request.type );
	log_info( "Path: %s", request.path );
	log_info( "Version: %s", request.version );

	full_path = create_astring( config.root_dir.string );
	full_path = append_to_string( full_path, request.path.string );

	log_info( "Fetching file %s", full_path.string );

	stat( full_path.string, &file_stat );
	 if ( (S_ISDIR( file_stat.st_mode)) )
	{
		log_warn( "%s is a directory redirecting to index.html", full_path.string );
		full_path = append_to_string( full_path, "index.html" );
	}

	stat( full_path.string, &file_stat );
	if (! (S_ISREG( file_stat.st_mode)) )
	{
		log_warn( "" );
		return;
	}
	

	if ( access( full_path.string, R_OK < 0 ) )
	{
		log_warn( "%s cannot be used for reading, sending 403 forbidden", full_path );
		return;
	}

	requested_file = open( full_path.string, O_RDONLY );
	file = read_file_into_alloced_string( requested_file );
	log_trace( "Read file successfully" );
	data[0] = file.string;
	data[1] = "";

	log_info( "Getting mime type.." );
	magic = magic_open(MAGIC_MIME); 
	magic_load(magic, NULL);
	magic_compile(magic, NULL);
	mime = magic_file(magic, full_path.string );
	log_info( "Mime type is %s", mime );
	magic_close( magic );
	/* log_trace( "Stuck?" ); */

	response.headers = headers;
	response.data = data;

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
				 root_config.root_dir = create_astring( optarg );
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
