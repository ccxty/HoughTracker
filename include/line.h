#include <array>
#include <cmath>
#ifndef __LINE_INCLUDE_CXX__
#define __LINE_INCLUDE_CXX__ 1

template <int N>
struct Line {
    std::array<int, N> x;
    std::array<int, N> y;
};

// y=a0+a1*x+...+a_{n-1}*x^{n-1}
template <int N>
struct Polynomial {
    int degree = N;
    std::array<int, N> eff;

    Polynomial();
    double value(double x);
};

template <int N>
Polynomial<N>::Polynomial() {
    for (int i = 0; i < N; i++) {
        eff[i] = 0;
    }
}

template <int N>
double Polynomial<N>::value(double x) {
    double result = 0;
    for (int i = 0; i < N; i++) {
        result += eff[i] * pow(x, i);
    }
    return result;
}

#endif