// Globals
    int       gbPrintPrimes = false;

// BEGIN OMP
    int       gnThreadsMaximum = 0 ;
    int       gnThreadsActive  = 0 ; // 0 = auto detect; > 0 manual # of threads
// END OMP

    
int parse_args(  const int nArg, const char *aArg[] )
{
    int iArg = 1;
    for( iArg = 1; iArg < nArg; iArg++ )
    {
        const char *pArg = aArg[ iArg ];
        if (pArg[0] == '-' )
        {
#if defined(_OPENMP)
            if (pArg[1] == 'j')
            {
                if (pArg[2])
                {
                    printf( "Syntax for threads is:\n-j #\n" );
                    exit( 0 );
                }

                iArg++;
                if (iArg > nArg)
                {
                    printf( "Invalid # of threads to use.\n" );
                    exit( 0 );
                }
                gnThreadsActive = atoi( aArg[ iArg ] );
                if (gnThreadsActive < 0)
                    gnThreadsActive = 0;
                if (gnThreadsActive > gnThreadsMaximum)
                    gnThreadsActive = gnThreadsMaximum;
            }
            else
#endif
            if( (strcmp( pArg, "-help" ) == 0)
            ||  (strcmp( pArg,"--help" ) == 0)
            ||  (strcmp( pArg,"-?"     ) == 0))
            {
                printf(
"Syntax: [options] [max]\n"
"Find primes between 0 and max.  (Default is 10000000)\n"
"\n"
"Options:\n"
"    -help   Display command line arguments\n"

#if defined(_OPENMP)
"    -j #    Use # threads. (Default is %d threads.)\n"
#endif

"    -table  Print table of primes. (Default is OFF.)\n"

#if defined(_OPENMP)
                , gnThreadsMaximum
#endif
                );
                exit( 0 );
            }
            else
            if (strcmp( pArg, "-table" ) == 0)
            {
                gbPrintPrimes = true;
            }
        }
        else
            break;
    }

    return iArg;
}

void Pause()
{
    fflush( stdout );
    (void) getchar(); // C/C++ crap on Win32/Win64 doesn't detect ESC (0x1B)
}
