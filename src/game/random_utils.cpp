#include "random_utils.h"

int brandom(int limit)
{
    if (limit == 0) {
        return 0;
    }

    return (int)(limit * (rand() / (RAND_MAX + 1.0)));
}

/* Mission random number generator used on level 1. For random numbers
 * above brandom_threshold, there is a second roll of a Gaussian
 * random number generator that is slightly biased against high
 * numbers.
 */
int MisRandom()
{
    const double mu = 57;
    const double sigma = sqrt(1000);
    const int brandom_threshold = 66;

    int r_uniform = brandom(100) + 1;

    if (r_uniform < brandom_threshold) {
        return r_uniform;
    }

    double r_gaussian;
    do {
        // Generate two uniformly distributed random numbers.
        double u1 = rand() / (double) RAND_MAX;
        double u2 = rand() / (double) RAND_MAX;

        // Box-Muller transform to obtain a Gaussian random variable
        // with mean mu and standard deviation sigma. A value of 0.5
        // is added to ensure correct rounding.

        r_gaussian = sigma * sqrt(-2 * log(u1)) * cos(2 * M_PI * u2) + mu + 0.5;

    } while ((r_gaussian >= 101) || (r_gaussian < brandom_threshold));

    return (int) r_gaussian;
}
