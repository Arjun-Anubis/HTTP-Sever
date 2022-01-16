#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "log.h"

struct astring append_to_string( struct astring string_to_append_to, char * string_to_append )
{
	log_trace( "Original length: %d", string_to_append_to.length );
	log_trace( "String to append %s", string_to_append );
	string_to_append_to.length += strlen( string_to_append );
	log_trace( "Changed length to %d", string_to_append_to.length );
	string_to_append_to.string = realloc( string_to_append_to.string, string_to_append_to.length );
	strncat( string_to_append_to.string, string_to_append, strlen( string_to_append ) );
	return  string_to_append_to;
}
struct astring  create_astring(  char * string )
{
	struct astring kstring;
	kstring.length = strlen( string ) + 1 ;
	kstring.string = malloc( kstring.length );
	strncpy( kstring.string, string, kstring.length );
	return kstring;
	
}

void free_astring( struct astring string )
{
	free( string.string );
}


struct astring read_file_into_alloced_string( int fd )
{
	struct astring string;
	int read_size;
	char buffer[ BUFFER_SIZE ];
	string = create_astring( "" );
	
	log_error( "File descriptor is %d", fd );
	for (;;)
	{
		read_size = read( fd, buffer, BUFFER_SIZE );
		log_trace( "Buffer is %s", buffer );
		string = append_to_string( string, buffer );
		if ( read_size < BUFFER_SIZE )
		{
			break;
		}
	}
	log_info( "Finished read from file " );
	log_trace( "File is \"%s\"", string.string );
	log_trace( "Thats it" );

	return string;
}
