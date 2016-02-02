
#ifndef __MMAP_INFO_H__
#define __MMAP_INFO_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

typedef struct _mmap_info {
  int fd;
  size_t mmap_size;
  char *mmap_name;
  char *mmap;
  sem_t *lock;
  sem_t *notify;
  sem_t *ch_size;
  size_t data_max_len;
  size_t *data_size;
  char *data;
  size_t *head;
  size_t *tail;
  long mask;
} mmap_info;

mmap_info* mmap_info_new(const char *mmap_name, long data_size, int flags, mode_t mode);

void init_mmap_and_sem(mmap_info* this);

void mmap_info_finish(mmap_info *this, int rm_flag);

void mmap_info_lock(mmap_info *this);

void mmap_info_unlock(mmap_info *this);

void mmap_info_notify(mmap_info *this);

void mmap_info_wait(mmap_info *this);

void mmap_info_ch_size(mmap_info *this);

void mmap_info_wait_ch_size(mmap_info *this);

int get_sem_val(sem_t *sem);

#endif
