#include <stdlib.h>
#include <string>

using namespace std;

class IndexNode {

public:
    string term;
    size_t count;
    size_t offset;
    size_t leftOffset;
    size_t rightOffset;

};