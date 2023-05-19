#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

extern void ecc_scalar_mul(ProjectivePoint *r, const ProjectivePoint *p, const uint64_t *scalar);

uint64_t B[NLIMBS] = {0x5289a0cf641d011f, 0x9b88257189fed2b9, 0xa3b365d58dc8f17a,
                      0x5bc57ab6eff168ec, 0x9e51998bd84d4423, 0xbf8999cbac3b5695,
                      0x003f6cfce8b81771};

uint64_t ZERO[NLIMBS] = {0, 0, 0, 0, 0, 0, 0};

uint64_t ONE[NLIMBS] = {0x000000000000742c, 0x0000000000000000, 0x0000000000000000,
                        0xb90ff404fc000000, 0xd801a4fb559facd4, 0xe93254545f77410c,
                        0x0000eceea7bd2eda};

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

ProjectivePoint copyProjectivePoint(const ProjectivePoint *point) {
    if (point) {
        ProjectivePoint copy;

        // Copy x, y, and z values
        memcpy(copy.x, point->x, sizeof(uint64_t) * NLIMBS);
        memcpy(copy.y, point->y, sizeof(uint64_t) * NLIMBS);
        memcpy(copy.z, point->z, sizeof(uint64_t) * NLIMBS);

        return copy;
    }
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

ProjectivePoint c_scalar_mult(const uint64_t *scalar, const ProjectivePoint *p) {
    // R0 ← 0
    // R1 ← P
    // for i from m downto 0 do // iterate over the bits of the scalar
    //     if di = 0 then
    //         R1 ← point_add(R0, R1)
    //         R0 ← point_double(R0)
    //     else
    //         R0 ← point_add(R0, R1)
    //         R1 ← point_double(R1)
    // return R0

    ProjectivePoint r0, r1;

    ProjectivePoint pointAtInfinity;  // (0 : 1 : 0)

    // Set x, y, and z values for the point at infinity
    memcpy(pointAtInfinity.x, ZERO, sizeof(uint64_t) * NLIMBS);
    memcpy(pointAtInfinity.y, ONE, sizeof(uint64_t) * NLIMBS);
    memcpy(pointAtInfinity.z, ZERO, sizeof(uint64_t) * NLIMBS);

    r0 = copyProjectivePoint(&pointAtInfinity);
    r1 = copyProjectivePoint(p);

    for (size_t i = 0; i < NLIMBS; i++) {  // iterate over the limbs of the scalar
        uint64_t limb = scalar[i];

        for (size_t j = 0; j < 64; j++) {  // iterate over the bits of the current limb

            bool shiftedBit = (limb & 1) != 0;  // Get the least significant bit
            limb >>= 1;                         // Right shift by 1

            // printf("Shifted bit: %d\n", shiftedBit);

            if (shiftedBit == 0) {
                // R1 ← point_add(R0, R1)
                r1 = c_add(&r0, &r1);

                // R0 ← point_double(R0)
                r0 = c_double(&r0);
            } else {
                // R0 ← point_add(R0, R1)
                r0 = c_add(&r0, &r1);

                // R1 ← point_double(R1)
                r1 = c_double(&r1);
            }
        }
    }

    return r0;
}

int main(int argc, const char **argv) {
#define NTESTS 500

    ProjectivePoint p, result, expected;

    uint64_t t[NLIMBS] = {0xfffffffffffffff0, 0xffffffffffffffff, 0xffffffffffffffff,
                          0xfdc1767ae2ffffff, 0x7bc65c783158aea3, 0x6cfc5fd681c52056,
                          0x0002341f27177344};

    for (int i = 0; i < NTESTS; i++) {
        p = randomPoint();

        expected = c_scalar_mult(t, &p);

        ecc_scalar_mul(&result, &p, t);

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