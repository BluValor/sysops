#ifndef ZAD1_IPC_FUNCTIONS_H
#define ZAD1_IPC_FUNCTIONS_H


#include <semaphore.h>
#include <stddef.h>


int shm_m_create(key_t key, size_t size);
int shm_m_open(key_t key, size_t size);
void* shm_m_attach(int shm_id, size_t size);
void shm_m_detach(void *addr, size_t size);
void shm_m_destroy(int shm_id, key_t key);


long sem_m_create(key_t key);
long sem_m_open(key_t key);
void sem_m_close(long sem_id);
void sem_m_block(long sem_id);
void sem_m_unblock(long sem_id);
void sem_m_destroy(long sem_id, key_t key);


#endif //ZAD1_IPC_FUNCTIONS_H
