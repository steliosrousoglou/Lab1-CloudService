#include <string.h>
#include <stdbool.h>

#include "mongoose.h"
#include "headers.h"

extern vertex_map map;

// Responds to given connection with code and length bytes of body
static void respond(struct mg_connection *c, const char* code, const int length, const char* body) {
  mg_printf(c, "HTTP/1.1 %s\r\nContent-Length: %d\r\n"
    "Content-Type: application/json\r\n\r\n%s",
    code, length, body);
}

// Respond with bad request
void badRequest(struct mg_connection *c) {
  respond(c, "400 Bad Request", 0, "");
}

// Finds the index of given key in an array of tokens
int argument_pos(struct json_token* tokens, const char* key) {
  struct json_token* ptr = tokens;
  int i = 0;
  while(strncmp(ptr[i].ptr, key, ptr[i].len)) i++;
  return i;
}

// Returns allocated string in json format, with one argument
char* make_json_one(const char* key, int key_length, int value) {
  // {} + "" + length of key + : + value + \0
  char dummy[20];
  sprintf(dummy, "%d", value);
  int value_length = strlen(dummy);
  int response_length = 2 + 2 + key_length + 1 + value_length + 1;
  char* response = malloc(sizeof(char) * response_length);
  sprintf(response, "{\"%s\":%d}",key,value);
  return response;
}

// Returns allocated string in json format, with two arguments
char* make_json_two(const char* key1, const char* key2, int key1_length, int key2_length, int value1, int value2) {
  // {} + "" + length of key1 + : + value + , + "" + length of key2 + : + value + \0
  char dummy1[20];
  char dummy2[20];
  sprintf(dummy1, "%d", value1);
  sprintf(dummy2, "%d", value2);
  int value1_length = strlen(dummy1);
  int value2_length = strlen(dummy2);

  int response_length = 2 + 2 + key1_length + value1_length + 5 + key2_length + value2_length + 1;
  char* response = malloc(sizeof(char) * response_length);
  sprintf(response, "{\"%s\":%d,\"%s\":%d}", key1, value1, key2, value2);
  return response;
}

char* format_neighbors(uint64_t* neighbors, int size) {
  char* response = malloc(sizeof(char));
  int response_length = 1;
  response[0] = '[';
  char dummy[20];
  int dummy_length;

  for(int i = 0; i < size; i++) {
    if (i<size-1) sprintf(dummy, "%llu,", neighbors[i]);
    else sprintf(dummy, "%llu", neighbors[i]);

    dummy_length = strlen(dummy);
    response_length += dummy_length;
    response = realloc(response, sizeof(char)*(response_length + 1));
    memcpy(response + response_length - dummy_length, dummy, dummy_length);
    response[response_length] = '\0';
  }
  response_length += 2;
  response = realloc(response, sizeof(char)*response_length);
  response[response_length-2] = ']';
  response[response_length-1] = '\0';
  return response;
}

// Returns allocated string in json format, formatted for get_neighbors
char* make_neighbor_response(const char* key1, const char* key2, int key1_length, int key2_length, int value1, char* neighbors) {
  char dummy[20];
  sprintf(dummy, "%d", value1);
  int value1_length = strlen(dummy);

  int response_length = 9 + key1_length + value1_length + key2_length + strlen(neighbors) + 1;
  char* response = malloc(sizeof(char) * response_length);
  sprintf(response, "{\"%s\":%d,\"%s\":%s}", key1, value1, key2, neighbors);
  return response;
}

