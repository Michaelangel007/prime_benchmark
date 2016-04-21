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
    prime_t *gaPrimes = 0; // bitvector of numbers: potential primes
    prime_t  gnBits   = 0;
    uint64_t gnLargest = 0; // dynamic max column width

// Build table of dual primes from 2,3, 5,7 up to 6i-1,6i+1 but not including n=6i+1
// ============================================================
void BuildPrimes( const prime_t max )
{
    struct Prime
    {
        static inline bool IsPrime( const size_t n )
        {
            // Optimization: Don't need to test if 'n' is even, n % 2 == 0, n & 1 == 0

            // does the i'th prime evenly divide into n? Yes, then n is not prime
            prime_t offsetBits = 1, offsetByte = 0;
            for( prime_t iPrime = 3; iPrime < gnBits; iPrime += 2 ) // start with 2nd Prime: 3
            {
                offsetByte = iPrime / (8 * sizeof(prime_t));
                offsetBits = iPrime % (8 * sizeof(prime_t));

                if (gaPrimes[ offsetByte ] & (1 << offsetBits))
                    if ((n % (iPrime)) == 0) // have no remainder, not prime
                        return false;
            }

            return true;
        }
    };

    gaPrimes[0] |= (1 << 2); // 0th bit is 0 (not-p), 1st bit is 1 (not-p)
    gaPrimes[0] |= (1 << 3); // 2nd bit is 2 (prime), 3rd bit is 3 (prime)
/*
    gaPrimes[0] = (1 <<  5);
    gaPrimes[0] = (1 <<  7);
    gaPrimes[0] = (1 << 11);
    gaPrimes[0] = (1 << 13);
*/
    gnPrimes = 2; // skip 2, start with 3 for division
    gnBits   = 3;

    // Except for 2 and 3, every prime is of the form: n=6*i +/- 1
    // We store a (bit) flag if i is prime
    // 7 5 3 1  n
    // 3 2 1 0  BitMask
    prime_t offsetBits, offsetByte, n = 6;
    for( ; n <= max; n += 6 )
    {
        gnBits = n-1; // check bits between [3,n-1) (not including n-1)
        if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
        {
            //gaPrimes[ gnPrimes++ ] = n-1;
            offsetByte = (n-1) / (8 * sizeof( prime_t));
            offsetBits = (n-1) % (8 * sizeof( prime_t));
            gaPrimes[ offsetByte ] |= (1 << offsetBits);
            gnPrimes++;
//printf( "%2d = (32*%d + %2d) = [%02X] |= %02X\n", n-1, offsetByte, offsetBits, offsetByte, offsetBits );
            gnLargest = n-1;
        }

        gnBits = n+1; // check bits between [3,n+1) (not including n+1)
        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
        {
            //gaPrimes[ gnPrimes++ ] = n+1;
            offsetByte = (n+1) / (8 * sizeof( prime_t));
            offsetBits = (n+1) % (8 * sizeof( prime_t));
            gaPrimes[ offsetByte ] |= (1 << offsetBits);
            gnPrimes++;
//printf( "%2d = (32*%d + %2d) = [%02X] |= %02X \n", n+1, offsetByte, offsetBits, offsetByte, offsetBits );
            gnLargest = n+1;
        }
    }
#if 0
    // If (n+1) > max then we can't include in list if last 6i+1 was prime
    n -= 6;
    if ((n+1) > max) // only build primes up to and including n
        if ( Prime::IsPrime( n+1 ) )
            gnPrimes--;
#endif
//printf( "\n\n\n*** gaPrimes[0] & 9 = %d", gaPrimes[0] & )
}

// ============================================================
void PrintPrimes()
{
    printf( "gaPrimes[ %u ] = {\n", gnPrimes );

    char padding[ 32 ];
    const int COLUMNS       = 10;
    const int WIDTH_PER_COL = sprintf( padding, "%zu", gnLargest );

    struct Format
    {
        static void Suffix( const int width,  const size_t begin, const size_t end )
        {
            //printf( "//#%*zu ..%*zu\n", CHARS_PER_COL, begin, CHARS_PER_COL, end );
            printf( "//#%*zu\n", width, begin );
        }
    };

    printf( "%*u, ", WIDTH_PER_COL, 2 );

    // iPrime is total number of primes printed so far
    prime_t offsetBits, offsetByte, iPrime = 1;
    for( prime_t n = 3; n <= gnLargest; n += 2 ) // only check odd numbers
    {
        offsetByte = n / (8 * sizeof( prime_t));
        offsetBits = n % (8 * sizeof( prime_t));
//printf( "%2d = (32*%d + %2d) = [%02X] & (1 << %02X) = %d\n", n, offsetByte, offsetBits, offsetByte, offsetBits, (gaPrimes[ offsetByte ] & (1 << offsetBits)) ? 1 : 0 );

        if (gaPrimes[ offsetByte ] & (1 << offsetBits))
        {
            if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
                Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS, iPrime-1 );

            printf( "%*u, ", WIDTH_PER_COL, n );
            iPrime++;
        }
    }

    int rem = (iPrime % COLUMNS);
    int pad = ((COLUMNS-rem) * (WIDTH_PER_COL+2)); //+2 == width ", "
    if (rem != 0)
        printf( "%*s", pad, "" );
    else
        rem = COLUMNS;
    Format::Suffix( WIDTH_PER_COL, gnPrimes - rem, gnPrimes- 1 );
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
void TimerStart( prime_t max )
{
    printf( "Finding primes: 1 .. %s\n", itoaComma( max ) );

    timer.Start();
}

// ============================================================
void TimerStop( const prime_t max )
{
    timer.Stop();
    timer.Throughput( max );

// BEGIN BitVector
// Instead of scanning all primes again, we keep track as we build the prime table
//  gnLargest = gaPrimes[ gnPrimes - 1 ];
// END BitVector

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
//      :      100; //          = 25 primes between 1 and 100
//      :      256; // Test 8 core           // Largest 8-bit prime
//      :    65536; // [  6,541] =    65,521 // Release:    0.073 secs Largest 16-bit prime
//      :   100000; // [  9,592] =    99,991 // Release:    0.157 secs
//      :   611953; // [ 49,999] =   611,953 // Release:    4.182 secs First 50,000 primes
        : 10000000; // [664,578] = 9,999,991 // Release: 3016.245 secs 00:50:16.589  Primes/Sec: 3 K#/s
//      : 15485863; // [999,999] =15,485,863 // Release:      ?   secs One millionth prime

    AllocArray ( max );
    TimerStart ( max );
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes();
    DeleteArray();

    return 0;
}
