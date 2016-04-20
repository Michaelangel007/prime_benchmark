/*
Prime Benchmark
by Michael Pohoreski

Version 1a - Clamp on  - Naive; Check previous primes only
Version 1b - Clamp off - Remove if (n+1)>max from main prime loop
Version 2a - Clamp on  - Don't use prime lookup table; only check odd factors
Version 2b - Clamp off - Cap max prime search at square root(n)
Version 3  - OpenMP
*/

// Includes
    #ifdef _WIN32 // MSVC sprintf() warning crap
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #include <stdio.h>  // printf()
    #include <string.h> // memcpy()
    #include <math.h>   // sqrt()
// BEGIN OMP
    #include <omp.h> // MSVC: C++, Language, Open MP: /openmp
// END OMP

    #include "util_types.h" // or <stdint.h>
    #include "util_text.h"
    #include "util_timer.h"

// Macros
    #define prime_t int

// Globals
    Timer timer;

    prime_t  gnPrimes = 0;
    prime_t *gaPrimes = 0;

    prime_t  gnLargest = 0; // dynamic max column width

// BEGIN OMP
    int       gnThreadsMaximum = 0 ;
    int       gnThreadsActive  = 0 ; // 0 = auto detect; > 0 manual # of threads
// END OMP

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
// BEGIN OMP
#pragma omp parallel for
// END OMP
    for( prime_t n = 6; n <= max; n += 6 )
    {
        if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
        {
// BEGIN OMP
#pragma omp critical
// END OMP
                gaPrimes[ gnPrimes++ ] = n-1;
        }

        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
        {
// BEGIN OMP
#pragma omp critical
// END OMP
                gaPrimes[ gnPrimes++ ] = n+1;
        }
    }
#if 0
    // If (n+1) > max then we can't include in list if last 6i+1 was prime
    prime_t n = max - (max % 6); // TODO: FIXME: Verify this is correct algorithm.
    n -= 6;
    if ((n+1) > max) // only build primes up to and including n
        if ( Prime::IsPrime( n+1 ) )
            gnPrimes--;
#endif
}

// ============================================================
void PrintPrimes()
{
    printf( "gnPrimes = %u\n", gnPrimes );
    printf( "gaPrimes[ %u ] = {\n", gnPrimes );

    char padding[ 32 ];
    const int COLUMNS       = 10;
    const int WIDTH_PER_COL = sprintf( padding, "%zu", gnLargest );

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
            Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS, iPrime-1 );
        printf( "%*u, ", WIDTH_PER_COL, gaPrimes[ iPrime ] );
    }

    int rem = (gnPrimes % COLUMNS);
    int pad = ((COLUMNS-rem) * (WIDTH_PER_COL+2)); //+2 == width ", "
    if (rem != 0)
        printf( "%*s", pad, "" );
    else
        rem = COLUMNS;
    Format::Suffix( WIDTH_PER_COL, gnPrimes - rem, gnPrimes - 1 );
    printf( "};\n" );
}

// ============================================================
void AllocArray( size_t elements )
{
// BEGIN OMP
    if(!gnThreadsActive) // user didn't specify how many threads to use, default to all of them
        gnThreadsActive = gnThreadsMaximum;

    omp_set_num_threads( gnThreadsActive );
// END OMP

    gnPrimes = 0;
    gaPrimes = new prime_t[ elements ];
    memset( gaPrimes, 0, sizeof( prime_t ) * elements );

// BEGIN OMP
    printf( "Using: %d / %d cores\n", gnThreadsActive, gnThreadsMaximum );
// END OMP
}

// ============================================================
void DeleteArray()
{
    delete [] gaPrimes;
}

// ============================================================
void TimerStart()
{
    printf( "Finding primes...\n" );

    timer.Start();
}

// ============================================================
void TimerStop( const prime_t max )
{
    timer.Stop();
    timer.Throughput( max );

    gnLargest = gaPrimes[ gnPrimes - 1 ];

// BEGIN OMP
/*
    Note:
        Finding the first n primes and printing the n primes in sequential order
        are two DIFFERENT problems!

        For the latter, we would need to sort ALL the primes since they could be added in any order.
*/
   printf( "Multi-threaded Verification: Check if any primes are zero..." );
    size_t nZeroes = 0;
    for( prime_t i = 0; i < (gnPrimes-1); i++ )
        if( gaPrimes[ i ] == 0 )
            nZeroes++;
    if( nZeroes ) 
        printf( "FAIL! Found zeroes: %zu\n", nZeroes );
    else
        printf( "pass\n" );

    printf( "Multi-threaded Search: Find largest prime..." );
    Timer timeLargest;
    timeLargest.Start();

        for( prime_t iPrime = 0; iPrime < gnPrimes; iPrime++ )
            if( gnLargest < gaPrimes[ iPrime ] )
                gnLargest = gaPrimes[ iPrime ] ;

    timeLargest.Stop();
    timeLargest.Throughput( gnPrimes );
    printf( "%s%s\n",
        timeLargest.data.hms, timeLargest.data.day
    );
// END OMP

    printf( "Primes found: [%s] = ", itoaComma( gnPrimes-1 ) );
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
// BEGIN OMP
    gnThreadsMaximum = omp_get_num_procs();
// END OMP
 
   prime_t max = (nArg > 1)
        ? (prime_t) atou( aArg[ 1 ] )
//      :        6; // Test for 6i+1 > max
//      :      100; //          = 25 primes between 1 and 100
//      :      255; // Test 8 core
//      :      256; // Test 8 core           // Largest 8-bit prime
//      :    65536; // [  6,541] =    65,521 // Largest 16-bit prime
//      :   100000; // [  9,591] =    99,991
//      :   611953; // [ 49,999] =   611,953 // x86: 0.03 secs, x64: 0.06 secs First 50,5000 primes
        : 10000000; // [664,578] = 9,999,991 // x86: 0.7  secs, x64: 1.2  secs
//      : 15485863; // [999,999] =           // x86: 1.2  secs, X64: 2.3  secs First 1,000,000 primes

    AllocArray ( max );
    TimerStart ();
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes();
    DeleteArray();

    return 0;
}
