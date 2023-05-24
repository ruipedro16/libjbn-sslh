#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 6
#endif

#define xstr(s, e) str(s) #e  // concatenates
#define str(s) #s

typedef struct {
    uint64_t x[NLIMBS];
    uint64_t y[NLIMBS];
    uint64_t z[NLIMBS];
} ProjectivePoint;

typedef struct {
    uint64_t x[NLIMBS];
    uint64_t y[NLIMBS];
} AffinePoint;

extern void ecc_normalize(const ProjectivePoint *p, AffinePoint *q);
extern void ecc_double(const ProjectivePoint *p, ProjectivePoint *q);
extern void ecc_add(ProjectivePoint *r, const ProjectivePoint *p, const ProjectivePoint *q);
extern void ecc_mixed_add(ProjectivePoint *r, const ProjectivePoint *p, const ProjectivePoint *q);
extern void ecc_scalar_mul(ProjectivePoint *r, const ProjectivePoint *p, const uint64_t *scalar);

#include "cpucycles.c"
#include "printbench.h"

#define LOOPS 5
#define TIMINGS 4096
#define OP 5

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
    char *op_str[OP] = {xstr(ecc_normalize, .csv), xstr(ecc_double, .csv), xstr(ecc_add, .csv),
                        xstr(ecc_mixed_add, .csv), xstr(ecc_scalar_mul, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP][LOOPS];          // only contains the median
    uint64_t values[OP][LOOPS][TIMINGS];  // contains all the measurements

    AffinePoint ap;
    ProjectivePoint p1, p2, p3;
    uint64_t scalar[NLIMBS];

    // warmup
    for (int i = 0; i < 10; i++) {
        ecc_normalize(&p1, &ap);
        ecc_double(&p1, &p2);
        ecc_add(&p1, &p2, &p3);
        ecc_mixed_add(&p1, &p2, &p3);
        ecc_scalar_mul(&p1, &p2, scalar);
    }

    for (loop = 0, op = 0; loop < LOOPS; loop++, op = 0) {
        // ecc_normalize
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_normalize(&p1, &ap);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_double
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_double(&p1, &p2);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_add
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_add(&p3, &p1, &p2);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_mixed_add
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_mixed_add(&p3, &p1, &p2);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_scalar_mul
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_scalar_mul(&p1, &p2, scalar);
        }
        memcpy(values[op][loop], cycles, sizeof(cycles));
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);
    }

    cpucycles_fprintf_2(results, op_str);
    write_values(values, results, op_str);

    return EXIT_SUCCESS;
}