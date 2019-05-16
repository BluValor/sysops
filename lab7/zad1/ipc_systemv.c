#include <stddef.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "ipc_func.h"
#include "exit_func.h"


int shm_m_create(key_t key, size_t size) {

    long shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);

    if (shm_id == -1)
        die_errno("Creating shared memory failed!\n");

    return shm_id;
}


int shm_m_open(key_t key, size_t size) {

    long shm_id = shmget(key, size, 0);

    if (shm_id == -1)
        die_errno("Opening shared memory failed!\n");

    return shm_id;
}


void* shm_m_attach(int shm_id, size_t size) {

    void* addr = shmat((int) shm_id, NULL, 0);

    if (addr == (void*) -1)
        die_errno("Attaching shared memory failed!\n");

    return addr;
}


void shm_m_detach(void *addr, size_t size) {

    if (shmdt(addr) == -1)
        die_errno("Detaching shared memory failed!\n");
}


void shm_m_destroy(int shm_id, key_t key) {

    if (shmctl((int) shm_id, IPC_RMID, NULL) == -1)
        die_errno("Destroying shared memory failed!\n");
}


long sem_m_create(key_t key) {

    long sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);

    if (sem_id == -1)
        die_errno("Creating semaphore failed!\n");

    if (semctl((int) sem_id, 0, SETVAL, 1) == -1)
        die_errno("Setting semaphore initial value failed!\n");

    return sem_id;
}


long sem_m_open(key_t key) {

    long sem_id = semget(key, 1, 0);

    if (sem_id == -1)
        die_errno("Opening semaphore failed!\n");

    return sem_id;
}


void sem_m_close(long sem_id) {}


void sem_m_block(long sem_id) {

    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;

    if (semop((int) sem_id, &sbuf, 1) == -1) {
        die_errno("Blocking semaphore failed!\n");
    }
}


void sem_m_unblock(long sem_id) {

    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = 1;
    sbuf.sem_flg = 0;

    if (semop((int) sem_id, &sbuf, 1) == -1)
        die_errno("Unblocking semaphore failed!\n");
}


void sem_m_destroy(long sem_id, key_t key) {

    if (semctl((int) sem_id, 0, IPC_RMID) == -1)
        die_errno("Destroying semaphore failed!\n");
}