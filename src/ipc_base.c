#include "ipc_base.h"

void rb_enqueue(mmap_info *this, char *buf, size_t n)
{
  char *top   = (char *)this->data;
  size_t tail = *(this->tail);
  long mask   = this->mask;

  while(*(this->data_size) + n > this->data_max_len) {
    mmap_info_unlock(this);
    mmap_info_wait_ch_size(this);
    mmap_info_lock(this);
  }

  if((n+tail) & ~mask) {
    // more than max length
    memcpy(top + tail, buf, (mask+1)-tail);
    memcpy(top, buf + (mask+1)-tail, (n+tail) & mask);
  } else {
    // less than max length
    memcpy(top + tail, buf, n);
  }

  *(this->tail) = ((n + tail) & mask);
  *(this->data_size) += n;
}

void rb_dequeue(mmap_info *this, char *buf, size_t n)
{
  char *top   = (char *)this->data;
  size_t head = *(this->head);
  long mask   = this->mask;

  if((n+head) & ~mask) {
    // max length over
    memcpy(buf, top + head, (mask+1)-head);
    memcpy(buf + (mask+1)-head, top, (n+head) & mask);
  } else {
    // within max length
    memcpy(buf, top + head, n);
  }

  *(this->head) = ((n + head) & mask);
  *(this->data_size) -= n;
}

ipc_base *ipc_base_new(const char* rx_map_name, const char* tx_map_name, long data_size, int flags)
{
  ipc_base *tmp;
  tmp = (ipc_base *)calloc(1, sizeof(ipc_base));
  tmp->rx_map = mmap_info_new(rx_map_name, data_size, flags, 0666);
  tmp->tx_map = mmap_info_new(tx_map_name, data_size, flags, 0666);
  return tmp;
}

void ipc_base_finish(ipc_base *this)
{
  mmap_info_finish(this->rx_map, 0);
  mmap_info_finish(this->tx_map, 0);
  free(this);
}


void ipc_base_send(ipc_base *this, char* buf, size_t size)
{
  mmap_info_lock(this->tx_map);

  rb_enqueue(this->tx_map, buf, size);

  mmap_info_unlock(this->tx_map);
  mmap_info_notify(this->tx_map);
}

ssize_t ipc_base_recv(ipc_base *this, char* buf, size_t size)
{
  mmap_info_lock(this->rx_map);

  ssize_t recv_size;

  while((recv_size = *(this->rx_map->data_size)) == 0) {
    mmap_info_unlock(this->rx_map);
    mmap_info_wait(this->rx_map);
    mmap_info_lock(this->rx_map);
  }

  if(size < recv_size) {
    recv_size = size;
  }

  rb_dequeue(this->rx_map, buf, recv_size);

  mmap_info_unlock(this->rx_map);
  mmap_info_ch_size(this->rx_map);
  return recv_size;
}

