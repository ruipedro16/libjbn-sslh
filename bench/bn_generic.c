#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 6
#endif

#define xstr(s, e) str(s) #e  // concatenates
#define str(s) #s

extern uint64_t bn_eq(uint64_t *, uint64_t *);
extern uint64_t bn_test0(uint64_t *);
extern void bn_copy(uint64_t *, uint64_t *);
extern void bn_set0(uint64_t *);
extern void bn_addn(uint64_t *, uint64_t *, uint64_t *);
extern void bn_subn(uint64_t *, uint64_t *, uint64_t *);
extern void bn_muln(uint64_t *, uint64_t *, uint64_t *);
extern void bn_sqrn(uint64_t *, uint64_t *);

#include "cpucycles.c"
#include "printbench.h"

#define LOOPS 5
#define TIMINGS 4096
#define OP 8

void write_values(uint64_t values[OP][LOOPS][TIMINGS], uint64_t results[OP][LOOPS],
                  char *op_str[OP]) {
    int op, loop;
    uint64_t min;
    FILE *f;
    uint64_t loop_used[OP];

    // choose the loop whose values will be used
    // We choose the one whose median is smaller
    for (op = 0; op < OP; op++) {
        min = results[op][0];
        loop_used[op] = 1;
        for (loop = 1; loop < LOOPS; loop++) {
            if (min > results[op][loop]) {
                min = results[op][loop];
                loop_used[op] = loop;
            }
        }
        results[op][0] = min;
    }

    const char filename_preffix[] = "values_";

    // write the values for each loop & operation
    for (op = 0; op < OP; op++) {
        char filename[100];
        strcpy(filename, filename_preffix);  // Copy the prefix
        strcat(filename, op_str[op]);

        f = fopen(filename, "w");

        // Write the values
        for (size_t i = 0; i < TIMINGS; i++) {
            size_t loop_index = loop_used[op];
            fprintf(f, "%" PRIu64 "\n", values[op][loop_index][i]);
        }

        fclose(f);

        // Clear the filename array
        memset(filename, 0, sizeof(filename));
    }
}

int main(void) {
    int loop, i, op;
    char *op_str[OP] = {xstr(bn_eq, .csv),   xstr(bn_test0, .csv), xstr(bn_copy, .csv),
                        xstr(bn_set0, .csv), xstr(bn_addn, .csv),  xstr(bn_subn, .csv),
                        xstr(bn_muln, .csv), xstr(bn_sqrn, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP][LOOPS];          // only contains the median
    uint64_t values[OP][LOOPS][TIMINGS];  // contains all the measurements

    uint64_t a[NLIMBS * 2], b[NLIMBS * 2], c[NLIMBS * 2];

    // warmup
    for (int i = 0; i < 10; i++) {
        bn_eq(a, b);
        bn_test0(a);
        bn_copy(a, b);
        bn_set0(a);
        bn_addn(a, b, c);
        bn_subn(a, b, c);
        bn_muln(a, b, c);
        bn_sqrn(a, b);
    }

    for (loop = 0, op = 0; loop < LOOPS; loop++, op = 0) {
        // bn_eq
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_eq(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_test0
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_test0(a);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_copy
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_copy(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_set0
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_set0(a);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_addn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_addn(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_subn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_subn(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_muln
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_muln(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_sqrn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_sqrn(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);
    }

    cpucycles_fprintf_2(results, op_str);
    write_values(values, results, op_str);

    return EXIT_SUCCESS;
}
