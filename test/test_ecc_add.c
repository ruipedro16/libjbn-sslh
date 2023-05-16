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

extern void ecc_add(ProjectivePoint *r, const ProjectivePoint *p, const ProjectivePoint *q);

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

ProjectivePoint c_add(const ProjectivePoint *p, const ProjectivePoint *q) {
    ProjectivePoint res;

    uint64_t t0[NLIMBS], t1[NLIMBS], t2[NLIMBS], t3[NLIMBS], t4[NLIMBS];

    // 1: t0 ← X1 · X2
    fp_mul(t0, p->x, q->x);

    // 2: t1 ← Y1 · Y2
    fp_mul(t1, p->y, q->y);

    // 3: t2 ← Z1 · Z2
    fp_mul(t2, p->z, q->z);

    // 4: t3 ← X1 + Y1
    fp_add(t3, p->x, p->y);

    // 5: t4 ← X2 + Y2
    fp_add(t4, q->x, q->y);

    // 6: t3 ← t3 · t4
    fp_mul(t3, t3, t4);

    // 7: t4 ← t0 + t1
    fp_add(t4, t0, t1);

    // 8: t3 ← t3 − t4
    fp_sub(t3, t3, t4);

    // 9: t4 ← Y1 + Z1
    fp_add(t4, p->y, p->z);

    // 10: X3 ← Y2 + Z2
    fp_add(res.x, q->y, q->z);

    // 11: t4 ← t4 · X3
    fp_mul(t4, t4, res.x);

    // 12: X3 ← t1 + t2
    fp_add(res.x, t1, t2);

    // 13: t4 ← t4 − X3
    fp_sub(t4, t4, res.x);

    // 14: X3 ← X1 + Z1
    fp_add(res.x, p->x, p->z);

    // 15: Y3 ← X2 + Z2
    fp_add(res.y, q->x, q->z);

    // 16: X3 ← X3 · Y3
    fp_mul(res.x, res.x, res.y);

    // 17: Y3 ← t0 + t2
    fp_add(res.y, t0, t2);

    // 18: Y3 ← X3 − Y3
    fp_sub(res.y, res.x, res.y);

    // 19: Z3 ← b · t2
    fp_mul(res.z, B, t2);

    // 20: X3 ← Y3 − Z3
    fp_sub(res.x, res.y, res.z);

    // 21: Z3 ← X3 + X3
    fp_add(res.z, res.x, res.x);

    // 22: X3 ← X3 + Z3
    fp_add(res.x, res.x, res.z);

    // 23: Z3 ← t1 − X3
    fp_sub(res.z, t1, res.x);

    // 24: X3 ← t1 + X3
    fp_add(res.x, t1, res.x);

    // 25: Y3 ← b · Y3
    fp_mul(res.y, B, res.y);

    // 26: t1 ← t2 + t2
    fp_add(t1, t2, t2);

    // 27: t2 ← t1 + t2
    fp_add(t2, t1, t2);

    // 28: Y3 ← Y3 − t2
    fp_sub(res.y, res.y, t2);

    // 29: Y3 ← Y3 − t0
    fp_sub(res.y, res.y, t0);

    // 30: t1 ← Y3 + Y3
    fp_add(t1, res.y, res.y);

    // 31: Y3 ← t1 + Y3
    fp_add(res.y, t1, res.y);

    // 32: t1 ← t0 + t0
    fp_add(t1, t0, t0);

    // 33: t0 ← t1 + t0
    fp_add(t0, t1, t0);

    // 34: t0 ← t0 − t2
    fp_sub(t0, t0, t2);

    // 35: t1 ← t4 · Y3
    fp_mul(t1, t4, res.y);

    // 36: t2 ← t0 · Y3
    fp_mul(t2, t0, res.y);

    // 37: Y3 ← X3 · Z3
    fp_mul(res.y, res.x, res.z);

    // 38: Y3 ← Y3 + t2
    fp_add(res.y, res.y, t2);

    // 39: X3 ← t3 · X3
    fp_mul(res.x, t3, res.x);

    // 40: X3 ← X3 − t1
    fp_sub(res.x, res.x, t1);

    // 41: Z3 ← t4 · Z3
    fp_mul(res.z, t4, res.z);

    // 42: t1 ← t3 · t0
    fp_mul(t1, t3, t0);

    // 43: Z3 ← Z3 + t1
    fp_add(res.z, res.z, t1);

    return res;
}

//

/*
 * jasminc ../src/ecc/amd64/ref/ecc_generic_export.jinc -o ecc.s
 * jasminc ../src/fp/amd64/ref/fp_generic_export.jinc -o fp.s
 * gcc test_ecc_add.c *.s -o add.o
 * ./add.o || echo "failed"
 */

// jasminc ../src/ecc/amd64/ref/ecc_generic_export.jinc -o ecc.s ; jasminc
// ../src/fp/amd64/ref/fp_generic_export.jinc -o fp.s ; gcc test_ecc_add.c *.s -o add.o

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

    expected = c_add(&p, &q);

    printf("Expected:\n");
    print_projective(&expected);

    ecc_add(&result, &p, &q);

    printf("Result:\n");
    print_projective(&result);

    if (eq_projective(&result, &expected)) {
        printf("Equal\n");
    } else {
        printf("Not Equal\n");
    }

    // return EXIT_SUCCESS;

    return eq_projective(&expected, &result) ? EXIT_SUCCESS : EXIT_FAILURE;
}