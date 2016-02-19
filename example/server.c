#include "ipc_server.h"

#define RECV_SIZE 1000L

int main()
{
  long data_size;
  int i, c;
  char *buf;
  ipc_base *srv;
  int recv_len;

  srv = server_new("rx_map.sock", "tx_map.sock", RECV_SIZE * 1000);

  buf = (char *)calloc(RECV_SIZE, sizeof(char));

  while(1) {
    recv_len = ipc_base_recv(srv, buf, RECV_SIZE);

    if(recv_len == 0) {
      break;
    } else {
//     printf("%03d : ", recv_len);
//     for(i = 0;i < recv_len;i++) {
//       if(buf[i] == '\0') {
//         printf("\\0");
//       } else {
//         printf("%c", buf[i]);
//       }
//     }
//     printf("\n");
    }
  }

  puts("end");

  server_finish(srv);

  free(buf);

  return 0;
}
