#include <string.h>
#include <stdbool.h>

#include "mongoose.h"
#include "headers.h"

extern vertex_map map;

static const char *s_http_port = "8000";

// // returns true if edge exists
// bool edge_exists();
// // returns length of shortest path, or -1 if does not exist
// int shortest_path();

// Responds to given connection with code and length bytes of body
static void respond(struct mg_connection *c, const char* code, const int length, const char* body) {
  mg_printf(c, "HTTP/1.1 %s\r\nContent-Length: %d\r\n"
                "Content-Type: application/json\r\n\r\n%s",
                code, length, body);
}

// Kill program
void DIE(struct mg_connection *c) {
  respond(c, "400 Bad Request", 0, "");
}

// Event handler for request
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;
    struct json_token* tokens = parse_json2(hm->body.p, hm->body.len);

    // Sanity check, all valid endpoints have length at least 16
    if(hm->uri.len < 16) return;

    if(!strncmp(hm->uri.p, "/api/v1/add_node", hm->uri.len)) 
    { 
      // sanity check of input body
      if(strncmp(tokens[1].ptr, "node_id", tokens[1].len)) DIE(c);

      // returns true if successfully added
      if(add_vertex(atoi(tokens[2].ptr))) 
      {
        // TODO: respond with json object
        respond(c, "200 OK", hm->body.len, hm->body.p);
      } 
      else 
      {
        // vertex already existed
        respond(c, "204 OK", 0, "");
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/add_edge", hm->uri.len)) 
    {
      // sanity check of input body
      if(strncmp(tokens[1].ptr, "node_a_id", tokens[1].len) || strncmp(tokens[3].ptr, "node_b_id", tokens[3].len)) DIE(c);
      switch (add_edge(atoi(tokens[2].ptr), atoi(tokens[4].ptr))) {
        case 400:
          respond(c, "400 Bad Request", 0, "");
        case 204:
          respond(c, "204 No Content", 0, "");
        case 200:
          respond(c, "200 OK", hm->body.len, hm->body.p);
      }
      // // if either node does not exist or if nodes are the same
      // if(!vertex_exists() || !vertex_exists() || same_vertex()) 
      // {
      //   respond(c, "400 Bad Request", 0, "");
      // } 
      // // if edge exists
      // else if(edge_exists()) 
      // {
      //   respond(c, "204 No Content", 0, "");
      // } 
      // else 
      // {
      //   respond(c, "200 OK", hm->uri.len, hm->uri.p);
      // }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_node", hm->uri.len)) 
    { 
       // sanity check of input body
      if(strncmp(tokens[1].ptr, "node_id", tokens[1].len)) DIE(c);

      // if node does not exist
      if(remove_vertex(atoi(tokens[2].ptr))) 
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
      // // if edge does not exist
      // if(!edge_exists()) 
      // {
      //   respond(c, "400 Bad Request", 0, "");
      // } 
      // else 
      // {
      //   respond(c, "200 OK", hm->uri.len, hm->uri.p);
      // }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_node", hm->uri.len)) 
    {
      // respond with a boolean JSON field in_graph indicating whether the node is in the graph
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_edge", hm->uri.len)) 
    {
      // // if either node does not exist
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
      // int path = shortest_path();

      // // if either node does not exist
      // if(!vertex_exists() || !vertex_exists()) 
      // {
      //   respond(c, "400 Bad Request", 0, "");
      // }
      // else if(path == -1) 
      // {
      //   respond(c, "204 No Content", 0, "");
      // }
      // else 
      // {
      //   // responds with a field distance containing the shortest path
      //   respond(c, "200 OK", hm->uri.len, hm->uri.p);
      // }
    } 
  }
}

int main(void) {
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