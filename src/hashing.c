
#include <stdint.h>

uint32_t performHash(int value, int h2) {
        return value % h2;
}