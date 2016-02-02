#include "ipc_server.h"

ipc_base *server_new(const char* rx_map_name, const char* tx_map_name, long data_size)
{
  ipc_base *srv;
  srv = ipc_base_new(rx_map_name, tx_map_name, data_size, O_RDWR|O_CREAT);
  init_mmap_and_sem(srv->rx_map);
  init_mmap_and_sem(srv->tx_map);
  return srv;
}

void server_finish(ipc_base *srv)
{
  ipc_base_finish(srv);
}

