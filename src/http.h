struct http_response 
{
	char ** headers;
	char ** data;
};

void send_http_through_socket
(
	int fd,
	struct http_response response
 );
