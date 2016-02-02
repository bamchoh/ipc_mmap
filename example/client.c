#include "ipc_client.h"

#define SEND_SIZE 100000

char** init_send_data(long size)
{
  char **a;
  long i;

  a = (char **)calloc(sizeof(char *), size);
  for(i = 0;i < size;i++) {
    a[i] = (char *)calloc(sizeof(char), 128);
  }

  for(i = 0;i < (SEND_SIZE - 1);i++) {
    sprintf(a[i], "%0127ld", i);
  }

  sprintf(a[i], "end");

  return a;
}

int main()
{
  ssize_t data_size, recv_size;
  int i, j, c;
  ipc_base *c1;
  char **buf;

  buf = init_send_data(SEND_SIZE);

  c1 = client_new("tx_map.sock", "rx_map.sock", SEND_SIZE * 1000);

  for(i = 0;i < SEND_SIZE;i++) {
    ipc_base_send(c1, buf[i], 128);
    // usleep(1000);
  }

  client_finish(c1);

  return 0;
}
