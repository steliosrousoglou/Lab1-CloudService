#include <string.h>
#include "mongoose.h"

static const char *s_http_port = "8000";

static void respond(struct mg_connection *c, const char* code, const int length, const char* body) {
  mg_printf(c, "HTTP/1.1 %s\r\nContent-Length: %d\r\n"
                "Content-Type: plain/text\r\n\r\n%s",
                code, length, body);
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;

    // We have received an HTTP request. Parsed request is contained in `hm`.
    // Send HTTP reply to the client which shows full original request.
    // mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
    // mg_printf(c, "%.*s", hm->message.len, hm->message.p);


    // mg_send_head(c, 404, hm->body.len, "Content-Type: text/plain");
    // mg_printf(c, "%s", hm->body.p);

    if(!strncmp(hm->uri.p, "/add_node", hm->uri.len)) {
      respond(c, "404 Not found", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/add_edge", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/remove_node", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/remove_edge", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/get_node", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/get_edge", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/get_neighbors", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
    } else if(!strncmp(hm->uri.p, "/shortest_path", hm->uri.len)) {
      respond(c, "200 OK", hm->uri.len, hm->uri.p);
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

// /* HTTP message */
// struct http_message {
//   struct mg_str message; /* Whole message: request line + headers + body */

//   /* HTTP Request line (or HTTP response line) */
//   struct mg_str method; /* "GET" */
//   struct mg_str uri;    /* "/my_file.html" */
//   struct mg_str proto;  /* "HTTP/1.1" -- for both request and response */

//   /* For responses, code and response status message are set */
//   int resp_code;
//   struct mg_str resp_status_msg;

  
//    * Query-string part of the URI. For example, for HTTP request
//    *    GET /foo/bar?param1=val1&param2=val2
//    *    |    uri    |     query_string     |
//    *
//    * Note that question mark character doesn't belong neither to the uri,
//    * nor to the query_string
   
//   struct mg_str query_string;

//   /* Headers */
//   struct mg_str header_names[MG_MAX_HTTP_HEADERS];
//   struct mg_str header_values[MG_MAX_HTTP_HEADERS];

//   /* Message body */
//   struct mg_str body; /* Zero-length for requests with no body */
// };