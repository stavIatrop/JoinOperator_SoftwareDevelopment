
#include <stdint.h>

uint32_t performHash(uint32_t value, uint32_t h2) {
        return value % h2;
}