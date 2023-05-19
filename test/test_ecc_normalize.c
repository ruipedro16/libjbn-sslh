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

ProjectivePoint randomPoint() {
    ProjectivePoint point;

    for (size_t i = 0; i < NLIMBS; i++) {
        point.x[i] = (uint64_t)rand();  // Generate random 64-bit value for x
        point.y[i] = (uint64_t)rand();  // Generate random 64-bit value for y
        point.z[i] = (uint64_t)rand();  // Generate random 64-bit value for z
    }

    return point;
}

AffinePoint c_normalize(const ProjectivePoint *p) {
    AffinePoint res;
    uint64_t z_inv[NLIMBS];

    fp_inv(z_inv, p->z);

    // RX <- Z^-1 . X
    fp_mul(res.x, z_inv, p->x);

    // RY <- Z^-1 . Y
    fp_mul(res.y, z_inv, p->y);

    return res;
}

int main(int argc, const char **argv) {
#define NTESTS 500

    ProjectivePoint p;
    AffinePoint result, expected;

    for (int i = 0; i < NTESTS; i++) {
        p = randomPoint();
        expected = c_normalize(&p);

        ecc_normalize(&result, &p);

        if (eq_affine(&result, &expected)) {
            printf("ECC Normalize: %d/%d: %s\n", i + 1, NTESTS, "OK");
        } else {
            printf("ECC Normalize: %d/%d: %s\n", i + 1, NTESTS, "NOT OK");
            return EXIT_FAILURE;
        }
    }

#undef NTESTS

    return EXIT_SUCCESS;
}