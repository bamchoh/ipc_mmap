#include "ipc_client.h"

ipc_base *client_new(const char* rx_map_name, const char* tx_map_name, long data_size)
{
  return ipc_base_new(rx_map_name, tx_map_name, data_size, O_RDWR);
}

void client_finish(ipc_base *c)
{
  ipc_base_finish(c);
}

