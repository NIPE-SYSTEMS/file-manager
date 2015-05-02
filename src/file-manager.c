/**
 * Copyright (C) 2015 NIPE-SYSTEMS
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include <netinet/in.h>
#include <dirent.h>

#define UNUSED(x) (void)(x)

typedef struct
{
	gchar *name;
} file_entry_t;

typedef struct
{
	gchar *path;
	GPtrArray *files;
	gchar *error_message;
} files_t;

static void directory_foreach_file(gpointer data, gpointer json_builder)
{
	file_entry_t *file = (file_entry_t *)data;
	
	json_builder_begin_object(json_builder);
	json_builder_set_member_name(json_builder, "name");
	if(file != NULL)
	{
		json_builder_add_string_value(json_builder, file->name);
	}
	else
	{
		json_builder_add_string_value(json_builder, "Error");
	}
	json_builder_end_object(json_builder);
}

static void directory_cleanup_ptr_array_element(gpointer data)
{
	file_entry_t *file = (file_entry_t *)data;
	
	if(file->name != NULL)
	{
		free(file->name);
	}
	
	free(file);
}

static files_t *directory_retrieve(gchar *path)
{
	GFile *directory_file = NULL;
	GDir *directory = NULL;
	const gchar *file_name = NULL;
	GError *error = NULL;
	files_t *files = NULL;
	file_entry_t *file = NULL;
	
	files = malloc(sizeof(files_t));
	if(files == NULL)
	{
		fprintf(stderr, "\e[0;31mFailed to allocate memory for files structure\e[0m\n");
		return NULL;
	}
	files->path = NULL;
	files->files = NULL;
	files->error_message = NULL;
	
	directory_file = g_file_new_for_path(path);
	files->path = g_file_get_path(directory_file);
	g_object_unref(directory_file);
	
	directory = g_dir_open((const gchar *)path, 0, &error);
	if(error != NULL)
	{
		fprintf(stderr, "\e[0;31mFailed to open directory: %s\e[0m\n", error->message);
		files->error_message = g_strdup_printf("Failed to open directory: %s", error->message);
		g_error_free(error);
		
		return files;
	}
	
	files->files = g_ptr_array_new_with_free_func(directory_cleanup_ptr_array_element);
	
	while((file_name = g_dir_read_name(directory)) != NULL)
	{
		file = malloc(sizeof(file_entry_t));
		if(file == NULL)
		{
			fprintf(stderr, "\e[0;31mFailed to allocate memory for file structure\e[0m\n");
			files->error_message = g_strdup_printf("Failed to allocate memory for file structure");
			break;
		}
		file->name = NULL;
		
		file->name = strdup(file_name);
		
		g_ptr_array_add(files->files, file);
	}
	
	g_dir_close(directory);
	
	return files;
}

static void directory_cleanup(files_t *files)
{
	if(files == NULL)
	{
		return;
	}
	
	if(files->files != NULL)
	{
		g_ptr_array_free(files->files, TRUE);
	}
	
	if(files->path != NULL)
	{
		free(files->path);
	}
	
	free(files);
}

static void cb_get(SoupServer *server, SoupMessage *msg, const gchar *path, GHashTable *query, SoupClientContext *client, gpointer data)
{
	JsonGenerator *json_generator = NULL;
	JsonBuilder *json_builder = NULL;
	JsonNode *json_root = NULL;
	gchar *json_generated = NULL;
	gsize json_generated_length = 0;
	
	JsonParser *json_parser = NULL;
	JsonReader *json_reader = NULL;
	GError *json_parser_error = NULL;
	
	files_t *files = NULL;
	
	UNUSED(server);
	UNUSED(query);
	UNUSED(client);
	UNUSED(data);
	
	// logging
	printf("--> \e[1m");
	
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
	
	printf("\e[0m %s\n", path);
	
	// GET
	if(msg->method == SOUP_METHOD_GET)
	{
		printf("<-- \e[1m%i\e[0m OK %s\n", SOUP_STATUS_OK, path);
		
		soup_message_set_status(msg, SOUP_STATUS_OK);
		soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "TODO: Insert regular index file.", 33);
		return;
	}
	
	// POST
	if(msg->method == SOUP_METHOD_POST)
	{
		if(msg->request_body->length == 0)
		{
			printf("<-- \e[1m%i\e[0m Bad Request %s\n", SOUP_STATUS_BAD_REQUEST, path);
			
			soup_message_set_status(msg, SOUP_STATUS_BAD_REQUEST);
			soup_message_set_response(msg, "text/plain", SOUP_MEMORY_STATIC, "400 Bad Request", 16);
			return;
		}
		
		printf("  > %s\n", msg->request_body->data);
		
		json_parser = json_parser_new();
		json_parser_load_from_data(json_parser, msg->request_body->data, msg->request_body->length, &json_parser_error);
		if(json_parser_error != NULL)
		{
			fprintf(stderr, "\e[0;31mFailed to parse JSON post data: %s\e[0m\n", json_parser_error->message);
			g_error_free(json_parser_error);
		}
		else
		{
			json_reader = json_reader_new(json_parser_get_root(json_parser));
			// path
			json_reader_read_member(json_reader, "path");
			if(json_reader_is_value(json_reader))
			{
				files = directory_retrieve((gchar *)json_reader_get_string_value(json_reader));
				if(files != NULL && files->path != NULL)
				{
					printf("    Retrieved path: %s\n", files->path);
				}
			}
			json_reader_end_member(json_reader);
			
			g_object_unref(json_reader);
		}
		
		g_object_unref(json_parser);
		
		printf("<-- \e[1m%i\e[0m OK %s\n", SOUP_STATUS_OK, path);
		
		json_generator = json_generator_new();
		json_builder = json_builder_new();
		
		json_builder_begin_object(json_builder);
		json_builder_set_member_name(json_builder, "status");
		
		if(files == NULL)
		{
			json_builder_add_boolean_value(json_builder, FALSE);
			json_builder_set_member_name(json_builder, "reason");
			json_builder_add_string_value(json_builder, "Invalid JSON request or low-level server error.");
		}
		else
		{
			if(files->error_message != NULL)
			{
				json_builder_add_boolean_value(json_builder, FALSE);
				json_builder_set_member_name(json_builder, "reason");
				json_builder_add_string_value(json_builder, files->error_message);
				g_free(files->error_message);
			}
			else
			{
				json_builder_add_boolean_value(json_builder, TRUE);
			}
			
			if(files->files != NULL)
			{
				json_builder_set_member_name(json_builder, "files");
				json_builder_begin_array(json_builder);
				g_ptr_array_foreach(files->files, directory_foreach_file, json_builder);
				json_builder_end_array(json_builder);
			}
			
			directory_cleanup(files);
		}
		
		json_builder_end_object(json_builder);
		
		// // {
		// json_builder_begin_object(json_builder);
		// //     "url": "http://www.gnome.org/img/flash/two-thirty.png"
		// json_builder_set_member_name(json_builder, "url");
		// json_builder_add_string_value(json_builder, "http://www.gnome.org/img/flash/two-thirty.png");
		// //     "size": [652,242]
		// json_builder_set_member_name(json_builder, "size");
		// json_builder_begin_array(json_builder);
		// json_builder_add_int_value(json_builder, 652);
		// json_builder_add_int_value(json_builder, 242);
		// json_builder_end_array(json_builder);
		// // }
		// json_builder_end_object(json_builder);
		
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
	printf("<-- \e[1m%i\e[0m Not Implemented %s\n", SOUP_STATUS_NOT_IMPLEMENTED, path);
	
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