// test.h
#ifndef REPAIR_READER // Header guard to prevent multiple inclusions
#define REPAIR_READER

extern "C" {
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

int test(DICT *dict); // Declaration of the test function

#endif
