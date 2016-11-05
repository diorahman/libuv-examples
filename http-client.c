#include <stdlib.h>
#include <stdio.h>
#include "uv.h"

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

int main() {
  uv_loop_t *loop = uv_default_loop();

  uv_tcp_t socket;
  struct sockaddr_in addr;
  uv_connect_t connect;

  uv_tcp_init(loop, &socket);
  uv_tcp_keepalive(&socket, 1, 64);
  uv_ip4_addr("127.0.0.1", 8000, &addr);

  uv_tcp_connect(&connect, &socket, (const struct sockaddr *) &addr, on_connect);

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}
