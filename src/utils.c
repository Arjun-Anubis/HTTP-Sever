#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "log.h"

struct astring append_to_string( struct astring string_to_append_to, char * string_to_append )
{
	log_trace( "Original length: %d", string_to_append_to.length );
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
