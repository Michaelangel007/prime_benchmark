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
    #include <stdlib.h> // atoi(), exit()
    #include <string.h> // memcpy()

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

// Build table of primes from 2,3,5,7, up to and including n
// ============================================================
void BuildPrimes( const prime_t max )
{
    struct Prime
    {
        static inline bool IsPrime( const size_t n )
        {
            // does the i'th prime evenly divide into n? Yes, then n is not prime
            for( prime_t iPrime = 1; iPrime < gnPrimes; iPrime++ ) // First prime is 2 but are already skipping even numbers; start with 2nd Prime: 3
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

    prime_t n = 5;
    for( ; n <= max; n += 2 )
    {
        if( Prime::IsPrime( n ) )
            gaPrimes[ gnPrimes++ ] = n;
    }
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
void AllocArray( const size_t max )
{
    size_t nElements   = max;
    size_t nBytesTotal = nElements * sizeof( prime_t );
    printf( "Allocating memory..: %s * %d = ", itoaComma( nElements ), (int) sizeof( prime_t ) );
    printf( "%s bytes\n", itoaComma( nBytesTotal ) );

    gnPrimes = 0;
    gaPrimes = new prime_t[ nElements ];
    memset( gaPrimes, 0, nBytesTotal );
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

    printf( "Elapsed: %.3f secs = %s%s  Primes/Sec: %s %c#/s\n"
        , timer.elapsed
        , timer.data.day, timer.data.hms // secs, timer.data.ms
        , itoaComma( timer.throughput.per_sec ), timer.throughput.prefix
    );
}

// ============================================================
int main( const int nArg, const char *aArg[] )
{
    prime_t max = (nArg > 1)
        ? (prime_t) atou( aArg[ 1 ] )
//      :          6; // Test for 6i+1 > max
//      :      65536; // 2^16   [          6,542] =        65,521 // Release: 00:00:00.154  Primes/Sec: 415 K#/s  Largest 16-bit prime
//      :     100000; //10^5    [          9,592] =        99,991 // Release: 00:00:00.331  Primes/Sec: 293 K#/s
//      :     611953; //        [         50,000] =       611,953 // Release: 00:00:08.890  Primes/Sec: 67 K#/s   First 50,000 primes
//      :    1000000; //10^6    [         78,497] =       999,983 // Release: 00:00:22.046  Primes/Sec: 44 K#/s
        :   10000000; //10^7    [        664,578] =     9,999,991 // Release: 1602.294 secs = 00:26:42.294  Primes/Sec: 6 K#/s
//      :   15485863; //        [        999,999] =    15,485,863 // Release: 3620.988 secs = 01:00:20.988  Primes/Sec: 4 K#/s One millionth prime
//      :  100000000; //10^8    [      5,761,455] =    99,999,989 // Release: 119489.947 secs = 1 day, 09:11:29.947  Primes/Sec: 836  #/s

    AllocArray ( max );
    TimerStart ( max );
    BuildPrimes( max );
    TimerStop  ( max );
    int c = getchar(); // C/C++ crap on Win32/Win64 doesn't detect ESC (0x1B)
    PrintPrimes();
    DeleteArray();

    return 0;
}
