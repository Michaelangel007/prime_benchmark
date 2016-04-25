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
Version 4  - Multi-Threaded with OpenMP, Byte Vector, uint32_t printing prime array
Version 5  - Multi-Threaded with OpenMP, Byte Vector, no printing prime array
*/

// Includes
    #ifdef _WIN32 // MSVC sprintf() warning crap
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #include <stdio.h>  // printf()
    #include <string.h> // memcpy()
    #include <stdlib.h> // atoi()
    #include <math.h>   // sqrt()
// BEGIN OMP
    #include <omp.h> // MSVC: C++, Language, Open MP: /openmp
// END OMP

    #include "util_types.h" // or <stdint.h>
    #include "util_text.h"
    #include "util_timer.h"

// Macros
    // To support > 2^32 must use int64_t, to support > 2^31 must have 16 GB+ RAM !
    #define prime_t size_t

// Globals
    Timer timer;

    uint64_t gnPrimes = 0;

    size_t   gnLargest = 0; // dynamic max column width
    char    *gaIsPrime = NULL; // small primes: 0 ..max

// BEGIN OMP
    int       gnThreadsMaximum = 0 ;
    int       gnThreadsActive  = 0 ; // 0 = auto detect; > 0 manual # of threads
// END OMP

// Build table of dual primes from 2,3, 5,7 up to 6i-1,6i+1 but not including n=6i+1
// ============================================================
void BuildPrimes( const size_t max )
{
    gnPrimes      = 2;

    gaIsPrime[0] = 0;
    gaIsPrime[1] = 0;
    gaIsPrime[2] = 1;
    gaIsPrime[3] = 1;

    // Normally we would fill the array with 1 but we can pre-filter out all even numbers
    gaIsPrime[4+0] = 0; // 4
    gaIsPrime[4+1] = 1; // 5
    gaIsPrime[4+2] = 0; // 6
    gaIsPrime[4+3] = 1; // 7
    int nFill = *((int*) &gaIsPrime[4] ); // 0x01010101 or Little Endian: 0x01000100

    int    *pDst = (int*) &gaIsPrime[8];
    size_t  nLen = max;
    int    *pEnd = (int*)  gaIsPrime + nLen/sizeof(int);
    // memset( gaIsPrime, nFill, nLen - 8 ); // bug: memset() takes 'int fill' but treats it as char!
    while( pDst <= pEnd ) // '<=' buffer allocates +4 bytes extra
        *pDst++ = nFill;

    double d = (double)max;
    prime_t r = (prime_t) sqrt( d ); // root

    // d = (i+1)^2 - i^2 = d
    // d = (i^2 - i^2) + 2*i + 1
    // d = 2*i + 1
    prime_t i = 3;
    prime_t s = 9; // i^2 // sqr
    for( ; s <= r; s += 2*i+1, i++ ) // Can't use i*i in OpenMP
        if( gaIsPrime[ i ] )
            for( prime_t j = s; j <= r; j += i )
                gaIsPrime[ j ] = 0;

// BEGIN OMP
    const int nThreads = max / r;

#pragma omp parallel for
    for( int iThread = 3; iThread <= r; iThread++ )
    {
        if( !gaIsPrime[ iThread ] )
            continue;

        for( size_t iStart = 2*iThread; iStart <= max; iStart += iThread )
            gaIsPrime[ iStart ] = 0; // No multi-threaded atomic/fence/barrier needed since all threads write 0
    }
}

// ============================================================
void PrintPrimes( const size_t max )
{
    printf( "gaPrimes[ %llu ] = {\n", gnPrimes );

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

    // We don't need to store the primes as we can do this when we print them

    // Except for 2 and 3, every prime is of the form: n=6*i +/- 1
    printf( "%*u, ", WIDTH_PER_COL, 2 );
    printf( "%*u, ", WIDTH_PER_COL, 3 );

    prime_t n, iPrime = 2;
    for( n = 6; n <= max; n += 6 )
    {
        if ( gaIsPrime[ n - 1 ] ) // 6*i-1
        {
            if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
                Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS+1, iPrime );
            printf( "%*zu, ", WIDTH_PER_COL, n-1 );
            iPrime++;
        }
        if ( gaIsPrime[ n + 1 ] )// 6*i+1
        {
            if ((iPrime > 0) && ((iPrime % COLUMNS) == 0))
                Format::Suffix( WIDTH_PER_COL, iPrime-COLUMNS+1, iPrime );
            printf( "%*zu, ", WIDTH_PER_COL, n+1 );
            iPrime++;
        }
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
// BEGIN OMP
    if(!gnThreadsActive) // user didn't specify how many threads to use, default to all of them
        gnThreadsActive = gnThreadsMaximum;

    omp_set_num_threads( gnThreadsActive );

    printf( "Using: %d / %d cores\n", gnThreadsActive, gnThreadsMaximum );
// END OMP

    size_t nElements   = max + 4; // +4 so we can write int32 init pattern
    size_t nBytesTotal = nElements * sizeof( char );
    printf( "Allocating memory..: %s * %d = ", itoaComma( nElements ), (int) sizeof( char ) );
    printf( "%s bytes\n", itoaComma( nBytesTotal ) );

    gnPrimes = 0;

    //    gaIsPrime = new char[ nElements ]; // Win32 crap, can't allocate > 2^31 bytes
#if !defined(_WIN64)
    if (nBytesTotal >= (1 << 30))
    {
        size_t lo = (1 << 30); // nBytesTotal/2 ; // or 1354039225
        size_t hi = nBytesTotal;

        // Search what the largest memory allocation block is
        while (lo < hi) // gaIsPrime == NULL)
        {
            size_t mid = (lo + hi) / 2;
            gaIsPrime = (char*) malloc( mid );
            if( gaIsPrime )
            {
                free( gaIsPrime );
                lo = mid + 1; // +1 to prevent infinite loop
            }
            else
                hi = mid;
        }

        printf( "ERROR: Can't allocate > %s bytes on 32-bit compiled application.\nUse a 64-bit compiled version.\n", itoaComma( lo ) );
        printf( "Max # = %s\n", itoaComma( lo - 4 ) );
        exit( 1 );
    }
#endif

    // Will be initialized to <0,0,1,1>,<0,1,0,1>*
    gaIsPrime = (char*) malloc( nBytesTotal );
}

