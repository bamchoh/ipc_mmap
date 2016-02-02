#ifndef __IPC_BASE_H__
#define __IPC_BASE_H__

#include "mmap_info.h"
#include <string.h>

typedef struct _ipc_base {
  mmap_info *rx_map;
  mmap_info *tx_map;
} ipc_base;

ipc_base *ipc_base_new(const char* rx_map_name, const char* tx_map_name, long data_size, int flags);
void ipc_base_finish(ipc_base *this);
void ipc_base_send(ipc_base *this, char* buf, size_t size);
ssize_t ipc_base_recv(ipc_base *this, char* buf, size_t size);
size_t ipc_base_tx_buf_size(ipc_base *this);
size_t ipc_base_rx_buf_size(ipc_base *this);

#endif
