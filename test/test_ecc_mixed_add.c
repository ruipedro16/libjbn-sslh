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

extern void ecc_mixed_add(ProjectivePoint *r, const ProjectivePoint *p, const ProjectivePoint *q);

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

ProjectivePoint c_mixed_add(const ProjectivePoint *p, const ProjectivePoint *q) {
    ProjectivePoint res;

    uint64_t t0[NLIMBS], t1[NLIMBS], t2[NLIMBS], t3[NLIMBS], t4[NLIMBS];

    // 1: t0 ← X1 · X2
    fp_mul(t0, p->x, q->x);

    // 2: t1 ← Y1 · Y2
    fp_mul(t1, p->y, q->y);

    // 3: t3 ← X2 + Y2
    fp_add(t3, q->x, q->y);

    // 4: t4 ← X1 + Y1
    fp_add(t4, p->x, p->y);

    // 5: t3 ← t3 · t4
    fp_mul(t3, t3, t4);

    // 6: t4 ← t0 + t1
    fp_add(t4, t0, t1);

    // 7: t3 ← t3 − t4
    fp_sub(t3, t3, t4);

    // 8: t4 ← Y2 · Z1
    fp_mul(t4, q->y, p->z);

    // 9: t4 ← t4 + Y1
    fp_add(t4, t4, p->y);

    // 10: Y3 ← X2 · Z1
    fp_mul(res.y, q->x, p->z);

    // 11: Y3 ← Y3 + X1
    fp_add(res.y, res.y, p->x);

    // 12: Z3 ← b · Z1
    fp_mul(res.z, B, p->z);

    // 13: X3 ← Y3 − Z3
    fp_sub(res.x, res.y, res.z);

    // 14: Z3 ← X3 + X3
    fp_add(res.z, res.x, res.x);

    // 15: X3 ← X3 + Z3
    fp_add(res.x, res.x, res.z);

    // 16: Z3 ← t1 − X3
    fp_sub(res.z, t1, res.x);

    // 17: X3 ← t1 + X3
    fp_add(res.x, t1, res.x);

    // 18: Y3 ← b · Y3
    fp_mul(res.y, B, res.y);

    // 19: t1 ← Z1 + Z1
    fp_add(t1, p->z, p->z);

    // 20: t2 ← t1 + Z1
    fp_add(t2, t1, p->z);

    // 21: Y3 ← Y3 − t2
    fp_sub(res.y, res.y, t2);

    // 22: Y3 ← Y3 − t0
    fp_sub(res.y, res.y, t0);

    // 23: t1 ← Y3 + Y3
    fp_add(t1, res.y, res.y);

    // 24: Y3 ← t1 + Y3
    fp_add(res.y, t1, res.y);

    // 25: t1 ← t0 + t0
    fp_add(t1, t0, t0);

    // 26: t0 ← t1 + t0
    fp_add(t0, t1, t0);

    // 27: t0 ← t0 − t2
    fp_sub(t0, t0, t2);

    // 28: t1 ← t4 · Y3
    fp_mul(t1, t4, res.y);

    // 29: t2 ← t0 · Y3
    fp_mul(t2, t0, res.y);

    // 30: Y3 ← X3 · Z3
    fp_mul(res.y, res.x, res.z);

    // 31: Y3 ← Y3 + t2
    fp_add(res.y, res.y, t2);

    // 32: X3 ← t3 · X3
    fp_mul(res.x, t3, res.x);

    // 33: X3 ← X3 − t1
    fp_sub(res.x, res.x, t1);

    // 34: Z3 ← t4 · Z3
    fp_mul(res.z, t4, res.z);

    // 35: t1 ← t3 · t0
    fp_mul(t1, t3, t0);

    // 36: Z3 ← Z3 + t1
    fp_add(res.z, res.z, t1);

    return res;
}

int main(int argc, const char **argv) {
    ProjectivePoint p, q, result, expected;

    // initialize P & Q
    for (size_t i = 0; i < NLIMBS; i++) {
        p.x[i] = i;
        p.y[i] = i + 1;
        p.z[i] = i + 2;

        q.x[i] = i;
        q.y[i] = i + 1;
        q.z[i] = i + 2;
    }

    printf("Hello\n");

    expected = c_mixed_add(&p, &q);

    printf("Expected:\n");
    print_projective(&expected);

    ecc_mixed_add(&result, &p, &q);

    printf("Result:\n");
    print_projective(&result);

    if (eq_projective(&result, &expected)) {
        printf("Equal\n");
    } else {
        printf("Not Equal\n");
    }

    return eq_projective(&result, &expected) ? EXIT_SUCCESS : EXIT_FAILURE;
}
