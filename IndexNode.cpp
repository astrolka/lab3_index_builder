#include <stdlib.h>

class IndexNode {

public:
    uint8_t termSize;
    char *term;
    size_t count;
    size_t offset;
    size_t leftOffset;
    size_t rightOffset;    
};