// Event handler for request
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;
    struct json_token* tokens = parse_json2(hm->body.p, hm->body.len);
    char* endptr;
    char* response;

    // Sanity check, all valid endpoints have length at least 16
    if(hm->uri.len < 16) return;

    // Sanity check for body not empty
    if(tokens == NULL) {
      badRequest(c);
      return;
    }

    if(!strncmp(hm->uri.p, "/api/v1/add_node", hm->uri.len)) 
    { 
      const char* arg1 = "node_id";

      struct json_token* find = find_json_token(tokens, arg1);

      // body does not contain expected key
      if(find == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);

      // returns true if successfully added
      if(add_vertex(arg1_int))
      {
        response = make_json_one("node_id", 7, arg1_int);
        respond(c, "200 OK", strlen(response), response);
        free(response);
      } 
      else 
      {
        // vertex already existed
        respond(c, "204 OK", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/add_edge", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id";
      const char* arg2 = "node_b_id";

      struct json_token* find1 = find_json_token(tokens, arg1);
      struct json_token* find2 = find_json_token(tokens, arg2);

      // body does not contain expected key
      if(find1 == 0 || find2 == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      int index2 = argument_pos(tokens, arg2);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);
      long long arg2_int = strtoll(tokens[index2 + 1].ptr, &endptr, 10);

      // fix incase of things fucking up
      switch (add_edge(arg1_int, arg2_int)) {
        case 400:
          respond(c, "400 Bad Request", 0, "");
        case 204:
          respond(c, "204 No Content", 0, "");
        case 200:
          response = make_json_two("node_a_id", "node_b_id", 9, 9, arg1_int, arg2_int);
          respond(c, "200 OK", strlen(response), response);
          free(response);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_node", hm->uri.len)) 
    { 
      const char* arg1 = "node_id";

      struct json_token* find = find_json_token(tokens, arg1);

      // body does not contain expected key
      if(find == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);

      // if node does not exist
      if(remove_vertex(arg1_int))
      {
        response = make_json_one("node_id", 7, arg1_int);
        respond(c, "200 OK", strlen(response), response);
        free(response);
      } 
      else 
      {
        respond(c, "400 Bad Request", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_edge", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id";
      const char* arg2 = "node_b_id";

      struct json_token* find1 = find_json_token(tokens, arg1);
      struct json_token* find2 = find_json_token(tokens, arg2);

      // body does not contain expected key
      if(find1 == 0 || find2 == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      int index2 = argument_pos(tokens, arg2);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);
      long long arg2_int = strtoll(tokens[index2 + 1].ptr, &endptr, 10);

      // if edge does not exist
      if(remove_edge(arg1_int, arg2_int))
      {
          response = make_json_two("node_a_id", "node_b_id", 9, 9, arg1_int, arg2_int);
          respond(c, "200 OK", strlen(response), response);
          free(response);;
      } 
      else 
      {
        respond(c, "400 Bad Request", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_node", hm->uri.len)) 
    {
      const char* arg1 = "node_id";

      struct json_token* find = find_json_token(tokens, arg1);

      // body does not contain expected key
      if(find == 0) {
        badRequest(c);
        return;
      }
      // index of value
      int index1 = argument_pos(tokens, arg1);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);

      bool in_graph = get_node(arg1_int);
      response = make_json_one("in_graph", 8, in_graph);
      respond(c, "200 OK", strlen(response), response);
      free(response);    
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_edge", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id";
      const char* arg2 = "node_b_id";

      struct json_token* find1 = find_json_token(tokens, arg1);
      struct json_token* find2 = find_json_token(tokens, arg2);

      // body does not contain expected key
      if(find1 == 0 || find2 == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      int index2 = argument_pos(tokens, arg2);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);
      long long arg2_int = strtoll(tokens[index2 + 1].ptr, &endptr, 10);

      if (!get_node(arg1_int) || !get_node(arg2_int)){
        respond(c, "400 Bad Request", 0, "");
      }
      else {
        bool in_graph = get_edge(arg1_int, arg2_int);
        response = make_json_one("in_graph", 8, in_graph);
        respond(c, "200 OK", strlen(response), response);
        free(response);    
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_neighbors", hm->uri.len)) 
    {
      const char* arg1 = "node_id";

      struct json_token* find1 = find_json_token(tokens, arg1);

      // body does not contain expected key
      if(find1 == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);

      if (!get_node(arg1_int) ) {
        respond(c, "900 Bad Request", 0, "");
      } else {
        int size;
        uint64_t *neighbors = get_neighbors(arg1_int, &size);
        char* neighbor_array = format_neighbors(neighbors, size);
        response = make_neighbor_response("node_id", "neighbors", 7, 9, arg1_int, neighbor_array);
        respond(c, "200 OK", strlen(response), response);
        free(response);
        free(neighbors);
      }

    } 
    else if(!strncmp(hm->uri.p, "/api/v1/shortest_path", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id";
      const char* arg2 = "node_b_id";

      struct json_token* find1 = find_json_token(tokens, arg1);
      struct json_token* find2 = find_json_token(tokens, arg2);

      // body does not contain expected key
      if(find1 == 0 || find2 == 0) {
        badRequest(c);
        return;
      }

      // index of value
      int index1 = argument_pos(tokens, arg1);
      int index2 = argument_pos(tokens, arg2);
      long long arg1_int = strtoll(tokens[index1 + 1].ptr, &endptr, 10);
      long long arg2_int = strtoll(tokens[index2 + 1].ptr, &endptr, 10);

      // if either node does not exist
      if(!ret_vertex(arg1_int) || !ret_vertex(arg2_int)) {
        respond(c, "400 Bad Request", 0, "");
      } else {
        int path = shortest_path(arg1_int, arg2_int);
        if (path == -1) {
          respond(c, "204 No Content", 0, "");
        } else {
          response = make_json_one("distance", 8, path);
          respond(c, "200 OK", strlen(response), response);
          free(response);
        }
      }
    } 
  }
}

int main(int argc, char** argv) {

  // ensure correct number of arguments
  if(argc != 2) {
    fprintf(stderr, "Usage: ./cs426_graph_server <port>\n");
    return 1;
  }

  const char *s_http_port = argv[1];

  // ensure <port> is a number
  struct mg_mgr mgr;
  struct mg_connection *c;

  mg_mgr_init(&mgr, NULL);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(c);

  // initialize global hashtable "map"
  map.size = 0;
  map.table = malloc(SIZE * sizeof(vertex*));
  for(int i=0; i<SIZE; i++) (map.table)[i] = NULL;

    for (;;) {
      mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

    return 0;
  }