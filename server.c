#include <string.h>
#include <stdbool.h>

#include "mongoose.h"
#include "headers.h"

extern vertex_map map;

// // returns true if edge exists
// bool edge_exists();

// Responds to given connection with code and length bytes of body
static void respond(struct mg_connection *c, const char* code, const int length, const char* body) {
  mg_printf(c, "HTTP/1.1 %s\r\nContent-Length: %d\r\n"
                "Content-Type: application/json\r\n\r\n%s",
                code, length, body);
}

// Kill program
void badRequest(struct mg_connection *c) {
  respond(c, "400 Bad Request", 0, "");
}

int argument_pos(struct json_token* tokens, const char* key) {
  struct json_token* ptr = tokens;
  int i = 0;
  while(strncmp(ptr[i].ptr, key, ptr[i].len)) i++;
  return i;
}

char* make_json_one(const char* key, int key_length, int value, int num_args) {
  int response_length = 2 + 2 + key_length + 1 + 1 + 1;
  char* response = malloc(sizeof(char) * response_length);
  sprintf(response, "{\"%s\":%d}",key,value);
  return response;
}

// Event handler for request
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;
    struct json_token* tokens = parse_json2(hm->body.p, hm->body.len);
    char* endptr;

    // Sanity check, all valid endpoints have length at least 16
    if(hm->uri.len < 16) return;

    // Sanity check for body not empty
      if(tokens == NULL) {
        badRequest(c);
        return;
      }

    if(!strncmp(hm->uri.p, "/api/v1/add_node", hm->uri.len)) 
    { 
      const char* arg1 = "node_id\0";

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
        // TODO: respond with json object
        char* response = make_json_one("node_id", 7, arg1_int);
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
      const char* arg1 = "node_a_id\0";
      const char* arg2 = "node_b_id\0";

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
          respond(c, "200 OK", hm->body.len, hm->body.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_node", hm->uri.len)) 
    { 
      const char* arg1 = "node_id\0";

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
        respond(c, "200 OK", tokens[2].len, tokens[2].ptr);
      } 
      else 
      {
        respond(c, "400 Bad Request", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_edge", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id\0";
      const char* arg2 = "node_b_id\0";

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
      if(!remove_edge(arg1_int, arg2_int))
      {
          respond(c, "200 OK", hm->uri.len, hm->uri.p);
      } 
      else 
      {
          respond(c, "400 Bad Request", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_node", hm->uri.len)) 
    {
      // respond with a boolean JSON field in_graph indicating whether the node is in the graph
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_edge", hm->uri.len)) 
    {
      // //todo: should we be testing that the field is the right length?
      // if(strncmp(tokens[1].ptr, "node_a_id", tokens[1].len) || 
      // strncmp(tokens[3].ptr, "node_b_id", tokens[3].len)) badRequest(c);
      // uint64_t id1 = atoi(tokens[2].ptr);
      //   uint64_t id2 = atoi(tokens[2].ptr);

      // // // if either node does not exist
      // if(!vertex_exists() || !vertex_exists()) 
      // {
      //   respond(c, "400 Bad Request", 0, "");
      // }
      // else 
      // {
      //   // respond with a boolean JSON field in_graph indicating whether the edge is in the graph
      //   respond(c, "200 OK", hm->uri.len, hm->uri.p);
      // }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_neighbors", hm->uri.len)) 
    {
      // // if node does not exist
      // if(!vertex_exists()) 
      // {
      //   respond(c, "400 Bad Request", 0, "");
      // }
      // else {
      //   // responds with a list on neighbors
      //   respond(c, "200 OK", hm->uri.len, hm->uri.p);
      // }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/shortest_path", hm->uri.len)) 
    {
      const char* arg1 = "node_a_id\0";
      const char* arg2 = "node_b_id\0";

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
      if(!vertex_exists(arg1_int) || !vertex_exists(arg2_int)) {
        respond(c, "400 Bad Request", 0, "");
      }
      else {
        int path = shortest_path(arg1_int, arg2_int);
        if (path == -1) {
          respond(c, "204 No Content", 0, "");
        }
        else {
        // responds with a field distance containing the shortest path
          respond(c, "200 OK", hm->uri.len, hm->uri.p);
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

  map.size = 0;
  map.table = malloc(SIZE*sizeof(vertex*));

  for(int i=0; i<SIZE; i++) (map.table)[i] = NULL;

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}