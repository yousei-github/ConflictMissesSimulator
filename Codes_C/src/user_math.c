#include "user_math.h"

float safe_division_float(float numerator, float denominator)
{
    float quotient = 0;
    if (denominator == 0)
    {
        return 0;
    }
    else
    {
        return quotient = numerator / denominator;
    }
}