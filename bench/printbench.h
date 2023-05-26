#ifndef PRINTBENCH_H
#define PRINTBENCH_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define cpucycles_fprintf_2(results, op_str)                          \
    {                                                                 \
        int op;                                                       \
        uint64_t min;                                                 \
        FILE *f;                                                      \
                                                                      \
        for (op = 0; op < OP; op++) {                                 \
            f = fopen(op_str[op], "w");                               \
            loop = 0;                                                 \
            fprintf(f, "%s, %" PRIu64 "\n", op_str[op], results[op]); \
            fclose(f);                                                \
        }                                                             \
    }

#endif
