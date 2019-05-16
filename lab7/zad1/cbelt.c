#include "cbelt.h"
#include "ipc_func.h"


cbelt make_cbelt(int capacity, int max_weight, key_t sem_key) {

    cbelt a_cbelt;
    a_cbelt . load = 0;
    a_cbelt . capacity = capacity;
    a_cbelt . weight = 0;
    a_cbelt . max_weight = max_weight;
    a_cbelt . head = 0;
    a_cbelt . tail = capacity - 1;
    a_cbelt . sem_key = sem_key;

    return a_cbelt;
}


int cbelt_put(cbelt* a_cbelt, long sem_id, package a_package) {

    if (a_cbelt -> weight + a_package . weight > a_cbelt -> max_weight)
        return -1;

    int result;
    sem_m_block(sem_id);

    if (a_cbelt -> load == a_cbelt -> capacity) {

        result = -2;

    } else {

        a_cbelt -> tail = (a_cbelt -> tail + 1) % a_cbelt -> capacity;
        a_cbelt -> spots[a_cbelt -> tail] = a_package;
        a_cbelt -> load++;
        a_cbelt -> weight += a_package . weight;
        result = 0;
    }

    sem_m_unblock(sem_id);
    return result;
}


package* cbelt_take(cbelt *a_cbelt, long sem_id, package* a_package) {

    sem_m_block(sem_id);

    if (a_cbelt -> load != 0 && a_package != NULL) {

        *a_package = a_cbelt -> spots[a_cbelt -> head];
        a_cbelt -> head = (a_cbelt -> head + 1) % a_cbelt -> capacity;
        a_cbelt -> load--;
        a_cbelt -> weight -= a_package -> weight;

    } else a_package = NULL;

    sem_m_unblock(sem_id);
    return a_package;
}
