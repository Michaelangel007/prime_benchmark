/*
Prime Benchmark
by Michael Pohoreski

Version 1a - Clamp on  - Naive; Check previous primes only
Version 1b - Clamp off - Remove if (n+1)>max from main prime loop
Version 2a - Clamp on  - Don't use prime lookup table; only check odd factors
Version 2b - Clamp off - Cap max prime search at square root(n)
*/

// Includes
    #ifdef _WIN32 // MSVC sprintf() warning crap
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #include <stdio.h>  // printf()
    #include <string.h> // memcpy()
    #include <math.h>   // sqrt()

    #include "util_types.h" // or <stdint.h>
    #include "util_text.h"
    #include "util_timer.h"

// Macros
    #define prime_t int

// Globals
    Timer timer;

    prime_t  gnPrimes = 0;
    prime_t *gaPrimes = 0;

// Build table of dual primes from 2,3, 5,7 up to 6i-1,6i+1 but not including n=6i+1
// ============================================================
void BuildPrimes( const prime_t max )
{
    struct Prime
    {
        static inline bool IsPrime( const size_t n )
        {
            // does the i'th prime evenly divide into n? Yes, then n is not prime
            // This blows the D$; 15 mins down to 9 secs for 10,000,000
            prime_t root = (prime_t) sqrt( (double)n )+2;

            for( prime_t iPrime = 3; iPrime < root; iPrime += 2 )
                if ((n % iPrime) == 0)
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

        if ((n+1) > max) // only build primes up to and including n
            break;

        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
            gaPrimes[ gnPrimes++ ] = n+1;
    }
}

// ============================================================
void PrintPrimes()
{
    printf( "gnPrimes = %u\n", gnPrimes );
    printf( "gaPrimes[ %u ] = {\n", gnPrimes );

    const int COLUMNS       = 10;
    const int CHARS_PER_COL = 7;

    struct Format
    {
        static void Spaces( const int width )
        {
            printf( "%*s", width, "" );
        }
        static void Suffix( const size_t begin, const size_t end )
        {
            printf( "//#%*zu ..%*zu\n", CHARS_PER_COL, begin, CHARS_PER_COL, end );
        }
    };

    for( prime_t iPrime = 0; iPrime < gnPrimes; iPrime++ )
    {
        if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
            Format::Suffix( iPrime-COLUMNS, iPrime-1 );
        printf( "%*u, ", CHARS_PER_COL-2, gaPrimes[ iPrime ] );
    }

    int rem = (gnPrimes % COLUMNS);
    int    pad = ((COLUMNS-rem) * CHARS_PER_COL);
    if (rem != 0)
        Format::Spaces( pad );
    else
        rem = COLUMNS;
    Format::Suffix( gnPrimes - rem, gnPrimes - 1 );
    printf( "};\n" );
}

// ============================================================
void AllocArray( size_t elements )
{
    gnPrimes = 0;
    gaPrimes = new prime_t[ elements ];
}

// ============================================================
void DeleteArray()
{
    delete [] gaPrimes;
}

// ============================================================
void TimerStart()
{
    timer.Start();
}

// ============================================================
void TimerStop( const prime_t max )
{
    timer.Stop();
    timer.Throughput( max );

    printf( "Primes found: [%s] = ", itoaComma( gnPrimes-1 ) );
    printf( "%s\n", itoaComma( gaPrimes[ gnPrimes-1 ] ) );

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
//      :    65536;
//      :   100000;
//      :   611953; // [49,999] = 611,953 // First 50,000 primes
        : 10000000; // [664,578] = 9,999,991 // Debug:  6.4 secs, Release:  6.2 secs
//      : 15485863; // one millionth prime   // Debug: 11.9 secs, Release: 11.6 secs

    AllocArray ( max );
    TimerStart ();
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes();
    DeleteArray();

    return 0;
}
