struct http_response 
{
	char ** headers;
	char ** data;
};

struct http_request
{
	int type;
	char * path;
	char * version;
	char ** headers;
	char * data;
};

void send_http_through_socket
(
	int fd,
	struct http_response response
);

enum { GET, POST, PUT };

struct http_request http_request_parser( char * transmission );
