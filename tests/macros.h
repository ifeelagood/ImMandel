#pragma once 

#include <Eigen/Dense>

#define EXPECT_EIGEN_VECTOR2_NEAR(a, b, tol)        \
    do {                                            \
        EXPECT_NEAR((a).x(), (b).x(), tol);         \
        EXPECT_NEAR((a).y(), (b).y(), tol);         \
    } while (0)
