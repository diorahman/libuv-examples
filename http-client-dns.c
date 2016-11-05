#include <stdlib.h>
#include <stdio.h>
#include "uv.h"

struct client_t {
  uv_loop_t* loop;
  uv_tcp_t socket;
  uv_connect_t connect;
};

void on_alloc(uv_handle_t* handle, size_t suggested, uv_buf_t* buf) {
  buf->base = malloc(suggested);
  buf->len = suggested;
}

void on_close(uv_handle_t* handle) {
  printf("closed\n");
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
  if (nread > 0) {
    fwrite(buf->base, nread, 1, stdout);
  } else {
    uv_close((uv_handle_t*) tcp, on_close);
  }

  free(buf->base);
}

void on_write(uv_write_t* req, int status) {
  printf("write status %d\n", status);
  free(req);
}

void on_connect(uv_connect_t* connection, int status) {
  uv_stream_t* stream = connection->handle;
  uv_write_t* req = malloc(sizeof(uv_write_t));

  uv_buf_t http = {
    .base = "GET / HTTP/1.0\r\n\r\n",
    .len = 18
  };

  uv_write(req, stream, &http, 1, on_write);
  uv_read_start(stream, on_alloc, on_read);
}

void on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo * resolved) {
  char ip[17] = {'\0'};
  uv_ip4_name((struct sockaddr_in*) resolved->ai_addr, ip, 16);
  printf("resolved host:  %s\n", ip);

  struct client_t* client = (struct client_t*) resolver->data;
  struct sockaddr_in addr;

  uv_tcp_init(client->loop, &client->socket);
  uv_tcp_keepalive(&client->socket, 1, 64);
  uv_ip4_addr(ip, 8000, &addr);

  uv_tcp_connect(&client->connect, &client->socket, (const struct sockaddr *) &addr, on_connect);
}

int main() {
  uv_loop_t *loop = uv_default_loop();

  uv_getaddrinfo_t resolver;

  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  struct sockaddr_in addr;
  struct client_t client;
  client.loop = loop;

  resolver.data = &client;
  int r = uv_getaddrinfo(loop, &resolver, on_resolved, "localhost", "8000", &hints);

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}
