#ifndef __IPC_CLIENT_H__
#define __IPC_CLIENT_H__

#include "ipc_base.h"

ipc_base *client_new(const char* rx_map_name, const char* tx_map_name, long data_size);
void client_finish(ipc_base *c);

#endif
