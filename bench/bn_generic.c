#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 7
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

int main(void) {
#define LOOPS 5
#define TIMINGS 4096
#define OP 8

    int loop, i, op;
    char *op_str[OP] = {xstr(bn_eq, .csv),   xstr(bn_test0, .csv), xstr(bn_copy, .csv),
                        xstr(bn_set0, .csv), xstr(bn_addn, .csv),  xstr(bn_subn, .csv),
                        xstr(bn_muln, .csv), xstr(bn_sqrn, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP][LOOPS];

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
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_test0
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_test0(a);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_copy
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_copy(a, b);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_set0
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_set0(a);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_addn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_addn(a, b, c);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_subn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_subn(a, b, c);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_muln
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_muln(a, b, c);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // bn_sqrn
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            bn_sqrn(a, b);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);
    }

    cpucycles_fprintf_2(results, op_str);

    return 0;

#undef OP
#undef TIMINGS
#undef LOOPS
}
