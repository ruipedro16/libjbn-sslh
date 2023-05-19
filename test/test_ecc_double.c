#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NLIMBS 7

typedef struct {
    uint64_t x[NLIMBS];
    uint64_t y[NLIMBS];
    uint64_t z[NLIMBS];
} ProjectivePoint;

extern void fp_add(uint64_t *r, const uint64_t *a, const uint64_t *b);
extern void fp_sub(uint64_t *r, const uint64_t *a, const uint64_t *b);
extern void fp_mul(uint64_t *r, const uint64_t *a, const uint64_t *b);
extern void fp_sqr(uint64_t *r, const uint64_t *a);

extern void ecc_double(ProjectivePoint *r, const ProjectivePoint *p);

uint64_t B[NLIMBS] = {0x5289a0cf641d011f, 0x9b88257189fed2b9, 0xa3b365d58dc8f17a,
                      0x5bc57ab6eff168ec, 0x9e51998bd84d4423, 0xbf8999cbac3b5695,
                      0x003f6cfce8b81771};

void print_num(const uint64_t *x) {
    printf("[");
    for (size_t i = 0; i < NLIMBS - 1; i++) {
        printf("%ld, ", x[i]);
    }
    printf("%ld]", x[NLIMBS - 1]);
    printf("\n");
}

void print_projective(const ProjectivePoint *p) {
    printf("x = ");
    print_num(p->x);

    printf("y = ");
    print_num(p->y);

    printf("z = ");
    print_num(p->z);

    printf("\n");
}

bool eq_projective(const ProjectivePoint *p, const ProjectivePoint *q) {
    // Compare each limb of x, y, and z arrays
    for (size_t i = 0; i < NLIMBS; i++) {
        if (p->x[i] != q->x[i] || p->y[i] != q->y[i] || p->z[i] != q->z[i]) {
            return false;
        }
    }

    return true;
}

ProjectivePoint randomPoint() {
    ProjectivePoint point;

    for (size_t i = 0; i < NLIMBS; i++) {
        point.x[i] = (uint64_t)rand();  // Generate random 64-bit value for x
        point.y[i] = (uint64_t)rand();  // Generate random 64-bit value for y
        point.z[i] = (uint64_t)rand();  // Generate random 64-bit value for z
    }

    return point;
}

ProjectivePoint c_double(const ProjectivePoint *p) {
    ProjectivePoint res;

    uint64_t t0[NLIMBS], t1[NLIMBS], t2[NLIMBS], t3[NLIMBS];

    // 1: t0 ← X · X
    fp_sqr(t0, p->x);

    // 2: t1 ← Y · Y
    fp_sqr(t1, p->y);

    // 3: t2 ← Z · Z
    fp_sqr(t2, p->z);

    // 4: t3 ← X · Y
    fp_mul(t3, p->x, p->y);

    // 5: t3 ← t3 + t3
    fp_add(t3, t3, t3);

    // 6: Z3 ← X · Z
    fp_mul(res.z, p->x, p->z);

    // 7: Z3 ← Z3 + Z3
    fp_add(res.z, res.z, res.z);

    // 8: Y3 ← b · t2
    fp_mul(res.y, B, t2);

    // 9: Y3 ← Y3 − Z3
    fp_sub(res.y, res.y, res.z);

    // 10: X3 ← Y3 + Y3
    fp_add(res.x, res.y, res.y);

    // 11: Y3 ← X3 + Y3
    fp_add(res.y, res.x, res.y);

    // 12: X3 ← t1 − Y3
    fp_sub(res.x, t1, res.y);

    // 13: Y3 ← t1 + Y3
    fp_add(res.y, t1, res.y);

    // 14: Y3 ← X3 · Y3
    fp_mul(res.y, res.x, res.y);

    // 15: X3 ← X3 · t3
    fp_mul(res.x, res.x, t3);

    // 16: t3 ← t2 + t2
    fp_add(t3, t2, t2);

    // 17: t2 ← t2 + t3
    fp_add(t2, t2, t3);

    // 18: Z3 ← b · Z3
    fp_mul(res.z, B, res.z);

    // 19: Z3 ← Z3 − t2
    fp_sub(res.z, res.z, t2);

    // 20: Z3 ← Z3 − t0
    fp_sub(res.z, res.z, t0);

    // 21: t3 ← Z3 + Z3
    fp_add(t3, res.z, res.z);

    // 22: Z3 ← Z3 + t3
    fp_add(res.z, res.z, t3);

    // 23: t3 ← t0 + t0
    fp_add(t3, t0, t0);

    // 24: t0 ← t3 + t0
    fp_add(t0, t3, t0);

    // 25: t0 ← t0 − t2
    fp_sub(t0, t0, t2);

    // 26: t0 ← t0 · Z3
    fp_mul(t0, t0, res.z);

    // 27: Y3 ← Y3 + t0
    fp_add(res.y, res.y, t0);

    // 28: t0 ← Y · Z
    fp_mul(t0, p->y, p->z);

    // 29: t0 ← t0 + t0
    fp_add(t0, t0, t0);

    // 30: Z3 ← t0 · Z3
    fp_mul(res.z, t0, res.z);

    // 31: X3 ← X3 − Z3
    fp_sub(res.x, res.x, res.z);

    // 32: Z3 ← t0 · t1
    fp_mul(res.z, t0, t1);

    // 33: Z3 ← Z3 + Z3
    fp_add(res.z, res.z, res.z);

    return res;
}

int main(int argc, const char **argv) {
#define NTESTS 500

    ProjectivePoint p, result, expected;

    for (int i = 0; i < NTESTS; i++) {
        p = randomPoint();

        expected = c_double(&p);

        ecc_double(&result, &p);

        if (eq_projective(&result, &expected)) {
            printf("ECC Double: %d/%d: %s\n", i + 1, NTESTS, "OK");
        } else {
            printf("ECC Double: %d/%d: %s\n", i + 1, NTESTS, "NOT OK");
            return EXIT_FAILURE;
        }
    }

#undef NTESTS

    return EXIT_SUCCESS;
}