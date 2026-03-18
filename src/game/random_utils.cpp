#include "random_utils.h"

int brandom(int limit)
{
    if (limit == 0) {
        return 0;
    }

    return (int)(limit * (rand() / (RAND_MAX + 1.0)));
}