// ============================================================
void DeleteArray()
{
    //delete [] gaIsPrime;
    free( gaIsPrime );
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

//    gnLargest = gaPrimes[ gnPrimes - 1 ];
    for( size_t n = 5; n <= max; n += 2 )
        if ( gaIsPrime[ n ] )
        {
            gnPrimes++; 
            gnLargest = n;
        }

printf( "%08X_%08X\n", (uint32_t)((gnPrimes >> 32) & 0xFFFFFFFF), (uint32_t)(gnPrimes & 0xFFFFFFFF) );
    printf( "Primes found: %s'th = ", itoaComma( gnPrimes ) );
    printf( "%s\n", itoaComma( gnLargest ) );

    printf( "Elapsed: %.3f secs = %s%s  Primes/Sec: %s %c#/s\n"
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

    int iArg = 1;
    for( iArg = 1; iArg < nArg; iArg++ )
    {
        if (aArg[ iArg ][0] == '-' )
        {
            if (aArg[iArg][1] == 'j')
            {
                iArg++;
                if (iArg > nArg)
                    return printf( "Invalid # of threads to use.\n" );
                gnThreadsActive = atoi( aArg[ iArg ] );
                if (gnThreadsActive < 0)
                    gnThreadsActive = 0;
                if (gnThreadsActive > gnThreadsMaximum)
                    gnThreadsActive = gnThreadsMaximum;
            }
        }
        else
            break;
    }

    prime_t max = (nArg > iArg)
        ? (prime_t) atou( aArg[ iArg ] )
//      :          6; // Test 6i+1>max && isprime(6i+1)==true
//      :         32; // Test 8 core
//      :         64; // Test 8 core
//      :        255; // 2^8 Test 8 core
//      :        256; //10^3 Test 8 core [54] = 251 // Largest 8-bit prime

//      :        100; //10^2    [             25] =            97 // 25 primes between 1 and 100
//      :       1000; //10^3    [            168] =           997
//             10000; //10^4    [          1,229] =         9,973 //
//      :      65536; // 2^16   [          6,542] =        65,521 // x86: 00:00:00.001, x64: 00:00:00.001  Primes/Sec: 64,000 K#/s   Largest 16-bit prime
//      :     100000; //10^5    [          9,592] =        99,991 // x86: 00:00:00.001, x64: 00:00:00.001  Primes/Sec: 97,000 K#/s
//      :     611953; //        [         50,000] =       611,953 // x86: 00:00:00.002, x64: 00:00:00.001  Primes/Sec: 597,000 K#/s  First 50,000 primes
//      :    1000000; //10^6    [         78,498] =       999,983 // x86: 00:00:00.003, x64: 00:00:00.002  Primes/Sec: 488,000 K#/s
        :   10000000; //10^7    [        664,579] =     9,999,991 // x86: 00:00:00.031, x64: 00:00:00.024  Primes/Sec: 375 M#/s
//      :   15485863; //        [      1,000,000] =    15,485,863 // x86: 00:00:00.057, x64: 00:00:00.047  Primes/Sec: 297 M#/s      First 1,000,000 primes
//      :  100000000; //10^8    [      5,761,455] =    99,999,989 // x86: 00:00:00.490, x64: 00:00:00.432  Primes/Sec: 219 M#/s
//      : 1000000000; //10^9    [     50,847,534] =   999,999,937 // x86: crash         x64: 00:00:09.930  Primes/Sec: 95 M#/s
//      : 1354039225; // Win32  [     67,801,444] = 1,354,039,153 //                    x64: 00:00:13.869  Primes/Sec: 93 M#/s
//      : 2038074743; //        [    100,000,000] = 2,038,074,743 //                    x64: 00:00:21.912  Primes/Sec: 88 M#/s       First 100,000,000 primes
//      : 2147483644; // 2^31-4 [    105,097,564] = 2,147,483,629 //                    x64: 00:00:23.162  Primes/Sec: 88 M#/s
//      : 2147483647; // 2^31-1 [    105,097,565] = 2,147,483,647 //                    x64: 00:00:23.114  Primes/Sec: 88 M#/s
//      : 2147483648; // 2^31   [    105,097,565] = 2,147,483,647 //                    x64: 00:00:23.179  Primes/Sec: 88 M#/s
//      : 4294967292; // 2^32-4 [    203,280,221] = 4,294,967,291 //                    x64: 00:00:50.852  Primes/Sec: 80 M#/s
//      : 4294967295; // 2^32-1 [               ]
//      : 4294967296; // 2^32   [    203,280,221] = 4,294,967,291 //                    x64: 00:00:50.808  Primes/Sec: 80 M#/s
//      :10000000000; //10^10   [    455,052,511] = 9,999,999,967 //                    x64: 00:02:13.700  Primes/Sec: 71 M#/s
//      :       1e11; //10^11   [  4,118,054,813] =
//      :       1e12; //10^12   [ 37,607,912,018] =
//      :       1e13; //10^13   [346,065,536,839] =

    AllocArray ( max );
    TimerStart ( max );
    BuildPrimes( max );
    TimerStop  ( max );
    getchar();
    PrintPrimes( max );
    DeleteArray();

    return 0;
}
