struct astring {
	char * string;
	int length;
};

struct astring  append_to_string( struct astring  string_to_append_to, char * string_to_append );
struct astring  create_astring( char * string );
void free_astring( struct astring string );
