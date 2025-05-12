
#include "tchecker/publicapi/util.hh"
#include <cstdlib>

const void free_string(const char * pointer) 
{ 
    free((void *) pointer);
}