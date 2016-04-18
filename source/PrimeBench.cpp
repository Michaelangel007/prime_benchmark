// Includes
    #ifdef _WIN32 // MSVC crap
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #include <stdio.h>
    typedef unsigned long long uint64_t;
    #include "util_timer.h"
    #include "util_text.h"

// Macros
    #define prime_t int

// Globals
    prime_t  nPrimes = 0;
    prime_t *aPrimes = 0;
    prime_t  nLast   = 0;

    prime_t *gaBitsSub1 = 0;
    prime_t *gaBitsAdd1 = 0;

    enum {
         OUTPUT_NONE
        ,OUTPUT_DOT
        ,OUTPUT_PERCENT
    };
    int gbOutput = OUTPUT_PERCENT;

/*

    31 30 29 28 27 26 25 24 23 22 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
                                                                                              n*6*1
                                                                                           n*6*2
*/
void BitsToN()
{
}

// Given an odd n, either 6*n-1 or 6*n+1,
void NtoBits( const prime_t n )
{
}


// ============================================================
setPrime( prime_t n, int isPrime )
{
    size_t offset = n / 6;
    if()
    {

    }
}

// Build table of dual primes from 2,3, 5,7 up to n-1,n+1
// ============================================================
void BuildPrimes( const prime_t max )
{
    struct Prime
    {
        static inline bool IsPrime( const size_t n )
        {
            // does the i'th prime evenly divide into n? Yes, not then n is not prime
            for( prime_t iPrime = 1; iPrime < nPrimes; iPrime++ ) // start with 2nd Prime: 3
                if ((n % aPrimes[ iPrime ]) == 0) // have no remainder, not prime
                    return false;
            return true;
        }
    };

    setPrime( 5, 1 ); // 5 = 6*1 - 1    5/6 = 0
    setPrime( 7, 1 ); // 7 = 6*1 + 1    7/6 = 1
//  setPrime(11, 1 ); //11 = 6*2 - 1   11/6 = 1
//  setPrime(13, 1 ); //13 = 6*2 + 1   13/6 = 2

    nPrimes    = 2;

    int output  = 0;
    int percent = (gbOutput & OUTPUT_DOT) ? (max / 10) : (max / 100);
    double ood  = 1. / max; 

    for( prime_t n = 12; n <= max; n += 6 ) // skip n=6+/-1, start with n=12+/-1
    {
        if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
            aPrimes[ nPrimes++ ] = n-1;

        if ((n+1) > max) // only build primes up to and including n
            break;

        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
            aPrimes[ nPrimes++ ] = n+1;

        if( gbOutput )
        {
            output += 2;
            if (output > percent)
            {
                output -= percent;

                if (gbOutput & OUTPUT_DOT)
                    printf( "." );
                else
                    printf( "%6.2f%%\b\b\b\b\b\b\b", (n * 100.) * ood );
            }
        }
    }
}

// ============================================================
void PrintPrimes()
{
    printf( "gnPrimes = %u\n", nPrimes );
    printf( "gaPrimes[ %u ] = {\n", nPrimes );

    const size_t COLUMNS       = 10;
    const size_t CHARS_PER_COL = 7;

    for( prime_t iPrime = 0; iPrime < nPrimes; iPrime++ )
    {
        if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
            printf( "// %2u .. %2u\n", iPrime-COLUMNS, iPrime-1 );
        printf( "%*u, ", CHARS_PER_COL-2, aPrimes[ iPrime ] );
    }

    size_t rem = (nPrimes % COLUMNS);
    int    pad = ((COLUMNS-rem) * CHARS_PER_COL);
    if (rem != 0)
        printf( "%*s// %2u .. %2u", pad, "", nPrimes-rem, nPrimes-1 );
    printf( "\n};\n" );
}

// ============================================================
int main( const int nArg, const char *aArg[] )
{
    //    65,535 0.072 s
    //   100,000 0.0147 s
    // 1,000,000 9.735 s
    //10,000,000 
    size_t max = 65536; // 256*256; // 256
    if (nArg > 1)
        max = atou( aArg[1] );

    aPrimes = new prime_t[ max ];

    size_t es  = sizeof( prime_t ); // elemnent size
    size_t nArraySize =  &aPrimes[max] - &aPrimes[0];
    printf( "%s Numbers @ %d bytes/prime =", itoaComma( max ), (int) es );
    printf( "= %s bytes Prime Table\n"    , itoaComma( max * es )      );

    Timer timer;
    timer.Start();

        BuildPrimes( max );
    
    timer.Stop();
    timer.Throughput( max );

    printf( "Primes found: [%s] = ", itoaComma( nPrimes-1 ) );
    printf( "%s\n", itoaComma( aPrimes[ nPrimes-1 ] ) );

    printf( "Elapsed: %7.3f   %s%s  Primes/Sec: %s %c#/s"
        , timer.elapsed
        , timer.data.hms, timer.data.day // secs, timer.data.ms
        , itoaComma( timer.throughput.per_sec ), timer.throughput.prefix
    );

    getchar();

    PrintPrimes();

    delete [] aPrimes;

    return 0;
}

