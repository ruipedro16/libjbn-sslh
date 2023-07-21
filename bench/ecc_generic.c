#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 7
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
extern void ecc_branchless_scalar_mul(ProjectivePoint *r, const ProjectivePoint *p,
                                      const uint64_t *scalar);

#include "cpucycles.c"
#include "printbench.h"
#include "randombytes.c"

#define TIMINGS 10000
#define OP 6

void get_random_number(uint64_t *arr) {
    uint8_t bytes[NLIMBS * 8];
    randombytes(bytes, NLIMBS * 8);  // 64 bits are 8 bytes
    memcpy(arr, bytes, NLIMBS * 8);
}

void get_random_projective_point(ProjectivePoint *p) {
    get_random_number(p->x);
    get_random_number(p->y);
    get_random_number(p->z);
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
    char *op_str[OP] = {xstr(ecc_normalize, .csv),  xstr(ecc_double, .csv),
                        xstr(ecc_add, .csv),        xstr(ecc_mixed_add, .csv),
                        xstr(ecc_scalar_mul, .csv), xstr(ecc_branchless_scalar_mul, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP];          // only contains the median
    uint64_t values[OP][TIMINGS];  // contains all the measurements

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
        ecc_branchless_scalar_mul(&p1, &p2, scalar);
    }

    op = 0;
    // ecc_normalize
    for (i = 0; i < TIMINGS; i++) {
        get_random_projective_point(&p1);
        cycles[i] = cpucycles();
        ecc_normalize(&p1, &ap);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // ecc_double
    for (i = 0; i < TIMINGS; i++) {
        get_random_projective_point(&p1);
        cycles[i] = cpucycles();
        ecc_double(&p1, &p2);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // ecc_add
    for (i = 0; i < TIMINGS; i++) {
        get_random_projective_point(&p1);
        get_random_projective_point(&p2);
        cycles[i] = cpucycles();
        ecc_add(&p3, &p1, &p2);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // ecc_mixed_add
    for (i = 0; i < TIMINGS; i++) {
        get_random_projective_point(&p1);
        get_random_projective_point(&p2);
        cycles[i] = cpucycles();
        ecc_mixed_add(&p3, &p1, &p2);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // ecc_scalar_mul
    for (i = 0; i < TIMINGS; i++) {
        get_random_projective_point(&p1);
        get_random_projective_point(&p2);
        get_random_number(scalar);
        cycles[i] = cpucycles();
        ecc_scalar_mul(&p1, &p2, scalar);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    // branchless scalar multiplication
    // ecc_scalar_mul
    for (i = 0; i < TIMINGS; i++) {
        cycles[i] = cpucycles();
        ecc_branchless_scalar_mul(&p1, &p2, scalar);
    }
    memcpy(values[op], cycles, sizeof(cycles));
    results[op++] = cpucycles_median(cycles, TIMINGS);

    cpucycles_fprintf_2(results, op_str);
    write_values(values, results, op_str);

    return EXIT_SUCCESS;
}
