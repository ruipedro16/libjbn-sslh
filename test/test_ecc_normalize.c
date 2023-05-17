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

typedef struct {
    uint64_t x[NLIMBS];
    uint64_t y[NLIMBS];
} AffinePoint;

extern void fp_inv(uint64_t *r, const uint64_t *a);
extern void fp_mul(uint64_t *r, const uint64_t *a, const uint64_t *b);

extern void ecc_normalize(AffinePoint *r, const ProjectivePoint *p);

void print_num(const uint64_t *x) {
    printf("[");
    for (size_t i = 0; i < NLIMBS - 1; i++) {
        printf("%ld, ", x[i]);
    }
    printf("%ld]", x[NLIMBS - 1]);
    printf("\n");
}

void print_affine(const AffinePoint *p) {
    printf("x = ");
    print_num(p->x);

    printf("y = ");
    print_num(p->y);

    printf("\n");
}

bool eq_affine(const AffinePoint *p, const AffinePoint *q) {
    for (size_t i = 0; i < NLIMBS; i++) {
        if (p->x[i] != q->x[i] || p->y[i] != q->y[i]) {
            return false;
        }
    }

    return true;
}

AffinePoint c_normalize(const ProjectivePoint *p) {
    AffinePoint res;
    uint64_t z_inv[NLIMBS];

    printf("Z^-1 = ");
    print_num(z_inv);

    printf("Z = ");
    print_num(p->z);

    fp_inv(z_inv, p->z);

    printf("Z^-1 = ");
    print_num(z_inv);

    // RX <- Z^-1 . X
    fp_mul(res.x, z_inv, p->x);

    // RY <- Z^-1 . Y
    fp_mul(res.y, z_inv, p->y);

    return res;
}

int main(int argc, const char **argv) {
    ProjectivePoint p;
    AffinePoint result, expected;

    // initialize P
    for (size_t i = 0; i < NLIMBS; i++) {
        p.x[i] = i;
        p.y[i] = i + 1;
        p.z[i] = i + 2;
    }

    expected = c_normalize(&p);
    printf("Expected:\n");
    print_affine(&expected);

    ecc_normalize(&result, &p);
    printf("Result:\n");
    print_affine(&result);

    if (eq_affine(&result, &expected)) {
        printf("Equal\n");
    } else {
        printf("Not Equal\n");
    }

    return eq_affine(&result, &expected) ? EXIT_SUCCESS : EXIT_FAILURE;
}