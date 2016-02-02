#include "mmap_info.h"

long pow2(unsigned long n)
{
  --n;

  long p = 0;
  for (;n != 0; n >>= 1) p = (p << 1) + 1;
  return p + 1;
}

void set_mmap(int fd, int size)
{
  char c;
  if(lseek(fd,size,SEEK_SET) < 0) {
    perror("lseek");
    exit(-1);
  }

  if(read(fd, &c, sizeof(char)) == -1) {
    c = '\0';
  }

  if(write(fd, &c, sizeof(char)) == -1) {
    perror("write");
    exit(-1);
  }
}

size_t get_mmap_size(long data_size)
{
  long psize;
  psize = sysconf(_SC_PAGE_SIZE);
  return ((sizeof(sem_t) * 3 + sizeof(size_t) * 3 + data_size) & ~(psize -1)) + psize;
}

void open_mmap(mmap_info* this, int flags, mode_t mode)
{
  while(1) {
    if((this->fd = open(this->mmap_name, flags, mode)) == -1) {
      usleep(100 * 1000 * (rand() & 0x0007));
      continue;
    }
    return;
  }
}

void init_mmap_and_sem(mmap_info* this)
{
  set_mmap(this->fd, this->mmap_size);

  if(sem_init(this->lock, 1, 1) < 0) {
    munmap(this->mmap, this->mmap_size);
    close(this->fd);
    remove(this->mmap_name);
    free(this);
    puts("initialization of lock semaphore was failed.");
    exit(-1);
  }

  if(sem_init(this->notify, 1, 0) < 0) {
    sem_destroy(this->lock);
    munmap(this->mmap, this->mmap_size);
    close(this->fd);
    remove(this->mmap_name);
    free(this);
    puts("initialization of notify semaphore was failed.");
    exit(-1);
  }

  if(sem_init(this->ch_size, 1, 1) < 0) {
    sem_destroy(this->lock);
    sem_destroy(this->notify);
    munmap(this->mmap, this->mmap_size);
    close(this->fd);
    remove(this->mmap_name);
    free(this);
    puts("initialization of notify semaphore was failed.");
    exit(-1);
  }
}

void mmap_info_init(mmap_info* this, int flags, mode_t mode)
{
  open_mmap(this, flags, mode);

  this->mmap_size = get_mmap_size(this->data_max_len);

  this->mmap = (char *)mmap(0, this->mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, this->fd, 0);

  if(this->mmap == MAP_FAILED) {
    close(this->fd);
    remove(this->mmap_name);
    free(this);
    exit(-1);
  }

  this->lock      =  (sem_t *)(this->mmap);
  this->notify    =  (sem_t *)(this->mmap + sizeof(sem_t) * 1);
  this->ch_size   =  (sem_t *)(this->mmap + sizeof(sem_t) * 2);
  this->data_size = (size_t *)(this->mmap + sizeof(sem_t) * 3);
  this->head      = (size_t *)(this->mmap + sizeof(sem_t) * 3 + sizeof(size_t) * 1);
  this->tail      = (size_t *)(this->mmap + sizeof(sem_t) * 3 + sizeof(size_t) * 2);
  this->data      =   (char *)(this->mmap + sizeof(sem_t) * 3 + sizeof(size_t) * 3);
}

mmap_info* mmap_info_new(const char *mmap_name, long data_size, int flags, mode_t mode) {
  mmap_info *this;

  this = (mmap_info *)calloc(1, sizeof(mmap_info));

  data_size = pow2((unsigned long)data_size);

  this->data_max_len = data_size;

  this->mask = data_size - 1;

  this->mmap_name = (char *)mmap_name;

  mmap_info_init(this,flags,mode);

  return this;
}

void mmap_info_finish(mmap_info *this, int rm_flag) {
  msync(this->mmap, this->mmap_size, 0);

  munmap(this->mmap, this->mmap_size);

  close(this->fd);

  if(rm_flag) {
    remove(this->mmap_name);
  }

  free(this);
}

void mmap_info_lock(mmap_info *this)
{
  if(sem_wait(this->lock) < 0) {
    puts("lock cannot get semaphore");
    exit(-1);
  }
}

void mmap_info_unlock(mmap_info *this)
{
  if(sem_post(this->lock) < 0) {
    puts("lock cannot post semaphore");
    exit(-1);
  }
}

void sem_notify(sem_t *sem)
{
  if(get_sem_val(sem) == 0) {
    if(sem_post(sem) < 0) {
      puts("notify cannot post semaphore");
      exit(-1);
    }
  }
}

void sem_watch(sem_t *sem)
{
  if(sem_wait(sem) < 0) {
    puts("notify cannot get semaphore");
    exit(-1);
  }
}

void mmap_info_notify(mmap_info *this)
{
  sem_notify(this->notify);
}

void mmap_info_ch_size(mmap_info *this)
{
  sem_notify(this->ch_size);
}

void mmap_info_wait(mmap_info *this)
{
  sem_watch(this->notify);
}

void mmap_info_wait_ch_size(mmap_info *this)
{
  sem_watch(this->ch_size);
}


int get_sem_val(sem_t *sem)
{
  int val;
  sem_getvalue(sem, &val);
  return val;
}

