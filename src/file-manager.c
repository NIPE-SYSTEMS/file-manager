#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsoup/soup.h>
#include <netinet/in.h>

// void (*)(struct SoupServer *, struct SoupMessage *, char *, struct GHashTable *, struct SoupClientContext *, void *)
static void cb_get(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query, SoupClientContext *client, gpointer data)
{
	// GSocketAddress *sockaddr = NULL;
	// struct sockaddr sockaddr_native;
	// SoupAddress *address = NULL;
	
	// sockaddr = soup_client_context_get_remote_address(client);
	// g_socket_address_to_native(sockaddr, &sockaddr_native, g_socket_address_get_native_size(sockaddr), NULL);
	// address = soup_address_new_from_sockaddr(&sockaddr_native, sizeof(struct sockaddr));
	
	// printf("Incoming connection from %s\n", soup_address_get_physical(address));
	
	printf(" -> \x1b[1m");
	
	if(msg->method == SOUP_METHOD_OPTIONS) printf("OPTIONS");
	else if(msg->method == SOUP_METHOD_GET) printf("GET");
	else if(msg->method == SOUP_METHOD_HEAD) printf("HEAD");
	else if(msg->method == SOUP_METHOD_PUT) printf("PUT");
	else if(msg->method == SOUP_METHOD_POST) printf("POST");
	else if(msg->method == SOUP_METHOD_DELETE) printf("DELETE");
	else if(msg->method == SOUP_METHOD_TRACE) printf("TRACE");
	else if(msg->method == SOUP_METHOD_CONNECT) printf("CONNECT");
	else if(msg->method == SOUP_METHOD_PROPFIND) printf("PROPFIND");
	else if(msg->method == SOUP_METHOD_PROPPATCH) printf("PROPPATCH");
	else if(msg->method == SOUP_METHOD_MKCOL) printf("MKCOL");
	else if(msg->method == SOUP_METHOD_COPY) printf("COPY");
	else if(msg->method == SOUP_METHOD_MOVE) printf("MOVE");
	else if(msg->method == SOUP_METHOD_LOCK) printf("LOCK");
	else if(msg->method == SOUP_METHOD_UNLOCK) printf("UNLOCK");
	
	printf("\x1b[0m %s\n", path);
	
	if(msg->method == SOUP_METHOD_GET)
	{
		printf("<- \x1b[1m%i\x1b[0m OK %s\n", SOUP_STATUS_OK, path);
		
		soup_message_set_status(msg, SOUP_STATUS_OK);
		soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "TODO: Insert regular index file.", 33);
		return;
	}
	
	if(msg->method == SOUP_METHOD_POST)
	{
		if(msg->request_body->length == 0)
		{
			printf("<- \x1b[1m%i\x1b[0m Bad Request %s\n", SOUP_STATUS_BAD_REQUEST, path);
			
			soup_message_set_status(msg, SOUP_STATUS_BAD_REQUEST);
			soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "400 Bad Request", 16);
			return;
		}
		
		printf("    sent data: %s\n", msg->request_body->data);
		
		printf("<- \x1b[1m%i\x1b[0m OK %s\n", SOUP_STATUS_OK, path);
		
		soup_message_set_status(msg, SOUP_STATUS_OK);
		soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "TODO: Insert reaction.", 23);
		return;
	}
	
	printf("<- \x1b[1m%i\x1b[0m Not Implemented %s\n", SOUP_STATUS_NOT_IMPLEMENTED, path);
	
	soup_message_set_status(msg, SOUP_STATUS_NOT_IMPLEMENTED);
}

int main(void)
{
	GMainLoop *loop = NULL;
	SoupServer *server = NULL;
	
	server = soup_server_new(SOUP_SERVER_SERVER_HEADER, "NIPE-SYSTEMS", NULL);
	soup_server_listen_local(server, 8000, 0, NULL);
	soup_server_add_handler(server, NULL, cb_get, NULL, NULL);
	
	loop = g_main_loop_new(NULL, TRUE);
	g_main_loop_run(loop);
	
	return 0;
}