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
	char * message;
	int message_len = 2;
	int data_len = 0;
	const char newline[2] = "\n";
	char ** headers = response.headers;
	char ** data = response.data;
	

	/* Find data length */
	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		data_len += strlen( data[ line_no ] ) + strlen( newline );
	}

	log_trace( "Data length is %d including new lines", message_len );

	message = malloc( message_len );
	strcpy( message, "" );

	/* Actually writing data to the buffer */

	for ( int line_no = 0; strlen( headers[ line_no ] ) != 0 ; line_no++ )
	{
		message_len += strlen( headers[ line_no ] ) + strlen( newline );
		message = realloc( message, message_len );
		strncat( message, headers[ line_no ], strlen( headers[ line_no ] ) );
		strncat( message, newline, strlen( newline ) );
	}

	/* Auto Setting headers like Content-Length, Server etc */
	{
		char * line_content_length;

		asprintf( &line_content_length, "Content-Length: %d\n", data_len );
		message_len += strlen( line_content_length );
		message = realloc( message, message_len );
		strcat( message, line_content_length );
		log_trace( "Added Header 1 is %s", line_content_length );
		free( line_content_length );
	
	}

	/* For new line */
	message_len += strlen( newline );
	message = realloc( message, message_len );
	strncat( message, newline, strlen( newline ) );

	for ( int line_no = 0; strlen( data[ line_no ] ) != 0 ; line_no++ )
	{
		message_len += strlen( data[ line_no ] ) + strlen( newline );
		message = realloc( message, message_len );
		strncat( message, data[ line_no ], strlen( data[ line_no ] ) );
		strncat( message, newline, strlen( newline ) );
	}

	log_trace( "Strlen of message is %d", strlen( message ) );
	log_info( "Writing %d bytes to fd %d", message_len, fd );
	log_trace( "Writing: \n%s", message );

	write( fd, message, message_len );
	free( message );
}
