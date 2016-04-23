/*
Prime Benchmark
by Michael Pohoreski

Version 1a - Clamp on  - Naive; cache & check previous primes only
Version 1b - Clamp off - Remove if (n+1)>max from main prime loop
Version 1c - Bit Vector - Naive  ; cache & check previous primes only
Version 1d - Bit Vector - Minimal; only odd numbers of 6i+/-1
Version 2a - Clamp on  - Don't use prime lookup table; only check odd factors
Version 2b - Clamp off - Cap max prime search at square root(n)
Version 3  - Multi-Threaded with OpenMP
*/

// Includes
    #ifdef _WIN32 // MSVC sprintf() warning crap
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #include <stdio.h>  // printf()
    #include <string.h> // memcpy()
    #include <math.h>

    #include "util_types.h" // or <stdint.h>
    #include "util_text.h"
    #include "util_timer.h"

// Macros
    #define prime_t unsigned int

// Globals
    Timer timer;

    prime_t  gnPrimes = 0;
    prime_t *gaPrimes = 0;

    uint64_t gnLargest = 0; // dynamic max column width

// Build table of dual primes from 2,3, 5,7 up to 6i-1,6i+1 but not including n=6i+1
// ============================================================
void BuildPrimes( const prime_t max )
{
    struct Prime
    {
        static inline bool IsPrime( const size_t n )
        {
            // does the i'th prime evenly divide into n? Yes, then n is not prime
            for( prime_t iPrime = 1; iPrime < gnPrimes; iPrime++ ) // start with 2nd Prime: 3
                if ((n % gaPrimes[ iPrime ]) == 0) // have no remainder, not prime
                    return false;

            return true;
        }
    };

    gaPrimes[0] = 2;
    gaPrimes[1] = 3;
    gnPrimes    = 2;
/*
    gaPrimes[2] = 5;
    gaPrimes[3] = 7;
    gaPrimes[4] = 11;
    gaPrimes[5] = 13;
*/

    // Except for 2 and 3, every prime is of the form: n=6*i +/- 1
    prime_t n = 6;
    for( ; n <= max; n += 6 )
    {
        if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
            gaPrimes[ gnPrimes++ ] = n-1;

        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
            gaPrimes[ gnPrimes++ ] = n+1;
    }

    // 11
    if( n > max)
        if ((n-1) <= max)
            if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
                gaPrimes[ gnPrimes++ ] = n-1;

    // 6, 7
    if( gaPrimes[ gnPrimes-1 ] > max ) // only build primes up to and including n
        gnPrimes--;
}

// ============================================================
void PrintPrimes()
{
    printf( "gaPrimes[ %u ] = {\n", gnPrimes );

    char padding[ 32 ];
    const int COLUMNS       = 10;
    const int WIDTH_PER_COL = sprintf( padding, "%llu", gnLargest );

    struct Format
    {
        static void Suffix( const int width, const size_t begin, const size_t end )
        {
            //printf( "//#%*zu ..%*zu\n", CHARS_PER_COL, begin, CHARS_PER_COL, end );
            printf( "//#%*zu\n", width, begin );
        }
    };

    for( prime_t iPrime = 0; iPrime < gnPrimes; iPrime++ )
    {
        if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
            Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS+1, iPrime );
        printf( "%*u, ", WIDTH_PER_COL, gaPrimes[ iPrime ] );
    }

    int rem = (gnPrimes % COLUMNS);
    int pad = ((COLUMNS-rem) * (WIDTH_PER_COL+2)); //+2 == width ", "
    if (rem != 0)
        printf( "%*s", pad, "" );
    else
        rem = COLUMNS;

    Format::Suffix( WIDTH_PER_COL, gnPrimes - rem+1, gnPrimes );
    printf( "};\n" );
}

// ============================================================
void AllocArray( const size_t elements )
{
    gnPrimes = 0;
    gaPrimes = new prime_t[ elements ];
    memset( gaPrimes, 0, sizeof( prime_t ) * elements );
}

// ============================================================
void DeleteArray()
{
    delete [] gaPrimes;
}

// ============================================================
void TimerStart( const prime_t max )
{
    printf( "Finding primes: 1 .. %s\n", itoaComma( max ) );

    timer.Start();
}

// ============================================================
void TimerStop( const prime_t max )
{
    timer.Stop();
    timer.Throughput( max );

    gnLargest = gaPrimes[ gnPrimes - 1 ];

    printf( "Primes found: %s'th = ", itoaComma( gnPrimes ) );
    printf( "%s\n", itoaComma( gnLargest ) );

    printf( "Elapsed: %.3f secs = %s%s  Primes/Sec: %s %c#/s"
        , timer.elapsed
        , timer.data.hms, timer.data.day // secs, timer.data.ms
        , itoaComma( timer.throughput.per_sec ), timer.throughput.prefix
    );
}

// ============================================================
int main( const int nArg, const char *aArg[] )
{
    prime_t max = (nArg > 1)
        ? (prime_t) atou( aArg[ 1 ] )
//      :        6; // Test for 6i+1 > max
//      :        7; // Test for 6i+1
//      :       11; // Test for 6i-1 > max
//      :    65536; // [  6,541] =    65,521 // Release:  0.152 secs Largest 16-bit prime
//      :   100000; // [  9,592] =    99,991 // Release:  0.326 secs
//      :   611953; // [ 49,999] =   611,953 // Release:  8.882 secs First 50,000 primes
//      :  1000000; // [ 78,497] =   999,983 // Release: 21.909 secs
        : 10000000; // [664,578] = 9,999,991 // Release: 15     mins
//      : 15485863; // [999,999] =15,485,863 // Release:        mins One millionth prime

    AllocArray ( max );
    TimerStart ( max );
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes();
    DeleteArray();

    return 0;
}
