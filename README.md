# Assignment 1: Simple HTTP API for an in-memory graph #

----

## Introduction ##

In this lab you will start by implementing a simple single-node in-memory version of your server, which will store undirected graphs.

## Interfaces ##

We're using a simple HTTP based system, which only uses the `HTTP` and `Content-Length` headers. All requests will be in the form:
```HTTP
POST <function_name> HTTP/1.1
<other-headers?>
Content-Length: <length>
<other-headers?>

<length bytes of JSON encoded content>
```
The arguments will be in [JSON](http://www.json.org/). You can use the [Mongoose Library](https://github.com/cesanta/mongoose) in C or C++ for both request handling and JSON decoding if you so desire to. Seach for JSON and HTTP in the header file for more information.

In this lab you will implement the following functions for your undirected-graph store:

   Function    | Method |    Arguments     | Return
-------------- | ------ | ---------------- | ------
 `add_node`    | `POST` | `u64 node_id`    |  `200` on success<br/> `204`if the node already exists
 `add_edge`    | `POST` | `u64 node_a_id`, `u64 node_b_id`  |  `200` on success<br/>`204` if the edge already exists<br /> `400` if either node doesn't exist, or if `node_a_id` is the same as `node_b_id`
 `remove_node` | `POST` | `u64 node_id` | `200` on success<br/> `400`if the node does not exist
 `remove_edge` | `POST` | `u64 node_a_id`, `u64 node_b_id`  |  `200` on success<br/>`400` if the edge does not exist
 `get_node`    | `POST` | `u64 node_id` | `200` and a boolean JSON field `in_graph` indicating whether the node is in the graph
 `get_edge`    | `POST` | `u64 node_a_id`, `u64 node_b_id`  |  `200` and a boolean JSON field `in_graph` indicating whether the edge is in the graph<br/>`400` of at least one of the vertices does not exist
 `get_neighbors`   | `POST`  | `u64 node_id` | `200` and a list of neighbors[*](#get_neighbors_description) or<br/> `400` if the node does not exist
 `shortest_path`   | `POST`  | `u64 node_a_id`, `uint node_b_id` | `u64` and a field `distance` containing the length of shortest path between the two nodes or<br/>`204` if there is no path <br/>`400` if either node does not exist

Any
 In the current server version, the endpoints must be prefixed with `/api/v1/`, see the [examples](#examples) for more detail.

<a name="get_neighbors_description">*</a> Specifically the contents returned from `get_neighbors` must be in the form
```JSON
{
  "node_id": <node_id>,
  "neighbors": [<neighbors>]
}
```
