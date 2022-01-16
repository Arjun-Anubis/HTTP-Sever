#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "http.h"
#include "log.h"

#define STRLEN(s) (sizeof(s)/sizeof(s[0]))

void send_http_through_socket
(
	int fd,
	struct http_response response
 )
{
	struct astring message;

	int data_len = 0;
	char newline[2] = "\n";
	char ** headers = response.headers;
	char ** data = response.data;
	

	log_info( "Entered HTTP sender" );
	/* Find data length */
	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		data_len += strlen( data[ line_no ] ) + strlen( newline );
	}

	message = create_astring( "" );

	/* Actually writing data to the buffer */

	for ( int line_no = 0; strlen( headers[ line_no ] ) != 0 ; line_no++ )
	{
		message = append_to_string( message, headers[ line_no ] );
		message = append_to_string( message, newline );
		log_trace( "Message is now  %d bytes", message.length );

	}

	/* Auto Setting headers like Content-Length, Server etc */
	{
		char * line_content_length;

		asprintf( &line_content_length, "Content-Length: %d\n", data_len );
		message = append_to_string( message, line_content_length );
		free( line_content_length );
	}

	/* For new line */
	message = append_to_string( message, newline );

	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		message = append_to_string( message, data[ line_no ] );
		message = append_to_string( message, newline );
	}	

	log_trace( "Strlen of message is %d", message.length );
	log_info( "Writing %d bytes to fd %d", message.length, fd );
	log_info( "Writing: \n%s", message );

	write( fd, message.string, message.length );
	free_astring( message );
}
struct http_request http_request_parser( char * transmission )
{
	char * buffer;
	char * transmission_cpy;
	struct http_request request;

	log_trace( "Transmission is %s", transmission );
	transmission_cpy = malloc( strlen( transmission )  );

	strcpy( transmission_cpy, transmission );

	char space[] = " ";
	char newline[] = "\n";

	log_trace( "Copy is %s", transmission_cpy );
	buffer = strtok( transmission_cpy, space );
	log_trace( "First Token is %s", buffer );

	if ( strcmp( buffer, "GET" ) == 0)
		request.type = GET;
	else if ( strcmp ( buffer, "POST" ) == 0 )
		request.type = POST;
	else if ( strcmp ( buffer, "PUT" ) == 0 )
		request.type = PUT;
	else 
	{
		log_error( "Invalid request" );
		request.type = -1;
		return request;
	}
	
	buffer = strtok( NULL, space );
	log_trace( "Second Token is %s", buffer );

	/* request.path = malloc( strlen( buffer )  + 1 ); */
	/* strncpy( request.path, buffer, strlen( buffer ) + 1 ); */
	request.path = create_astring( buffer );

	buffer = strtok( NULL, newline );
	log_trace( "Third Token is %s", buffer );

	/* request.version = malloc( strlen( buffer ) + 1 ); */
	/* strncpy( request.version, buffer, strlen( buffer ) + 1 ); */
	request.version = create_astring( buffer );

	free( transmission_cpy );
	return request;
}

void free_http_request( struct http_request request )
{
	/* free( request.path ); */
	/* free( request.version ); */
	free_astring( request.path );
	free_astring( request.version );
}
