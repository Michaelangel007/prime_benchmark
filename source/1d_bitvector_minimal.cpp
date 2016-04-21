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
    prime_t *gaPrimes = 0; // pair of potential primes: M1 (Minus1), P1 (Plus1)
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
            if ((n % 3) == 0)
                return false;

            prime_t offsetBits = 1, offsetByte = 0;
            prime_t prime = 6;
            for( prime_t iBits = 0; iBits < gnBits; iBits++ ) // start with 2nd Prime: 3
            {
                // 32-bits / 2 bits/primepair = 16 
//                for( int word = 0; word < sizeof(prime_t)*2; word++ )
                {
                    if (gaPrimes[ offsetByte ] & offsetBits)
                        if ((n % (prime-1)) == 0) // have no remainder, not prime
                            return false;
                    offsetBits <<= 1;

                    iBits++;
                    if( iBits >= gnBits )
                        return true;

                    if (gaPrimes[ offsetByte ] & offsetBits)
                        if ((n % (prime+1)) == 0)
                            return false;
                    offsetBits <<= 1;

                    prime += 6;
                }

                if( !offsetBits )
                {
                    offsetBits = 1;
                    offsetByte++;
                }
            }

            return true;
        }
    };

// We don't store 2 & 3 in our prime table, since they are not multiples of 6i+/-1
/*
    gaPrimes[0] = 2;
    gaPrimes[1] = 3;
*/
    gnPrimes    = 0;
/*
    gaPrimes[2] = 5;
    gaPrimes[3] = 7;
    gaPrimes[4] = 11;
    gaPrimes[5] = 13;
*/
    gnPrimes = 0;
    gnBits   = 0;

    // Except for 2 and 3, every prime is of the form: n=6*i +/- 1
    prime_t offsetBits = 1, offsetByte = 0;
    prime_t n = 6;
    for( ; n <= max; n += 6 )
    {
        if( Prime::IsPrime( n-1 ) ) // 6*i-1; n=6*x
        {
            //gaPrimes[ gnPrimes++ ] = n-1;
            gaPrimes[ offsetByte ] |= offsetBits;
            gnPrimes++;

            gnLargest = n-1;
        }
        offsetBits <<= 1;
        gnBits++;

        if( Prime::IsPrime( n+1 ) ) // 6*i+1; n=6*x
        {
            //gaPrimes[ gnPrimes++ ] = n+1;
            gaPrimes[ offsetByte ] |= offsetBits;
            gnPrimes++;

            gnLargest = n+1;
        }
        offsetBits <<= 1;
        gnBits++;

        if( !offsetBits ) //32-bit or 64-bit overflow wraps around to zero
        {
            offsetBits = 1;
            offsetByte++;
        }
    }
#if 0
    // If (n+1) > max then we can't include in list if last 6i+1 was prime
    n -= 6;
    if ((n+1) > max) // only build primes up to and including n
        if ( Prime::IsPrime( n+1 ) )
            gnPrimes--;
#endif
}

// ============================================================
void PrintPrimes()
{
    printf( "gnPrimes = %u\n", gnPrimes + 2 );
    printf( "gaPrimes[ %u ] = {\n", gnPrimes+2 );

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

    // Since we don't store 2 and 3 in the bit table, need to print first two primes
    printf( "%*u, ", WIDTH_PER_COL, 2 );
    printf( "%*u, ", WIDTH_PER_COL, 3 );

    // iPrime is total number of primes printed so far
    prime_t offsetBits = 1, offsetByte = 0, n = 6, iPrime = 2;
    for( prime_t iBits = 0; iBits < gnBits; iBits++ )
    {
        if (gaPrimes[ offsetByte ] & offsetBits)
        {
            if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
                Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS, iPrime-1 );

            printf( "%*u, ", WIDTH_PER_COL, n-1 );
            iPrime++;
        }
        offsetBits <<= 1;

        iBits++;
        if( iBits >= gnBits)
            break;

        if (gaPrimes[ offsetByte ] & offsetBits)
        {
            if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
                Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS, iPrime-1 );

            printf( "%*u, ", WIDTH_PER_COL, n+1 );
            iPrime++;
        }
        offsetBits <<= 1;

        n += 6;
        if( !offsetBits ) //32-bit or 64-bit overflow wraps around to zero
        {
            offsetBits = 1;
            offsetByte++;
        }
    }

    int rem = (iPrime % COLUMNS);
    int pad = ((COLUMNS-rem) * (WIDTH_PER_COL+2)); //+2 == width ", "
    if (rem != 0)
        printf( "%*s", pad, "" );
    else
        rem = COLUMNS;
    Format::Suffix( WIDTH_PER_COL, (gnPrimes+2) - rem, (gnPrimes + 2 )- 1 );
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

    printf( "Primes found: %s'th = ", itoaComma( gnPrimes+1 ) );
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
//      :    65536; // [  6,541] =    65,521 // Release:    0.301 secs Largest 16-bit prime
//      :   100000; // [  9,592] =    99,991 // Release:    0.682 secs
//      :   611953; // [ 49,999] =   611,953 // Release:   20.435 secs First 50,000 primes
        : 10000000; // [664,578] = 9,999,991 // Release: 3016.245 secs = 00:50:16.589  Primes/Sec: 3 K#/s
//      : 15485863; // one millionth prime

    AllocArray ( max );
    TimerStart ( max );
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes();
    DeleteArray();

    return 0;
}
