#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 4
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
#include "randombytes.c"

#define TIMINGS 10000
#define OP 8

void get_random_number(uint64_t *arr) {
    uint8_t bytes[NLIMBS * 8 * 2];
    randombytes(bytes, NLIMBS * 8 * 2);  // 64 bits are 8 bytes
    memcpy(arr, bytes, NLIMBS * 8 * 2);
}

void write_values(uint64_t values[OP][TIMINGS], uint64_t results[OP], char *op_str[OP]) {
    int op;
    uint64_t min;
    FILE *f;

    const char filename_preffix[] = "values_";

    // write the values for each loop & operation
    for (op = 0; op < OP; op++) {
        char filename[100];
        strcpy(filename, filename_preffix);  // Copy the prefix
        strcat(filename, op_str[op]);

        f = fopen(filename, "w");

        for (size_t i = 0; i < TIMINGS - 1; i++) {
            values[op][i] = values[op][i + 1] - values[op][i];
        }

        // Write the values
        for (size_t i = 0; i < TIMINGS - 1; i++) {
            fprintf(f, "%" PRIu64 "\n", values[op][i]);
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
    uint64_t results[OP];          // only contains the median
    uint64_t values[OP][TIMINGS];  // contains all the measurements

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

    op = 0;
    // fp_add
    get_random_number(a);
    get_random_number(b);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_add(a, b, c);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_sub
    get_random_number(a);
    get_random_number(b);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_sub(a, b, c);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_mul
    get_random_number(a);
    get_random_number(b);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_mul(a, b, c);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_sqr
    get_random_number(a);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_sqr(a, b);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_expm_noct
    get_random_number(a);
    get_random_number(b);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_expm_noct(a, b, c);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_inv
    get_random_number(a);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_inv(a, b);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_toM
    get_random_number(a);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_toM(a, b);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // fp_fromM
    get_random_number(a);
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        fp_fromM(a, b);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    cpucycles_fprintf_2(results, op_str);
    write_values(values, results, op_str);

    return EXIT_SUCCESS;
}
