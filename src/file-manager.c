#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include <netinet/in.h>

#define UNUSED(x) (void)(x)

static void cb_get(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query, SoupClientContext *client, gpointer data)
{
	JsonGenerator *json_generator = NULL;
	JsonBuilder *json_builder = NULL;
	JsonNode *json_root = NULL;
	gchar *json_generated = NULL;
	gsize json_generated_length = 0;
	
	UNUSED(server);
	UNUSED(query);
	UNUSED(client);
	UNUSED(data);
	
	// logging
	printf("--> \x1b[1m");
	
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
	
	// GET
	if(msg->method == SOUP_METHOD_GET)
	{
		printf("<-- \x1b[1m%i\x1b[0m OK %s\n", SOUP_STATUS_OK, path);
		
		soup_message_set_status(msg, SOUP_STATUS_OK);
		soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "TODO: Insert regular index file.", 33);
		return;
	}
	
	// POST
	if(msg->method == SOUP_METHOD_POST)
	{
		if(msg->request_body->length == 0)
		{
			printf("<-- \x1b[1m%i\x1b[0m Bad Request %s\n", SOUP_STATUS_BAD_REQUEST, path);
			
			soup_message_set_status(msg, SOUP_STATUS_BAD_REQUEST);
			soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "400 Bad Request", 16);
			return;
		}
		
		printf("  > %s\n", msg->request_body->data);
		
		printf("<-- \x1b[1m%i\x1b[0m OK %s\n", SOUP_STATUS_OK, path);
		
		json_generator = json_generator_new();
		json_builder = json_builder_new();
		
		// {
		json_builder_begin_object(json_builder);
		//     "url": "http://www.gnome.org/img/flash/two-thirty.png"
		json_builder_set_member_name(json_builder, "url");
		json_builder_add_string_value(json_builder, "http://www.gnome.org/img/flash/two-thirty.png");
		//     "size": [652,242]
		json_builder_set_member_name(json_builder, "size");
		json_builder_begin_array(json_builder);
		json_builder_add_int_value(json_builder, 652);
		json_builder_add_int_value(json_builder, 242);
		json_builder_end_array(json_builder);
		// }
		json_builder_end_object(json_builder);
		
		json_root = json_builder_get_root(json_builder);
		json_generator_set_root(json_generator, json_root);
		json_generated = json_generator_to_data(json_generator, &json_generated_length);
		
		soup_message_set_status(msg, SOUP_STATUS_OK);
		soup_message_set_response(msg, "text/plain", SOUP_MEMORY_COPY, json_generated, json_generated_length);
		
		printf("<   %s\n", json_generated);
		
		g_free(json_generated);
		
		json_node_free(json_root);
		g_object_unref(json_generator);
		g_object_unref(json_builder);
		return;
	}
	
	// Not Implemented
	printf("<-- \x1b[1m%i\x1b[0m Not Implemented %s\n", SOUP_STATUS_NOT_IMPLEMENTED, path);
	
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