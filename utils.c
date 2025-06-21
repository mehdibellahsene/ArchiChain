#include "utils.h"
#include <string.h>

uint32_t simple_hash(const char *input) {
    if (input == NULL) {
        return 0;
    }
    
    uint32_t hash = 5381;
    int c;
    while ((c = *input++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}
