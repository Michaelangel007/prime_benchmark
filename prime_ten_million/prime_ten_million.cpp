#include <stdio.h>
#include <math.h>

int
main(int argc, char *argv[])
{
    for (long n = 1; n < 10000000; n += 2)
    {
        double nsqrt      = sqrt((double)n);
        long   nsqrt_long = (long)nsqrt;
        bool   isPrime    = true;

        if ((double)nsqrt_long == nsqrt)
        {
            // printf("n [%ld] has a whole number sqrt [%ld]\n", n, nsqrt_long);
            isPrime = false;
        }
        else
        {
            for (long i = 3; i < (nsqrt_long + 1); i += 2)
            {
                double n_div_i      = (double)n / (double)i;
                long   n_div_i_long = (long)n_div_i;

                if ((double)n_div_i_long == n_div_i)
                {
                    // printf("n [%ld] divides evenly with [%ld]\n", n, i);
                    isPrime = false;
                }
            }
        }

        if (isPrime)
        {
            printf("n [%ld] is prime.\n", n);
        }

    }

    return 0;
}