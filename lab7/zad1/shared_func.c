#include "shared_func.h"


key_t get_cbelt_key() { return ftok(getenv("HOME"), 159); }


key_t get_truck_key() { return ftok(getenv("HOME"), 145); }