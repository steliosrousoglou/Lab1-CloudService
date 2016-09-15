#include <string.h>
#include "mongoose.h"

static const char *s_http_port = "8000";

// returns true if node exists
bool node_exists();
// returns true if edge exists
bool edge_exists();
// returns true if nodes are the same
bool same_node();

// returns length of shortest path, or -1 if does not exist
int shortest_path();

static void respond(struct mg_connection *c, const char* code, const int length, const char* body) {
  mg_printf(c, "HTTP/1.1 %s\r\nContent-Length: %d\r\n"
                "Content-Type: plain/text\r\n\r\n%s",
                code, length, body);
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;

    // Sanity check, all valid endpoints have length at least 16
    if(hm->uri.len < 16) return;

    if(!strncmp(hm->uri.p, "/api/v1/add_node", hm->uri.len)) 
    {
      // if node already exists
      if(node_exists()) 
      {
          respond(c, "204 No Content", 0, "");
      } 
      else 
      {
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/add_edge", hm->uri.len)) 
    {
      // if either node does not exist or if nodes are the same
      if(!node_exists() || !node_exists() || same_node()) 
      {
        respond(c, "400 Bad Request", 0, "");
      } 
      // if edge exists
      else if(edge_exists()) 
      {
        respond(c, "204 No Content", 0, "");
      } 
      else 
      {
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_node", hm->uri.len)) 
    {
      // if node does not exist
      if(!node_exists()) 
      {
        respond(c, "400 Bad Request", 0, "");
      } 
      else 
      {
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/remove_edge", hm->uri.len)) 
    {
      // if edge does not exist
      if(!edge_exists()) 
      {
        respond(c, "400 Bad Request", 0, "");
      } 
      else 
      {
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_node", hm->uri.len)) 
    {
      // respond with a boolean JSON field in_graph indicating whether the node is in the graph
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_edge", hm->uri.len)) 
    {
      // if either node does not exist
      if(!node_exists() || !node_exists()) 
      {
        respond(c, "400 Bad Request", 0, "");
      }
      else 
      {
        // respond with a boolean JSON field in_graph indicating whether the edge is in the graph
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/get_neighbors", hm->uri.len)) 
    {
      // if node does not exist
      if(!node_exists()) 
      {
        respond(c, "400 Bad Request", 0, "");
      }
      else {
        // responds with a list on neighbors
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
    else if(!strncmp(hm->uri.p, "/api/v1/shortest_path", hm->uri.len)) 
    {
      int path = shortest_path();

      // if either node does not exist
      if(!node_exists() || !node_exists()) 
      {
        respond(c, "400 Bad Request", 0, "");
      }
      else if(path == -1) {
        respond(c, "204 No Content", 0, "");
      }
      else 
      {
        // responds with a field distance containing the shortest path
        respond(c, "200 OK", hm->uri.len, hm->uri.p);
      }
    } 
  }
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *c;

  mg_mgr_init(&mgr, NULL);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(c);

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}