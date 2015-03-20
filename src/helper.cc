#include "stdafx.h"
#include "helper.h"

// return a random number form [a,b)
int Helper::U_Randint(int a, int b) {
    double x;
    x = (double) rand() / RAND_MAX;

    return (int) (a + (b - a) * x);
}

double Helper::P_Rand(double Lamda) {
#ifdef POISSON_KNUTH
    /*
     algorithm poisson random number (Knuth):
     init:
     Let L ← e−λ, k ← 0 and p ← 1.
     do:
     k ← k + 1.
     Generate uniform random number u in [0,1] and let p ← p × u.
     while p > L.
     return k − 1.
     */
    double x = 0, b = 1, c = exp(-Lamda), u;
    do{
        u = (double) rand() / RAND_MAX;
        b *= u;
        if(b >= c)
        x++;
    }while(b >= c);
    return x;
#else
    /*
     algorithm Poisson generator based upon the inversion by sequential search:
     init:
     Let x ← 0, p ← e−λ, s ← p.
     Generate uniform random number u in [0,1].
     do:
     x ← x + 1.
     p ← p * λ / x.
     s ← s + p.
     while u > s.
     return x.
     */
    double x = 0, p = exp(-Lamda), s = p;
    double u = (double) rand() / RAND_MAX;
    do{
        x = x + 1;
        p = p * Lamda / x;
        s = s + p;
    }while(u > s);
    return x;
#endif
}

double Helper::E_Rand(double Lamda) {
    // E: 1/Lamda
    double e, r;
    r = (double) rand() / RAND_MAX;
    e = -log(r) / Lamda;
    if(e == 0)
        e = 1 / Lamda;

    return e;
}

void Helper::setSeed(unsigned int seed) {
    srand(seed);
}

double Helper::F_Rand(double a, double b) {
    if(a <= 0){
        a = 1;
    }
    double limit_a = log(a);
    double limit_b = log(b);
    double x;
    x = (double) rand() / RAND_MAX;
    return pow(M_E, (limit_a + (limit_b - limit_a) * x));
}
