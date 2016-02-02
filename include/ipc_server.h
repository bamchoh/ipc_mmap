#ifndef __IPC_SERVER_H__
#define __IPC_SERVER_H__

#include "ipc_base.h"

ipc_base *server_new(const char* rx_map_name, const char* tx_map_name, long data_size);
void server_finish(ipc_base *srv);

#endif
