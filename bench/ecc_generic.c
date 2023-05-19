#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NLIMBS
#define NLIMBS 4
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

int main(void) {
#define LOOPS 5
#define TIMINGS 4096
#define OP 5

    int loop, i, op;
    char *op_str[OP] = {xstr(ecc_normalize, .csv), xstr(ecc_double, .csv), xstr(ecc_add, .csv),
                        xstr(ecc_mixed_add, .csv), xstr(ecc_scalar_mul, .csv)};
    uint64_t cycles[TIMINGS];
    uint64_t results[OP][LOOPS];

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
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_double
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_double(&p1, &p2);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_add
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_add(&p3, &p1, &p2);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_mixed_add
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_mixed_add(&p3, &p1, &p2);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);

        // ecc_scalar_mul
        for (i = 0; i < TIMINGS; i++) {
            cycles[i] = cpucycles();
            ecc_scalar_mul(&p1, &p2, scalar);
        }
        results[op++][loop] = cpucycles_median(cycles, TIMINGS);
    }

    cpucycles_fprintf_2(results, op_str);

#undef OP
#undef TIMINGS
#undef LOOPS
    return 0;
}