#include "ipc_client.h"

#define SEND_SIZE 1000L

char** init_send_data(long size)
{
  char **a;
  long i;
  long j;
  char buf[5];

  a = (char **)calloc(sizeof(char *), size);
  for(i = 0;i < size;i++) {
    a[i] = (char *)calloc(sizeof(char), 128);

    for(j = 0;j < 128;j++) {
      a[i][j] = (j % 26) + 'A';
    }

    a[i][j - 5] = '-';
    a[i][j - 4] = 'e';
    a[i][j - 3] = 'n';
    a[i][j - 2] = 'd';
    a[i][j - 1] = '\0';
  }


  return a;
}

int main(int argc, char *argv[])
{
  ssize_t data_size, recv_size;
  int i, j, c;
  ipc_base *c1;
  char **buf;
  long max_len = atoi(argv[1]);

  buf = init_send_data(1);

  c1 = client_new("tx_map.sock", "rx_map.sock", SEND_SIZE * 1000);

  for(i = 0;i < max_len;i++) {
    ipc_base_send(c1, buf[0], 128);
    // usleep(1000);
  }

  client_finish(c1);

  return 0;
}
