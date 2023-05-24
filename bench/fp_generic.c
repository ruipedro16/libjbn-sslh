#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 6
#endif

#define xstr(s, e) str(s) #e  // concatenates
#define str(s) #s

extern void fp_add(uint64_t *, uint64_t *, uint64_t *);
extern void fp_sub(uint64_t *, uint64_t *, uint64_t *);
extern void fp_mul(uint64_t *, uint64_t *, uint64_t *);
extern void fp_sqr(uint64_t *, uint64_t *);
extern void fp_expm_noct(uint64_t *, uint64_t *, uint64_t *);
extern void fp_inv(uint64_t *, uint64_t *);
extern void fp_toM(uint64_t *, uint64_t *);
extern void fp_fromM(uint64_t *, uint64_t *);

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
    char *op_str[OP] = {xstr(fp_add, .csv), xstr(fp_sub, .csv),       xstr(fp_mul, .csv),
                        xstr(fp_sqr, .csv), xstr(fp_expm_noct, .csv), xstr(fp_inv, .csv),
                        xstr(fp_toM, .csv), xstr(fp_fromM, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP][LOOPS];          // only contains the median
    uint64_t values[OP][LOOPS][TIMINGS];  // contains all the measurements

    uint64_t a[NLIMBS * 2], b[NLIMBS * 2], c[NLIMBS * 2];
    uint64_t r;

    // warmup
    for (int i = 0; i < 10; i++) {
        fp_add(a, b, c);
        fp_sub(a, b, c);
        fp_mul(a, b, c);
        fp_sqr(a, b);
        fp_expm_noct(a, b, c);
        fp_inv(a, b);
        fp_toM(a, b);
        fp_fromM(a, b);
    }

    for (loop = 0, op = 0; loop < LOOPS; loop++, op = 0) {
        // fp_add
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_add(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_sub
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_sub(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_mul
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_mul(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_sqr
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_sqr(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_expm_noct
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_expm_noct(a, b, c);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_inv
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_inv(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_toM
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_toM(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // fp_fromM
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            fp_fromM(a, b);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);
    }

    cpucycles_fprintf_2(results, op_str);
    write_values(values, results, op_str);

    return EXIT_SUCCESS;
}
